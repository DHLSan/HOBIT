/*
 * pgconv.h
 *
 *  Created on: 23 Haz 2021
 *      Author: Samet  DURU
 */

#ifndef SRC_PGCONV_H_
#define SRC_PGCONV_H_

#include "xil_io.h"
#include "dimension_def.h"
#include "FracBNN.h"

char compute_engine_64(unsigned long long b, unsigned long long w){
	unsigned int b_lsb = b % 4294967296; //2^32 = 4294967296
	unsigned int b_msb = b / 4294967296;
	unsigned int w_lsb = w % 4294967296;
	unsigned int w_msb = w / 4294967296;

	FRACBNN_mWriteReg(0x43C00000, 8, b_msb);
	FRACBNN_mWriteReg(0x43C00000, 4, b_lsb);
	FRACBNN_mWriteReg(0x43C00000, 16, w_msb);
	FRACBNN_mWriteReg(0x43C00000, 12, w_lsb);

	FRACBNN_mWriteReg(0x43C00000,0,0x00000001); //start = 1
	while((FRACBNN_mReadReg(0x43C00000,0) & 0x00000002) != 2);  //wait until done is asserted
	FRACBNN_mWriteReg(0x43C00000,0,0x00000000);   //start = 0
	unsigned int result = FRACBNN_mReadReg(0x43C00000,20);

	FRACBNN_mWriteReg(0x43C00000,0,0x00000010);  //clear ready and wait registers
	FRACBNN_mWriteReg(0x43C00000,0,0x00000000);

	return (char)result;
}

void binary_conv3x3_tile( unsigned long long msb_inputs[WIDTH][WIDTH],
		const unsigned long long weights[OUT_CHANNEL_PARALLELISM][3][3],
		short msb_outputs[CHANNEL_OUT_T][WIDTH][WIDTH],

		short comparator[CHANNEL_OUT_T][WIDTH][WIDTH],
		const float threshold[OUT_CHANNEL_PARALLELISM],
        int switch_on,

		int c_in,
		int in_channels,
		int H_fmap_out
)
{
	const float msb_scale = 2.0/3.0;
	unsigned long long msb_line_buffer[2][WIDTH] = {0};
	unsigned long long msb_window_buffer[3][3] = {0};
	short msb_partial_out_feature[OUT_CHANNEL_PARALLELISM] = {0};

	for (int row=0; row<H_fmap_out+1; row++) {
		for (int col=0; col<H_fmap_out+1; col++) {
			for (int i=0; i<3; i++) {
				msb_window_buffer[i][0] = msb_window_buffer[i][1];
				msb_window_buffer[i][1] = msb_window_buffer[i][2];
			}

			msb_window_buffer[0][2] = (msb_line_buffer[0][col]);
			msb_window_buffer[1][2] = (msb_line_buffer[0][col] = msb_line_buffer[1][col]);
			msb_window_buffer[2][2] = (msb_line_buffer[1][col] = msb_inputs[row][col]);

			for (int channel_pt=0; channel_pt<OUT_CHANNEL_PARALLELISM; channel_pt++) {
				if (c_in > 0){
					msb_partial_out_feature[channel_pt] = msb_outputs[channel_pt][row][col];
				}
				else{
					msb_partial_out_feature[channel_pt] = 0;
				}
			}

			for (int channel_pt=0; channel_pt<OUT_CHANNEL_PARALLELISM; channel_pt++) {
				short msb_accumulation = 0;

				if (switch_on || (msb_scale*comparator[channel_pt][row][col]>threshold[channel_pt])){
					for (int k_row=0; k_row<3; k_row++) {
						for (int k_col=0; k_col<3; k_col++) {
							int row_idx_pad = row - 2 + k_row;
							int col_idx_pad = col - 2 + k_col;

							if(row_idx_pad>=0 && row_idx_pad<H_fmap_out && col_idx_pad>=0 && col_idx_pad<H_fmap_out){
								unsigned long long msb_a = msb_window_buffer[k_row][k_col];
								unsigned long long w = weights[channel_pt][k_row][k_col];
								msb_accumulation += in_channels - 2*compute_engine_64(msb_a, w);
							}
						}
					}
				}
				msb_partial_out_feature[channel_pt] += msb_accumulation;
			}
			for (int channel_pt=0; channel_pt<OUT_CHANNEL_PARALLELISM; channel_pt++) {
				msb_outputs[channel_pt][row][col] = msb_partial_out_feature[channel_pt];
			}

		}
	}
	return;
}

void pg_conv3x3_tile(
        unsigned long long msb_inputs[WIDTH][WIDTH],
        unsigned long long lsb_inputs[WIDTH][WIDTH],
        const unsigned long long weights[OUT_CHANNEL_PARALLELISM][3][3],
        short msb_outputs[CHANNEL_OUT_T][WIDTH][WIDTH],
        short lsb_outputs[CHANNEL_OUT_T][WIDTH][WIDTH],
        const float threshold[OUT_CHANNEL_PARALLELISM],

        int c_in,
        int in_channels,
        int H_fmap_out
)
{
    int switch_on = 1;
    binary_conv3x3_tile(
        msb_inputs, weights, msb_outputs,
        lsb_outputs,
        threshold,
        switch_on,
        c_in, in_channels, H_fmap_out);

    switch_on = 0;
    binary_conv3x3_tile(
        lsb_inputs, weights, lsb_outputs,
        msb_outputs,
        threshold,
        switch_on,
        c_in, in_channels, H_fmap_out);
}

#endif /* SRC_PGCONV_H_ */



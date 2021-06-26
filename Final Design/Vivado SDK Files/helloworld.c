/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "pgconv.h"
#include "weights_fracnet_64.h"
#include "conv_weights.h"
#include "layer.h"
#include "math.h"

#include "xil_io.h"
#include "xtime_l.h"

int main()
{
    init_platform();

    //unsigned long long msb_fmap[3][WIDTH][WIDTH];
	//float out_buf_0[CHANNEL_OUT/CHANNEL_OUT_T][CHANNEL_OUT_T][WIDTH][WIDTH];
    //short out_buf_t0[CHANNEL_OUT_T][WIDTH][WIDTH];
    //short out_buf_t1[CHANNEL_OUT_T][WIDTH][WIDTH];

    int h,r,k;

    unsigned long long ***msb_fmap = (unsigned long long ***) malloc(sizeof(unsigned long long ***)*3);
	for(h = 0; h < 4; h++ ){
		msb_fmap[h] = (unsigned long long **) malloc(sizeof(unsigned long long*)*33);
		for(r = 0; r <35; r++) {
			msb_fmap[h][r] = (unsigned long long *) malloc(sizeof(unsigned long long)*33);
		}

	}

	float ****out_buf_0 = (float ****) malloc (sizeof(int ****)*4);
	for(h = 0; h < 4; h++ ){
		out_buf_0[h] = (float ***) malloc(sizeof(float**)*16);
		for(r = 0; r <16; r++) {
			out_buf_0[h][r] = (float **) malloc(sizeof(float*)*33);
			for(k=0; k<33; k++){
				out_buf_0[h][r][k] = (float *) malloc(sizeof(float)*33);
			}
		}

	}

    short ***out_buf_t0 = (short ***) malloc (sizeof(short ***)*16);
	for(h = 0; h < 16; h++ ){
		out_buf_t0[h] = (short **) malloc(sizeof(short*)*33);
		for(r = 0; r <33; r++) {
			out_buf_t0[h][r] = (short *) malloc(sizeof(short)*33);
		}
	}

    short ***out_buf_t1 = (short ***) malloc (sizeof(short ***)*16);
	for(h = 0; h < 16; h++ ){
		out_buf_t1[h] = (short **) malloc(sizeof(short*)*33);
		for(r = 0; r <33; r++) {
			out_buf_t1[h][r] = (short *) malloc(sizeof(short)*33);
		}
	}

	//printf("Deneme1\n\r");

	int i,j,c;

	for (i = 0; i < WIDTH; i ++){
			for (j = 0; j < WIDTH; j ++){
				for (int k = 0; k < 3; k ++) {
					msb_fmap[k][i][j] = 0;
				}
				for ( c = 0; c < CHANNEL_OUT/CHANNEL_OUT_T; c ++){
					for (int k = 0; k < CHANNEL_OUT_T; k ++) {
						out_buf_0[c][k][i][j] = 0;
					}
				}
				for (k = 0; k < CHANNEL_OUT_T; k ++) {
					out_buf_t0[k][i][j] = 0;
					out_buf_t1[k][i][j] = 0;
				}
			}
		}



	int H_fmap_in, H_fmap_out, in_channels, in_channels_after_pack;
	int out_channels, out_channel_start, stride, conv_weight_ptr;

	int row, col;
	for (c = 0; c < 3; c++) {
		//printf("c:%d\n\r",c);
		for (row = 0; row < 33; row++) {
			//printf("row:%d\n\r",row);
			for (col = 0; col < 33; col++) {
				//printf("col:%d\n\r",col);
				msb_fmap[c][row][col] = 5;
				//printf("%llu\n\r",msb_fmap[c][row][col]);
			}
		}
	}

	//printf("exit\n\r");


	in_channels = 32;
	in_channels_after_pack = 3;
	out_channels = 16;
	H_fmap_out = 32;
	conv_weight_ptr = 0;

	  for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++){
	        int c_in = 0;
	        pg_conv3x3_tile(
	                msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
	                out_buf_t0, out_buf_t1,
	                layer1_0_conv1_threshold_fix[c_out],
	                c_in, in_channels, H_fmap_out
	        );
	        conv_weight_ptr += 1;
	        c_in = 1;
	        pg_conv3x3_tile(
	                msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
	                out_buf_t0, out_buf_t1,
	                layer1_0_conv1_threshold_fix[c_out],
	                c_in, in_channels, H_fmap_out
	        );
	        conv_weight_ptr += 1;
	        c_in = 2;
	        pg_conv3x3_tile(
	                msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
	                out_buf_t0, out_buf_t1,
	                layer1_0_conv1_threshold_fix[c_out],
	                c_in, in_channels, H_fmap_out
	        );
	        conv_weight_ptr += 1;

	        bn1(
	                out_buf_0, out_buf_t0,
	                bn1_weight_fix[c_out], bn1_bias_fix[c_out],
	                stride, c_out, H_fmap_out
	        );
	    }

		H_fmap_in = 32;
		H_fmap_out = 32;
		in_channels = 16;
		in_channels_after_pack = 1;
		out_channels = 16;
		stride = 1;

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);

		//printf("Deneme4\n\r");

		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
	                layer1_0_conv1_threshold_fix[c_out],
					c_in, in_channels, H_fmap_out
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer1_0_bn1_weight_fix[c_out],
					layer1_0_bn3_weight_fix[c_out],
					layer1_0_bn1_bias_fix[c_out],
					layer1_0_bn3_bias_fix[c_out],
					layer1_0_rprelu1_shift_x_bias_fix[c_out],
					layer1_0_rprelu1_shift_y_bias_fix[c_out],
					layer1_0_rprelu1_prelu_weight_fix[c_out],

					stride, c_out, H_fmap_out, out_channels
			);
		}

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);

		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
				int c_in = 0;
				pg_conv3x3_tile(
						msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
						out_buf_t0, out_buf_t1,
						layer1_0_conv2_threshold_fix[c_out],
						c_in, in_channels, H_fmap_out
				);
				conv_weight_ptr += 1;
				bn_relu_shortcut(
						out_buf_0, out_buf_t0, out_buf_t1,

						layer1_0_bn2_weight_fix[c_out],
						layer1_0_bn4_weight_fix[c_out],
						layer1_0_bn2_bias_fix[c_out],
						layer1_0_bn4_bias_fix[c_out],
						layer1_0_rprelu2_shift_x_bias_fix[c_out],
						layer1_0_rprelu2_shift_y_bias_fix[c_out],
						layer1_0_rprelu2_prelu_weight_fix[c_out],

						stride, c_out, H_fmap_out, out_channels
				);
			}

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);

		//printf("Deneme5\n\r");

		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
				int c_in = 0;
				pg_conv3x3_tile(
						msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
						out_buf_t0, out_buf_t1,
						layer1_1_conv1_threshold_fix[c_out],
						c_in, in_channels, H_fmap_out
				);
				conv_weight_ptr += 1;
				bn_relu_shortcut(
						out_buf_0, out_buf_t0, out_buf_t1,

						layer1_1_bn1_weight_fix[c_out],
						layer1_1_bn3_weight_fix[c_out],
						layer1_1_bn1_bias_fix[c_out],
						layer1_1_bn3_bias_fix[c_out],
						layer1_1_rprelu1_shift_x_bias_fix[c_out],
						layer1_1_rprelu1_shift_y_bias_fix[c_out],
						layer1_1_rprelu1_prelu_weight_fix[c_out],

						stride, c_out, H_fmap_out, out_channels
				);
			}

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);

		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
				int c_in = 0;
				pg_conv3x3_tile(
						msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
						out_buf_t0, out_buf_t1,
						layer1_1_conv2_threshold_fix[c_out],
						c_in, in_channels, H_fmap_out
				);
				conv_weight_ptr += 1;
				bn_relu_shortcut(
						out_buf_0, out_buf_t0, out_buf_t1,

						layer1_1_bn2_weight_fix[c_out],
						layer1_1_bn4_weight_fix[c_out],
						layer1_1_bn2_bias_fix[c_out],
						layer1_1_bn4_bias_fix[c_out],
						layer1_1_rprelu2_shift_x_bias_fix[c_out],
						layer1_1_rprelu2_shift_y_bias_fix[c_out],
						layer1_1_rprelu2_prelu_weight_fix[c_out],

						stride, c_out, H_fmap_out, out_channels
				);
			}

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);

		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
				int c_in = 0;
				pg_conv3x3_tile(
						msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
						out_buf_t0, out_buf_t1,
						layer1_2_conv1_threshold_fix[c_out],
						c_in, in_channels, H_fmap_out
				);
				conv_weight_ptr += 1;
				bn_relu_shortcut(
						out_buf_0, out_buf_t0, out_buf_t1,

						layer1_2_bn1_weight_fix[c_out],
						layer1_2_bn3_weight_fix[c_out],
						layer1_2_bn1_bias_fix[c_out],
						layer1_2_bn3_bias_fix[c_out],
						layer1_2_rprelu1_shift_x_bias_fix[c_out],
						layer1_2_rprelu1_shift_y_bias_fix[c_out],
						layer1_2_rprelu1_prelu_weight_fix[c_out],

						stride, c_out, H_fmap_out, out_channels
				);
			}

		//printf("Deneme6\n\r");

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);

		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
				int c_in = 0;
				pg_conv3x3_tile(
						msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
						out_buf_t0, out_buf_t1,
						layer1_2_conv2_threshold_fix[c_out],
						c_in, in_channels, H_fmap_out
				);
				conv_weight_ptr += 1;
				bn_relu_shortcut(
						out_buf_0, out_buf_t0, out_buf_t1,

						layer1_2_bn2_weight_fix[c_out],
						layer1_2_bn4_weight_fix[c_out],
						layer1_2_bn2_bias_fix[c_out],
						layer1_2_bn4_bias_fix[c_out],
						layer1_2_rprelu2_shift_x_bias_fix[c_out],
						layer1_2_rprelu2_shift_y_bias_fix[c_out],
						layer1_2_rprelu2_prelu_weight_fix[c_out],

						stride, c_out, H_fmap_out, out_channels
				);
			}

		H_fmap_in = 32;
		H_fmap_out = 16;
		in_channels = 16;
		in_channels_after_pack = 1;
		out_channels = 32;
		stride = 2;

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);
		avgpool_concat(out_buf_0, H_fmap_out, in_channels);
		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
					layer2_0_conv1_threshold_fix[c_out],
					c_in, in_channels, H_fmap_in
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer2_0_bn1_weight_fix[c_out],
					layer2_0_bn3_weight_fix[c_out],
					layer2_0_bn1_bias_fix[c_out],
					layer2_0_bn3_bias_fix[c_out],
					layer2_0_rprelu1_shift_x_bias_fix[c_out],
					layer2_0_rprelu1_shift_y_bias_fix[c_out],
					layer2_0_rprelu1_prelu_weight_fix[c_out],

					stride, c_out, H_fmap_out, out_channels
			);
		}

		H_fmap_in = 16;
		H_fmap_out = 16;
		in_channels = 32;
		in_channels_after_pack = 1;
		out_channels = 32;
		stride = 1;

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);
		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
					layer2_0_conv2_threshold_fix[c_out],
					c_in, in_channels, H_fmap_out
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer2_0_bn2_weight_fix[c_out],
					layer2_0_bn4_weight_fix[c_out],
					layer2_0_bn2_bias_fix[c_out],
					layer2_0_bn4_bias_fix[c_out],
					layer2_0_rprelu2_shift_x_bias_fix[c_out],
					layer2_0_rprelu2_shift_y_bias_fix[c_out],
					layer2_0_rprelu2_prelu_weight_fix[c_out],

					stride, c_out, H_fmap_out, out_channels
			);
		}

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);
		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
					layer2_1_conv1_threshold_fix[c_out],
					c_in, in_channels, H_fmap_out
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer2_1_bn1_weight_fix[c_out],
					layer2_1_bn3_weight_fix[c_out],
					layer2_1_bn1_bias_fix[c_out],
					layer2_1_bn3_bias_fix[c_out],
					layer2_1_rprelu1_shift_x_bias_fix[c_out],
					layer2_1_rprelu1_shift_y_bias_fix[c_out],
					layer2_1_rprelu1_prelu_weight_fix[c_out],


					stride, c_out, H_fmap_out, out_channels
			);
		}

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);
		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
					layer2_1_conv2_threshold_fix[c_out],
					c_in, in_channels, H_fmap_out
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer2_1_bn2_weight_fix[c_out],
					layer2_1_bn4_weight_fix[c_out],
					layer2_1_bn2_bias_fix[c_out],
					layer2_1_bn4_bias_fix[c_out],
					layer2_1_rprelu2_shift_x_bias_fix[c_out],
					layer2_1_rprelu2_shift_y_bias_fix[c_out],
					layer2_1_rprelu2_prelu_weight_fix[c_out],

					stride, c_out, H_fmap_out, out_channels
			);
		}

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);
		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
					layer2_2_conv1_threshold_fix[c_out],
					c_in, in_channels, H_fmap_out
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer2_2_bn1_weight_fix[c_out],
					layer2_2_bn3_weight_fix[c_out],
					layer2_2_bn1_bias_fix[c_out],
					layer2_2_bn3_bias_fix[c_out],
					layer2_2_rprelu1_shift_x_bias_fix[c_out],
					layer2_2_rprelu1_shift_y_bias_fix[c_out],
					layer2_2_rprelu1_prelu_weight_fix[c_out],


					stride, c_out, H_fmap_out, out_channels
			);
		}

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);
		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
					layer2_2_conv2_threshold_fix[c_out],
					c_in, in_channels, H_fmap_out
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer2_2_bn2_weight_fix[c_out],
					layer2_2_bn4_weight_fix[c_out],
					layer2_2_bn2_bias_fix[c_out],
					layer2_2_bn4_bias_fix[c_out],
					layer2_2_rprelu2_shift_x_bias_fix[c_out],
					layer2_2_rprelu2_shift_y_bias_fix[c_out],
					layer2_2_rprelu2_prelu_weight_fix[c_out],

					stride, c_out, H_fmap_out, out_channels
			);
		}

		H_fmap_in = 16;
		H_fmap_out = 8;
		in_channels = 32;
		in_channels_after_pack = 1;
		out_channels = 64;
		stride = 2;

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);
		avgpool_concat(out_buf_0, H_fmap_out, in_channels);
		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
					layer3_0_conv1_threshold_fix[c_out],
					c_in, in_channels, H_fmap_in
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer3_0_bn1_weight_fix[c_out],
					layer3_0_bn3_weight_fix[c_out],
					layer3_0_bn1_bias_fix[c_out],
					layer3_0_bn3_bias_fix[c_out],
					layer3_0_rprelu1_shift_x_bias_fix[c_out],
					layer3_0_rprelu1_shift_y_bias_fix[c_out],
					layer3_0_rprelu1_prelu_weight_fix[c_out],

					stride, c_out, H_fmap_out, out_channels
			);
		}

		H_fmap_in = 8;
		H_fmap_out = 8;
		in_channels = 64;
		in_channels_after_pack = 1;
		out_channels = 64;
		stride = 1;

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);
		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
					layer3_0_conv2_threshold_fix[c_out],
					c_in, in_channels, H_fmap_out
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer3_0_bn2_weight_fix[c_out],
					layer3_0_bn4_weight_fix[c_out],
					layer3_0_bn2_bias_fix[c_out],
					layer3_0_bn4_bias_fix[c_out],
					layer3_0_rprelu2_shift_x_bias_fix[c_out],
					layer3_0_rprelu2_shift_y_bias_fix[c_out],
					layer3_0_rprelu2_prelu_weight_fix[c_out],

					stride, c_out, H_fmap_out, out_channels
			);
		}

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);
		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
					layer3_1_conv1_threshold_fix[c_out],
					c_in, in_channels, H_fmap_out
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer3_1_bn1_weight_fix[c_out],
					layer3_1_bn3_weight_fix[c_out],
					layer3_1_bn1_bias_fix[c_out],
					layer3_1_bn3_bias_fix[c_out],
					layer3_1_rprelu1_shift_x_bias_fix[c_out],
					layer3_1_rprelu1_shift_y_bias_fix[c_out],
					layer3_1_rprelu1_prelu_weight_fix[c_out],


					stride, c_out, H_fmap_out, out_channels
			);
		}

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);
		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
					layer3_1_conv2_threshold_fix[c_out],
					c_in, in_channels, H_fmap_out
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer3_1_bn2_weight_fix[c_out],
					layer3_1_bn4_weight_fix[c_out],
					layer3_1_bn2_bias_fix[c_out],
					layer3_1_bn4_bias_fix[c_out],
					layer3_1_rprelu2_shift_x_bias_fix[c_out],
					layer3_1_rprelu2_shift_y_bias_fix[c_out],
					layer3_1_rprelu2_prelu_weight_fix[c_out],

					stride, c_out, H_fmap_out, out_channels
			);
		}

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);
		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
					layer3_2_conv1_threshold_fix[c_out],
					c_in, in_channels, H_fmap_out
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer3_2_bn1_weight_fix[c_out],
					layer3_2_bn3_weight_fix[c_out],
					layer3_2_bn1_bias_fix[c_out],
					layer3_2_bn3_bias_fix[c_out],
					layer3_2_rprelu1_shift_x_bias_fix[c_out],
					layer3_2_rprelu1_shift_y_bias_fix[c_out],
					layer3_2_rprelu1_prelu_weight_fix[c_out],


					stride, c_out, H_fmap_out, out_channels
			);
		}

		quant_and_pack(out_buf_0, msb_fmap, H_fmap_in, in_channels);
		for (int c_out = 0; c_out < out_channels/OUT_CHANNEL_PARALLELISM; c_out ++) {
			int c_in = 0;
			pg_conv3x3_tile(
					msb_fmap[c_in], msb_fmap[(c_in+1)%CHANNEL_IN], conv_weight_all[conv_weight_ptr],
					out_buf_t0, out_buf_t1,
					layer3_2_conv2_threshold_fix[c_out],
					c_in, in_channels, H_fmap_out
			);
			conv_weight_ptr += 1;
			bn_relu_shortcut(
					out_buf_0, out_buf_t0, out_buf_t1,

					layer3_2_bn2_weight_fix[c_out],
					layer3_2_bn4_weight_fix[c_out],
					layer3_2_bn2_bias_fix[c_out],
					layer3_2_bn4_bias_fix[c_out],
					layer3_2_rprelu2_shift_x_bias_fix[c_out],
					layer3_2_rprelu2_shift_y_bias_fix[c_out],
					layer3_2_rprelu2_prelu_weight_fix[c_out],

					stride, c_out, H_fmap_out, out_channels
			);
		}

		float pool_out_buf[64];
		float linear_out_buf[10];
		for (int i = 0; i < 64; i ++){
			pool_out_buf[i] = 0;
		}

		for (int i = 0; i < 10; i ++){
			linear_out_buf[i] = 0;
		}
		avgpool_8x8(out_buf_0, pool_out_buf);
		matmul(pool_out_buf, linear_weight_fix, linear_bias_fix, linear_out_buf);

		float output[10];

		for(int i=0; i<10; i++){
			output[i] = linear_out_buf[i];
			printf("%f\n\r", output[i]);
		}





    cleanup_platform();
    return 0;
}

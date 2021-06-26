/*
 * layer.h
 *
 *  Created on: 23 Haz 2021
 *      Author: Samet  DURU
 */

#ifndef SRC_LAYER_H_
#define SRC_LAYER_H_

#include "pgconv.h"
#include "dimension_def.h"
#include "weights_fracnet_64.h"
#include "conv_weights.h"

char to2bit(float x)
{
	const float scale = 1.5;
	float temp = ((x+1)*scale);
	return (char)temp;
}




void quant_and_pack(
		float prior_outputs[CHANNEL_OUT/CHANNEL_OUT_T][CHANNEL_OUT_T][WIDTH][WIDTH],
		unsigned long long msb_buffer[CHANNEL_IN][WIDTH][WIDTH],
		int H_fmap,
		int in_channels
)
{
	char val = 0;
	for (int ch_t=0; ch_t<CHANNEL_OUT/CHANNEL_OUT_T; ch_t++){
		for (int row=0; row<H_fmap; row++){
			for (int col=0; col<H_fmap; col++){
				int channel_start = ch_t*CHANNEL_OUT_T;
				for (int ch_offset=0; ch_offset<CHANNEL_OUT_T; ch_offset++){
					float raw_input = prior_outputs[ch_t][ch_offset][row][col];
					int ch = channel_start + ch_offset;
					if (ch < in_channels) {
						val = to2bit(raw_input);
					} else {
						val = 0;
					}
					unsigned long long valU = 2;//Upper bit mask -> 10
					unsigned long long valL = 1;//Lower bit mask -> 01
					valU &= val;
					valL &= val;
					if(valU == 2 && valL == 1){
						valU = 1;
					}
					else if(valU == 2){
						valU = 1;
						valL = 0;
					}
					else if(valL == 1){
						valU = 0;
					}
					else if(valU == 0 && valL == 0){
						valU = 0;
						valL = 0;
					}

					int i = 63 - ch;
					int j = 63 - ch;

					if(valU == 1){
						while(i > 0){
							valU *= 2;
							i--;
						}
						msb_buffer[0][row][col] |= valU;
					}

					if(valU == 0){
						valU = 1;
						while(i > 0){
							valU *= 2;
							i--;
						}
						valU = ~valU;
						msb_buffer[0][row][col] &= valU;
					}

					if(valL == 1){
						while(j > 0){
							valL *= 2;
							j--;
						}
						msb_buffer[1][row][col] |= valL;
					}

					if(valL == 0){
						valL = 1;
						while(j > 0){
							valL *= 2;
							j--;
						}
						valL = ~valL;
						msb_buffer[1][row][col] &= valL;
					}
				}
			}
		}
	}
}

void bn1(
		float out_buf[CHANNEL_OUT/CHANNEL_OUT_T][CHANNEL_OUT_T][WIDTH][WIDTH],
		short block_t0[CHANNEL_OUT_T][WIDTH][WIDTH],

		const float *bn_weight,
		const float *bn_bias,

		int stride,
		int channel_tile,
		int H_fmap
)
{

	for (int row = 0; row < H_fmap; row ++) {
		for (int col = 0; col < H_fmap; col ++) {
			for (int ch = 0; ch < OUT_CHANNEL_PARALLELISM; ch ++) {
				out_buf[channel_tile][ch][row][col] = bn_weight[ch]*block_t0[ch][row+1][col+1] + bn_bias[ch];
			}
		}
	}
}

void bn_relu_shortcut(
		float residual[CHANNEL_OUT/CHANNEL_OUT_T][CHANNEL_OUT_T][WIDTH][WIDTH],
		short block_t0[CHANNEL_OUT_T][WIDTH][WIDTH],
		short block_t1[CHANNEL_OUT_T][WIDTH][WIDTH],

		const float *bn_weight_0,
		const float *bn_weight_1,
		const float *bn_bias_0,
		const float *bn_bias_1,
		const float *relu_x_bias,
		const float *relu_y_bias,
		const float *relu_weight,

		int stride,
		int channel_tile,
		int H_fmap,
		int out_channels
)
{
	float out_feature_t0[BN_CHANNEL_PARALLELISM];
	float out_feature_t1[BN_CHANNEL_PARALLELISM];

	const float msb_scale = 2.0/3.0;
	const float lsb_scale = 1.0/3.0;

	for (int i=0; i<H_fmap; i++) {
		for (int j=0; j<H_fmap; j++) {

			// Merge Tile
			for (int ch_offset=0; ch_offset<BN_CHANNEL_PARALLELISM; ch_offset++){
				out_feature_t0[ch_offset] = block_t0[ch_offset][i*stride+1][j*stride+1];
				out_feature_t1[ch_offset] = block_t1[ch_offset][i*stride+1][j*stride+1];
			}
			for (int ch_offset=0; ch_offset<BN_CHANNEL_PARALLELISM; ch_offset++){
				out_feature_t0[ch_offset] *= msb_scale;
				out_feature_t0[ch_offset] += (out_feature_t1[ch_offset]*lsb_scale);
			}

			// Load Residual
			for (int channel_pt=0; channel_pt<BN_CHANNEL_PARALLELISM; channel_pt++) {
				out_feature_t1[channel_pt] = residual[channel_tile][channel_pt][i][j];
			}

			// Batch Normalization
			for (int channel_pt=0; channel_pt<BN_CHANNEL_PARALLELISM; channel_pt++) {
				out_feature_t0[channel_pt] = bn_weight_0[channel_pt]*out_feature_t0[channel_pt] + bn_bias_0[channel_pt]; // the first row and column are invalid
			}

			// ReLU
			for (int channel_pt=0; channel_pt<BN_CHANNEL_PARALLELISM; channel_pt++) {
				out_feature_t0[channel_pt] += relu_x_bias[channel_pt];
				if (out_feature_t0[channel_pt] < 0) out_feature_t0[channel_pt] *= relu_weight[channel_pt];
				out_feature_t0[channel_pt] += relu_y_bias[channel_pt];
			}

			// Shortcut
			for (int channel_pt=0; channel_pt<BN_CHANNEL_PARALLELISM; channel_pt++) {
				out_feature_t1[channel_pt] += out_feature_t0[channel_pt];
			}

			// Batch Normalization and Write-back
			for (int channel_pt=0; channel_pt<BN_CHANNEL_PARALLELISM; channel_pt++) {
				residual[channel_tile][channel_pt][i][j] = bn_weight_1[channel_pt]*out_feature_t1[channel_pt] + bn_bias_1[channel_pt];
			}
		}
	}
}

void avgpool_concat(
		float outputs[CHANNEL_OUT/CHANNEL_OUT_T][CHANNEL_OUT_T][WIDTH][WIDTH],
		int H_fmap,
		int in_channels
)
{
	int in_channel_blocks = in_channels/BN_CHANNEL_PARALLELISM;

	float out_feature[BN_CHANNEL_PARALLELISM];
	float out_tmp[BN_CHANNEL_PARALLELISM][WIDTH/2][WIDTH/2];

	for (int i = 0; i < WIDTH/2; i ++){
		for (int j = 0; j < WIDTH/2; j ++){
			for (int channel_pt = 0; channel_pt < BN_CHANNEL_PARALLELISM; channel_pt ++){
				out_tmp[channel_pt][i][j] = 0;
			}
		}
	}
	for (int tile=0; tile < in_channel_blocks; tile ++) {
		for (int i = 0; i < H_fmap; i ++){
			for (int j = 0; j < H_fmap; j ++){
				for (int ii = 0; ii < 2; ii ++){
					for (int jj = 0; jj < 2; jj ++){
						for (int channel_pt = 0; channel_pt < BN_CHANNEL_PARALLELISM; channel_pt ++){
							if (ii + jj == 0) {
								out_feature[channel_pt] = outputs[tile][channel_pt][i*2 + ii][j*2 + jj];;
							} else{
								out_feature[channel_pt] += outputs[tile][channel_pt][i*2 + ii][j*2 + jj];;
							}
						}
						for (int channel_pt = 0; channel_pt < BN_CHANNEL_PARALLELISM; channel_pt ++){
							out_tmp[channel_pt][i][j] = out_feature[channel_pt]/(float)4.0;
						}
					}
				}
			}
		}
		for (int i = 0; i < H_fmap; i ++){
			for (int j = 0; j < H_fmap; j ++){
				for (int channel_pt = 0; channel_pt < BN_CHANNEL_PARALLELISM; channel_pt ++){
					outputs[tile][channel_pt][i][j] = out_tmp[channel_pt][i][j];
					outputs[tile+in_channel_blocks][channel_pt][i][j] = out_tmp[channel_pt][i][j];
				}
			}
		}
	}
}

void avgpool_8x8(
		float inputs[CHANNEL_OUT/CHANNEL_OUT_T][CHANNEL_OUT_T][WIDTH][WIDTH],
		float outputs[CHANNEL_OUT]
)
{
	float tmp[CHANNEL_OUT_T];


	for (int tile = 0; tile < CHANNEL_OUT/OUT_CHANNEL_PARALLELISM; tile ++) {
		for (int k = 0; k < OUT_CHANNEL_PARALLELISM; k ++) {
			tmp[k] = 0;
		}
		for (int i = 0; i < 8; i ++) {
			for (int j = 0; j < 8; j ++) {
				for (int ch = 0; ch < OUT_CHANNEL_PARALLELISM; ch ++) {
					tmp[ch] += inputs[tile][ch][i][j];
				}
			}
		}
		for (int k = 0; k < OUT_CHANNEL_PARALLELISM; k++) {
			outputs[tile*OUT_CHANNEL_PARALLELISM + k] = tmp[k]/(float)64.0;
		}
	}
}

void matmul(
		float inputs[64],
		const float linear_weight[10][64],
		const float linear_bias[10],
		float outputs[10]
)
{

	float buf[10];

	for(int coo = 0; coo < 10; coo ++) {
		buf[coo] = linear_bias[coo];
	}

	for(int cii = 0; cii < 64; cii++) {
		float tmp_in = inputs[cii];
		float tmp_wt[10];
		for(int coo = 0; coo < 10; coo ++) {
			tmp_wt[coo] = linear_weight[coo][cii];
		}
		for(int coo = 0; coo < 10; coo ++) {
			buf[coo] += tmp_in*tmp_wt[coo];
		}
	}
	for(int coo = 0; coo < 10; coo ++) {
		outputs[coo] = buf[coo];
	}
}




#endif /* SRC_LAYER_H_ */

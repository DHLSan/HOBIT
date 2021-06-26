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
#include "platform.h"
#include "xil_printf.h"
#include "compute_engine_64.h"
#include "FracBNN.h"
#include "xil_io.h"
#include "xtime_l.h"

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

void Test_S(unsigned long long b, unsigned long long w){
	unsigned long long m1 = 6148914691236517205;
	unsigned long long m2 = 3689348814741910323;
	unsigned long long m4 = 1085102592571150095;

	char out;

	unsigned long long x = b ^ w;
	x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    x += x >>  8;
    x += x >> 16;
    x += x >> 32;
    x = x & 0x7f;
	out = (char) x;
	printf("%d\n",out);
}

int main()
{
    init_platform();

    /*printf("Custom IP:\n");
    XTime tStart1, tEnd1;
    double ElapsedTime1;

    XTime_GetTime(&tStart1);

    compute_engine_64(0xabcd12345432bafe, 0x123678945467abde);
    compute_engine_64(0xbcdefeab12548765, 0x98058764abfeffff);
    compute_engine_64(0x1000211132224333, 0x9888899976666777);

    XTime_GetTime(&tEnd1);

    printf("Output took %llu clock cycles.\n", 2*(tEnd1-tStart1));
    ElapsedTime1 = 1.0 * (tEnd1 - tStart1) / (COUNTS_PER_SECOND);
    printf("Output took %f s.\n",ElapsedTime1);*/


    printf("ARM CPU:\n");
    XTime tStart2, tEnd2;
    double ElapsedTime2;

    XTime_GetTime(&tStart2);

    Test_S(0xabcd12345432bafe, 0x123678945467abde);
    Test_S(0xbcdefeab12548765, 0x98058764abfeffff);
    Test_S(0x1000211132224333, 0x9888899976666777);

    XTime_GetTime(&tEnd2);

    printf("Output took %llu clock cycles.\n", 2*(tEnd2-tStart2));
    ElapsedTime2 = 1.0 * (tEnd2 - tStart2) / (COUNTS_PER_SECOND);
    printf("Output took %f s.\n",ElapsedTime2);

    cleanup_platform();
    return 0;
}

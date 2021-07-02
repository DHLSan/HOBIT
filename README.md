### HOBIT: Binarized neural network (BNN) hardware acceleration for edge devices
Artificial Neural networks (ANNs) are significant for deep learning systems. Deep neural networks (DNNs) have attracted significant attention for their excellent accuracy especially in areas such as computer vision and pattern recognition; they continue to become more powerful. However, DNN models require higher computational power and more on-chip memory as they become larger. Certain workloads can be offloaded to the cloud to utilize the computational power of the cloud devices. However, networking and communication is a problem from network bandwidth and security perspectives most of the time. Therefore, enabling the devices to do computation is a good solution to relieve the burden on the network infrastructure. However, since the edge devices such as Internet of Things (IoT) devices are small mobile devices, they are power- and resource-constrained which restricts the use of cutting edge DNN models on them. To compress the NN model, binarization is a powerful method which reduces the bit-width of input and weight values from single-precision floating point values to a single bit. This improves the resource usage and reduces the memory and computational requirements to a level that IoT edge devices can run while still offering similar accuracy capabilities of full precision DNN models. FPGAs are a good choice for these applications due to their low power consumption and high flexibility. To be able to accelerate the computation power of BNNs that are used in these IoT edge devices, this project proposes an RTL-level BNN hardware accelerator design on the specific BNN which is named as FracBNN using an FPGA-based programmable SoC platform, ZedBoard. 

University name: Eskisehir Technical University

Students: Samet Duru,  Burak Akkuş

Supervisor: Assist. Prof. Dr. Ismail San

Email of students and Supervisor: sametduru26@gmail.com / burak.akkus9707@gmail.com / isan@eskisehir.edu.tr

The Board Used In Project: ZedBoard

Youtube Link for Demonstration: https://www.youtube.com/watch?v=GhHnXPMCgyU&ab_channel=Ruinex

Welcome to the HOBIT project. This project is based on a Binarized Neural Network(BNN) inference project called FracBNN which you can find in the following link: https://github.com/cornell-zhang/FracBNN 

The aim of that project is to solve a common bottleneck in BNNs which is the input layer that is not binarized thus doing floating point operations. 
In HOBIT, we aimed to take the HLS design in FracBNN and partition it into hardware and software parts via VHDL and C languages respectively.
To be able to run the project you should do the following steps:
1) Import the FracBNN IP core, which is located in Final Design folder, to your block design which should also consist of Processing System, Processor System Reset and AXI interconnect.
2) Generate the bitstream of the design. Check for WNS value which should not be negative. If it is, you should reduce the clock frequency of the PL Fabric Clocks. In our case it runs at 45 MHz.
3) While both block design and implemented design are open, export the hardware to SDK including the bitstream.
4) Now create a project in SDK and import all the C files and Header files in the HOBIT/Final Design/Vivado SDK Files.
5) If you'd like to test the functionality of the HW part of the project, use the helloworld_test.c file instead of helloworld.c file as your main function. It calls both the HW and software implementation of function, assigns values to it and gives back the result. 
6) However, if you'd like to test the whole project you should use the helloworld.c file as your main function. Nevertheless, when we tried to run the whole project in Vivado 2017.4 SDK on ZedBoard we have encountered some memory problems because of arrays with large store areas and large dimensions. We are not entirely sure if it is due to the version of Vivado compiler or lack of memory on ZedBoard. However since we can run the project in DevC++ in purely software manner and the functionality of HW IP core is verified from HW and SW parts, it is certain that the problem is related to memory. Also we don't know whether there might be a setting in Vivado compiler to solve this memory problem. If you are using another board with higher memory or another version of Vivado, you might be able to run the whole project. 


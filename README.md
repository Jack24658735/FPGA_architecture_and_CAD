# FPGA_architecture_and_CAD

* Course number: CS5160 
* Instructor: W.K. Mak, NTHU CS
* Final Project: ***Fixed-Outline Floorplanner for FPGAs with Heterogeneous Resources***

For the directory mentioned by TA, I put all source files and makefile in **src** folder.
So, you need to navigate to **src** folder first.
### 1. How to compile:
* Under this directory, use the “make” command.
* `$ make` <br>
* Then, it will compile my work by g++.

### 2. How to execute (calculate running time also)
* Enter command such as following to execute project file. (Note: I use the “time” program in Linux to calculate my running time)
* `$ time ./project <.arch file> <.module file> <.net file> <.floorplan file>`
  * <.arch file>: the absolute or relative path of input .arch file. <br>
  * <.module file>: the absolute or relative path of input .module file. <br>
  * <.net file>: the absolute or relative path of input .net file. <br>
  * <.floorplan file>: the absolute or relative path of output .floorplan file.



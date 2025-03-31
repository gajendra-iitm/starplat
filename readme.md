# StarPlat
---
### Installation and Usage Instruction for linux/WSL

##### Make sure you have build-essential (latest), bison 3.8.2, gcc 11, g++ 11, flex 2.6.4, libomp-dev
 Use the following command in the debian terminal to install all the required tools
 ``` sudo apt install build-essential gcc g++ bison flex libomp-dev ```

### Making the StarPlat src

Get to the src folder of the repository and run the following command in the terminal
``` make ```

### Compiling the DSL Codes
To Compile the dsl codes to generate OMP specific code use the following command in the terminal
```
./StarPlat [-s|-d] -f <dsl.sp> -b [cuda|omp|mpi|acc|sycl]

#Example
./StarPlat -s -f ../graphcode/staticDSLCodes/triangle_counting_dsl -b omp

-s for static and -d for dynamic
-b select the type of backend
-f the dsl file to input
```
>Once the omp code is generated it would be reflected in generated_omp folder of the graphcode directory in the repository

### Running the OMP code

In the graphcode folder there is a main.cpp file, edit out the dsl file name in the header and modify path for the loading different graphs and then compile and run using g++.
```  
export OMP_NUM_THREADS=16
g++ main.cpp -o main -fopenmp 
./main
```
## Publication
[StarPlat: A versatile DSL for graph analytics](https://www.sciencedirect.com/science/article/pii/S074373152400131X); Nibedita Behera, Ashwina Kumar, Ebenezer Rajadurai T, Sai Nitish, Rajesh Pandian M, Rupesh Nasre; IJPP 2024


## Acknowledgments
```
This project is funded by India's National Supercomputing Mission, whose timely funding and regular reviews are greatly appreciated.
Part of the work is supported by Qualcomm Innovation Fellowship 2023. 
```
## Licensing
The StarPlat codebase is made available for academic and non-commercial use.
StarPlat is patented. Commercial use of the code needs licensing. The interested may contact rupesh@cse.iitm.ac.in.




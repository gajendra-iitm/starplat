#!/bin/bash
#PBS -e errorfile.err
#PBS -o logfile.log
#PBS -l select=1:ncpus=1:ngpus=1
#PBS -q rupesh_gpuq
tpdir=`echo $PBS_JOBID | cut -f 1 -d .`
tempdir=$HOME/scratch/job$tpdir
mkdir -p $tempdir
cd $tempdir
cp -R $PBS_O_WORKDIR/* .
module load gcc920
module load cuda11.4
module load python385
nvcc -o a.out a3.cu
./a.out < input0.txt > output0.txt
mv * $PBS_O_WORKDIR/.
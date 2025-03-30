#!/bin/bash
#PBS -e errorfile.err
#PBS -o logfile.log
#PBS -l walltime=40:00:00
#PBS -l select=1:ngpus=1
#PBS -q rupesh_gpuq

tpdir=`echo $PBS_JOBID | cut -f 1 -d .`
tempdir=$HOME/scratch/job$tpdir
mkdir -p $tempdir
cd $tempdir
cp $PBS_O_WORKDIR/* .

module load cuda11.4
nvcc -arch=sm_70 -o manual_dyn manual_dyn.cu 

./manual_dyn test_13.txt small_13_update_dec.txt  1 7 2  > output_dyn.log

mv * $PBS_O_WORKDIR/.
rmdir $tempdir

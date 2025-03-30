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
nvcc -arch=sm_70 -o static_flow static_flow.cu 

./static_flow /lfs1/usrscratch/phd/cs16d003/11suiteDSL/weightedGraphs/germanyudwt.txt 0 11548844 50000  > output_generated_static_germany.log

mv * $PBS_O_WORKDIR/.
rmdir $tempdir

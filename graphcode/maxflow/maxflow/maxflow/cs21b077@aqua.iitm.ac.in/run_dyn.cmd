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
nvcc -arch=sm_70 -o generated_dyn generated_dyn.cu 

./generated_dyn /lfs1/usrscratch/phd/cs16d003/11suiteDSL/weightedGraphs/germanyudwt.txt   /lfs/usrhome/btech/cs21b077/inputs/update_germanysmall_weighted.txt 0 11548844 5000  > output_generated_dyn_germany.log

mv * $PBS_O_WORKDIR/.
rmdir $tempdir

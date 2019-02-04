#!/bin/csh
module purge
module load slurm/current
module load mrbayes/ompi/gcc/3.2.6
cd Hyoliths
mpirun -n $SLURM_NTASKS mb hyo_hom7_ss.nex
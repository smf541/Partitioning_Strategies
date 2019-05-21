#!/bin/csh

## By default SLURM combines the standard output and error streams in a single
## file based on the jobID and with extension .out
## These streams can be directed to separate files with these two directives

#SBATCH  -o  %j.out
#SBATCH  -e  %j.err

## where SLURM will expand %j to the jobID of the job.



## Request email to the user when certain type of events occur to
## the job

#SBATCH  --mail-type=FAIL

## where <type> can be one of BEGIN, END, FAIL, REQUEUE or ALL,
## and send to email address

#SBATCH  --mail-user  stella.m.felsinger@durham.ac.uk
#SBATCH -t 1-02
##form     hh:mm:ss or d-hh
## The default email name is that of the submitting user as known to the system.


module purge
module load slurm/current

##load any modules required here

module load mrbayes/ompi/gcc/3.2.6


##execute the MPI program

mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.21.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.22.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.23.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.24.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.25.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.26.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.27.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.28.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.29.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.30.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.31.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.32.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.33.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.34.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.35.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.36.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.37.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.38.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.39.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.40.nex


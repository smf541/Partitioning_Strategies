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

mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.81.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.82.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.83.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.84.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.85.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.86.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.87.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.88.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.89.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.90.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.91.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.92.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.93.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.94.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.95.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.96.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.97.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.98.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.99.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZL_random/OZL_random.nex.100.nex


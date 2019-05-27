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

mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.61.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.62.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.63.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.64.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.65.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.66.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.67.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.68.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.69.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.70.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.71.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.72.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.73.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.74.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.75.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.76.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.77.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.78.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.79.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEA_random/CEA_random.nex.80.nex


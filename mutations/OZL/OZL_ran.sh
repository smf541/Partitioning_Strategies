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
#SBATCH -t 03:00:00
##form     hh:mm:ss
##SBATCH --exclude cn7051
## The default email name is that of the submitting user as known to the system.


module purge
module load slurm/current

##load any modules required here

module load mrbayes/ompi/gcc/3.2.6


##execute the MPI program
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZLrandom/OZL_random.nex.1.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZLrandom/OZL_random.nex.2.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZLrandom/OZL_random.nex.3.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZLrandom/OZL_random.nex.4.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateOZL/OZLrandom/OZL_random.nex.5.nex


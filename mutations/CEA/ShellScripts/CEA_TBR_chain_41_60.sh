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
##SBATCH --exclude cn7051
## The default email name is that of the submitting user as known to the system.


module purge
module load slurm/current

##load any modules required here

module load mrbayes/ompi/gcc/3.2.6


##execute the MPI program

mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.41.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.42.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.43.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.44.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.45.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.46.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.47.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.48.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.49.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.50.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.51.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.52.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.53.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.54.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.55.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.56.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.57.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.58.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.59.nex
mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutateCEA/CEATBR_chain/CEA_TBR_chain.nex.60.nex


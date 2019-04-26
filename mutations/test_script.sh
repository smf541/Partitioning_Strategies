#!/bin/csh

##this is a master shell script.
## it will let me call as many SBATCH shell files as I want.

for i in 1:5
do
	sbatch -p par6.q -n 64 OZL_NNI_chain_$i.sh
done
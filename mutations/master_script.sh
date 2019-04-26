#!/bin/csh

##this is a master shell script.
## it will let me call as many SBATCH shell files as I want.

foreach i (1 2 3 4 5)

	sbatch -p par6.q -n 64 OZL_NNI_chain_$i.sh

end
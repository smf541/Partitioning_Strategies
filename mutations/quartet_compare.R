#comparing trees

require(ape)
require(phytools)
require(Quartet)
n_tips <- 14
AllQuartets(n_tips) 

trees <- read.nexus("sco_optimal_tree.nex")

plot(trees)

QStrees <- QuartetStatus(trees) 
#output of QuartetStatus:
#     N = 2Q, total number of quartet statements
#     Q = total number of quartets for n tips
#     s = number of quartets resolved identically in both trees
#     d = noq resolved differently in each tree
#     r1 = noq resolved in tree 1, but not tree 2
#     r2 = noq resolved in tree 2, but not tree 1
#     u = noq that are unresolved in both trees

SimilarityMetrics(QStrees)

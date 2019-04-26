#comparing trees

require(ape)
require(phytools)
require(Quartet)
setwd("C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations/OZL")
startTree <- read.nexus("./Randomized Trees/OZL_TBR_chain19.nex")
conTree1 <- read.nexus("./OZL_TBRch_19.nex.con.tre")
#conTree2 <- read.nexus("./OZL_TBRch_4.nex.tree2.con.tre")
#conTree3 <- read.nexus("./OZL_TBRch_4.nex.tree3.con.tre")
#conTree4 <- read.nexus("./OZL_TBRch_4.nex.tree4.con.tre")
trees <- list(startTree, conTree1)


QStrees <- QuartetStatus(trees) 

#output of QuartetStatus:
#     N = 2Q, total number of quartet statements
#     Q = total number of quartets for n tips
#     s = number of quartets resolved identically in both trees
#     d = noq resolved differently in each tree
#     r1 = noq resolved in tree 1, but not tree 2
#     r2 = noq resolved in tree 2, but not tree 1
#     u = noq that are unresolved in both trees

sim <- SimilarityMetrics(QStrees)
sim
writeClipboard(as.character(sim[2,]))


############################################################################
############################################################################

#compare start tree to ideal tree

idealTree <- read.nexus("./OZL_optimal_tree.nex")
startTree <- read.nexus("./Randomized Trees/OZL_TBR_chain19.nex")
#conTree <- read.nexus("./OZL_TBRch_19.nex.con.tre")
trees <- list(idealTree, startTree)


QStrees <- QuartetStatus(trees) 

#output of QuartetStatus:
#     N = 2Q, total number of quartet statements
#     Q = total number of quartets for n tips
#     s = number of quartets resolved identically in both trees
#     d = noq resolved differently in each tree
#     r1 = noq resolved in tree 1, but not tree 2
#     r2 = noq resolved in tree 2, but not tree 1
#     u = noq that are unresolved in both trees

sim <- SimilarityMetrics(QStrees)
sim
writeClipboard(as.character(sim[2,]))

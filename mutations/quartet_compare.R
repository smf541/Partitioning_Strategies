#comparing trees

require(ape)
require(phytools)
require(Quartet)
setwd("C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations")
startTree <- read.nexus("./CEA/Randomized Trees/CEA_TBR_chain4.nex")
conTree1 <- read.nexus("./CEA/CEA_TBRch_4.nex.tree1.con.tre")
conTree2 <- read.nexus("./CEA/CEA_TBRch_4.nex.tree2.con.tre")
conTree3 <- read.nexus("./CEA/CEA_TBRch_4.nex.tree3.con.tre")
conTree4 <- read.nexus("./CEA/CEA_TBRch_4.nex.tree4.con.tre")
trees <- list(startTree, conTree1, conTree2, conTree3, conTree4)


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

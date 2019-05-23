#to view an optimal tree

library(TreeSearch)
library(ape)

tla <- 'THER'

setwd(paste0('C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations/', tla))


nTips <- 62

inputTree <- read.nexus(paste0(tla, '_optimal_tree1.nex'))
inputTree$edge.length <- NULL
#inputTree$edge.length <- rep(1, nrow(inputTree$edge))
inputLabels <- inputTree$tip.label
plot(inputTree, label.offset = 0.1)

topTree <- rotateConstr(inputTree, rev(inputLabels))
plot(topTree, label.offset = 0.1)

altTree <- read.nexus(paste0(tla, '_optimal_tree2.nex'))
altTree$edge.length <- NULL
#altTree$edge.length <- rep(1, nrow(altTree$edge))
altLabels <- altTree$tip.label
plot(altTree, label.offset = 0.1)


comparePhylo(inputTree, altTree, plot=TRUE)

#show node labels
nodelabels()

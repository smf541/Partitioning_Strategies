#to view an optimal tree

library(TreeSearch)
library(ape)

setwd(paste0('C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations/'))

tla <- 'SYL'
nTips <- 76

inputTree <- read.nexus(paste0(tla, '_optimal_tree_experimental.nex'))
inputTree$edge.length <- NULL
#inputTree$edge.length <- rep(1, nrow(inputTree$edge))
inputLabels <- inputTree$tip.label
plot(inputTree, label.offset = 0.1)
topTree <- rotateConstr(inputTree, rev(inputLabels))
plot(topTree, label.offset = 0.1)

#show node labels
nodelabels()

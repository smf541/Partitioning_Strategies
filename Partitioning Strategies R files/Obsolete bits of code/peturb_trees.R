library(TreeSearch)
library(ape)
setwd('C:/Box/Postgrad/Stella FELSINGER')
File <- function (suffix) paste0(tla, '/', tla, suffix)
Chain <- function (Func, oTree) {
  ret <- vector('list', 100)
  for (i in reps) {
    oTree <- Func(oTree)
    ret[[i]] <- oTree
  }
  structure(ret, class='multiPhylo')
}

tla <- 'OZL'
dir.create(tla)

inputTree <- read.nexus(paste0(tla, '_optimal_tree.NEX'))
inputTree$edge.length <- NULL
inputLabels <- inputTree$tip.label
plot(inputTree)
reps <- seq_len(100)

write.nexus(structure(lapply(reps, function (i) NNI(input_tree)),
                      class='multiPhylo'),
            file=File('_single_NNI_move.nex'))


write.nexus(structure(lapply(reps, function (i) SPR(input_tree)),
                     class='multiPhylo'),
           file=File('_single_SPR_move.nex'))


write.nexus(structure(lapply(reps, function (i) TBR(input_tree)),
                     class='multiPhylo'),
           file=File('_single_TBR_move.nex'))



write.nexus(Chain(NNI, inputTree), file=File('_NNI_chain.nex'))
write.nexus(Chain(SPR, inputTree), file=File('_SPR_chain.nex'))
write.nexus(Chain(TBR, inputTree), file=File('_TBR_chain.nex'))

write.nexus(structure(lapply(reps, function (i)
  ape::rtree(n = length(inputLabels), br=NULL, tip.label = inputLabels)),
  class='multiPhylo'), file=File('_random.nex'))


# Visualize distance of trees in chain from 'best' tree
library(Quartet)
trees <- read.nexus(File('_NNI_chain.nex'))
stati <- lapply(trees, QuartetStatus, cf=inputTree)
plot(vapply(stati, QuartetDivergence, double(1)))

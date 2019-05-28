#calculate consistency indices (CI) for each character over a set of random trees
require(TreeSearch)
require(ape)
require(phangorn)
require(ggplot2)
require(tidyr)

# Select dataset
datasetName <- "SYL"
rootDir <- "C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations/"
setwd(rootDir)
mrBayesTemplateFile <- paste0(rootDir, '/',datasetName,'/', datasetName, '_TEMPLATE.nex')
dataset <- ReadAsPhyDat(mrBayesTemplateFile)


##################################################################################################
############################# make a .nex file with 500 random trees #############################
##################################################################################################
#how many trees to write to file
rep <- seq_len(500)

#functions
File <- function (suffix) paste0('CharStructure/', datasetName, suffix)  ##path and name of the target file

#read optimal tree
inputTree <- read.nexus(paste0(datasetName, '_optimal_tree.nex'))   ###optimal tree is in dir 'mutations', not in dataset subdir
inputTree$edge.length <- NULL
inputLabels <- inputTree$tip.label
inputTree <- multi2di(inputTree)
plot(inputTree)

#write 500 trees to file, located in dir "mutations/CharStructure"
write.nexus(structure(lapply(reps, function (i)
  ape::rtree(n = length(inputLabels), br=NULL, tip.label = inputLabels)),
  class='multiPhylo'), file=File(paste0('_random_',length(rep),'.nex')))



# Visualize distance of trees in chain from 'best' tree
library(Quartet)
trees <- read.nexus(File('_random.nex'))
stati <- lapply(trees, QuartetStatus, cf=inputTree)
plot(vapply(stati, QuartetDivergence, double(1)))



###################################################################################
###### calculate homoplasy for each character, 500 times ##########################
###################################################################################

setwd(paste0(rootDir,datasetName))    #####which wd do I need here? The mrBayes template is already in memory, so don't need to call that


powerOf2 <- 2^(0:ncol(attr(dataset, "contrast"))) #contrast shows the possible permutations of 
                                                  #the character states, i.e. 0, 1, 2, 3, {01}, {02} etc.
decode <- apply(attr(dataset, "contrast"), 1, function(r) 
  sum(powerOf2[as.logical(r)])
)
tab <- t(vapply(dataset, I, dataset[[1]])) # translates lists of taxa and character data into matrix
tab <- tab[, attr(dataset, 'index')]
nChar <- ncol(tab)
chars <- seq_len(nChar)

minSteps <- apply(tab, 2, function(char) 
  TreeSearch:::MinimumSteps(decode[char])
)

CImat <- matrix(data=NA,nrow=length(rep), ncol=nChar) #preallocate a matrix of NAs, with as many rows as trees and as many columns as characters
colnames(CImat) <- chars
#which trees to calculate CI for
trees <- read.nexus(paste0('CharStructure/', datasetName, '_random_',length(rep),'.nex')) # reads all 500 random trees 

#loop through trees, calculating f for all characters for each tree i
for (i in seq_along(trees)) {
  tree <- trees[[i]]
  parsScore <- Fitch(tree, dataset)
  
  #calculate number to go in exp() for branch lengths prior
  expVal <- ncol(tab)/parsScore
  
  obsSteps <- FitchSteps(tree, dataset)
  obsSteps <- obsSteps[attr(dataset, 'index')] #if two characters have the same profile, they are now not collapsed into one
  
  #calculate Goloboff's unbiased measure of homoplasy for a given k (concavity constant) and data set
  k <- 3
  f <- (k+1)/(obsSteps+k+1+minSteps)
  
  #fill ith row with the vector of CIs
  CImat[i, ] <- f     
}

#reshape CImat into long format
CImat <- as.data.frame(CImat)
CImat <- gather(CImat, key="character", value="CI", 1:nChar)
#CImat$facet <- rep(c(1:4),times=50)

###plot results - 

ggplot(CImat, aes(character,CI )) +
#  geom_jitter(alpha=0.3) +
  scale_x_discrete() +
  stat_bin_2d()

ggsave() ###specify the size of the plot and path to it (should end up in dir CharStructure)



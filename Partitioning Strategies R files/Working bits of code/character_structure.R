#calculate consistency indices (CI) for each character over a set of random trees
require(TreeSearch)
require(ape)
require(phangorn)
require(ggplot2)
require(tidyr)

# Select dataset
datasetName <- "SCO"
rootDir <- paste0("C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations/", datasetName)
setwd(rootDir)
mrBayesTemplateFile <- paste0(rootDir, '/', datasetName, '_TEMPLATE.nex')
dataset <- ReadAsPhyDat(mrBayesTemplateFile)


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

CImat <- matrix(, nrow=500, ncol=nChar)
colnames(CImat) <- chars
#which trees to calculate CI for
trees <- read.nexus(paste0('StartingTrees/', datasetName, '_random500.nex')) # reads all 500 random trees 
                            
for (i in seq_along(trees)) {
  tree <- trees[[i]]
  parsScore <- Fitch(tree, dataset)
  
  #calculate number to go in exp() for branch lengths prior
  expVal <- ncol(tab)/parsScore
  
  obsSteps <- FitchSteps(tree, dataset)
  
  #calculate Goloboff's unbiased measure of homoplasy for a given k (concavity constant) and data set
  k <- 3
  f <- (k+1)/(obsSteps+k+1+minSteps)
  
  CImat[i, ] <- f     #fill ith row with the vector of CIs
}  ###################################################### doesn't work because obsSteps and minSteps are different lengths. 
############################################# obsSteps is calculated directly from the tree (from read.nexus) while minSteps
############################################# is calculated from tab < dataset < ReadAsPhyDat. 
############################################# obsSteps has 26 elements, while minSteps has 27 

#reshape CImat into long format
CImat <- as.data.frame(CImat)
CImat <- gather(CImat, key="character", value="CI", 1:nChar)
#CImat$facet <- rep(c(1:4),times=50)

###plot results - 

ggplot(CImat, aes(character,CI )) +
#  geom_jitter(alpha=0.3) +
  scale_x_discrete() +
  stat_bin_2d()



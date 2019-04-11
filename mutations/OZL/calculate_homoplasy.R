#calculate homoplasy indices from starting trees
require(TreeSearch)
require(ape)
require(phangorn)
setwd('C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations/OZL/Randomized Trees')

file <- 'OZL_TBR_chain100.nex'
tree <- read.nexus(file)
data <- ReadAsPhyDat(file)

parsScore <- Fitch(tree, data)
print(parsScore)

powerOf2 <- 2^(0:ncol(attr(data, "contrast"))) #contrast shows the possible permutations of 
    #the character states, i.e. 0, 1, 2, 3, 4, 5, {01}, {02} etc.

decode <- apply(attr(data, "contrast"), 1, function(r) #
  sum(powerOf2[as.logical(r)])
  )

tab <- t(vapply(data, I, data[[1]])) # translates lists of taxa and character data into matrix
#calculate number to go in exp() for branch lengths prior
expVal <- ncol(tab)/parsScore

minSteps <- apply(tab, 2, function(char) 
  TreeSearch:::MinimumSteps(decode[char])
  )

obsSteps <- FitchSteps(tree, data)
 



#calculate Goloboff's unbiased measure of homoplasy for a given k (concavity constant) and data set

k <- 3
f <- (k+1)/(obsSteps+k+1+minSteps)
f




#rank characters by homoplasy values and then divide equally into a number of partitions

chars <- 1:ncol(tab)

mat <- rbind(chars, f) #combine char no. and homoplasy value into one matrix
sortedMat <- mat[,order(f)] #sort columns by homoplasy (f) in ascending order

nPart <- 4 #number of partitions
prop <- 1/nPart # proportion of characters per partition
chunk <- round(prop*ncol(tab)) # number of characters per partition

partA <- t(sortedMat[1, 1:chunk])
partB <- sortedMat[1, (chunk+1) : (2*chunk)]
partC <- sortedMat[1, (2*chunk+1):(3*chunk)]
partD <- sortedMat[1, (3*chunk +1) : ncol(sortedMat)]

writeClipboard(as.character(partD))


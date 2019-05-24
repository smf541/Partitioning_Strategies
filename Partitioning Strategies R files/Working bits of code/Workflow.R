#calculate homoplasy indices from starting trees
require(TreeSearch)
require(ape)
require(phangorn)

# Define constants
nPart <- 4 #number of partitions
prop <- 1/nPart # proportion of characters per partition
insertionComment <- "INSERT PARTITIONS HERE"
bayesFilesDir <- 'MrBayes'


# Select dataset
datasetName <- "OZL"
rootDir <- paste0("C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations/", datasetName)
setwd(rootDir)
mrBayesTemplateFile <- paste0(rootDir, '/', datasetName, '_TEMPLATE.nex')
dataset <- ReadAsPhyDat(mrBayesTemplateFile)

# Mr Bayes template
mrBayesTemplate <- readLines(mrBayesTemplateFile)
insertLine <- grep(insertionComment, mrBayesTemplate)
if (!dir.exists(bayesFilesDir)) dir.create(bayesFilesDir)

powerOf2 <- 2^(0:ncol(attr(dataset, "contrast"))) #contrast shows the possible permutations of 
#the character states, i.e. 0, 1, 2, 3, 4, 5, {01}, {02} etc.
decode <- apply(attr(dataset, "contrast"), 1, function(r) #
  sum(powerOf2[as.logical(r)])
)
tab <- t(vapply(dataset, I, dataset[[1]])) # translates lists of taxa and character data into matrix
tab <- tab[, attr(dataset, 'index')]
nChar <- ncol(tab)
chars <- seq_len(nChar)

minSteps <- apply(tab, 2, function(char) 
  TreeSearch:::MinimumSteps(decode[char])
)

#make MrBayes files from perturbed trees
for (ourFile in list.files('StartingTrees', pattern='*.nex')) {
  trees <- read.nexus(paste0('StartingTrees/', ourFile))
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

    
    #rank characters by homoplasy values and then divide equally into a number of partitions
    mat <- rbind(chars, f) #combine char no. and homoplasy value into one matrix
    sortedMat <- mat[, order(f)] #sort columns by homoplasy (f) in ascending order
    chunk <- round(prop * nChar) # number of characters per partition
    
    partA <- sortedMat[1, 1:chunk]
    partB <- sortedMat[1, (chunk+1) : (2*chunk)]
    partC <- sortedMat[1, (2*chunk+1):(3*chunk)]
    partD <- sortedMat[1, (3*chunk +1) : nChar]
    
    mrBayesOutput <- c(mrBayesTemplate[seq_len(insertLine - 1)], 
                       paste("prset brlenspr = unconstrained: exp(",expVal, ");"),
                       paste("charset partA =", paste(partA, collapse=' '), ";"),
                       paste("charset partB =", paste(partB, collapse=' '), ";"),
                       paste("charset partC =", paste(partC, collapse=' '), ";"),
                       paste("charset partD =", paste(partD, collapse=' '), ";"),
                       "",
                       "partition chartype=4: partA, partB, partC, partD;",
                       "set partition=chartype;",
                       "",
                       mrBayesTemplate[(insertLine + 1L):length(mrBayesTemplate)])
    
    outputFile <- paste0(bayesFilesDir, '/', ourFile, '.', i, '.nex')
    writeLines(mrBayesOutput, outputFile)
  }
}











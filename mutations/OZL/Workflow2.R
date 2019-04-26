#comparing trees
require(ape)
require(phytools)
require(Quartet)
require(phangorn)
require(TreeSearch)


# Define constants
nPart <- 4 #number of partitions
prop <- 1/nPart # proportion of characters per partition
insertionComment <- "INSERT PARTITIONS HERE"
bayesFilesDir <- 'MrBayes'
outputDir <- 'Results'
mrBayesTemplateFile <- paste0(rootDir, '/', datasetName, '_TEMPLATE.nex')

# Select dataset
datasetName <- "OZL"
rootDir <- paste0("C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations/", datasetName)
setwd(rootDir)
if (!dir.exists('Results')) dir.create('Results')
idealTree <- read.nexus(paste0("./",datasetName, "_optimal_tree.nex"))
dataset <- ReadAsPhyDat(mrBayesTemplateFile)

#for (ourFile in list.files('StartingTrees', pattern='*.nex')) 
ourFile <- "OZL_random.nex"
  startTrees <- read.nexus(paste0("StartingTrees/", ourFile))
  #resultTrees <- lapply(seq_along(startTrees), function (i)         #####works only if there are 100 con.tre's
  #                      read.nexus(paste0(rootDir,"/",bayesFilesDir, "/", ourFile, '.', i,".nex.con.tre")))
  resultTrees <- lapply(c(1:20), function(i)
                        read.nexus(paste0(rootDir,"/",bayesFilesDir, "/", ourFile, '.', i,".nex.con.tre")))
  
  
  QStatuses <- vapply(
    #seq_along(startTrees), 
    c(1:20),
    function(i)
    QuartetStatus(startTrees[[i]], resultTrees[[i]]),
    c(N=0, Q=0, s=0, d=0, r1=0, r2=0, u=0)
  )
  
  startSim <- SimilarityMetrics(t(QStatuses))
  write.csv(startSim, file=paste0('Results/', ourFile, '.start-sim.csv'))
  
  idealStatuses <- QuartetStatus(resultTrees, cf=idealTree)
  write.csv(SimilarityMetrics(idealStatuses), file=paste0('Results/', ourFile, '.ideal-sim.csv'))

  startToIdealStatuses <- QuartetStatus(startTrees, cf=idealTree)
  write.csv(SimilarityMetrics(startToIdealStatuses), file=paste0('Results/', ourFile, '.start-to-ideal-sim.csv'))
#}

#output of QuartetStatus:
#     N = 2Q, total number of quartet statements
#     Q = total number of quartets for n tips
#     s = number of quartets resolved identically in both trees
#     d = noq resolved differently in each tree
#     r1 = noq resolved in tree 1, but not tree 2
#     r2 = noq resolved in tree 2, but not tree 1
#     u = noq that are unresolved in both trees

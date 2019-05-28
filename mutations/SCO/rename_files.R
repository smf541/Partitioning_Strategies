# rename files to consistent format
require(stringr)

rootDir <-"C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations"
inDir <- "SCO"
setwd(paste0(rootDir,'/', inDir))
newDir <- "MrBayesRenamed"
if (!dir.exists(newDir)) dir.create(newDir)

#list filenames
oldNames <- list.files("MrBayes",full.names=TRUE)

#construct new filenames
pattern <- '_varExpVal'
newNames <- str_replace(oldNames,pattern, '')  ##remove the _varExpVal

#rename files
file.rename(from=oldNames, to=newNames)


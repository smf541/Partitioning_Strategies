#Bayes Factor plots

#packages
require(readxl)
require(ggplot2)
require(tidyr)
require(stringr)
require(readr)
require(utils)

#first i need to extract the marginal likelihoods from the .out files 
      # use regular expressions to find all occurrences of the ML block 
      # match with the number of the analysis (i.e. 54 for SYL_NNI_chain.nex.54.nex)
            #use below strategies to get number of analysis out of the string

  #can use strsplit() to split a string at a certain symbol (parameter). 
  #this returns a list. to choose only one of the elements:
    #wrap in unlist() to turn the list into a vector
    #index with [] to return only the wanted element
    #example:
          # > s = "TGAS_1121"
          # > s1 = unlist(strsplit(s, split='_', fixed=TRUE))[2]
          # > s1    
        # [1] "1121"

    #if s is a vector:
          # > s <- c("TGAS_1121", "MGAS_1432")
          # > s1 <- sapply(strsplit(s, split='_', fixed=TRUE), function(x) (x[2]))
          # > s1
        # [1] "1121" "1432"

  #tidyverse solution:
          # R> library(stringr)
          # R> strings = c("TGAS_1121", "MGAS_1432", "ATGAS_1121") 
          # R> strings %>% str_replace(".*_", "_")
        # [1] "_1121" "_1432" "_1121"
          # # Or:
          # R> strings %>% str_replace("^[A-Z]*", "")
        # [1] "_1121" "_1432" "_1121"

  #gsub() solution: also works if s is a vector
          # s <- "ATGAS_1121"
          # gsub("^.*?_","_",s)
        # [1] "_1121"
  


#set dataset and perturbation move
dataSet <- "SCO"              #CEA, OZL, SCO, SYL, THER
perturbMove <- "NNI_chain"       #random, NNI_chain, TBR_chain

#read in .out file 
rootDir <-"C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations"
setwd(paste0(rootDir, '/', dataSet, '/', 'MrBayesExperimental'))

#preallocate matrix of arithmetic means of MLs
amMatrix <- matrix(data=NA,nrow=100, ncol=2)
amMatrix[,1] <- seq_len(100)

for (ourFile in list.files(pattern=paste0(perturbMove,'.+lstat'))) {     #e.g. SCO_NNI_chain.nex.1.nex.lstat    paste0(dataSet, '_', perturbMove, '.+lstat')
  outFile <- read_tsv(ourFile,skip=1)   ## reads the data into a tibble of dim nrow=9, ncol=4
  ##the number I want is the arithmetic mean of the harmonic means
  #calculate arithmetic mean of harmonic means
  ariMean <- mean(outFile[['harmonic_mean']])
  
  
} 

#to go into the for loop
ourFile <- 'SCO_NNI_chain.nex.1.nex.lstat'
  outFile <- read_tsv(ourFile,skip=1)
  
  #calculate arithmetic mean of harmonic means
  ariMean <- mean(outFile[['harmonic_mean']])
  
  


  
#next step: plot ratio ML of each result tree against ML of result tree generated from parsimony (published) tree
        #this will tell us if model fit is impacted even if the tree that is used for weighting is not ideal!
        #we already know that the ability of MC3 to find a good tree is not impacted negatively 



































#relics from TreeSimilarity.R
      
      # #read in Bayes Factor data
      # BFfile <- "TreeComparisonsBayesFactors.xlsx"
      # BFsheet <- "OZL_TBRch"
      # BFdata <- read_excel(BFfile, BFsheet, col_names = TRUE)
      # BF <- BFdata[nrow(BFdata) ,]
      # BF <- gather(BF, 'start tree', 'Bayes Factor', "1":"20")
      # BF <- BF[,complete.cases(t(BF))] #get rid of any NA columns
      # 
      
      # BFplot <-ggplot(data = BF) +
      #   geom_point(aes(x=BF$`start tree`, y=BF$`Bayes Factor`)) +
      #   scale_y_continuous(name = "Bayes Factor", limits = c(0,max(BF$`Bayes Factor`)))+
      #   scale_x_discrete(name="Distance from start tree", limits=c(1:20)) 
      # BFplot

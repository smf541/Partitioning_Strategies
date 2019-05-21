#making a shell script

datasetName <- "OZL"
rootDir <- paste0("C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations/", datasetName)
setwd(rootDir)
shellTemplateFile <- '../shell_TEMPLATE.sh.txt'
shellTemplate <- readLines(shellTemplateFile)
shellDir <- "ShellScripts"
if (!dir.exists(shellDir)) dir.create(shellDir)
  
m <- "NNI_chain"    ##set tree generation method

## to make a shell file for each analysis

j <- 1

  for (j in c(1:100)) {
    
    #write to shell file

shellOutput <- c(shellTemplate, 
                   
                   paste0('mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutate',datasetName,
                            '/',datasetName,i,'/',datasetName,'_',m,'.nex.',j,'.nex'),  ##SBATCH line
                   ""
                   )

shellOutputFile <- paste0(shellDir, '/', datasetName, '_',m,'_', j, '.sh')  ##name of .sh file
writeLines(shellOutput, shellOutputFile)
    
  }

  

################################################################################################
################################################################################################

# to make one shell file for running a given number of analyses

# for each k in 1:20 
# append to vector lines 
#   paste0('mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutate',datasetName,
#   '/',datasetName,i,'/',datasetName,'_',i,'.nex.',k,'.nex')

########### slurm expects \n linebreaks, not \r\n? I have to get these in during the for loop, i.e. elements of
      # the vector lines have to be separated by \n. Adding \n into the paste0() call produces an empty line 
      # after each slurm call line, but i think upon transferring files to hamilton through WinSCP these get
      # turned to \r\n. 
      # Setting sep="\n" in writeLines() or cat() does nothing.

chunk1 <- c(1:20)
chunk2 <- c(21:40)
chunk3 <- c(41:60)
chunk4 <- c(61:80)
chunk5 <- c(81:100)
chunks <- list(chunk1, chunk2, chunk3, chunk4, chunk5)

for (howManyTrees in chunks) {
  lines <- character(length(howManyTrees))
  for (k in 1:length(howManyTrees)) {
    lines[k] <- paste0('mpirun -n $SLURM_NTASKS mb /ddn/data/dxsb43/mutate',datasetName,
              '/',datasetName,'_',m,'/',datasetName,'_',m,'.nex.',howManyTrees[k],'.nex')
  }
  
  shellOutput <- c(shellTemplate, 
                   lines,  
                   ""
  )
  
  shellOutputFile <- paste0(shellDir, '/', datasetName, '_',m,'_', min(howManyTrees), '_', max(howManyTrees),
                            '.sh')  ##name of .sh file
  #cat(shellOutput, file=shellOutputFile, sep="\n")
  writeLines(shellOutput, shellOutputFile, sep="\n")
  
}
  


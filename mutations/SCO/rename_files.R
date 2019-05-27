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










#function from stackoverflow
umxRenameFile <- function(baseFolder = "Finder", findStr = NA, replaceStr = NA, listPattern = NA, test = T, overwrite = F) {
  # uppercase = u$1
  if(baseFolder == "Finder"){
    baseFolder = system(intern = T, "osascript -e 'tell application \"Finder\" to get the POSIX path of (target of front window as alias)'")
    message("Using front-most Finder window:", baseFolder)
  } else if(baseFolder == "") {
    baseFolder = paste(dirname(file.choose(new = FALSE)), "/", sep = "") ## choose a directory
    message("Using selected folder:", baseFolder)
  }
  if(is.na(listPattern)){
    listPattern = findStr
  }
  a = list.files(baseFolder, pattern = listPattern)
  message("found ", length(a), " possible files")
  changed = 0
  for (fn in a) {
    findB = grepl(pattern = findStr, fn) # returns 1 if found
    if(findB){
      fnew = gsub(findStr, replace = replaceStr, fn) # replace all instances
      if(test){
        message("would change ", fn, " to ", fnew)  
      } else {
        if((!overwrite) & file.exists(paste(baseFolder, fnew, sep = ""))){
          message("renaming ", fn, "to", fnew, "failed as already exists. To overwrite set T")
        } else {
          file.rename(paste(baseFolder, fn, sep = ""), paste(baseFolder, fnew, sep = ""))
          changed = changed + 1;
        }
      }
    }else{
      if(test){
        # message(paste("bad file",fn))
      }
    }
  }
  message("changed ", changed)
}
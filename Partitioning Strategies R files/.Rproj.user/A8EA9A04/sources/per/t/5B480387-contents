#these are the working bits from HyoChars.R
#meant to take dataset and partitioning strategy as input
#find partitions as factor levels for a given partition
#then find vectors of character numbers for each partition


#read in data
input.dataset <- "./HyolithCharacters.csv" #***change file to needed data set in .csv format***

data.set <- read.csv(input.dataset)


#get partition names
partition.levels <- levels(data.set$body.region)  #***change $column name***

#get vector of character no. for each partition name

get.char.vector <- function(level) {
  data.set$char.no[data.set$body.region==level] #***change $column name***
}


#output as array
char.vector.array <- as.array(sapply(partition.levels, 
       FUN=get.char.vector))


#print results
print(char.vector.array[8])



#histogram of partition sizes
#vector of partition names and no. of characters in each
part.lengths <- vapply(char.vector.array, length, FUN.VALUE=numeric(1))
#plot dotplot
plot(part.lengths)

str(part.lengths)

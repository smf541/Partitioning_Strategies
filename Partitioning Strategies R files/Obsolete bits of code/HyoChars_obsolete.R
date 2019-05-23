





#input data
input.dataset <- "./HyolithCharacters.csv"
data.set <- read.csv(input.dataset)


#get partition names



#get vector of character no. for each partition name
get.char.vector <- function(partition) {
  partition.levels <- levels(data.set$partition)
  sapply(partition.levels, 
         FUN=function(level) {
           data.set$char.no[data.set$partition==level]
         })
}

#test
get.char.vector()

part1 <- "body.region"
partition.levels.1 <- levels(get(paste0("data.set$", part1)))

get("data.set$body.region")

#try out get()
tryget <- function(df) {
  get(df)
}
tryget("data.set[8]")


## doesn't work below

#more abstracted
get.char.vector2 <- function(level, part.strat) { #level == which partition's vector I want
  data.set$char.no[data.set$part.strat==level]    #part.strat == the partitioning strategy the level is from
}
get.char.vector2("transformational", part.strat = "neomorphic.transformational")

is(data.set$body.region)


get.levels <- function(part.strat) {
  partition.levels <- levels(data.set$part.strat)
}

body.region.levels <- get.levels(body.region)

#^^doesn't work - i need to specify the partitioning strategy column but if I spec it as a string, it isn't interpreted right.
# if i spec it without "", it is assumed to be an object, which then doesn't exist.



#second try
# 1.  input=part.strat
#     output=partition.levels
# 2.  input=partition.levels
#     output=array of char.no vectors

require(dplyr)


get.levels.2 <- function(part.strat) {
  column.name <- as.name(paste0("hyochars$", part.strat))
  levels(column.name)
}

get.levels.2("body.region")




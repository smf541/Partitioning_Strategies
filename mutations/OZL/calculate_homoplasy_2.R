#calculate homoplasy indices from starting trees
require(TreeSearch)
require(ape)
require(phangorn)

dataset <- "CEA"
start.tree.number <- "12"
start.tree.method <- "_TBR_chain"


setwd(paste0("C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations/", dataset))



file <- paste0(dataset, start.tree.method,start.tree.number,".nex")
tree <- read.nexus(file)
data <- ReadAsPhyDat(file)
#df <- as.data.frame(data)

#calculate parsimony score
parsScore <- Fitch(tree, data)
#print(parsScore)

powerOf2 <- 2^(0:ncol(attr(data, "contrast"))) #contrast shows the possible permutations of 
    #the character states, i.e. 0, 1, 2, 3, 4, 5, {01}, {02} etc.

decode <- apply(attr(data, "contrast"), 1, function(r) #
  sum(powerOf2[as.logical(r)])
  )

tab <- t(vapply(data, I, data[[1]])) # translates lists of taxa and character data into matrix
tab <- tab[, attr(data, 'index')]
                                                              

#calculate number to go in exp() for branch lengths prior
expVal <- ncol(tab)/parsScore

minSteps <- apply(tab, 2, function(char) 
  TreeSearch:::MinimumSteps(decode[char])
  )

obsSteps <- FitchSteps(tree, data)
 

#calculate Goloboff's unbiased measure of homoplasy for a given k (concavity constant) and data set

k <- 3
f <- (k+1)/(obsSteps+k+1+minSteps)
#f


#rank characters by homoplasy values and then divide equally into a number of partitions

chars <- 1:ncol(tab)

mat <- rbind(chars, f) #combine char no. and homoplasy value into one matrix
sortedMat <- mat[,order(f)] #sort columns by homoplasy (f) in ascending order

nPart <- 4 #number of partitions
prop <- 1/nPart # proportion of characters per partition
chunk <- round(prop*ncol(tab)) # number of characters per partition

partA <- sortedMat[1, 1:chunk]
partB <- sortedMat[1, (chunk+1) : (2*chunk)]
partC <- sortedMat[1, (2*chunk+1):(3*chunk)]
partD <- sortedMat[1, (3*chunk +1) : ncol(sortedMat)]

writeClipboard(as.character(partD))



# chunk of .nex file that partitions go into:
#
#   charset partA =	62	144	191	59	195	183	197	114	141	160 199	1165	169	173	17	5	7	32	38	55	56	67	;
#   
#   charset partB = 	72	76	91	95	97	70	92	100	107	108	115	140	149	15	23	28	30	31	33	34	39	;
#   
#   charset partC = 	43	54	58	71	75	78	83	86	89	105	109	111	112	125	127	129	130	133	134	136	145	;
#   
#   charset partD = 	147	157	158	166	176	104	110	116	117	121	122	123	80	154	177	189	204	46	131	132	172 ;
#   













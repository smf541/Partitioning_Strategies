#generate plots of tree similarity. 
library(readxl)
library(ggplot2)
library(tidyr)


# plot similarity of result tree to starting tree on y against number# of starting tree (TBRch_#)

#read in excel file 
setwd("C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations")
file <- "TreeComparisonsQuartet.xlsx"
sheet <- "CEA"
data <- read_excel(file, sheet = sheet, skip=2, col_names=TRUE)

data.no.na <- data[complete.cases(data), ] #removes rows with any na
#data.no.contre <- data.no.na[,c(1,2,4,5,6,7,8,9,10,11)] #removes column with consensus tree number


#convert data from wide to long
data.no.na$`Start Tree` <- factor(data.no.na$`Start Tree`)
clean.data <- gather(data.no.na, Metric, Similarity, DoNotConflict:QuartetDivergence)
clean.data$Similarity <- as.numeric(clean.data$Similarity)
clean.data$Similarity <- round(clean.data$Similarity, 2)
#clean.data$`Start Tree` <- as.numeric(clean.data$`Start Tree`)

# plot similarity against distance from parsimony tree (=no. of starting tree)

ggplot(data=clean.data) +
  geom_point(aes(x=clean.data$`Start Tree`, clean.data$Similarity)) +
  facet_wrap(~ Metric) +
  scale_y_continuous(name='Metric', limits = c(0,1)) +
  scale_x_discrete(name = 'Distance from "ideal" tree',breaks = c(1,10,50,100)) +
  ggtitle('CEA Similarity/Difference Metrics')
ggsave(filename="CEA_similarity.pdf", path="C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations")

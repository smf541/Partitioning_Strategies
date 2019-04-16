#generate plots of tree similarity. 
library(readxl)
library(ggplot2)
library(tidyr)


# plot similarity of result tree to starting tree on y against number# of starting tree (TBRch_#)

#read in excel file 
setwd("C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations")
file <- "TreeComparisonsQuartet.xlsx"
sheet <- "CEA_ran"
data <- read_excel(file, sheet = sheet, skip=0, col_names=TRUE)

data.no.na <- data[complete.cases(data), ] #removes rows with any na
#data.no.contre <- data.no.na[,c(1,2,4,5,6,7,8,9,10,11)] #removes column with consensus tree number


#convert data from wide to long
data.no.na$`start tree` <- factor(data.no.na$`start tree`)
clean.data <- gather(data.no.na, Metric, Similarity, DoNotConflict:QuartetDivergence)
clean.data$Similarity <- as.numeric(clean.data$Similarity)
clean.data$Similarity <- round(clean.data$Similarity, 2)
by.metric.data <- spread(clean.data, Metric, Similarity)

# plot similarity against distance from parsimony tree (=no. of starting tree)

ggplot(data=by.metric.data) +
  geom_point(aes(x=`start tree`, y=QuartetDivergence)) + # aes(colour=`type of tree`)
#  facet_wrap(~ Metric) +
  scale_y_continuous(name='Similarity', limits = c(0,1)) +
  scale_x_discrete(name = 'Distance from published tree',breaks = c(1,10,50,100)) +
  ggtitle('OZL Similarity of Bayesian result tree to parsimony start tree')
ggsave(filename="OZL_random_similarity.pdf", path="C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations")

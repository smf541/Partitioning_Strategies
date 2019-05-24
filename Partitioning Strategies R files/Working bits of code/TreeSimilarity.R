#generate plots of tree similarity. 
require(readxl)
require(ggplot2)
require(tidyr)
require(utils)


# plot similarity of result tree to starting tree on y against number# of starting tree (TBRch_#)

#set dataset and type of baseline tree (ideal or start tree)
dataSet <- "SYL"              #CEA, OZL, SCO, SYL, THER
perturbMove <- "random"       #random, NNI_chain, TBR_chain
baseTree <- "published"           #published (=optimal), start (=tree that homoplasy was calculated based on)

#read in excel file 
rootDir <-"C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations"
setwd(paste0(rootDir, '/', dataSet, '/', 'Results'))
file <- paste0(dataSet, '_', perturbMove, '.nex.', baseTree, '-sim.csv')  #e.g. SYL_random.nex.ideal-sim.csv
data <- read.csv(file, skip=0, header=TRUE)


StartTree <- data$X
QuartetDivergence <- data$QuartetDivergence
QD <- data.frame(StartTree, QuartetDivergence)


#convert data from wide to long
QD$StartTree <- factor(QD$StartTree)
QD$QuartetDivergence <- as.numeric(QD$QuartetDivergence)
QD$QuartetDivergence <- round(QD$QuartetDivergence, 2)


# plot similarity against distance from parsimony tree (=no. of starting tree)

ggplot(data=QD) +
  geom_point(aes(x=StartTree, y=QuartetDivergence)) + # aes(colour=`type of tree`)
#  facet_wrap(~ Metric) +
  scale_y_continuous(name='Quartet Divergence', limits = c(0,1)) +
  scale_x_discrete(name = paste0('Distance from ', baseTree, 'tree'),breaks = c(1,10,20,30,40,50,60,70,80,90,100), limits=c(1:100)) +
  ggtitle(paste0(dataSet,' Similarity of Bayesian result tree to ', baseTree, ' tree')) #+
  #geom_smooth(method = 'lm', mapping = aes(x=`start tree`, y=QuartetDivergence))

#breaks = where ticks go
#limits in discrete scale = which data points are plotted
#limits in continuous scale = first and last data point to plot

ggsave(filename=paste0(dataSet,'_',perturbMove,'_QD_vs_', baseTree,'.pdf'), path="C:/local/dxsb43/GitHub/Partitioning_Strategies/mutations/SimPlots")


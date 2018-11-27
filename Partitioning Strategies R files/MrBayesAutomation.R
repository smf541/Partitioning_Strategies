#on windows:
setwd("C:\\Users\\dxsb43\\GitHub\\NexusFiles\\ss_outputs")

#parse .nex.ss file
make.ss.df <- function(n) {
  read.table(n, header=TRUE, skip=6)
}
  

#function for checking the ss data frame
check.ss <- function(x) {
  if (x$aSplit0[40] > -1) {
    print("Runs have not converged")
  } 
  else if (any(sign(df[,3:6]) > -1)) {
    print("Positive log likelihood found")
  }
  else {
    print("All good!")
  }
}

#function for summing run columns
sum.ln.ml <- function(x) {
  apply(df[,3:6], 2, sum)
}

#function for finding mean ln marginal likelihood
mean.ln.ml <- function(x) {
  mean(sum.ln.ml(x))
}

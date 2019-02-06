require('stringr')
require('readr')
syll_unpart <- read_file(file="C:/Users/dxsb43/GitHub/Partitioning_Strategies/tests/syll/syll_unpart_ss.nex.run8.t")

#regular expression to change any branch length after : to 1

test <- "gen.0 = [&U] ((45:2.000000e-02,(55:2.000000e-02,(((61:2.000000e-02,40:2.000000e-02):2.000000e-02,30:2.000000e-02):2.000000e-02,(((62:2.000000e-02,20:2.000000e-02):2.000000e-02,(57:2.000000e-02,(51:2.000000e-02,10:2.000000e-02):2.000000e-02):2.000000e-02):2.000000e-02"
result1 <- str_replace_all(syll_unpart, ":.{12},", ":1,")
result2 <- str_replace_all(result1, ":.{12}\\)", ":1)")
write_file(result2, path="C:/Users/dxsb43/GitHub/Partitioning_Strategies/tests/syll/syll_unpart_brlens1.nex.run8.t")

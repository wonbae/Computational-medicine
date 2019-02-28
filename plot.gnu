set term png
set output "result/result0.png"
set xrange[-80.000000:80.000000]
set yrange[-1.000000:1.000000]
plot 'result/result0.png' using 1:2 with lines 

CONTRIBUTIONS

TODO: write a brief summary of how each team member contributed to
the project.

REPORT

TODO: add your report according to the instructions in the
"Experiments and analysis" section of the assignment description.

Wrote 16777216 bytes to '/tmp/srincon4/data_16M.in'
Test run with threshold 2097152

real    0m0.453s
user    0m0.442s
sys     0m0.008s
Test run with threshold 1048576

real    0m0.260s
user    0m0.445s
sys     0m0.022s
Test run with threshold 524288

real    0m0.187s
user    0m0.478s
sys     0m0.037s
Test run with threshold 262144

real    0m0.155s
user    0m0.550s
sys     0m0.059s
Test run with threshold 131072

real    0m0.164s
user    0m0.555s
sys     0m0.089s
Test run with threshold 65536

real    0m0.163s
user    0m0.585s
sys     0m0.100s
Test run with threshold 32768

real    0m0.163s
user    0m0.599s
sys     0m0.129s
Test run with threshold 16384

real    0m0.173s
user    0m0.613s
sys     0m0.178s

Analysis:

1.
As expected we saw a decrease in the real time as thresholds decreased until the threshold value of 131072 where it leved out.
This occured because at that point there are no more CPUS available

2.
Qsort and merge are able to run in parrallel to each other in this code and therefore be faster. The recursive nature of
that merge algorithm, it lends itself useful to this technique.
#!/bin/bash

rm test_rooutil_timing.log

N_TRIES=10

root -l -b -q rooutil/examples/TimingTest.C++\(\"nts.ntuple.v6.root\"\)
echo "n_event:time_per_event_ns:event_loop_time_ns:setup_time_ns:total_time_ns" > test_rooutil_timing.log
for i in `seq 1 ${N_TRIES}`; do
    root -l -b -q 'rooutil/examples/TimingTest.C("nts.ntuple.v6.root",true)'
done

root -l 'validation/plot_rooutil_timing.C'

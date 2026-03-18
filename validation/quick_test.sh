#
# quick_test.sh - runs a quick test on a recent mock dataset
#

log_file="quick_test.log"
rm ${log_file}

dataset="mcs.mu2e.ensembleMDS3aOnSpillTriggered.MDC2025af_best_v1_3.art"
if [ ! -f $dataset.list ]; then
    echo "File list for $dataset doesn't exist. Creating..."
    setup mu2efiletools
    mu2eDatasetFileList $dataset > $dataset.list
fi

# Run EventNtuple
echo -n "from_mcs-mockdata.fcl... "
echo "mu2e -c EventNtuple/fcl/from_mcs-mockdata.fcl -S $dataset.list --TFileName nts.ntuple.mock.root -n 100" > ${log_file} 2>&1
mu2e -c EventNtuple/fcl/from_mcs-mockdata.fcl -S $dataset.list --TFileName nts.ntuple.mock.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

# Generate ROOT histograms in a ROOT file
echo -n "creating histograms... "
echo "root -l -b -q EventNtuple/validation/create_val_file_rooutil.C\(\"nts.ntuple.mock.root\",\"val.ntuple.mock.root\"\)" >> ${log_file} 2>&1
root -l -b -q EventNtuple/validation/create_val_file_rooutil.C\(\"nts.ntuple.mock.root\",\"val.ntuple.mock.root\"\) >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

# Show plots
echo -n "showing histograms... "
echo "root -l EventNtuple/validation/show_val_plots.C\(\"val.ntuple.mock.root\"\)" >> ${log_file} 2>&1
root -l EventNtuple/validation/show_val_plots.C\(\"val.ntuple.mock.root\"\) >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

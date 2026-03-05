#
# test_fcls.sh - runs fcl files to make sure that they complete successfully
# Note: requires relevant filelists in a directory above this one
#
if [ $# -ne 1 ]; then
    echo "Usage: . ./validation/test_fcls.sh [MDC2020 / MDC2025 / Run1B]"
    return
fi
vMDC=$1

log_file="test_fcls.log"
rm ${log_file}

mock_dataset=""
primary_dataset=""
mixed_dataset=""
extracted_dataset=""
digi_dataset=""
crv_kpp_dataset="temp_crv_kpp"

if [[ "$vMDC" == "MDC2020" ]]; then
    echo "Testing MDC2020 datasets"
    mock_dataset="mcs.mu2e.ensembleMDS2cMix1BBTriggered.MDC2020ba_best_v1_3.art"
    primary_dataset="mcs.mu2e.CeEndpointOnSpillTriggered.MDC2020aw_best_v1_3.art"
    mixed_dataset="mcs.mu2e.CeEndpointMix1BBTriggered.MDC2020aw_best_v1_3.art"
    extracted_dataset="mcs.mu2e.CosmicCRYExtractedTriggered.MDC2020aw.art"
    digi_dataset="dig.mu2e.DIOtail95OnSpillTriggered.MDC2020au_perfect_v1_3.art"
elif [[ "$vMDC" == "MDC2025" ]]; then
    echo "Testing MDC2025 datasets"
    mock_dataset="mcs.mu2e.ensembleMDS3aOnSpillTriggered.MDC2025af_best_v1_3.art"
    primary_dataset="mcs.mu2e.CeEndpointOnSpillTriggered.MDC2025ae_best_v1_3.art"
    mixed_dataset="mcs.mu2e.CeMLeadingLogMix1BBTriggered.MDC2025af_best_v1_1.art"
    extracted_dataset="mcs.mu2e.CosmicCRYExtractedTriggered.MDC2025ae_best_v1_3.art"
    digi_dataset="dig.mu2e.FlatGammaMix1BBTriggered.MDC2025af_best_v1_1.art"
elif [[ "$vMDC" == "Run1B" ]]; then
    echo "Testing Run1B datasets"
    echo "No mock datasets for Run1B - some tests will fail..."
    mock_dataset="" # "mcs.mu2e.ensembleMDS3aOnSpillTriggered.MDC2025af_best_v1_3.art"
    primary_dataset="mcs.mu2e.CeEndpointOnSpillTriggerable-KL.Run1Baf_best_v1_4-000.art"
    mixed_dataset="mcs.mu2e.CeEndpointMix1BB-KL.Run1Bah_best_v1_4-001.art"
    echo "No extracted datasets for Run1B - some tests will fail..."
    extracted_dataset="" # "mcs.mu2e.CosmicCRYExtractedTriggered.MDC2025ae_best_v1_3.art"
    echo "No reco+ntuple fcl for Run1B - some tests will fail..."
    digi_dataset="" # "dig.mu2e.FlatGammaMix1BBTriggered.MDC2025af_best_v1_1.art"
else
    echo "Unsupported option $vMDC"
    return
fi

all_datasets=( $mock_dataset $primary_dataset $mixed_dataset $extracted_dataset $digi_dataset $crv_kpp_dataset )

if [ ! -d ../filelists ]; then
     echo "Making directory ../filelists/"
     mkdir ../filelists/
fi

for dataset in "${all_datasets[@]}"
do
    if [ ! -f ../filelists/$dataset.list ]; then
        echo "File list for $dataset doesn't exist. Creating..."
        setup mu2efiletools
        mu2eDatasetFileList $dataset > ../filelists/$dataset.list
    fi
done


echo -n "from_mcs-mockdata.fcl... "
echo "mu2e -c fcl/from_mcs-mockdata.fcl -S ../filelists/$mock_dataset.list --TFileName nts.ntuple.mock.root -n 100" > ${log_file} 2>&1
mu2e -c fcl/from_mcs-mockdata.fcl -S ../filelists/$mock_dataset.list --TFileName nts.ntuple.mock.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "from_mcs-mockdata_noMC.fcl... "
echo "mu2e -c fcl/from_mcs-mockdata_noMC.fcl -S ../filelists/$mock_dataset.list --TFileName nts.ntuple.mockNoMC.root -n 100" >> ${log_file} 2>&1
mu2e -c fcl/from_mcs-mockdata_noMC.fcl -S ../filelists/$mock_dataset.list --TFileName nts.ntuple.mockNoMC.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "from_mcs-primary.fcl... "
echo "mu2e -c fcl/from_mcs-primary.fcl -S ../filelists/$primary_dataset.list --TFileName nts.ntuple.primary.root -n 100" >> ${log_file} 2>&1
mu2e -c fcl/from_mcs-primary.fcl -S ../filelists/$primary_dataset.list --TFileName nts.ntuple.primary.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "from_mcs-mixed.fcl... "
echo "mu2e -c fcl/from_mcs-mixed.fcl -S ../filelists/$mixed_dataset.list --TFileName nts.ntuple.mixed.root -n 100" >> ${log_file} 2>&1
mu2e -c fcl/from_mcs-mixed.fcl -S ../filelists/$mixed_dataset.list --TFileName nts.ntuple.mixed.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "from_mcs-extracted.fcl... "
echo "mu2e -c fcl/from_mcs-extracted.fcl -S ../filelists/$extracted_dataset.list --TFileName nts.ntuple.extracted.root -n 100" >> ${log_file} 2>&1
mu2e -c fcl/from_mcs-extracted.fcl -S ../filelists/$extracted_dataset.list --TFileName nts.ntuple.extracted.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "from_mcs-ceSimReco.fcl... "
echo "mu2e -c Production/Validation/ceSimReco.fcl -n 10" >> ${log_file} 2>&1
mu2e -c Production/Validation/ceSimReco.fcl -n 10 >> ${log_file} 2>&1
echo "mu2e -c fcl/from_mcs-ceSimReco.fcl -s mcs.owner.val-ceSimReco.dsconf.seq.art --TFileName nts.ntuple.ceSimReco.root" >> ${log_file} 2>&1
mu2e -c fcl/from_mcs-ceSimReco.fcl -s mcs.owner.val-ceSimReco.dsconf.seq.art --TFileName nts.ntuple.ceSimReco.root >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "from_mcs-ceSimRecoVal.fcl... "
echo "mu2e -c validation/ceSimReco.fcl -n 100" >> ${log_file} 2>&1
mu2e -c validation/ceSimReco.fcl -n 100 >> ${log_file} 2>&1
echo "mu2e -c validation/ceTrig.fcl -s mcs.owner.val-ceSimRecoVal.dsconf.seq.art --TFileName nts.ntuple.ceTrig.root" >> ${log_file} 2>&1
mu2e -c validation/ceTrig.fcl -s mcs.owner.val-ceSimRecoVal.dsconf.seq.art --TFileName nts.ntuple.ceSimRecoVal.root >> ${log_file} 2>&1
echo "mu2e -c fcl/from_mcs-ceSimRecoVal.fcl -s mcs.owner.val-ceTrig.dsconf.seq.art --TFileName nts.ntuple.ceSimRecoVal.root" >> ${log_file} 2>&1
mu2e -c fcl/from_mcs-ceSimRecoVal.fcl -s mcs.owner.val-ceTrig.dsconf.seq.art --TFileName nts.ntuple.ceTrig.root >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "from_mcs-mockdata_separateTrkBranches.fcl... "
echo "mu2e -c fcl/from_mcs-mockdata_separateTrkBranches.fcl -S ../filelists/${mock_dataset}.list --TFileName nts.ntuple.mockSepTrkBranches.root -n 100" >> ${log_file} 2>&1
mu2e -c fcl/from_mcs-mockdata_separateTrkBranches.fcl -S ../filelists/${mock_dataset}.list --TFileName nts.ntuple.mockSepTrkBranches.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "from_mcs-mockdata_selectorExample.fcl... "
echo"mu2e -c fcl/from_mcs-mockdata_selectorExample.fcl -S ../filelists/${mock_dataset}.list --TFileName nts.ntuple.mockSelector.root -n 100" >> ${log_file} 2>&1
mu2e -c fcl/from_mcs-mockdata_selectorExample.fcl -S ../filelists/${mock_dataset}.list --TFileName nts.ntuple.mockSelector.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "from_mcs-primary_addVDSteps.fcl... "
echo "mu2e -c fcl/from_mcs-primary_addVDSteps.fcl -S ../filelists/$primary_dataset.list --TFileName nts.ntuple.primaryVDSteps.root -n 100" >> ${log_file} 2>&1
mu2e -c fcl/from_mcs-primary_addVDSteps.fcl -S ../filelists/$primary_dataset.list --TFileName nts.ntuple.primaryVDSteps.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi


echo -n "from_dig-mockdata.fcl... "
echo "mu2e -c fcl/from_dig-mockdata.fcl -S ../filelists/${digi_dataset}.list --TFileName nts.ntuple.dig.root -n 100" >> ${log_file} 2>&1
mu2e -c fcl/from_dig-mockdata.fcl -S ../filelists/${digi_dataset}.list --TFileName nts.ntuple.dig.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "creating file for validation script... "
echo "mu2e -c fcl/from_mcs-mockdata.fcl -S ../filelists/${mock_dataset}.list --TFileName nts.ntuple.after.root -n 100" >> ${log_file} 2>&1
mu2e -c fcl/from_mcs-mockdata.fcl -S ../filelists/${mock_dataset}.list --TFileName nts.ntuple.after.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "creating validation file... "
echo "root -l -b -q validation/create_val_file_rooutil.C\(\"nts.ntuple.after.root\",\"val.ntuple.after.root\"\)" >> ${log_file} 2>&1
root -l -b -q validation/create_val_file_rooutil.C\(\"nts.ntuple.after.root\",\"val.ntuple.after.root\"\) >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "from_rec-crv-kpp.fcl... "
echo "mu2e -c fcl/from_rec-crv-kpp.fcl -S ../filelists/${crv_kpp_dataset}.list --TFileName nts.ntuple.crv-kpp.root -n 100" >> ${log_file} 2>&1
mu2e -c fcl/from_rec-crv-kpp.fcl -S ../filelists/${crv_kpp_dataset}.list --TFileName nts.ntuple.crv-kpp.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "from_rec-crv-kpp_withCrvDigis.fcl... "
echo "mu2e -c fcl/from_rec-crv-kpp_withCrvDigis.fcl -S ../filelists/${crv_kpp_dataset}.list --TFileName nts.ntuple.crv-kpp_withCrvDigis.root -n 100" >> ${log_file} 2>&1
mu2e -c fcl/from_rec-crv-kpp_withCrvDigis.fcl -S ../filelists/${crv_kpp_dataset}.list --TFileName nts.ntuple.crv-kpp_withCrvDigis.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

echo -n "from_mcs-DeCalib.fcl... "
echo "mu2e -c fcl/from_mcs-DeCalib.fcl -S ../filelists/${mixed_dataset}.list --TFileName nts.ntuple.deCalib.root -n 100" >> ${log_file} 2>&1
mu2e -c fcl/from_mcs-DeCalib.fcl -S ../filelists/${mixed_dataset}.list --TFileName nts.ntuple.deCalib.root -n 100 >> ${log_file} 2>&1
if [ $? == 0 ]; then
    echo "OK" | tee -a ${log_file}
else
    echo "FAIL" | tee -a ${log_file}
fi

#
# roodask_test.sh - test that roodask runs
#

pyenv ana

dataset="nts.mu2e.ensembleMDS3aOnSpillTriggered.MDC2025-002.root"
if [ ! -f $dataset.list ]; then
    echo "File list for $dataset doesn't exist. Creating..."
    muse setup ops
    metacat query files from mu2e:$dataset | mdh print-url -l tape -s path - > $dataset.list
fi

# Run EventNtuple
echo "testing roodask macro_manifest... "
echo "roodask --manifest EventNtuple/rooutil/roodask/macro_manifest.json --filelist $dataset.list --n-workers 2 --threads-per-worker 1 --max-files=3 --hadd merged.root"
roodask --manifest EventNtuple/rooutil/roodask/macro_manifest.json --filelist $dataset.list --n-workers 2 --threads-per-worker 1 --max-files=3 --hadd merged.root

echo "testing roodask refana_manifest... "
echo "roodask --manifest EventNtuple/rooutil/roodask/refana_manifest.json --filelist $dataset.list --n-workers 2 --threads-per-worker 1 --max-files=3 --hadd merged.root --post-hadd 'RooCountAna {merged} MDS3a'"
roodask --manifest EventNtuple/rooutil/roodask/refana_manifest.json --filelist $dataset.list --n-workers 2 --threads-per-worker 1 --max-files=3 --hadd merged.root --post-hadd 'RooCountAna {merged} MDS3a'

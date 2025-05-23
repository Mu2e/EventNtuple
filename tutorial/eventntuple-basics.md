# EventNtuple Basics

## Introduction
The EventNtuple is a [ROOT TTree](https://root.cern.ch/doc/master/classTTree.html) that contains Mu2e data from the tracker, calorimeter, and CRV. You can think of it like a giant spreadsheet: each row is a Mu2e event and each leaf is a column. You will soon realize that the structure is a little more complex than that...

## Learning Objectives

By the end of this tutorial, you will be able to:
* describe the difference between single-object branches, vector branches, and vector-of-vector branches, and
* find the leaf and branch definitions using either ```ntuplehelper``` or GitHub


## Accessing the EventNtuple
Because the EventNtuple is a [ROOT TTree](https://root.cern.ch/doc/master/classTTree.html), it can be accessed in either ROOT or python like any other TTree.

### Getting a list of EventNtuple files
(Note these instructions may soon be outdated when Mu2e moves to the next generation of file handling tools. If they do not work, or you know that we have already updated, then post a message on the #analysis-tools Slack channel to let the Analysis Tools conveners know).

First, get a list of EventNtuple datasets as follows:

```
setup mu2efiletools
mu2eDatasetList --tier=nts | grep v06_03_00
```
replacing ```v06_03_00``` with the most recent version number of EventNtuple from the [Mu2e wiki page](https://mu2ewiki.fnal.gov/wiki/EventNtuple#Version_History_%26_Musings).


Pick a dataset from the list and to get a list of files that are already [prestaged](https://mu2ewiki.fnal.gov/wiki/Prestage), do the following:

```
setup dhtools
samListLocations -d --defname=nts.name.of.dataset.root > filelist.txt
```

If you have authentication errors then consult [this page](https://mu2ewiki.fnal.gov/wiki/Authentication#Tokens) on the Mu2e wiki.

### Printing the EventNtuple

You can print the branches and leaves of the EventNtuple in your file in ROOT or python.

* in ROOT:

```
root -l /path/to/nts.root
EventNtuple->cd()
ntuple->Print()
```

* in python:

```
import uproot
ntuple = uproot.open(filename+":EventNtuple/ntuple")
ntuple.show()
```

That's a lot of branches! But what do they _mean_?

## Understanding Branches with ```ntuplehelper```

We will use the [```ntuplehelper```](https://github.com/Mu2e/EventNtuple/blob/main/doc/ntuplehelper.md) to find out more about your branch.

The ```ntuplehelper``` is a command line tool that prints descriptions of branches and leaves. Pick a branch from the list of branches that you want to understand more about and do the following (replacing ```branch``` with the name of the branch you chose).

* on the command line:

```
ntuplehelper branch.*
```

* in python
```
>>> import ntuplehelper
>>> nthelper = ntuplehelper.nthelper()
>>> nthelper.whatis("branch.*")
```

As you can see, you get a brief description of the branch, as well as a description of every leaf on that branch.

You can list all branches with ```ntuplehelper --list-all-branches``` (command line) or ```nthelper.list_all_branches()``` (python). This complete list is also documented in the EventNtuple repository [here](https://github.com/Mu2e/EventNtuple/blob/main/doc/branches.md))

## The Structure of the EventNtuple

The structure of the EventNtuple is complex due to the reality of our data. Each entry in the EventNtuple corresponds to a single Mu2e [event](https://mu2ewiki.fnal.gov/wiki/Computing_Concepts#Events). Within an event, we can have different numbers of tracks, and within each track we can have different numbers of hits.

### Single-Object, Vector, and Vector-of-Vector Branches

You might have noticed when the list of all branches was printed was that some branches were described as "single-object" branches, some where described as "vector" branches, and the rest were described as "vector of vector" branches.
* a single-object branch contains information that describes a single event (e.g. event ID, total number of hits in the tracker)
* a vector branch contains a list of multiple objects that are in the same event (e.g. information about multiple track fits)
* a vector-of-vector branch contains a list of objects

### Some Examples

The ```trk``` branch is a vector branch because a single event can contain more than one track fit. The ```trk``` branch looks something like this:

```
[ trk1, trk2, ..., trkN ]
```

The ```trkhits``` branch is a vector-of-vectors branch because a single track fit contains more than one hit. Their structure will look something like:

```
[ [trk1_hit1, trk1_hit2, ..., trk1_hitM], [trk2_hit1, trk2_hit2, ..., trk2_hitN], ..., [trkN_hit1, trkN_hit2, ..., trkN_hitM] ]
```

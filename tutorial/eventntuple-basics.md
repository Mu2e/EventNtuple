# EventNtuple Basics

## Introduction
The EventNtuple is a [ROOT TTree](https://root.cern.ch/doc/master/classTTree.html) that contains Mu2e data from the tracker, calorimeter, and CRV. You can think of it like a giant spreadsheet: each row is a Mu2e event and each leaf is a column. You will soon realize that the structure is a little more complex than that...

## Learning Objectives

By the end of this tutorial, you will be able to:
* describe the difference between single-object branches, vector branches, and vector-of-vector branches, and
* find the leaf and branch definitions using either ```ntuplehelper``` or GitHub

## Setting Up
In this tutorial, you have a choice between using ROOT and python. Choose now.

If you chose ROOT, do the following:

```
cd /to/a/new/working/area/
mu2einit
muse setup Analysis
```

If you chose python, do the following:

```
cd /to/a/new/working/area
mu2einit
pyenv ana
```

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

Check the contents of ```filelist.txt```. If it is empty, then pick another dataset. If you have authentication errors, then consult [this page](https://mu2ewiki.fnal.gov/wiki/Authentication#Tokens) on the Mu2e wiki.

### Printing the EventNtuple

You can print the branches and leaves of the EventNtuple in ROOT or python. From your ```filelist.txt` pick a single file and use it in the following:

* in ROOT:

```
root -l /path/to/nts.root
EventNtuple->cd()
ntuple->Print()
```

* in python:

```
python
>>> import uproot
>>> ntuple = uproot.open("/path/to/nts.root:EventNtuple/ntuple")
>>> ntuple.show()
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
muse setup Analysis
python
>>> import ntuplehelper
>>> nthelper = ntuplehelper.nthelper()
>>> nthelper.whatis("branch.*")
```

As you can see, you get a brief description of the branch, as well as a description of every leaf on that branch.

You can list all branches with ```ntuplehelper --list-all-branches``` (command line) or ```nthelper.list_all_branches()``` (python). This complete list is also documented in the EventNtuple repository [here](https://github.com/Mu2e/EventNtuple/blob/main/doc/branches.md))

## The Structure of the EventNtuple

The structure of the EventNtuple is complex due to the reality of our data. Each entry in the EventNtuple corresponds to a single Mu2e [event](https://mu2ewiki.fnal.gov/wiki/Computing_Concepts#Events). Within an event, we can have different numbers of tracks, and within each track we can have different numbers of hits.

### Single-Object, Vector, and Vector-of-Vector Branches

You might have noticed when printing branches with ```ntuplehelper```, that some branches were described as "single-object" branches, some where described as "vector" branches, and the rest were described as "vector of vector" branches. The definitions of these are:
* a single-object branch contains information that describes a single event
* a vector branch contains a list of multiple objects that are in the same event
* a vector-of-vector branch contains a list of list of objects

As an example, let's discuss the track fits. In each event the reconstruction will attempt multiple track fits to the same set of hits in the tracker. Each of these fits will assume something different about the particle that made the hits (e.g. whether the particle was positively or negatively charged, or whether the particle was an electron or a muon). Each fit will independently decide which hits are included or excluded based on these fit hypotheses. The total number of hits in the tracker will remain the same but the number of hits in each fit might be different. So we have three different types of branches:
* ```hitcount```: total number of hits in the tracker (a single-object branch)
* ```trk```: information about all the fits in that event (a vector branch)
  * structure: ```[ trk1, trk2, ..., trkN ]```
* ```trkhits```: information about the hits used in each fit (a vector-of-vector branch)
  * structure: ```[ [trk1_hit1, trk1_hit2, ..., trk1_hitX], [trk2_hit1, trk2_hit2, ..., trk2_hitY], ..., [trkN_hit1, trkN_hit2, ..., trkN_hitZ] ]```

### Complicated Inter-Branch Relations

In this example, the ```trk``` branch and the outer vector of the ```trkhits``` branch have the same length so they correspond 1:1 with each other. However, this is not generally true for all branches. For example, the ```trkhits``` and ```trkhitsmc``` have different lengths because the reconstruction sometimes misses a true a hit. To hide these complications from analyzers we have RooUtil and pyutils, which you will learn about in the next exercises of the [Analysis Tools Tutorial](https://mu2ewiki.fnal.gov/wiki/Analysis_Tools_Tutorial).

## Conclusion
You should now be able to:
* describe the difference between single-object branches, vector branches, and vector-of-vector branches, and
* find the leaf and branch definitions using either ```ntuplehelper``` or GitHub

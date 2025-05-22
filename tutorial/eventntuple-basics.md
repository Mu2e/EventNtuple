# EventNtuple Basics (Draft)

## Introduction
The EventNtuple is a [ROOT TTree](https://root.cern.ch/doc/master/classTTree.html) that contains Mu2e data from the tracker, calorimeter, and CRV. You can think of it like a giant spreadsheet: each row is a Mu2e event and each leaf is a column. You will soon realize that the structure is a little more complext than a simple spreadsheet.

## Learning Objectives

By the end of this tutorial, you will be able to:
* describe the difference between single-object branches, vector branches, and vector-of-vector branches, and
* find the leaf and branch definitions using either ```ntuplehelper``` or GitHub


## Accessing the EventNtuple
Because the EventNtuple is a [ROOT TTree](https://root.cern.ch/doc/master/classTTree.html), it can be accessed in either ROOT or python like any other TTree.

First, get a list of files following the instructions on the wiki [here]() and pick a file.

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
...
```

As you can see, there are a lot of branches. But what do they _mean_? Pick a branch that you want to understand more about and continue to the next section

## Understanding Branches with ```ntuplehelper```

We will use the [```ntuplehelper```](https://github.com/Mu2e/EventNtuple/blob/main/doc/ntuplehelper.md) to find out more about your branch.

The ```ntuplehelper``` is a command line tool that prints descriptions of branches and leaves. To understand more about the branch you selected, do the following (replacing ```branch``` with the name of the branch you want to know more about).

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

Note that you can also list all branches with ```ntuplehelper --list-all-branches``` (command line) or ```nthelper.list_all_branches()``` (python). This complete list is also documented in the EventNtuple repository [here](https://github.com/Mu2e/EventNtuple/blob/main/doc/branches.md))

## The Structure of the EventNtuple

Each entry in the EventNtuple corresponds to a single Mu2e event ([wiki link](https://mu2ewiki.fnal.gov/wiki/Computing_Concepts#Events)).

You might have noticed when the list of all branches was printed was that some branches were described as "single-object" branches, some where described as "vector" objects, and the rest were described as "vector of vector" branches.

Single object branches describe a whole event.

Vector object branches.  For example the ```trk``` branch is a vector because a single event contains more than one track. Their structure will look something like:

```
[ trk1, trk2, ..., trkN ]
```

Vector-of-vector branches.

For example the ```trkhits``` branch is a vector-of-vectors because a single track contains more than one hit. Their structure will look something like:

```
[ [trk1_hit1, trk1_hit2, ..., trk1_hitM], [trk2_hit1, trk2_hit2, ..., trk2_hitN], ..., [trkN_hit1, trkN_hit2, ..., trkN_hitM] ]
```

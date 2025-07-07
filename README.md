# EventNtuple

## Table of Contents
1. [Introduction](#Introduction)
2. [Structure](#Structure)
3. [How to get the version of EventNtuple](#How-to-get-the-version-of-EventNtuple)
4. [How to Analyze an EventNtuple](#How-to-Analyze-an-EventNtuple)
5. [How to Create an EventNtuple](#How-to-Create-an-EventNtuple)
6. [Validation](#Validation)
7. [Tagging a New Release](#Tagging-a-New-Release)
8. [Notes for Developers](#Notes-for-Developers)
9. [Previous Verisons](#Previous-Versions)
   - [Upgrading from v5 to 6](#Upgrading-from-v5-to-v6)
10. [Other Useful Links](#Other-Useful-Links)

## Introduction

The EventNtuple is a simplified data format that can be used for Mu2e analyses.

## Structure
The EventNtuple structure is complex. Some branches consist of a single object (e.g. ```evtinfo```), some are vectors of objects (e.g. ```trk```) and some are vectors-of-vectors of objects (e.g. ```trksegs```).

A [list of branches is available](./doc/branches.md)

The help understand what all the branches and leaves mean, we have an [```ntuplehelper```](doc/ntuplehelper.md) tool

## How to get the version of EventNtuple that is in a file
For versions of EventNtuple later than v6.3.0, we store the version number in the ROOT file. You can check the version number with the ```checkEventNtuple``` tool:

```
checkEventNtuple file1.root file2.root
```

## How to Analyze an EventNtuple
To help with analyzing the EventNtuple given its complex structure, we have two sets of utilities:
* [```RooUtil```](rooutil/README.md) for ROOT-based analyses, and
* [```pyutils```](https://github.com/Mu2e/pyutils/blob/main/README.md) for python-based analyses.

Feel free to send questions, comments, or suggestions for improvement to the #analysis-tools Slack channel.

## How to Create an EventNtuple
In case you need to create your own EventNtuple you can do the following from a clean login in your working directory with the list of files you want to run over in a text file called ```your-art-filelist.txt```:

```
mu2einit
muse setup EventNtuple
mu2e -c EventNtuple/fcl/from_mcs-mockdata.fcl -S your-art-filelist.txt
```

This will create a file called ```nts.owner.description.version.sequencer.root``` that contains the EventNtuple. The EventNtuple will then be in the ROOT file as ```EventNtuple/ntuple```.

Note: Other fcl files are available. See table [here](fcl/README.md) to see if another one is more appropriate for your use case.

## Notes for Developers
Notes for developers contributing to EventNtuple are [here](doc/developers.md)

## Validation
Validation scripts and instructions are [here](validation/README.md)

## Tagging a New Release
Instructions for tagging a new release:

* from your development area:
```
git fetch mu2e main
git checkout --no-track -b vXX-YY-ZZ mu2e/main
```
* update version number in ```src/EventNtupleMaker_module.cc``` and commit
* make sure EventNtuple runs following these [steps](validation/README.md#Validating-eventntuple-runs)
* create a comparison booklet following these [steps](validation/README.md#Validating-EventNtuple-Contents)
   * make sure any differences are understood
* open PR with final changes and merge
* make sure EventNtuple builds with spack following these [steps](validation/README.md#Validating-EventNtuple-Builds-(Spack/CMake)), fix any errors and merge them
   * this needs to be done after merging so that we can checkout the ```main``` branch
* create new release on GitHub
   * auto-generate release notes but add text to highlight important changes
* ask for a new Musing to be made
* post announcement on #analysis-tools Slack channel
* update [Mu2eWiki page](https://mu2ewiki.fnal.gov/wiki/EventNtuple)
* present updates at next Infra / Tools meeting

## Previous Versions
The version history of EventNtuple is [here](https://mu2ewiki.fnal.gov/wiki/EventNtuple).

### Upgrading from v5 to v6
* For analyzers: a list of branch/leaf/name changes is [here](doc/v5-to-v6.md)
* For developers: instructions to update your development area are [here](doc/v5-to-v6_developers.md)

## Other Useful Links

* [Mu2eWiki page](https://mu2ewiki.fnal.gov/wiki/EventNtuple)
* [tutorial](tutorial/README.md)

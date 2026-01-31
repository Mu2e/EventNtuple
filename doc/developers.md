# Notes for Developers

This page contains some miscellanous notes for developers

On this page:

* [Structure of the Code](#Structure-of-the-Code)
* [Setting up your Development Area](#Setting-up-your-Development-Area)
* [Before making any changes](#Before-making-any-changes)
* [Adding a branch to EventNtuple](#Adding-a-branch-to-EventNtuple)

## Structure of the Code
The code is structured in the following way:

* ```src/EventNtupleMaker_module.cc``` contains the art analyzer module that creates the ```TTree```
* ```inc/*Info*.hh``` contain C++-style ```structs``` (which we call "InfoStructs"). Each InfoStruct corresponds to a branch and the struct members correspond to leaves
* ```src/InfoStructHelper.cc``` and ```src/InfoMCStructHelper.cc``` contain helper functions that take an Offline data product and an InfoStruct and fills the members of the InfoStruct with information from the Offline data product

## Setting up your Development Area
We follow the general mu2e developer workflow given [here](https://mu2ewiki.fnal.gov/wiki/GitHubWorkflow#Developer_Workflow). Namely, create your own fork on GitHub, clone it, and set up the central Mu2e/EventNtuple repository as a remote.

## Before making any changes
Create an EventNtuple and validation file following [these instructions](../validation/README.md#Creating-a-Validation-File)

## Adding a branch to EventNtuple
Before adding a branch to the EventNtuple consider whether it will be a single-object branch (i.e. there will be one InfoStruct for a whole event), or whether it will be a vector branch or a vector-of-vector branch (i.e. there will be many InfoStructs per event). Examples are listed in [doc/branches.md](./branches.md).

Here are the changes to make to the code:

1. Create a new InfoStruct in ```inc/``` (or identify one that can be re-used)
   * make sure to add a comment line like ```// NameOfStruct: description``` for [```ntuplehelper```](./ntuplehelper.md) compatibility
   * also make sure to add a description for every leaf as an in-line comment for [```ntuplehelper```](./ntuplehelper.md) compatibility
   * all leaves should be initialized to default values (i.e. ```type leafname = default_value;```)
   * there should be a ```reset()``` function
2. Add fill functions to either ```InfoStructHelper``` or ```InfoMCStructHelper``` depending on whether these are reco or MC data products
   * make sure to pass by reference
3. In ```src/EventNtuple_module.cc```:
   * add a ```#include``` to your new InfoStruct (if applicable)
   * in ```struct Config``` add a ```fhicl::Atom<art::InputTag>``` for the collection name
   * in the ```private:``` block, add
      * an ```art::InputTag``` for the collection name
      * an ```art::Handle<DataProduct>``` for the collection
      * an ```InfoStruct``` (or ```std::vector<InfoStruct>```) to hold the data for the EventNtuple
   * in the constructor:
      * get the tags from the input in the initializer list
      * fill in empty InfoStructs
   * in ```beginJob()``` create the branch
      * make sure to add a ```.``` to the end of the branch name so that the leaves can be accessed with ROOT
   * in ```analyze()```:
      * at the start, reset your InfoStructs so that data is not left in them from previous events
      * get the ```art::Handle``` to your data product in the event using the ```art::InputTag```
      * fill in your InfoStructs with the ```_infoStructHelper```
4. In ```src/classes.h``` add your new InfoStruct header file
5. In ```src/classes_def.xml``` add your InfoStruct and vector<InfoStruct> etc. if applicable
6. In ```fcl/prolog.fcl``` add your new input tag fhicl parameter
7. Compile and test:

```
muse build -j4 --mu2eCompactPrint
mu2e -c EventNtuple/fcl/from_mcs-mockdata.fcl -s test-art-file.art -n 10
```

8. Add to ```ntuplehelper```
   * instructions [here](./ntuplehelper.md#Adding-a-branch)
9. Add to validation, RooUtil, and pyutils
   * instructions for validation and RooUtil are [here](../rooutil/README.md#For-Developers). Contact Andy for more info if needed
   * contact Sophie for pyutils
10. Run validation
    * both [test_fcls.sh](../validation/README.md#Validating-EventNtuple-Runs), and
    * and [valCompare](../validation/README.md#Validating-EventNtuple-Contents) steps
11. Update documentation
    * update ```doc/branches.md``` using ```ntuplehelper``` (see [here](./ntuplehelper.md#creating-branchesmd))
    * add an example script somewhere that uses the new branch

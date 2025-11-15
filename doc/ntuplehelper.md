# How to get help -- ```ntuplehelper```

On this page, you will learn how to use the [```ntuplehelper```](../../utils/helper/ntuplehelper.py) utility to get more information about what the different branches and leaves are.

## Setting Up

You will need to be in either:

* an Analysis environment:

```
mu2einit
muse setup [AnalysisMusing]
```
where ```[AnalysisMusing]``` is the musing relevant for your dataset. At the time of writing, this is either ```AnalysisMDC2020``` or ```AnalysisMDC2025```.

* or have the EventNtuple repository checked out

```
git clone https://github.com/Mu2e/EventNtuple.git
cd EventNtuple
```

## How to Use

### Getting a list of all branches

To get a list of all branches you can do either:

* ```ntuplehelper --list-all-branches``` on the command line, or
* ```.! ntuplehelper --list-all-branches``` in a ROOT sessions, or
* in a python session:

```
>>> import ntuplehelper
>>> nthelper = ntuplehelper.nthelper()
>>> nthelper.list_all_branches()
```

### Getting information about specific leaves

To get descriptions of certain leaves you can do either:

* ```ntuplehelper trk.nhits trkmcsim.*``` on the command line,
* ```.! ntuplehelper trk.nhits trkmcsim.*``` in a ROOT session, or
* in a  python session:
```
>>> import ntuplehelper
>>> nthelper = ntuplehelper.nthelper()
>>> nthelper.whatis(['trk.hits', 'trkmcsim.*'])
```

Notice that wildcarding (```*```) the leaf name prints descriptions of all leaves on the branch

## If you notice something isn't right

If you notice that something is missing or there is a bug, then either:

* post in the #analysis-tools Slack channel, or
* submit a GitHub issues

and we will fix it.

## For developers

### How it works

The ```ntuplehelper``` works when individual structs are in separate header files and contains a comment line like:

```
// StructName: a branch that contains something
```

and individual variables have their own line formatted like:

```
type  leafname = default_value; // comment explaining the leaf
```

### Creating branches.md
We create the [branches.md](./branches.md) page by running this script as follows:

```
rm doc/branches.md
ntuplehelper --list-all-branches --export-to-md > doc/branches.md
```

### Adding a branch
If you add a branch to EventNtuple, then make the following changes to ```utils/helper/ntuplehelper.py``` so that users can discover your new branch:
* add the branch name to one of ```single_object_branches```, ```vector_object_branches```, or ```vector_vector_object_branches```
* to ```branch_struct_dict``` add the branch name and the name of the InfoStruct

You can then test that your branch works with:

```
ntuplehelper branchname.*
```

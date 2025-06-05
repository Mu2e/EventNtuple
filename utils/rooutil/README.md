# RooUtil

## Table of Contents
1. [Introduction](#Introduction)
2. [```RooUtil``` Class](#RooUtil-Class)
3. [The ```Event``` Class](#The-Event-Class)
4. [User-Friendly Classes](#User-Friendly-Classes)
5. [Accessing User-Friendly Classes](#Accessing-User-Friendly-Classes)
6. [Cut Functions](#Cut-Functions)
7. [Common Cut Functions](#Common-Cut-Functions)
8. [Combining Cut Function](#Combining-Cut-Functions)
9. [Creating Ntuples From EventNtuple](#Creating-Ntuples-From-EventNtuple)
10. [Speed Optimizations](#Speed-Optimizations)
11. [Debugging](#Debugging)
12. [For Developers](#For-Developers)

## Introduction

RooUtil is a utility class to help analyze the EventNtuple in ROOT-based analysis frameworks. It handles all the ROOT and provides user-friendly classes that allow for coherent looping through parallel branches.

This documentation contains quick references. For a tutorial see the [Analysis Tools Tutorial](https://mu2ewiki.fnal.gov/wiki/Analysis_Tools_Tutorial)

## ```RooUtil``` Class
The constructor takes two arguments:
* ```filename``` can be the name of a single ROOT file (ending in ```.root```) or a list of ROOT files
* (optional) ```treename``` the name of the tree

## The Event Class
All branches and leaves can be accessed through the ```Event``` class like so:

```
#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
void PrintEvents(std::string filename) {

  RooUtil util(filename);
  std::cout << filename << " has " << util.GetNEvents() << " events" << std::endl;

  // Now loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    auto& event = util.GetEvent(i_event);
    // Any single-object branch can be accessed with...
    event.branchname->leafname;

    // ...and vector-object branches can be looped through like this
    for (auto& obj : *(event.branchname)) {
       obj.leafname;
    }
  }
}
```
However to aid in the complex structure of the ntuple, we also provide helper classes that combine certain branches together.

## User-Friendly Classes
Because some vector-object branches in the EventNtuple should be paired and looped through coherenetly, we have several user-friendly classes that help with this. They contain the relevent branches from the EventNtuple, often reduced in dimensionality.

### The ```Track``` Class
The ```Track``` class contains all information related to a single track

* single objects: ```trk```, ```trkmc```, ```trkcalohit```, ```trkqual```
* vectors: ```trksegs```, ```trksegmcs```, ```trksegpars_{lh,ch,kl}```, ```trkmcsim```, ```trkhits```, ```trkhitsmc```, ```trkmats```

Example: [PlotTrackNHits_RecoVsTrue.C](./examples/PlotTrackNHits_RecoVsTrue.C)

### ```TrackSegment``` Class
The ```TrackSegment``` class contains paired reco and MC information about a single track segment

* single objects: ```trkseg```, ```trksegmc```, ```trksegpars_{lh,ch,kl}```

Note: that some TrackSegements contain only a ```trkseg``` or only a ```trksegmc```

Example: [PlotEntranceMomentumResolution.C](./examples/PlotEntranceMomentumResolution.C)

### ```TrackHit``` Class
The ```TrackHit``` class contains paired reco and MC information about a single track hit.

* single objects: ```trkhit```, ```trkhitmc```

Note: In the EventNtuple, the branches are filled such that the first N hits in each correspond one-to-one with each other, and the remainining, unused MC hits complete the ```trkhitmc``` branch. The ```TrackHit``` class handles this for you.

Example: [PlotTrackHitTimesMC.C](./examples/PlotTrackHitTimesMC.C)

### ```CrvCoinc``` Class
The ```CrvCoinc``` class contains paired reco and MC information about a single CRV coincidence

* single objects: ```crvcoinc```, ```crvcoincmc```

Example: [PlotCRVPEsVsMCEDep.C](./examples/PlotCRVPEsVsMCEDep.C)

### ```MCParticle``` Class
The ```MCParticle``` class contains information about a single SimParticle in the genealogy

* single objects: ```mcsim```

Example: [PlotMCParentPosZ.C](./examples/PlotMCParentPosZ.C)

### Branches not contained within a class
Some branches are not contained in any of the above classes:

* ```trkcalohitmc```
* ```crvdigis```
* ```crvpulses``` and ```crvpulsesmc```
* ```crvcoincsmcplane```

Reach out to the developers on the #analysis-tools Slack channel if you have a need to have these added somewhere.

## Accessing User-Friendly Classes
The ```Event``` class provides access to ```Tracks``` and ```CrvCoincs```:

* ```CountTracks()```: counts the number of tracks in the event
* ```GetTracks()```: gets the tracks (passes you a copy)
* ```CountCrvCoincs()```: counts the number of CRV coincidences in the event
* ```GetCrvCoincs()```: gets the CRV coincidences

The ```Track``` class provides access to the ```TrackSegments```, the ```TrackHits```, and the ```MCParticles```:
* ```GetSegments()```, ```CountSegments()```
* ```GetHits()```, ```CountHits()```
* ```GetMCParticles()```, ```CountMCParticles()```

All of these can be passed a cut function (see below) to count / select just a subset of the objects

## Cut Functions
RooUtil also provides easy ways to select a subset of objects using the above user-friendly classes. A simple C++-style function with the signature:

```
bool function_name(Object& obj);
```

can be passed to the various ```GetObject()``` and ```CountObject()``` functions.

Here is an example using the ```is_e_minus()``` function that is defined in ```EventNtuple/utils/rooutil/inc/common_cuts.hh```

```
#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"
...
RooUtil util(filename);
for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    auto& event = util.GetEvent(i_event);
    auto& all_tracks = event.GetTracks();
    auto& e_minus_tracks = event.GetTracks(is_e_minus); // gives you a vector of tracks that are only used the e-minus fit hypothesis
}

```

## Common Cut Functions
Many cuts are already available in ```common_cuts.hh`` and can be easiliy listed with:

```
rooutilhelper --list-available-cuts
```

Feel free to add to common_cuts.hh. Some notes on the file:
* We use the following special characters for the cuts to be printed nicely with ```rooutilhelper```:
   * ```//+``` gives the cut section heading
   * ```bool function_name(args) // explanation``` ensures the cut name has an explanation printed too

## Combining Cut Functions

There are two ways to combine cut functions:

1. Write a new one yourself that uses already existing functions e.g:

```
// before your main function
bool my_cut(const Track& track) {
   return good_track(track) && is_e_minus(track);
}
...
// in your main function's event loop
int n_e_minus_good_tracks = event.CountTracks(my_cut);
```

2. or, you can combine cuts in a [lambda function](https://learn.microsoft.com/en-us/cpp/cpp/lambda-expressions-in-cpp?view=msvc-170) (similar to python):

```
int n_e_minus_good_tracks = event.CountTracks([](const Track& track){ return is_e_minus(track) && good_track(track); });
```

## Creating Ntuple from EventNtuple
### Reduced EventNtuple
RooUtil can also be used to create reduced EventNtuples. You create the output ntuple with ```RooUtil::CreateOutputEventNtuple()``` and fill it in your event loop with ```RooUtil::FillOutputEventNtuple()```. The created reduced EventNtuple is compatible with RooUtil again. See example here: [CreateNtuple.C](./examples/CreateNtuple.C)

If you want to also remove tracks from the event, you should use ```SelectTracks(cut)``` instead of ```GetTracks(cut)```. Note that this changes the ```Event``` so if you want to get the whole event again, you should use ```RooUtil::GetEvent()``` again.

### Non event-based ntuples
It's also possible to use RooUtil to create a new ntuple with a different structure (e.g. one entry per track). See an example in [CreateTrackNtuple.C](./examples/CreateTrackNtuple.C) for how this can be done

## Speed Optimizations
By default, RooUtil will read all the branches for every entry. If you are finding that this is too slow, then you can explicity turn on only the branches that you will be reading. This can increase the speed by as much as a factor of 10.

```
RooUtil util(filename);
util.TurnOffAllBranches(); // first turn all branches off
util.TurnOnBranches(std::vector<std::string>{"branch1", "branch2"});
```

## Debugging
Debugging output can be turned on like this:

```
RooUtil util(filename);
util.SetDebug(true);
```

Beware: there are a lot of debug messages.


## For Developers
### Adding a new branch
If a new branch is added to the EventNtuple, then the following needs to be done to so that RooUtil can access the branch:

1. In [Event.hh](inc/Event.hh) add the pointers at the bottom of the file
2. In [Event.hh](inc/Event.hh) constructor, set the branch address
  - make sure to test for existence if the branch may not exist (e.g. it is an MC branch)
3. In [Event.hh](inc/Event.hh) add the #include to the underlying object
4. In [RooUtil.hh](inc/RooUtil.hh) add it to the ```CreateOutputEventNtuple()``` function
5. Add to validation places:
  - [PrintEvents.C](examples/PrintEvents.C): at least the first and last leaf in the struct
  - [create_val_file_rooutil.C](../../validation/create_val_file_rooutil.C)
     - copy contents of struct into place where histograms are defined
     - then copy and replace "type " with "TH1F* h_branchname_" (e.g. "float " with "TH1F* h_trkcalohit_")
     - then copy in " = new TH1F("h_branchname_", "", 100,0,100); //"
     - delete line after "//"
     - add in leaf names to histname (and make separate x, y, z histograms for XYZVectorF leaves)
     - copy histogram lines into main loop
     - search and replace "TH1F* " with ""
     - search and replace " = new TH1F("h_" with "->Fill("
     - search and replace "", "", 100,0,100);" with ");"
     - search and replace "(branchname_" with "(branchname."
     - update histogram ranges
6. (Optional) If appropriate, add branches to other classes (e.g. Track.hh) and to ```Event::Update()```
   - be sure to test it with an example script
7. Add to this documentation

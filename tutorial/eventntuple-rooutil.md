# Analyzing the EventNtuple with RooUtil (Draft)

## Introduction
RooUtil offers an analyzer-friendly interface to EventNtuple for ROOT-based analyses. RooUtil handles the relationships between branches. The idea is that you use RooUtil to loop through the EventNtuple and select the specific events/tracks/etc that you need for your analysis and put them into ROOT histograms / ntuples / RDataFrames.

## Learning Objectives
By the end of this tutorial, you will be able to:
* analyze EventNtuple datasets,
* select subsets of the data, and
* create histograms of selected data.

Quick reference information about RooUtil is in the EventNtuple repository [here](https://www.github.com/Mu2e/EventNtuple/blob/main/utils/rooutil/README.md)

## Setting Up
In this tutorial, should set up your working area like so:

```
cd /to/a/new/working/area/
mu2einit
muse setup Analysis
```


## Loading an EventNtuple Dataset
RooUtil is easy to set up. Once you have a list of EventNtuple files (see instructions [here](./eventntuple-basics.md#Getting-a-list-of-EventNtuple-files)), you can set up RooUtil in a ROOT macro names ```LoadRooUtil.C``` like so:

```
#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"

void LoadRooUtil() {
   std::string filename = "name-of-file-list";
   RooUtil util(filename);
   std::cout << filename << " has " << util.GetNEvents() << " events" << std::endl;
}
```

and run it on the command line with:

```
root -l -b LoadRooUtil.C
```

or to compile the macro:

```
root -l -b LoadRooUtil.C+
```

(Technical detail: RooUtil is loading the data into a [ROOT TChain](https://root.cern.ch/doc/master/classTChain.html))

## The ```Event``` Class
All the branches in the EventNtuple can be accessed with the [```Event``` class](../utils/rooutil/README.md#The-Event-Class). In a new ROOT macro called ```EventLoop.C``` you can write:

```
#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"

void EventLoop() {
   std::string filename = "name-of-file-list";
   RooUtil util(filename);

   // Loop through the events
   for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
      auto& event = util.GetEvent(i_event);
      event.branchname->leafname;
   }
}
```

where ```filename``` can be the name of a single ROOT file containing an EventNtuple, or the name of a file list containing the names of many ROOT files.

Here are a few things to try in this script:
* printing the event ID of each event to the terminal

<details>
<summary>Hint</summary>

use ```ntuplehelper``` to look at the ```evtinfo``` branch
</details>


## Plotting an Event-Level Variable

## Cutting

## Analyzer-Friendly Classes: ```Tracks```, ```TrackSegments``` etc.


## Places to find help
There are various places to find help for RooUtil:
* the [quick reference README](https://www.github.com/Mu2e/EventNtuple/blob/main/utils/rooutil/README.md)
* the [examples](https://github.com/Mu2e/EventNtuple/tree/main/utils/rooutil/examples) can be used as a reference
* the #analysis-tools Slack channel

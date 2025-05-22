# Analyzing the EventNtuple with RooUtil (Draft)

## Introduction
In the [previous exercise](eventntuple-basics.md), we learned how to access the EventNtuple directly and get information about the branches and leaves contained in it. As mentioned, this has limited use for a real analysis.

RooUtil offers an analyzer-friendly interface for ROOT-based analyses. In this exercise, we will use RooUtil to:

* plot information from the ntuple,
* make cuts on the ntuple,
* produce a reduced ntuple(?)

Quick reference information about RooUtil is in the EventNtuple repository [here](https://www.github.com/Mu2e/EventNtuple/utils/rooutil/README.md)

## Setting Up
We will create a ROOT macro to make plots.

```
#include "EventNtuple/utils/rooutil/RooUtil.hh"

void MacroName() {
   std::string filename = "name-of-file"; // see below
   RooUtil util(filename);

   // Loop through the events
   for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
      auto& event = util.GetEvent(i_event);
      
   }
}
```

where ```filename``` can be the name of a single ROOT file containing an EventNtuple, or the name of a filelist containing the names of many ROOT files.

## The ```Event``` Class
All the branches in the EventNtuple can be

## Plotting an Event-Level Variable

## Cutting

## Analyzer-Friendly Classes: ```Tracks```, ```TrackSegments``` etc.

## Creating Ntuples?

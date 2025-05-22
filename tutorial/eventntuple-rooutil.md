# Analyzing the EventNtuple with RooUtil (Draft)

## Introduction
RooUtil offers an analyzer-friendly interface to EventNtuple for ROOT-based analyses.

## Learning Objectives
By the end of this tutorial, you will be able to:
* analyze EventNtuple datasets,
* select subsets of the data, and
* create histograms of selected data

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

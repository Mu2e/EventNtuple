# Analyzing the EventNtuple with RooUtil

## Introduction
RooUtil offers an analyzer-friendly interface to EventNtuple for ROOT-based analyses. RooUtil handles the relationships between branches. The idea is that you use RooUtil to loop through the EventNtuple and select the specific events/tracks/etc that you need for your analysis and put them into ROOT histograms / ntuples / RDataFrames.

## Learning Objectives
By the end of this tutorial, you will be able to:
* analyze EventNtuple datasets,
* select subsets of the data, and
* create histograms of selected data.

Each section in this tutorial gives you a small skeleton macro and then a few challenges that you should use to fill out the skeleton. Most challenges also provide a hints that you can show by clicking the arrow:

<details>
<summary>Hint</summary>

your first hint is that there are various places to find help for RooUtil:
* the [quick reference README](https://www.github.com/Mu2e/EventNtuple/blob/main/utils/rooutil/README.md)
* the [examples](https://github.com/Mu2e/EventNtuple/tree/main/utils/rooutil/examples) can be used as a reference
* the #analysis-tools Slack channel

</details>


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

Challenge #1: Print the event ID of each event to the terminal

<details>
<summary>Hint</summary>

use ```ntuplehelper``` to look at the ```evtinfo``` branch
</details>

Challenge #2: Plot the event ID of each event into a histogram

<details>
<summary>Hint</summary>

use ROOT's [histogram class](https://root.cern/manual/histograms/)
</details>

Challenge #3: Plot only on the odd-numbered event IDs into a histogram and raw it on the same set of axes as the histogram in Challenge #2

<details>
<summary>Hint</summary>

use the ```"HIST SAME"``` histogram [drawing option](https://root.cern/manual/histograms/#drawing-options)
</details>


## Plotting Track Variables with the ```Track``` Class
We can access every single branch through the ```Event``` class. However, as described in [EventNtuple Basics](./eventntuple-basics#Complicated-Inter-Branch-Relations) some branches are related to each other and are meant to be looped through coherently. In RooUtil, analyzer-friendly classes are available to handle these relationships.

Here we will look at the ```Track``` class. In a new ROOT macro named ```TrackLoop.C```, you can loop through all the tracks in all the events like so:

```
#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"

void TrackLoop() {
   std::string filename = "name-of-file-list";
   RooUtil util(filename);

   // Loop through the events
   for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
      auto& event = util.GetEvent(i_event);

      auto tracks = event.GetTracks();
      for (auto& track : tracks) {
      	  track.branchname->leafname;
      }
   }
}
```
where ```branchname``` and ```leafname``` are the same as in the EventNtuple. Howver, only track-related branches are available. These branches are listed in the quick-reference README [here](../utils/rooutil/README.md#The-Track-Class).

Challenge #1: Plot the number of hits on the reconstructed track in a histogram

<details>
<summary>Hint</summary>

the reconstructed information is in the ```trk``` branch
</details>

Challenge #2: Plot the number of hits on the MC-truth track in a histogram

<details>
<summary>Hint</summary>

the reconstructed information is in the ```trkmc``` branch
</details>

Challenge #3: Plot the number of hits on the MC-truth track against the number of hits on the reconstructed track in a 2D histogram

<details>
<summary>Hint</summary>

look at the [examples](https://github.com/Mu2e/EventNtuple/tree/main/utils/rooutil/examples)
</details>

## Selecting Tracks
There will be multiple track fits in each event. At the moment, we are plotting information from all of them and so we need to select which ones we want to analyze. There are a some common cuts already defined in a header file that you can ```#include``` in your macro.

In a new ROOT macro named ```TrackCutLoop.C```, you can do the following:

```
#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"

void TrackCutLoop() {
   std::string filename = "name-of-file-list";
   RooUtil util(filename);

   // Loop through the events
   for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
      auto& event = util.GetEvent(i_event);

      auto tracks = event.GetTracks(common_cut_function_name);
      for (auto& track : tracks) {
      	  track.branchname->leafname;
      }
   }
}
```

where ```common_cut_function_name``` is the name of a function in ```common_cuts.hh```. You can get a list of these cuts with the ```rooutilhelper``` like so:

```
rooutilhelper --list-available-cuts
```

To use the common cuts, the macro should be compiled like so:

```
root -l TrackCutLoop.C++
```

Challenge #1: Plot the number of hits in track fits that used the e-minus particle hypothesis
<details>
<summary>Hint</summary>

use ```rooutilhelper --list-available-cuts``` to find a suitable common cut
</details>

Challenge #2: Plot the number of hits in track fits that are travelling downstream
<details>
<summary>Hint</summary>

use ```rooutilhelper --list-available-cuts``` to find a suitable common cut. You might notice there are two: use the one related to ```Track```
</details>

Challenge #3: Plot the number of hits in track fits that are **both** travelling downstream and used the e-minus particle hypothesis

<details>
<summary>Hint</summary>

look at the section in the quick reference README on [combining cut functions](../utils/rooutil/README.md#Combining-Cut-Functions)
</details>

## Plotting Track Momentum with ```TrackSegment``` Class
What we actually want to measure in Mu2e is the momentum of particles. Our reconstruction algorithm doesn't give us one momentum per track because it can account for changes in momentum as the particle travels downstream and passes through material. We store the momentum of particles at various surfaces along the track in  ```TrackSegments```.

```
#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"

void TrackSegmentLoop() {
   std::string filename = "name-of-file-list";
   RooUtil util(filename);

   // Loop through the events
   for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
      auto& event = util.GetEvent(i_event);

      auto tracks = event.GetTracks(common_cut_function_name);
      for (auto& track : tracks) {
          auto track_segments = track.GetSegments(common_cut_function_name);
	  for (auto& track_segment : track_segments) {
	    track_segment.branchname->leafname;
	  }
      }
   }
}
```
where the ```branchname``` and ```leafname``` can only be those that are available in the [```TrackSegment``` class](../utils/rooutil/README.md#The-TrackSegment-Class).

Here we need to be careful because not all tracks will have a reconstructed track segment at every surface so we need to use the ```has_reco_step()``` common cut function when we get tracker segments

Challenge #1: Plot the reconstructed momentum of tracks at the middle of the tracker
<details>
<summary>Hint</summary>

use ```rooutilhelper --list-available-cuts``` to find a suitable common cut
</details>

Challenge #2: Plot the momentum resolution of tracks
<details>
<summary>Hint</summary>

you will need to make sure that the ```TrackSegment``` has both a reconstructed step and a MC-truth step using the common cut functions
</details>

## Final Challenges
Here are a few additional challenges for those who are interested:

Additional Challenge #1: Plot the starting position of the parents of particles that create tracks in the tracker
<details>
<summary>Hint</summary>

use the ```trkmcsim``` branch in the ```Track``` class. The ```trkmcsim``` branch contains the MC-truth genealogy from the simulation
</details>

Additional Challenge #2: Write your histograms to an output ROOT file. (This will save you from having to run over all the data whenever you want to either make cosmetic changes to the histogram or use it for additional analysis / fitting)
<details>
<summary>Hint</summary>

see [ROOT documentation on files](https://root.cern/manual/root_files/)
</details>

Additional Challenge #3: Plot variables from the ```CrvCoinc``` class. Can you find a ```CrvCoinc``` close in time to a ```Track```?

## Conclusion
You should now be able to:
* analyze EventNtuple datasets,
* select subsets of the data, and
* create histograms of selected data.

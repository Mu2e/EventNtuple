# Analyzing the EventNtuple with PyUtil

## Introduction

# Analyzing the EventNtuple with PyUtil

# Introduction

In the [previous exercise](eventntuple-basics.md), we learned how to access the EventNtuple directly and get information about the branches and leaves contained in it. As mentioned, this has limited use for a real analysis.

`pyutils` offers an analyzer-friendly interface to EventNtuple for python-based analyses. `pyutils` handles importing EventNtuple files into python and allows simple application of cuts, creation of histograms and printing of event information. `pyutil` is scalable for multi-threading.

# Learning Objectives

By the end of this tutorial, you will be able to:

* analyze EventNtuple datasets using `pyutil`,
* select subsets of the data, and
* create histograms of selected data.

Each section in this tutorial gives you a small skeleton macro and then a few challenges that you should use to fill out the skeleton. Most challenges also provide a hints that you can show by clicking the arrow:

<details>
<summary>Hint</summary>

there are a few places to get additional help as you work through these challenges:
* the [quick reference README](https://github.com/Mu2e/pyutils/blob/main/README.md)
* the [examples](https://github.com/Mu2e/pyutils/tree/main/examples) can be used as a reference
* the #analysis-tools Slack channel

</details>


# Setting Up

Before we begin we need to setup the standard Mu2e python environment which contains the latest `pyutil`. This can be done as follows:

```
mu2einit
pyenv ana
```

## Loading an EventNtuple Dataset

Open up a text editor. 

<details>
<summary> or launch a jupyter-notebook  (OPTIONAL)</summary>


```
jupyter-lab --no-browser
```
Then create an ssh-tunnel:

```
 ssh -f -KXY -N -L 8888:localhost:8888 user@mu2egpvm01.fnal.gov
```

replace the port and username/machine-name as appropriate.
</details>


Now we are ready to import a file:

```
import awkward as ak
from pyutils.pyprocess import Processor 
processor = Processor(verbosity=2)


file_name = "/pnfs/mu2e/tape/phy-nts/nts/mu2e/MDS2ac-OnSpillTriggered/MDC2020aw_perfect_v1_3/root/8c/0b/nts.mu2e.MDS2ac-OnSpillTriggered.MDC2020aw_perfect_v1_3.0.root"

branches = ["event"]

data = processor.process_data(
    file_name=file_name,
    branches=branches
)
```

Note that we use the `awkward` python package throughout this example (https://awkward-array.org/doc/main/).

Now print some of the data using the pyprint class:

```
from pyutils.pyprint import Print
```


Initialise the printer, verbose=False is default and prevents overwhelming output with large arrays

```
printer = Print(verbose=False)
```

Print the first 10 events:

```
printer.print_n_events(data, n_events=10)
```

At this point you should see a list of event numbers. As this example proceeds you'll see how useful the `pyprint` class can be.

## Challenges

Let's begin by carrying out the simplest operations.

1) Print the event numbers to terminal:

<details>
<summary>Hint</summary>

```
from pyutils.pyprint import Print

printer = Print(verbose=False)

printer.print_n_events(data, n_events=10)
```
</details>

2) Plot the event numbers in a histogram:

<details>
<summary>Hint</summary>

```
from pyutils.pyplot import Plot 
plotter = Plot()
plotter.plot_1D(
    data["event"],               # Data to plot
    nbins=100,               # Number of bins
    xmin=0,                # Minimum x-axis value
    xmax=1000,               # Maximum x-axis value
    xlabel="Event Number",
    ylabel="# occurances",
    out_path='event.png',  # Output file path
    stat_box=True,           # Show statistics box
    error_bars=True          # Show error bars
)
```
</details>

3) Plot only on the odd-numbered event IDs into a histogram and raw it on the same set of axes as the histogram in Challenge (2). Make sure the two histograms have distinct styles:

<details>
<summary>Hint</summary>

```
odd_mask = (data["event"] % 2 != 0)
odd_events = data["event"].mask[odd_mask]
plotter.plot_1D_overlay(
    {
        "All" : data["event"], # First histogram
        "Odd" : odd_events,
    },
    nbins=100,
    xmin=0,
    xmax=100,
    xlabel="Event Number",
    ylabel="# occurances",
    out_path="events_odd.png",
    norm_by_area=True # Optionally normalise the histograms by area
)
```
</details>

Now you have learned the basics:

* how to import the EventNTuple
* how to access a branch and print it
* how to filter the branch
* how to produce a histogram with overlays.

# Track selction cuts

Let's apply that knowledge to something that might be used in a real analysis.

First, lets select the branches associated with our `trk` branch:

```
import awkward as ak
from pyutils.pyprocess import Processor 
processor = Processor(verbosity=2)

file_name = "/pnfs/mu2e/tape/phy-nts/nts/mu2e/MDS2ac-OnSpillTriggered/MDC2020aw_perfect_v1_3/root/8c/0b/nts.mu2e.MDS2ac-OnSpillTriggered.MDC2020aw_perfect_v1_3.0.root"

branches = ["trk.nactive", 
            "trk.pdg", 
            "trk.status"]

data = processor.process_data(
    file_name=file_name,
    branches=branches
)

```

Note that the `branches` list can be expanded. For these vector branches the specific elements must be listed. You will notice later on that this is not the case for the vector-of-vector branches like `trksegs`.  You can read more about the branches in the EventNTuple README.md. 

Let's investigate the first few events:

```
from pyutils.pyprint import Print
printer = Print(verbose=False)
printer.print_n_events(data, n_events=10)
```

You should see something like this:

```
-------------------------------------------------------------------------------------
trk.status: [1, 1, 1, 1]
trk.pdg: [11, -11, 13, -13]
trk.nactive: [33, 23, 33, 26]
-------------------------------------------------------------------------------------
```

This tells us that our `trk` branch contains a vector with 4 elements. Each of these tracks has an associated pdg id (11=electron, -11=positron, 13=muon, -13=anti-muon). The 4 tracks are track hypothesis fits with these particles being assumed in the fit. The first array tells us whether the track fit converged, these all did. The last array tells us how many active hits are in the fit, for reference, signal selection requires nactive > 20 for a "good fit."


Now, lets use this to make some interesting plots:

### Challenges

4) Plot the number of hits on the reconstructed track in a histogram

<details>
<summary>Hint</summary>

```
from pyutils.pyplot import Plot 
plotter = Plot()
nactive = ak.flatten(data["trk.nactive"], axis=None)
plotter.plot_1D(
    nactive,               # Data to plot
    nbins=100,               # Number of bins
    xmin=0,                # Minimum x-axis value
    xmax=100,               # Maximum x-axis value
    xlabel="trk.nactive",
    ylabel="# occurances per bin",
    out_path='trk_nactive.png',  # Output file path
    stat_box=True,           # Show statistics box
    error_bars=True          # Show error bars
)
```
</details>

Note that becase the branch is a vector we need to flatten it before plotting:

```
nactive = ak.flatten(data["trk.nactive"], axis=None)
```

So for example, if the first three events are:

```
[33, 27, 33, 23] [50, 42, 49, 37] [46, 35, 46, 33]
```

when flattened this becomes:
```
[33, 27, 33, 23, 50, 42, 49, 37, 46, 35, 46, 33]
```

These will be easy to plot in a 1D histogram.



5) Plot the number of hits on the MC-truth track in a histogram

<details>
<summary>Hint</summary>

```
mc_branches = ["trkmc.nactive"]
mc = processor.process_data(
    file_name=file_name,
    branches=mc_branches
)
mc_nactive = ak.flatten(mc["trkmc.nactive"], axis=None)
plotter.plot_1D(
    mc_nactive,               # Data to plot
    nbins=100,               # Number of bins
    xmin=0,                # Minimum x-axis value
    xmax=100,               # Maximum x-axis value
    xlabel="trkmc.nactive",
    ylabel="# occurances per bin",
    out_path='trkmc_nactive.png',  # Output file path
    stat_box=True,           # Show statistics box
    error_bars=True          # Show error bars
)
```
</details>

6) Plot the number of hits on the MC-truth track against the number of hits on the reconstructed track in a 2D histogram

<details>
<summary>Hint</summary>

```
plotter.plot_2D(
    x=mc_nactive,           # x-axis data
    y=nactive,              # y-axis data
    nbins_x=100,              # Number of x bins
    xmin=0,                  # Minimum x value
    xmax=100,                 # Maximum x value
    nbins_y=100,              # Number of y bins
    ymin=0,                 # Minimum y value
    ymax=100,                # Maximum y value
    xlabel="trkmc.nactive",
    ylabel="trk.nactive",
    out_path='2D_mc_rec_nactive.png'
)
```
</details>

7) Plot the number of hits in track fits that used the e-minus particle hypothesis

<details>
<summary>Hint</summary>

```
electron_mask = (data["trk.pdg"] == 11)
electron_trks = data["trk.nactive"].mask[electron_mask]
electron_nactive = ak.flatten(electron_trks, axis=None)

plotter.plot_1D(
    electron_nactive,               # Data to plot
    nbins=100,               # Number of bins
    xmin=0,                # Minimum x-axis value
    xmax=100,               # Maximum x-axis value
    # title="Time at Tracker Entrance",
    xlabel="electron trk.nactive",
    ylabel="# occurances per bin",
    out_path='trk_nactive_elec.png',  # Output file path
    stat_box=True,           # Show statistics box
    error_bars=True          # Show error bars
)
```
</details>


# Track Fit Selections

Now we will look at our the vector-of-vector branches: trksegs and the trksegsmc. These contain information about track segments.

`trksegs` is a vector-of-vector branch. You can see from the print out it is more complicated and nested than the branches we looked at previously.

Before completing this let's add `trksegs` to our list of branches:

```
branches = ["trksegs"]

data = processor.process_data(
    file_name=file_name,
    branches=branches
)
from pyutils.pyprint import Print
printer = Print(verbose=False)
printer.print_n_events(data, n_events=10)
```



## Challenges

8) Plot the reconstructed momentum of tracks at the middle of the tracker

<details>
<summary>Hint</summary>

```
trk_mid_mask = selector.select_surface(data, surface_name="TT_Mid")
trk_mid = data["trksegs"].mask[(trk_mid_mask)]
from pyutils.pyvector import Vector
vector = Vector()
mom_mag = vector.get_mag(trk_mid ,'mom')
mom_mag_flat = ak.flatten(mom_mag, axis=None)
from pyutils.pyplot import Plot 
plotter = Plot()
plotter.plot_1D(
    mom_mag_flat,               # Data to plot
    nbins=100,               # Number of bins
    xmin=95,                # Minimum x-axis value
    xmax=115,               # Maximum x-axis value
    xlabel="Reconstructed Momentum at Tracker Middle [MeV/c]",
    ylabel="# occurances per bin",
    out_path='trk_mom_mid.png',  # Output file path
    stat_box=True,           # Show statistics box
    error_bars=True          # Show error bars
)

```
</details>

Note that we have introduced `pyvector` here. This class uses the `vector` package (https://pypi.org/project/vector/) and allows the user to run the usual vector operations on the objects created here.

# Conclusions

This tutorial hasn't covered every single use-case of, or class in, `pyutils` but it has shown you how to:

* analyze EventNtuple datasets,
* select subsets of the data, and
* create histograms of selected data.

all within a python environment.


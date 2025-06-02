# `pyutils`: Mu2e Python utilities

`pyutils` is a suite of tools intended for Python-based analyses of Mu2e data, particularly `EventNtuple`. We use packages available in the standard Mu2e Python environment and provide functionality that will be common to many Mu2e analysis groups. The goal is to minimise the amount of overhead required when setting up an analysis.  

## 1. Setting up 

Setting up involves two simple steps: 

1. Activating the Mu2e Python environment
2. Setting your Python path
   
### 1.1 Activating the Mu2e Python environment

`pyutils` is designed to work with packages installed in the Mu2e Python environment, which is currently maintained by the L4 for Analysis Interfaces, Sam Grant.

To activate the environment, first run `mu2einit` and then **one** `pyenv` command, like

```
mu2einit # or "source /cvmfs/mu2e.opensciencegrid.org/setupmu2e-art.sh"
pyenv ana # Setup the current environment
pyenv rootana # Setup the current environment, plus ROOT for pyROOT users 
pyenv ana 1.2.0 # Setup a specific version
pyenv -h # Get help (--help and pyenv with no flag will also return help)
```

If `pyenv` does not work, please source the activation script directly, as shown below, and report the issue to Sam Grant.

```
source /cvmfs/mu2e.opensciencegrid.org/env/ana/current/bin/activate
```

Also note that `mu2einit` should be aliased to `source /cvmfs/mu2e.opensciencegrid.org/setupmu2e-art.sh` in your `~/.bashrc`.

See the [tutorial](https://github.com/Mu2e/Tutorial/blob/main/EAF/Docs/06-TheMu2eEnvironment.md) on GitHub and the [wiki](https://mu2ewiki.fnal.gov/wiki/Elastic_Analysis_Facility_(EAF)#The_Mu2e_environment) page for more information.

### 1.2 Setting your Python path

The `pyutils` path has been added to the `muse` Python path, so you import the scripts within your working directory if you have `muse` setup. If you do not want to use `muse`, you can append your system path at the start of your analysis script, as shown below. 

```python
import sys
sys.path.append("path/to/pyutils/")
```

>**Note**: `pip install pyutils` coming soon. 

## 2. Using `pyutils` 

The suite consists of the following modules.
```python
pyread      # Data reading 
pyprocess   # Listing and parallelisation 
pyimport    # TTree (EventNtuple) importing interface 
pyplot      # Plotting and visualisation 
pyprint     # Array visualisation 
pyselect    # Data selection and cut management 
pyvector    # Element wise wector operations
pymcutil    # Monte Carlo utilities (coming soon)
pylogger    # Helper module for managing printouts
```

### 2.1 Tutorials

To learn by example, follow the `pyutils` tutorial series.

1. [pyutils_basics.ipynb](../../example-analysis-scripts/pyutils-examples/pyutils_basics.ipynb) - Introduction to core functionality
1. [pyutils_on_EAF.ipynb](../../example-analysis-scripts/pyutils-examples/pyutils_on_EAF.ipynb) - Reading data with `pyutils` from the Elastic Analysis Facility (EAF) 
1. [pyutils_multifile.ipynb](../../example-analysis-scripts/pyutils-examples/pyutils_multifile.ipynb) - Basic parallelisation with file lists and SAM definitions, as well as complex parallelised analysis tasks using the `pyprocess` `Skeleton` template class.


### 2.2 Module documentation 

Help information be accessed with `help(name)`, where `name` can be the module name, a class name, or a function name. 

---

#### `pyread` 

Contains the `Reader` class: a utility for reading files with uproot (supports local and remote files). Called by `pyprocess`, which is the main interface for processing data.

<details>
<summary><strong>Click for details<strong></summary>
    
```
NAME
    pyread

CLASSES
    builtins.object
        Reader

    class Reader(builtins.object)
     |  Reader(use_remote=False, location='tape', schema='root', verbosity=1)
     |
     |  Unified interface for accessing files, either locally or remotely
     |
     |  Methods defined here:
     |
     |  __init__(self, use_remote=False, location='tape', schema='root', verbosity=1)
     |      Initialise the reader
     |
     |      Args:
     |          use_remote (bool, opt): Whether to use remote access methods
     |          location (str, opt): File location for remote files: tape (default), disk, scratch, nersc
     |          schema (str, opt): Schema for remote file path: root (default), http, path , dcap, samFile
     |          verbosity (int, opt): Level of output detail (0: errors only, 1: info & warnings, 2: max)
     |
     |  read_file(self, file_path)
     |      Read a file using the appropriate method
     |
     |      Args:
     |          file_path: Path to the file
     |
     |      Returns:
     |          Uproot file object
     |
     |  ----------------------------------------------------------------------

```
</details>

---

#### `pyimport`

Contains the `Importer` class: a utility for importing ROOT TTree branches into Awkward arrays. Called by `pyprocess`, which is the main interface for processing data.

<details>
<summary><strong>Click for details<strong></summary>

```
NAME
    pyimport

CLASSES
    builtins.object
        Importer

    class Importer(builtins.object)
     |  Importer(file_name, branches, dir_name='EventNtuple', tree_name='ntuple', use_remote=False, location='tape', schema='root', verbosity=1)
     |
     |  Utility class for importing branches from ROOT TTree files
     |
     |  Intended to used via by the pyprocess Processor class
     |
     |  Methods defined here:
     |
     |  __init__(self, file_name, branches, dir_name='EventNtuple', tree_name='ntuple', use_remote=False, location='tape', schema='root', verbosity=1)
     |      Initialise the importer
     |
     |      Args:
     |          file_name: Name of the file
     |          branches: Flat list or grouped dict of branches to import
     |          dir_name: Ntuple directory in file
     |          tree_name: Ntuple name in file directory
     |          use_remote: Flag for reading remote files
     |          location: Remote files only. File location: tape (default), disk, scratch, nersc
     |          schema: Remote files only. Schema used when writing the URL: root (default), http, path, dcap, samFile
     |          verbosity: Print detail level (0: minimal, 1: medium, 2: maximum)
     |
     |  import_branches(self)
     |      Internal function to open ROOT file and import specified branches
     |
     |      Returns:
     |          Awkward array with imported data
     |
     |  ----------------------------------------------------------------------
```

</details>

---

#### `pyprocess`

**This is the primary interface for processing data**: supports processing of single files, file lists, and SAM definitions. Contains the `Processor` class, which provides methods for producing file lists and parallel processing, where the `process_data` method provides a single entry point for these utilties. It also contains the `Skeleton` class, which provides a template class for performing complex analyses.

<details>
<summary><strong>Click for details<strong></summary>

```
NAME
    pyprocess

CLASSES
    builtins.object
        Processor
        Skeleton

    class Processor(builtins.object)
     |  Processor(dir_name='EventNtuple', tree_name='ntuple', use_remote=False, location='tape', schema='root', verbosity=1)
     |
     |  Interface for processing files or datasets
     |
     |  Methods defined here:
     |
     |  __init__(self, dir_name='EventNtuple', tree_name='ntuple', use_remote=False, location='tape', schema='root', verbosity=1)
     |      Initialise the processor
     |
     |      Args:
     |          dir_name (str, opt): Ntuple directory in file
     |          tree_name (str, opt): Ntuple name in file directory
     |          use_remote (bool, opt): Flag for reading remote files
     |          location (str, opt): Remote files only. File location: tape (default), disk, scratch, nersc
     |          schema (str, opt): Remote files only. Schema used when writing the URL: root (default), http, path, dcap, samFile
     |          verbosity (int, opt): Level of output detail (0: errors only, 1: info, warnings, 2: max)
     |
     |  get_file_list(self, defname=None, file_list_path=None)
     |      Utility to get a list of files from a SAM definition OR a text file
     |
     |      Args:
     |          defname: SAM definition name
     |          file_list_path: Path to a plain text file containing file paths
     |
     |      Returns:
     |          List of file paths
     |
     |  process_data(self, file_name=None, file_list_path=None, defname=None, branches=None, max_workers=None, custom_process_func=None, use_processes=False)
     |      Process the data
     |
     |      Args:
     |          file_name: File name
     |          defname: SAM definition name
     |          file_list_path: Path to file list
     |          branches: Flat list or grouped dict of branches to import
     |          max_workers: Maximum number of parallel workers
     |          custom_process_func: Optional custom processing function for each file
     |          use_processes: Whether to use processes rather than threads
     |
     |      Returns:
     |          - If custom_process_func is None: a concatenated awkward array with imported data from all files
     |          - If custom_process_func is not None: a list of outputs from the custom process
     |
     |  ----------------------------------------------------------------------

    class Skeleton(builtins.object)
     |  Skeleton(verbosity=1)
     |
     |  Template class for creating a custom analysis processor
     |
     |  This template demonstrates how to create a class to run
     |  custom analysis jobs with the Processor framework
     |
     |  To use this skeleton:
     |  1. Either initilaise the entire class or pass it as an argument to your Processor class
     |  2. Customize the __init__ method with your configuration
     |  3. Implement your processing logic in the process method
     |  4. Add any additional helper methods you need
     |  5. Override methods as needed
     |
     |  Methods defined here:
     |
     |  __init__(self, verbosity=1)
     |      Initialise your file processor with configuration parameters
     |
     |      Customise this method to include parameters specific to your analysis.
     |
     |      Args:
     |          verbosity (int, opt): Level of output detail (0: errors only, 1: info, 2: debug, 3: max)
     |
     |  execute(self)
     |      Run the processor on the configured files
     |
     |      Returns:
     |          Combined results from all processed files
     |
     |  process_file(self, file_name)
     |      Process a single file
     |
     |      This is the core method that will be called for each file.
     |      Implement your file processing logic here.
     |
     |      Args:
     |          file_name: Name of the file to process
     |
     |      Returns:
     |          Any data structure representing the processed result
     |
     |  process_results(self)
     |      Run post processing on the results list
     |
     |  ----------------------------------------------------------------------q
```

</details>

---

#### `pyplot`

Tools for creating publication-quality histograms and graphs from flattened arrays. It uses the `mu2e.mplstyle` style file by default, although users can choose to import a custom style file.

>**Note**: Does not support plotting for histogram objects. Working on resolving this.

<details>
<summary><strong>Click for details<strong></summary>

```
     |  Methods defined here:
     |
     |  __init__(self, style_path=None, verbosity=1)
     |      Initialise the Plot class.
     |
     |      Args:
     |          style_path (str, opt): Path to matplotlib style file. (Default: Mu2e style)
     |          verbosity (int, opt): Level of output detail (0: errors only, 1: info & warnings, 2: max)
     |
     |  get_stats(self, array, xmin, xmax)
     |      Calculate 'stat box' statistics from a 1D array.
     |
     |      Args:
     |        array (np.ndarray): Input array
     |        xmin (float): Minimum x-axis value
     |        xmax (float): Maximum x-axis value
     |
     |      Returns:
     |        tuple: (n_entries, mean, mean_err, std_dev, std_dev_err, underflows, overflows)
     |
     |  plot_1D(self, array, nbins=100, xmin=-1.0, xmax=1.0, weights=None, title=None, xlabel=None, ylabel=None, col='black', leg_pos='best', out_path=None, dpi=300, log_x=False, log_y=False, norm_by_area=False, under_over=False, stat_box=True, stat_box_errors=False, error_bars=False, ax=None, show=True)
     |      Create a 1D histogram from an array of values.
     |
     |      Args:
     |        array (np.ndarray): Input data array
     |        weights (np.ndarray, optional): Weights for each value
     |        nbins (int, optional): Number of bins. Defaults to 100
     |        xmin (float, optional): Minimum x-axis value. Defaults to -1.0
     |        xmax (float, optional): Maximum x-axis value. Defaults to 1.0
     |        title (str, optional): Plot title
     |        xlabel (str, optional): X-axis label
     |        ylabel (str, optional): Y-axis label
     |        col (str, optional): Histogram color. Defaults to 'black'
     |        leg_pos (str, optional): Legend position. Defaults to 'best'
     |        out_path (str, optional): Path to save the plot
     |        dpi (int, optional): DPI for saved plot. Defaults to 300
     |        log_x (bool, optional): Use log scale for x-axis. Defaults to False
     |        log_y (bool, optional): Use log scale for y-axis. Defaults to False
     |        under_over (bool, optional): Show overflow/underflow stats. Defaults to False
     |        stat_box (bool, optional): Show statistics box. Defaults to True
     |        stat_box_errors (bool, optional): Show errors in stats box. Defaults to False
     |        error_bars (bool, optional): Show error bars on bins. Defaults to False
     |        ax (plt.Axes, optional): External custom axes
     |        show (bool, optional): Display the plot, defaults to True
     |
     |      Raises:
     |        ValueError: If array is empty or None
     |
     |  plot_1D_overlay(self, hists_dict, weights=None, nbins=100, xmin=-1.0, xmax=1.0, title=None, xlabel=None, ylabel=None, out_path=None, dpi=300, leg_pos='best', log_x=False, log_y=False, norm_by_area=False, ax=None, show=True)
     |      Overlay multiple 1D histograms from a dictionary of arrays.
     |
     |      Args:
     |          hists_dict (Dict[str, np.ndarray]): Dictionary mapping labels to arrays
     |          weights (List[np.ndarray], optional): List of weight arrays for each histogram
     |          nbins (int, optional): Number of bins. Defaults to 100
     |          xmin (float, optional): Minimum x-axis value. Defaults to -1.0
     |          xmax (float, optional): Maximum x-axis value. Defaults to 1.0
     |          title (str, optional): Plot title
     |          xlabel (str, optional): X-axis label
     |          ylabel (str, optional): Y-axis label
     |          out_path (str, optional): Path to save the plot
     |          dpi (int, optional): DPI for saved plot. Defaults to 300
     |          leg_pos (str, optional): Legend position. Defaults to 'best'
     |          log_x (bool, optional): Use log scale for x-axis. Defaults to False
     |          log_y (bool, optional): Use log scale for y-axis. Defaults to False
     |          ax (plt.Axes, optional): External custom axes.
     |          show (bool, optional): Display the plot. Defaults to True
     |
     |      Raises:
     |          ValueError: If hists_dict is empty or None
     |          ValueError: If weights length doesn't match number of histograms
     |
     |  plot_2D(self, x, y, weights=None, nbins_x=100, xmin=-1.0, xmax=1.0, nbins_y=100, ymin=-1.0, ymax=1.0, title=None, xlabel=None, ylabel=None, zlabel=None, out_path=None, cmap='inferno', dpi=300, log_x=False, log_y=False, log_z=False, colorbar=True, ax=None, show=True)
     |      Plot a 2D histogram from two arrays of the same length.
     |
     |      Args:
     |          x (np.ndarray): Array of x-values
     |          y (np.ndarray): Array of y-values
     |          weights (np.ndarray, optional): Optional weights for each point
     |          nbins_x (int): Number of bins in x. Defaults to 100
     |          xmin (float): Minimum x value. Defaults to -1.0
     |          xmax (float): Maximum x value. Defaults to 1.0
     |          nbins_y (int): Number of bins in y. Defaults to 100
     |          ymin (float): Minimum y value. Defaults to -1.0
     |          ymax (float): Maximum y value. Defaults to 1.0
     |          title (str, optional): Plot title
     |          xlabel (str, optional): X-axis label
     |          ylabel (str, optional): Y-axis label
     |          zlabel (str, optional): Colorbar label
     |          out_path (str, optional): Path to save the plot
     |          cmap (str): Matplotlib colormap name. Defaults to 'inferno'
     |          dpi (int): DPI for saved plot. Defaults to 300
     |          log_x (bool): Use log scale for x-axis
     |          log_y (bool): Use log scale for y-axis
     |          log_z (bool): Use log scale for color values
     |          cbar (bool): Whether to show colorbar. Defaults to True
     |          ax (plt.Axes, optional): External custom axes.
     |          show (bool): show (bool, optional): Display the plot. Defaults to True
     |
     |      Raises:
     |          ValueError: If input arrays are empty or different lengths
     |
     |  plot_graph(self, x, y, xerr=None, yerr=None, title=None, xlabel=None, ylabel=None, xmin=None, xmax=None, ymin=None, ymax=None, col='black', linestyle='None', out_path=None, dpi=300, log_x=False, log_y=False, ax=None, show=True)
     |      Plot a scatter graph with optional error bars.
     |
     |      Args:
     |        x (np.ndarray): Array of x-values
     |        y (np.ndarray): Array of y-values
     |        xerr (np.ndarray, optional): X error bars
     |        yerr (np.ndarray, optional): Y error bars
     |        title (str, optional): Plot title
     |        xlabel (str, optional): X-axis label
     |        ylabel (str, optional): Y-axis label
     |        xmin (float, optional): Minimum x value
     |        xmax (float, optional): Maximum x value
     |        ymin (float, optional): Minimum y value
     |        ymax (float, optional): Maximum y value
     |        color (str): Marker and error bar color, defaults to 'black'
     |        linestyle (str): Style for connecting lines, defaults to 'None'
     |        out_path (str, optional): Path to save the plot
     |        dpi (int): DPI for saved plot. Defaults to 300
     |        log_x (bool): Use log scale for x-axis, defaults to False
     |        log_y (bool): Use log scale for y-axis, defaults to False
     |        ax (plt.Axes, optional): Optional matplotlib axes to plot on
     |        show (bool): Whether to display plot, defaults to True
     |
     |      Raises:
     |        ValueError: If input arrays have different lengths
     |
     |  plot_graph_overlay(self, graphs, title=None, xlabel=None, ylabel=None, xmin=None, xmax=None, ymin=None, ymax=None, legend_position='best', linestyle='None', out_path=None, log_x=False, log_y=False, dpi=300, ax=None, show=True)
     |      Overlay multiple scatter graphs with optional error bars.
     |
     |      Args:
     |        graphs (dict): Dictionary of graphs to plot, where each graph is a dictionary:
     |          {
     |            'label1': {
     |              'x': x_array,
     |              'y': y_array,
     |              'xerr': xerr_array,  # optional
     |              'yerr': yerr_array   # optional
     |            },
     |            'label2': {...}
     |          }
     |        title (str, optional): Plot title
     |        xlabel (str, optional): X-axis label
     |        ylabel (str, optional): Y-axis label
     |        xmin (float, optional): Minimum x value
     |        xmax (float, optional): Maximum x value
     |        ymin (float, optional): Minimum y value
     |        ymax (float, optional): Maximum y value
     |        leg_pos (str): Position of legend. Defaults to 'best'
     |        linestyle (str): Style for connecting lines. Defaults to 'None'
     |        out_path (str, optional): Path to save plot
     |        log_x (bool): Use log scale for x-axis, defaults to False
     |        log_y (bool): Use log scale for y-axis, defaults to False
     |        dpi (int): DPI for saved plot, defaults to 300
     |        ax (plt.Axes, optional): Optional matplotlib axes to plot on
     |        show (bool): Whether to display plot. Defaults to True
     |
     |      Raises:
     |          ValueError: If any graph data is malformed or arrays have different lengths
     |
     |  round_to_sig_fig(self, val, sf)
     |      Round a value to a specified number of significant figures.
     |
     |      Args:
     |          val (float): Value to round
     |          sf (int): Number of significant figures
     |
     |      Returns:
     |          float: Rounded value
     |
     |      Note:
     |          Returns original value for 0 or NaN inputs
     |
     |  ----------------------------------------------------------------------
```

</details>

---

#### `pyprint`

For array visualisation, allowing the user to print out the structure of their array per event in a human-readable format

<details>
<summary><strong>Click for details<strong></summary>
    
```
NAME
    pyprint

CLASSES
    builtins.object
        Print

    class Print(builtins.object)
     |  Print(verbose=False, precision=1)
     |
     |  Utility class for printing structured event data in a human-readable format.
     |
     |  This class provides methods to print individual events or multiple events from
     |  an Awkward array, handling nested fields and subfields recursively.
     |
     |  Methods defined here:
     |
     |  __init__(self, verbose=False, precision=1)
     |      Initialise Print
     |
     |      Args:
     |          verbose (bool, optional): Print full arrays without truncation. Defaults to False.
     |          precision (int, optional): Specifiy the number of decimal points when using verbose option. Defaults to 1.
     |
     |  print_event(self, event, prefix='')
     |      Print a single event in human-readable format, including all fields and subfields.
     |
     |      Args:
     |        event (awkward.Array): Event to print, containing fields and possibly subfields
     |        prefix (str, optional): Prefix to prepend to field names. Used for nested fields. Defaults to empty string.
     |
     |      Note:
     |        Recursively handles nested fields, e.g. field.subfield.value
     |
     |  print_n_events(self, array, n_events=1)
     |      Print the first n events from an array in human-readable format.
     |
     |      Args:
     |        array_ (awkward.Array): Array of events to print
     |        n (int, optional): Number of events to print. Defaults to 1.
     |
     |      Note:
     |        Prints a separator line between events for better readability.
     |        Events are numbered starting from 1.
     |
     |      Example:
     |        >>> printer = Print()
     |        >>> printer.PrintNEvents(events, n_events=2)
     |
     |        ---> Printing 2 event(s)...
     |
     |        -------------------------------------------------------------------------------------
     |        field1: value
     |        field2.subfield1: value
     |        -------------------------------------------------------------------------------------
     |
     |        -------------------------------------------------------------------------------------
     |        field1: value
     |        field2.subfield1: value
     |        -------------------------------------------------------------------------------------
     |
     |  ----------------------------------------------------------------------
```

</details>

---

#### `pyselect`

Tools for creating and managing selection cut masks. 

>**Note**: `MakeMask` and `MakeMaskList` may need revisiting; CutManager class for complex analyses coming soon. 

<details>
<summary><strong>Click for details<strong></summary>

```
NAME
    pyselect

CLASSES
    builtins.object
        Select

    class Select(builtins.object)
     |  Select(verbosity=1)
     |
     |  Class for standard selection cuts with EventNtuple data in Awkward format
     |
     |  Methods defined here:
     |
     |  MakeMask(self, branch, treename, leaf, eql, v1, v2=None)
     |      makes a mask for the chosen branch/leaf v1 = min, v2 = max, use eql if you want it == v1
     |
     |  MakeMaskList(self, branch, treenames, leaves, eqs, v1s, v2s)
     |      makes a mask for the chosen branch/leaf v1 = min, v2 = max, use eql if you want it == v1
     |
     |  __init__(self, verbosity=1)
     |      Initialise the selector
     |
     |      Args:
     |          verbosity (int, optional): Print detail level (0: minimal, 1: medium, 2: maximum). Defaults to 1.
     |
     |  hasTrkCrvCoincs(self, trks, ntuple, tmax)
     |      simple function to remove anything close to a crv coinc
     |
     |  has_n_hits(self, data, n_hits)
     |      Return boolean array for tracks with hits above a specified value
     |
     |      Hits in this context is nactive planes
     |
     |      Args:
     |          data (awkward.Array): Input array containing the trk.nactive branch
     |          n_hits (int): The minimum number of track hits (nactive)
     |
     |  is_downstream(self, data, branch_name='trksegs')
     |      Return boolean array for upstream track segments
     |
     |      Args:
     |          data (awkward.Array): Input array containing the segments branch
     |          branch_name (str, optional): Name of the segments branch for backwards compatibility. Defaults to 'trksegs'
     |
     |  is_electron(self, data)
     |      Return boolean array for electron tracks which can be used as a mask
     |
     |      Args:
     |          data (awkward.Array): Input array containing the "trk" branch
     |
     |  is_mu_minus(self, data)
     |      Return boolean array for negative muon tracks which can be used as a mask
     |
     |      Args:
     |          data (awkward.Array): Input array containing the "trk" branch
     |
     |  is_mu_plus(self, data)
     |      Return boolean array for positive muon tracks which can be used as a mask
     |
     |      Args:
     |          data (awkward.Array): Input array containing the "trk" branch
     |
     |  is_particle(self, data, particle)
     |      Return boolean array for tracks of a specific particle type which can be used as a mask
     |
     |      Args:
     |          data (awkward.Array): Input array containing the "trk" branch
     |          particle (string): particle type, 'e-', 'e+', 'mu-', or 'mu+'
     |
     |  is_positron(self, data)
     |      Return boolean array for positron tracks which can be used as a mask
     |
     |      Args:
     |          data (awkward.Array): Input array containing the "trk" branch
     |
     |  is_reflected(self, data, branch_name='trksegs')
     |      Return boolean array for reflected tracks
     |
     |      Reflected tracks have both upstream and downstream segments at the tracker entrance
     |
     |      Args:
     |          data (awkward.Array): Input array containing segments branch
     |          branch_name (str, optional): Name of the segments branch for backwards compatibility. Defaults to 'trksegs'
     |
     |  is_upstream(self, data, branch_name='trksegs')
     |      Return boolean array for downstream track segments
     |
     |      Args:
     |          data (awkward.Array): Input array containing the segments branch
     |          branch_name (str, optional): Name of the segments branch for backwards compatibility. Defaults to 'trksegs'
     |
     |  select_surface(self, data, sid, sindex=0, branch_name='trksegs')
     |      Return boolean array for track segments intersecting a specific surface
     |
     |      Args:
     |          data (awkward.Array): Input array containing segments branch
     |          sid (int): ID of the intersected surface
     |          sindex (int, optional): Index to the intersected surface (for multi-surface elements). Defaults to 0.
     |          branch_name (str, optional): Name of the segments branch for backwards compatibility. Defaults to 'trksegs'
     |
     |  select_trkqual(self, data, quality)
     |      Return boolean array for tracks above a specified quality
     |
     |      Args:
     |          data (awkward.Array): Input array containing the trkqual.result branch
     |          quality (float): The numerical output of the MVA
     |
     |  ----------------------------------------------------------------------
```
    
</details>

---

#### `pyvector`

Tools for 3D element-wise vector operations in a pure Python environment. 

<details>
<summary><strong>Click for details<strong></summary>

```
NAME
    pyvector

CLASSES
    builtins.object
        Vector

    class Vector(builtins.object)
     |  Vector(verbosity=1)
     |
     |  Methods for handling vector operations with Awkward arrays
     |
     |  Methods defined here:
     |
     |  __init__(self, verbosity=1)
     |      Initialise Vector
     |
     |      Args:
     |          Print detail level (0: minimal, 1: medium, 2: maximum)
     |
     |  get_mag(self, branch, vector_name)
     |      Return an array of vector magnitudes for specified branch
     |
     |      Args:
     |          branch (awkward.Array): The branch, such as trgsegs or crvcoincs
     |          vector_name: The parameter associated with the vector, such as 'mom' or 'pos'
     |
     |  get_vector(self, branch, vector_name)
     |      Return an array of XYZ vectors for specified branch
     |
     |      Args:
     |          branch (awkward.Array): The branch, such as trgsegs or crvcoincs
     |          vector_name: The parameter associated with the vector, such as 'mom' or 'pos'
     |
     |  ----------------------------------------------------------------------
```

</details>

---

#### `pylogger`

Helper module for managing printouts across the `pyutils` suite.


<details>
<summary><strong>Click for details<strong></summary>

    
```
NAME
    pylogger

CLASSES
    builtins.object
        Logger

    class Logger(builtins.object)
     |  Logger(verbosity=1, print_prefix='[pylogger]')
     |
     |  Helper class for consistent logging with emoji indicators
     |
     |  Methods defined here:
     |
     |  __init__(self, verbosity=1, print_prefix='[pylogger]')
     |      Initialize the Logger
     |
     |      Args:
     |          verbosity (int, opt): Level of output detail (0: errors only, 1: info, 2: max)
     |          print_prefix (str, opt): Prefix for printouts, e.g. "[pyprocess]"
     |
     |  log(self, message, level_name=None)
     |      Print a message based on verbosity level
     |
     |      Args:
     |          message (str): The message to print
     |          level (str, optional): Level name (error, info, success, warning, debug, max)
     |
     |  ----------------------------------------------------------------------
```

</details>

---

#### `pymcutil`

Utility for helping users to understand the MC origins of given tracks.

Development underway by Leo Borrel (contact for update).

<details>
<summary><strong>Click for details<strong></summary>

```
NAME
    pymcutil - #TODO
```

</details>

## Contact

Reach out via Slack (#analysis-tools or #analysis-tools-devel) if you need help or would like to contribute.


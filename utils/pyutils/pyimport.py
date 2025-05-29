import uproot
import awkward as ak
from pyread import Reader
from pylogger import Logger

class Importer:
    """Utility class for importing branches from ROOT TTree files

    Intended to used via by the pyprocess Processor class
    """
    
    def __init__(self, file_name, branches, dir_name="EventNtuple", tree_name="ntuple", use_remote=False, location="tape", schema="root", verbosity=1):
        """Initialise the importer
        
        Args:
            file_name: Name of the file
            branches: Flat list or grouped dict of branches to import
            dir_name: Ntuple directory in file 
            tree_name: Ntuple name in file directory
            use_remote: Flag for reading remote files 
            location: Remote files only. File location: tape (default), disk, scratch, nersc 
            schema: Remote files only. Schema used when writing the URL: root (default), http, path, dcap, samFile
            verbosity: Print detail level (0: minimal, 1: medium, 2: maximum) 
        """
        self.file_name = file_name
        self.branches = branches
        self.dir_name = dir_name
        self.tree_name = tree_name
        self.use_remote = use_remote
        self.location = location
        self.schema = schema
        self.verbosity = verbosity

        self.logger = Logger( # Start logger
            print_prefix = "[pyimport]", 
            verbosity = verbosity
        )
    
        # Create reader 
        self.reader = Reader(
            use_remote=self.use_remote,
            location=self.location,
            schema=self.schema,
            verbosity=self.verbosity 
        )
        
    def import_branches(self):
        """Internal function to open ROOT file and import specified branches
            
        Returns:
            Awkward array with imported data
        """

        # Get uproot object
        # Init file variable before try block
        # Reader has it's own try blocks
        file = self.reader.read_file(self.file_name) 
        
        try:
            # Access the tree
            if self.dir_name in file and self.tree_name in file[self.dir_name]:
                # Get tree
                tree = file[self.dir_name][self.tree_name]
                # Print tree info 
                self.logger.log("Accessing branches in tree:", "max")
                if self.verbosity > 1:
                    tree.show(filter_name=self.branches, interpretation_width=100)
                
                # Result container
                result = {}

                if self.branches is None: 
                    self.logger.log("Please provide a list of branches, or self.branches='*' to import all", "error")
                    return None
        
                # Flat list
                elif isinstance(self.branches, list):
                    result = tree.arrays(filter_name=self.branches, library="ak")
        
                # Grouped dictionary
                elif isinstance(self.branches, dict):
                    data = {}
                    # Get arrays per field/group
                    for group, sub_branches in self.branches.items():
                        data[group] = tree.arrays(filter_name=sub_branches, library="ak")
                    # Zip them together 
                    result = ak.zip(data) 
        
                # If using "*" get all branches
                elif self.branches == "*":
                    self.branches = [branch for branch in tree.keys()] 
                    self.logger.log("Importing all branches", "info")
                    # Return array 
                    result = tree.arrays(filter_name=self.branches, library="ak")
                    
                else: 
                    self.logger.log(f"Branches type {self.branches.type} not recognised", "error")
                    return None
                
                if result is not None:
                    self.logger.log(f"Imported branches", "success")
                    self.logger.log(f"Array structure:", "max")
                    if self.verbosity > 1:
                        result.type.show()
                else:
                    self.logger.log(f"Failed to import branches", "error")           

                # Return
                return result
                
            else:
                self.logger.log(f"Could not find tree {self.dir_name}/{self.tree_name} in file {self.file_name}", "error")
                return None
    
        except Exception as e:
            self.logger.log(f"Exception getting branches in file {self.file_name}: {e}", "error")
            return None

        finally:
            # Ensure the file is closed
            if hasattr(file, "close"):
                file.close()
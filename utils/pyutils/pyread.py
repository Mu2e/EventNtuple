import uproot
import subprocess

class Reader:
    """Unified interface for accessing files, either locally or remotely"""
    
    def __init__(self, use_remote=False, location="tape", schema="root", verbosity=1):
        """Initialise the reader
        
        Args:
            use_remote: Whether to use remote access methods
            location: Remote files only. File location: tape (default), disk, scratch, nersc 
            schema: Remote files only. Schema when writing the URL: root (default), http, path , dcap, samFile
            verbosity: Print detail level (0: minimal, 1: medium, 2: maximum) 
        """
        self.use_remote = use_remote # access files from outside NFS
        self.location = location
        self.schema = schema
        self.verbosity = verbosity 
        self.print_prefix = "[pyread] "

        # Check arguments for remote reading
        if self.use_remote:
            valid_locations = ["tape", "disk", "scratch", "nersc"]
            if self.location not in valid_locations:
                print(f"{self.print_prefix}⚠️ Location '{location}' may not be valid. Expected one of {valid_locations}")
            valid_schemas = ["root", "http", "path", "dcap", "samFile"]
            if self.schema not in valid_schemas:
                print(f"{self.print_prefix}⚠️ Schema '{schema}' may not be valid. Expected one of {valid_schemas}")

    def read_file(self, file_path):
        """Read a file using the appropriate method
        
        Args:
            file_path: Path to the file
            
        Returns:
            Uproot file object
        """
        if self.use_remote:
            return self._read_remote_file(file_path)
        else:
            return self._read_local_file(file_path)
    
    def _read_local_file(self, file_path):
        """Open a local file directly"""
        try: 
            file = uproot.open(file_path)
            if self.verbosity > 0:
                print(f"{self.print_prefix}✅  Opened {file_path}")
            return file
        except Exception as e:
            raise Exception(f"{self.print_prefix}❌  Exception while opening {file_path}: {e}") from e
    
    def _read_remote_file(self, file_path, location="tape", schema="root"):
        """Open a file from /pnfs via xroot"""
        if self.verbosity > 1:
            print(f"{self.print_prefix}Opening remote file: {file_path}")
        
        try:
            # Setup commands to generate xroot URL
            commands = "source /cvmfs/mu2e.opensciencegrid.org/setupmu2e-art.sh; muse setup ops;"
            commands += f"echo {file_path} | mdh print-url -l {self.location} -s {self.schema} -"
            # Execute commands
            xroot_url = subprocess.check_output(commands, shell=True, universal_newlines=True).strip()
            if self.verbosity > 1:
                print(f"{self.print_prefix}Created xroot URL: {xroot_url}")
            # Open the file using the xroot URL
            return self._read_local_file(xroot_url)
            
        except Exception as e1:
            # If there's an error, try copying to local and opening
            print(f"{self.print_prefix}❌  Exception while opening {file_path}: {e1}")
            print(f"{self.print_prefix}Retrying with local copy...")

            try:
                commands = 'source /cvmfs/mu2e.opensciencegrid.org/setupmu2e-art.sh; muse setup ops;'
                commands += f'echo {file_path} | mdh copy-file -s tape -l local -'
                
                # Execute the copy command
                subprocess.check_output(commands, shell=True, universal_newlines=True)
                
                # Open the file directly after copying
                return self._read_local_file(file_path)
            
            except Exception as e2:
                raise Exception(f"{self.print_prefix}❌  Failed to open local copy: {e2}") from e2
import os
import subprocess
from concurrent.futures import ThreadPoolExecutor, as_completed
import awkward as ak

class Processor:
    """Handles file list operations and parallel processing of multiple files"""
    
    def __init__(self, verbosity=1):
        """Initialise the processor
        
        Args:
            verbosity: Print detail level (0: minimal, 1: medium, 2: maximum) 
        """
        self.verbosity = verbosity
        self.print_prefix = "[pyprocess] "

    def get_file_list(self, defname=None, file_list_path=None):
        """Get a list of files from a SAM definition OR a text file
        
        Args:
            defname: SAM definition name 
            file_list_path: Path to a plain text file containing file paths
            
        Returns:
            List of file paths
        """
        
        # Check if a file list path was provided
        if file_list_path:
            try:
                if not os.path.exists(file_list_path):
                    print(f"{self.print_prefix}❌  File list path does not exist: {file_list_path}")
                    return []

                if self.verbosity > 1:
                     print(f"{self.print_prefix}Loading file list from {file_list_path}") 
                    
                with open(file_list_path, "r") as file_list:
                    file_list = file_list.readlines()
                    
                file_list = [line.strip() for line in file_list if line.strip()]

                if file_list and self.verbosity > 0:
                    print(f"{self.print_prefix}✅  Successfully loaded file list\n\tPath: {file_list_path}\n\tCount: {len(file_list)} files")
                
                return file_list
                
            except Exception as e:
                print(f"{self.print_prefix}❌  Error reading file list from {file_list_path}: {e}")
                return []
        
        # Otherwise, try to use the SAM definition
        elif defname:
            if self.verbosity > 1:
                print(f"{self.print_prefix}Loading file list for SAM definition: {defname}")
            
            try:
                # Setup commands for SAM query
                commands = 'source /cvmfs/mu2e.opensciencegrid.org/setupmu2e-art.sh; muse setup ops;'
                commands += f"samweb list-files 'defname: {defname} with availability anylocation' | sort -V"
                
                # Execute commands
                file_list_output = subprocess.check_output(commands, shell=True, universal_newlines=True)
                file_list = [line for line in file_list_output.splitlines() if line]

                if file_list and self.verbosity > 0:
                    print(f"{self.print_prefix}✅  Successfully loaded file list\n\tSAM definition: {defname}\n\tCount: {len(file_list)} files")
                
                # Return the file list
                return file_list
                
            except Exception as e:
                print(f"{self.print_prefix}❌  Exception while getting file list for {defname}: {e}")
                return []
        
        else:
            print(f"{self.print_prefix}❌  Error: Either 'defname' or 'file_list_path' must be provided")
            return []        

    def process_files_parallel(self, file_list, process_func, max_workers=None):
        """Process multiple files in parallel with a given a process function
        
        Args:
            file_list: List of files to process
            process_func: Function to call for each file (must accept file name as first argument)
            max_workers: Maximum number of worker threads
            
        Returns:
            List of results from each processed file
        """
        
        if not file_list:
            print(f"{self.print_prefix}❌  Warning: Empty file list provided")
            return None
            
        if max_workers is None:
            # Return a sensible default for max threads
            max_workers = min(len(file_list), 2*os.cpu_count() or 4) 

        print(f"{self.print_prefix}Starting processing on {len(file_list)} files with {max_workers} workers")

        # Store results in a list
        results = []  

        # For tracking progress
        completed_files = 0 
        total_files = len(file_list)

        # Start thread pool executor
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            # Create futures for each file processing task
            futures = {executor.submit(process_func, file_name): file_name for file_name in file_list}
            
            # Process results as they complete
            for future in as_completed(futures):
                filename = futures[future]
                try:
                    result = future.result()
                    if result is not None:
                        results.append(result)
                    
                    completed_files += 1
                    percent_complete = (completed_files / total_files) * 100
                    
                    # Extract just the base filename for cleaner output
                    base_filename = filename.split('/')[-1]
                    
                    if self.verbosity > 0:
                        print(f"{self.print_prefix}✅  {base_filename}")
                        print(f"\tProgress: {completed_files}/{total_files} files ({percent_complete:.1f}%)\n")
                    
                except Exception as e:
                    print(f"{self.print_prefix}❌  Error processing {filename}:\n{e}")

        # Report completion status
        print(f"{self.print_prefix}✅  Done: {completed_files}/{total_files} files processsed")
        
        # Return the results
        return results
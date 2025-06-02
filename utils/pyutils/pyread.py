#! /usr/bin/env python
import uproot
import os
import subprocess
import _env_manager # Environment manager
from pylogger import Logger # Messaging/logging

class Reader:
    """Unified interface for accessing files, either locally or remotely"""
    
    def __init__(self, use_remote=False, location="tape", schema="root", verbosity=1):
        """Initialise the reader
        
        Args:
            use_remote (bool, opt): Whether to use remote access methods
            location (str, opt): File location for remote files: tape (default), disk, scratch, nersc 
            schema (str, opt): Schema for remote file path: root (default), http, path , dcap, samFile
            verbosity (int, opt): Level of output detail (0: errors only, 1: info & warnings, 2: max)
        """
        self.use_remote = use_remote # access files from outside NFS
        self.location = location
        self.schema = schema
        
        self.logger = Logger( # Start logger
            print_prefix = "[pyread]", 
            verbosity = verbosity
        )
        # Setup and validation for remote reading
        if self.use_remote:
            #  Ensure mdh environment
            _env_manager.ensure_environment()  
            # Check arguments
            valid_locations = ["tape", "disk", "scratch", "nersc"]
            if self.location not in valid_locations:
                self.logger.log(f"Location '{location}' may not be valid. Expected one of {valid_locations}", "warning")
            valid_schemas = ["root", "http", "path", "dcap", "samFile"]
            if self.schema not in valid_schemas:
                self.logger.log(f"Schema '{schema}' may not be valid. Expected one of {valid_schemas}", "warning")

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
            self.logger.log(f"Opened {file_path}", "success")
            return file
        except Exception as e:
            self.logger.log(f"Error while opening {file_path}", "error")
            raise Exception(f"\tException: {e}") from e
    
    def _read_remote_file(self, file_path, location="tape", schema="root"):
        """Open a file from /pnfs via mdh"""
        self.logger.log(f"Opening remote file: {file_path}", "info")
        
        try:
            # Setup commands
            commands = f"mdh print-url {file_path} -l {self.location} -s {self.schema}"
            # Execute commands
            file_path = subprocess.check_output(commands, shell=True, universal_newlines=True, stderr=subprocess.DEVNULL).strip() 
            self.logger.log(f"Created file path: {file_path}", "info")
            # Open the file using the xroot URL
            return self._read_local_file(file_path)
            
        except Exception as e1:
            # If there's an error, try copying to local and opening
            self.logger.log(f"Exception while opening {file_path}: {e1}", "error")
            self.logger.log("Retrying with local copy...", "info")

            try:
                # Setup commands
                commands = f"mdh copy-file {file_path} -s tape -l local"
                
                # Execute the copy command
                subprocess.check_output(commands, shell=True, universal_newlines=True, stderr=subprocess.DEVNULL)
                
                # Open the file directly after copying
                return self._read_local_file(file_path)
            
            except Exception as e2:
                self.logger.log("Failed to open local copy", "error")
                raise Exception(f"Exception: {e2}") from e2
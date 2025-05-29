#! /usr/bin/env python
import uproot
import awkward as ak
import math
import numpy as np
import vector
from pylogger import Logger

class Vector:

    """ 
    Methods for handling vector operations with Awkward arrays
    """
  
    def __init__(self, verbosity=1):
        """  Initialise Vector

        Args:
            Print detail level (0: minimal, 1: medium, 2: maximum) 
            
        """
        self.verbosity = verbosity
        self.logger = Logger( # Start logger
            print_prefix = "[pyvector]", 
            verbosity = self.verbosity
        )

        # Register vector behaviours with awkward arrays
        vector.register_awkward()

        # Confirm init
        self.logger.log(f"Initialised Vector with verbosity = {self.verbosity}", "info")
    
    def get_vector(self, branch, vector_name):
        """ Return an array of XYZ vectors for specified branch

        Args:
            branch (awkward.Array): The branch, such as trgsegs or crvcoincs
            vector_name: The parameter associated with the vector, such as 'mom' or 'pos'
        """     
        # Get the vector
        try:
            vector = ak.zip({
                "x": branch[str(vector_name)]["fCoordinates"]["fX"],
                "y": branch[str(vector_name)]["fCoordinates"]["fY"],
                "z": branch[str(vector_name)]["fCoordinates"]["fZ"],
            }, with_name="Vector3D")
        except:
            # Try different syntax
            try:
                vector = ak.zip({
                    "x": branch[f"{vector_name}.fCoordinates.fX"],
                    "y": branch[f"{vector_name}.fCoordinates.fY"],
                    "z": branch[f"{vector_name}.fCoordinates.fZ"],
                }, with_name="Vector3D")
            except Exception as e:
                self.logger.log(f"Failed to create 3D vector: {e}", "error")
                return None

        self.logger.log(f"Created 3D '{vector_name}' vector", "success")
        if self.verbosity > 1:
            vector.type.show()
            
        return vector
  
    def get_mag(self, branch, vector_name):
        """ Return an array of vector magnitudes for specified branch

        Args:
            branch (awkward.Array): The branch, such as trgsegs or crvcoincs
            vector_name: The parameter associated with the vector, such as 'mom' or 'pos'
        """
        vector = self.get_vector(
        branch=branch,
            vector_name=vector_name
        )
        try: 
            mag = vector.mag
        except Exception as e:
            self.logger.log(f"Failed to get vector magnitude: {e}", "error")
            return None
        
        self.logger.log(f"Got '{vector_name}' magnitude", "success")
        if self.verbosity > 1:
            mag.type.show()
        
        return mag

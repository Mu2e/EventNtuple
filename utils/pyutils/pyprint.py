#! /usr/bin/env python
import awkward as ak
from pylogger import Logger

class Print:
    """
    Utility class for printing structured event data in a human-readable format.
    
    This class provides methods to print individual events or multiple events from
    an Awkward array, handling nested fields and subfields recursively.
    """
    def __init__(self, verbose=False, precision=1):
        """ Initialise Print
        
        Args:
            verbose (bool, optional): Print full arrays without truncation. Defaults to False.
            precision (int, optional): Specifiy the number of decimal points when using verbose option. Defaults to 1. 
        """ 
        self.verbose = verbose  
        self.precision = precision

        self.logger = Logger( # Start logger
            print_prefix = "[pyprint]", 
            verbosity = 1
        )

        self.logger.log(f"Initialised Print with verbose = {self.verbose} and precision = {self.precision}", "info")

    def _set_precision(self, value):
        """
        Helper function to set the precision of value array elements 
        
        Args:
            value: The value to format
            
        Returns:
            Formatted value 
        """
        if isinstance(value, float):
            return float(f"{value:.{self.precision}f}")
        elif isinstance(value, list):
            return [self._set_precision(item) for item in value]
        else:
            return value
    
    def print_event(self, event, prefix=''):
        """
        Print a single event in human-readable format, including all fields and subfields.
        
        Args:
          event (awkward.Array): Event to print, containing fields and possibly subfields
          prefix (str, optional): Prefix to prepend to field names. Used for nested fields. Defaults to empty string.
                                  
        Note:
          Recursively handles nested fields, e.g. field.subfield.value
        """ 
        for field in event.fields: # Loop through array elements in the event
            value = event[field] # Get the value
            full_field = f"{prefix}{field}" # Set full field using prefix provided in function call
            if hasattr(value, "fields") and value.fields: # Check for subfields 
                self.print_event(value, prefix=f"{full_field}.")  # Recurse into subfields
            else: # If no further subfields, print the full field and value
                if self.verbose and hasattr(value, "__iter__"): # Print full array 
                    try:
                        # Convert ak.array to list
                        value = ak.to_list(value)
                        # Format the values with specified precision
                        value = self._set_precision(value)
                    except Exception as e:
                        self.logger.log(f"Exception on {full_field}: {e}", "error")
                # Print array 
                print(f"{full_field}: {value}")
    
    def print_n_events(self, array, n_events=1):
        """
        Print the first n events from an array in human-readable format.
            
        Args:
          array_ (awkward.Array): Array of events to print
          n (int, optional): Number of events to print. Defaults to 1.
                
        Note:
          Prints a separator line between events for better readability.
          Events are numbered starting from 1.
                
        Example:
          >>> printer = Print()
          >>> printer.PrintNEvents(events, n_events=2)
                
          ---> Printing 2 event(s)...
                
          -------------------------------------------------------------------------------------
          field1: value
          field2.subfield1: value
          -------------------------------------------------------------------------------------
          
          -------------------------------------------------------------------------------------
          field1: value
          field2.subfield1: value
          -------------------------------------------------------------------------------------
        """
        self.logger.log(f"Printing {n_events} event(s)...\n", "info")
        
        for i, event in enumerate(array, start=1): # Iterate event-by-event 
            print("-"*85)
            self.print_event(event) # Call self.print_event() 
            print("-"*85)
            print()
            if i == n_events: # Return if 'n_events' is reached
                return 
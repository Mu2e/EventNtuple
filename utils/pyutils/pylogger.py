#! /usr/bin/env python
class Logger:
    """Helper class for consistent logging with emoji indicators
    """ 
    
    def __init__(self, verbosity=1, print_prefix="[pylogger]"): 
        """Initialize the Logger
        
        Args:
            verbosity (int, opt): Level of output detail (0: errors only, 1: info, 2: debug, 3: max)
            print_prefix (str, opt): Prefix for printouts, e.g. "[pyprocess]" 
        """
        self.verbosity = verbosity
        self.print_prefix = print_prefix
        
        # Define log levels with their corresponding icons
        self.LOG_LEVELS = {
            "error": {"icon": "❌", "level": 0},
            "test": {"icon": "🧪", "level": 0}, # for pytest
            "info": {"icon": "⭐️", "level": 1},
            "success": {"icon": "✅", "level": 1},
            "warning": {"icon": "⚠️", "level": 1},
            "max": {"icon": "👀", "level": 2}
        }
        
    def log(self, message, level_name=None):
        """Print a message based on verbosity level
        
        Args:
            message (str): The message to print
            level (str, optional): Level name (error, info, success, warning, debug, max)
        """
        # Determine the log level based on keywords in the message if not explicitly provided
        if level_name is None:
            level_name = self._detect_level(message)

        # Get icon 
        icon = self.LOG_LEVELS[level_name]["icon"]

        # Get level value
        level_value = self.LOG_LEVELS[level_name]["level"]
        
        # Only print if the inherited verbosity is high enough
        if self.verbosity >= level_value:
            print(f"{self.print_prefix} {icon} {message}")
    
    def _detect_level(self, message):
        """Automatically detect appropriate log level based on message content
        
        Args:
            message (str): The message 
            
        Returns:
            str: Detected log level name
        """

        # Convert message to lower case 
        message = message.lower()

        if "error" in message or "fail" in message:
            return "error"
        elif "complete" in message or "success" in message or "done" in message:
            return "success"
        elif "warn" in message:
            return "warning"
        elif "max" or "debug" in message:
            return "max"
        else:
            return "info"
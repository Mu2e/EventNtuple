# Sam Grant 2025 
# Internal helper to set up the environment 

import os
import subprocess
from pylogger import Logger

ENV_IS_SETUP = False

def setup_environment():
    """Set up the environment variables once per process"""
    global ENV_IS_SETUP
    
    logger = Logger(print_prefix="[pyutils]")
    
    if not ENV_IS_SETUP:
        logger.log("Setting up...", "info")
        try:
            # Step 0: unset the X509_USER_PROXY in the current process 
            if "X509_USER_PROXY" in os.environ:
                del os.environ["X509_USER_PROXY"]
            
            # Step 1: Get token
            token_cmd = "/cvmfs/mu2e.opensciencegrid.org/bin/getToken"
            logger.log(f"Running: {token_cmd}", "max")
            
            # Capture both stdout and stderr for debugging
            token_result = subprocess.run(
                token_cmd, 
                shell=True, 
                capture_output=True, 
                text=True,
                env=os.environ.copy()  # Use current environment
            )
            
            if token_result.returncode != 0:
                logger.log(f"getToken failed: {token_result.stderr}", "error")
                return False
                
            # Step 2: Setup mu2e environment and get all environmentals
            setup_cmd = "source /cvmfs/mu2e.opensciencegrid.org/setupmu2e-art.sh; muse setup ops; env"
            
            result = subprocess.check_output(
                setup_cmd, 
                shell=True, 
                universal_newlines=True,
                env=os.environ.copy(),  # Use current environment with token
                stderr=subprocess.DEVNULL # Suppress error messages. FIXME: use mdh directly if you can
            )
            
            # Parse and set environment variables
            for line in result.strip().split('\n'):
                if '=' in line:
                    key, value = line.split('=', 1)
                    os.environ[key] = value
            
            ENV_IS_SETUP = True
            logger.log("Ready", "success")
            return True
            
        except subprocess.CalledProcessError as e:
            logger.log(f"Failed to set environment variables: {e}", "error")
            return False
        except Exception as e:
            logger.log(f"Unexpected error: {e}", "error")
            return False
    
    return True

def ensure_environment():
    """Ensure environment is set up before using mdh"""
    if not ENV_IS_SETUP:
        setup_environment()

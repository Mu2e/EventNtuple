#!/usr/bin/env python3
"""
Test script to run EventNtuple fcl files on datasets.
Similar to test_fcls.sh but in Python for better control and error handling.
"""

import os
import subprocess
import sys
import argparse
from pathlib import Path

from enum import Enum

# Define the enum
class Result(Enum):
    FAILED = 0
    PASSED = 1
    SKIPPED = 2

def run(cmd, datasets_dict):
    """
    Run a test
    
    Args:
        test (str): Command to run
    
    Returns:
        bool: True if successful, False otherwise
    """
    """ if not os.path.exists(fcl_file):
        print(f"Error: FCL file not found: {fcl_file}")
        return False
    
    filelist_path = Path("../filelists/") / f"{input_dataset}.list"
    if not os.path.exists(filelist_path):
        # Get the filelist using metacat
        cmd = [f"muse setup ops && metacat query files from mu2e:{input_dataset} | mdh print-url -l tape - > {filelist_path}"]
        print(f"Creating filelist for dataset: {input_dataset}")
        try:
            subprocess.run(cmd, shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(f"Error: Failed to create filelist for dataset: {input_dataset}")
            return False
    
    # Build mu2e command
    cmd = ["mu2e", "-c", fcl_file]
    
    # Add input dataset
    cmd.extend(["-S", filelist_path.as_posix()])
    
    # Add output file
    cmd.extend(["-o", output_file])
    
    # Add max events if specified
    if max_events:
        cmd.extend(["-n", str(max_events)]) """

    # Check for a filelists/ directory
    filelist_dir = Path("../filelists/")
    if not os.path.exists(filelist_dir):
        print(f"Target directory does not exist: {filelist_dir}")
        reply = input("Would you like to create this new directory? (y/n): ").strip().lower()
    
        if reply in ('y', 'yes'):
            try:
                # parents=True creates intermediate folders if missing (like mkdir -p)
                filelist_dir.mkdir(parents=True, exist_ok=True)
                print(f"Successfully created directory: {filelist_dir}")
            except PermissionError:
                print(f"Permission denied: Cannot create directory at {filelist_dir}")
            except Exception as e:
                print(f"Failed to create directory. Error: {e}")
        else:
            print("Directory creation canceled by user.")
            return Result.FAILED
    
    # Search for dataset name and turn into filelist
    input_dataset = cmd.split('-S ')
    if len(input_dataset)>1:
        input_dataset = input_dataset[1].split()[0]
        if input_dataset in datasets_dict:
            old_input_dataset = input_dataset
            input_dataset = datasets_dict[input_dataset]
            if (input_dataset == "N/A"):
                print(f"Warning: Dataset name for '{old_input_dataset}' is marked as N/A in datasets file. Skipping test...")
                return Result.SKIPPED # returning SKIPPED to avoid counting as a failure, but skipping the test since dataset is not available
            cmd = cmd.replace(old_input_dataset, input_dataset)
        else:
            if input_dataset.startswith("{"):
                print(f"Warning: Dataset name '{input_dataset}' looks like it should be in the datasets file but it can't be found. Skipping test...")
                return Result.SKIPPED
            else:
                print(f"Warning: Dataset name '{input_dataset}' not found in datasets file. Using as-is.")
        
        # Check for filelist
        filelist_path = Path("../filelists/") / f"{input_dataset}.list"
        if not os.path.exists(filelist_path):
            # Get the filelist using metacat
            subcmd = [f"muse setup ops && metacat query files from mu2e:{input_dataset} | mdh print-url -l tape - > {filelist_path}"]
            print(f"Creating filelist for dataset: {input_dataset}")
            try:
                subprocess.run(subcmd, shell=True, check=True)
            except subprocess.CalledProcessError as e:
                print(f"Error: Failed to create filelist for dataset: {input_dataset}")
                return FAILED
        cmd = cmd.replace(input_dataset, str(filelist_path))

    print(f"Running: {cmd}")
    
    try:
        result = subprocess.run(cmd.split(), check=True, capture_output=True, text=True)
        print(f"✓ Successfully processed: {cmd}")
        #print(f"  Output: {output_file}")
        return Result.PASSED
    except subprocess.CalledProcessError as e:
        print(f"✗ Error processing {cmd}")
        print(f"  Stdout: {e.stdout}")
        print(f"  Stderr: {e.stderr}")
        return Result.FAILED

    except FileNotFoundError as e:
        print(f"{e}")
        return Result.FAILED


def main():
    parser = argparse.ArgumentParser(
        description="Run EventNtuple fcl files on datasets"
    )
    parser.add_argument(
        "--tests",
        help="Path to file containing list of tests to run (one command per line)",
        required=True
    )
    parser.add_argument(
        "--datasets",
        help="Path to file containing list of dataset names",
        required=True
    )
    
    args = parser.parse_args()
    
    # Check if batch processing requested
    datasets = { }
    if not os.path.exists(args.datasets):
        print(f"Error: Datasets file not found: {args.datasets}")
        sys.exit(1)
    with open(args.datasets, 'r') as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith('#'):
                parts = line.split()
                if len(parts) == 2:
                    datasets[parts[0]] = parts[1]
                else:
                    print(f"Warning: Invalid line in datasets file (skipping): {line}")

    if not os.path.exists(args.tests):
        print(f"Error: Tests file not found: {args.tests}")
        sys.exit(1)
        
    results = []
    with open(args.tests, 'r') as f:
        tests = [line.strip() for line in f if line.strip() and not line.strip().startswith('#')]
        
    print(f"Processing {len(tests)} tests...")
        
    for i, (test) in enumerate(tests, 1):
        print(f"\n[{i}/{len(tests)}] Processing: {test}")
        result = run(test, datasets)
        results.append((test, result))
        
    # Print summary
    print("\n" + "="*60)
    print("TEST PROCESSING SUMMARY")
    print("="*60)
    passed = sum(1 for _, result in results if result == Result.PASSED)
    failed = sum(1 for _, result in results if result == Result.FAILED)
    skipped = sum(1 for _, result in results if result == Result.SKIPPED)
    print(f"Total: {len(results)}, Passed: {passed}, Failed: {failed}, Skipped: {skipped}")
      
    if passed < len(results):
        print("\nFailed datasets:")
        for command, result in results:
            if result == Result.FAILED:
                print(f"  - {command}")
        sys.exit(1)


if __name__ == "__main__":
    main()

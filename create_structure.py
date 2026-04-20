#!/usr/bin/env python3
import os
import sys

# Change to script directory
script_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(script_dir)

# Create directories
dirs = ['src', 'tests', 'examples', '.obj']
for d in dirs:
    os.makedirs(d, exist_ok=True)
    print(f"Created directory: {d}")

print("\nDirectories setup complete!")

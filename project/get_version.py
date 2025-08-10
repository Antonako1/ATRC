# prints version of the project
import os

file = "./VERSION"
try:
    with open(file, "r") as f:
        version = f.read().strip()
        print(version)
except FileNotFoundError:
    print(f"File {file} not found. Assuming version 0.0.")
except ValueError:
    print(f"Invalid content in {file}. Assuming version 0.0.")
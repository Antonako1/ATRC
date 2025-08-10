# Usage python inc_version.py <major> <minor>
import sys

if len(sys.argv) != 3:
    print("Usage: python inc_version.py <major> <minor>")
    sys.exit(1)

major = int(sys.argv[1])
minor = int(sys.argv[2])
file = "./VERSION"
try:
    with open(file, "r") as f:
        version = f.read().strip()
        major_current, minor_current = map(int, version.split('.'))
except FileNotFoundError:
    print(f"File {file} not found. Creating a new one with version {major}.{minor}.")
    major_current, minor_current = 0, 0
except ValueError:
    print(f"Invalid content in {file}. Resetting version to {major}.{minor}.")
    major_current, minor_current = 0, 0
# Increment the version
major_current += major
minor_current += minor
# Write the new version back to the file
with open(file, "w") as f:
    f.write(f"{major_current}.{minor_current}")
print(f"Version updated to {major_current}.{minor_current}.")
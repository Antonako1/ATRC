# Increases the build number in the BUILDNUMBER file.
file="./BUILDNUMBER"
build_number = 0
try:
    with open(file, "r") as f:
        build_number = int(f.read().strip())
except FileNotFoundError:
    print(f"File {file} not found. Creating a new one with build number 0.")
except ValueError:
    print(f"Invalid content in {file}. Resetting build number to 0.")
    build_number = 0

build_number += 1
with open(file, "w") as f:
    f.write(str(build_number))
print(f"Build number updated to {build_number}.")
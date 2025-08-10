# Outputs the current build number in hexadecimal format.
file = "./BUILDNUMBER"
try:
    with open(file, "r") as f:
        build_number = int(f.read().strip())
except FileNotFoundError:
    print(f"File {file} not found. Assuming build number 0.")
    build_number = 0
except ValueError:
    print(f"Invalid content in {file}. Assuming build number 0.")
    build_number = 0
buildnum_hex = hex(build_number)[2:].lower()  # Convert to hex and remove '0x' prefix, then convert to uppercase
print(buildnum_hex)
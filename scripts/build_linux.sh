#!/bin/bash

# Define the root of your project
cd "$(dirname "$0")/.."
PROJECT_ROOT=$(pwd)
VERSION=$(< "${PROJECT_ROOT}/project/VERSION")
OUT_DIR="${PROJECT_ROOT}/out"
BUILD_NUMBER=$(< "${PROJECT_ROOT}/project/BUILDNUMBER")
# convert BUILD_NUMBER to hexadecimal
BUILD_NUMBER=$(printf '%x' "$BUILD_NUMBER")
# Remove leading zeros from BUILD_NUMBER
BUILD_NUMBER=${BUILD_NUMBER#0}
# Ensure BUILD_NUMBER is at least 1 character long
if [ -z "$BUILD_NUMBER" ]; then
    BUILD_NUMBER="1"
fi
#convert to lowercase
BUILD_NUMBER=$(echo "$BUILD_NUMBER" | tr '[:upper:]' '[:lower:]')
ATRC_DIR="${PROJECT_ROOT}/ATRC-${VERSION}_${BUILD_NUMBER}"

# Initialize error handling
ERROR_ENCOUNTERED=0
ATTEMPT_COUNT=2
counter=$1

# Array of presets for Linux
declare -a presets=(
    "linux-x64-debug" "linux-x64-release"
)

# Create the ATRC directory structure
mkdir -p "${ATRC_DIR}/Linux/x64/Debug"
mkdir -p "${ATRC_DIR}/Linux/x64/Release"

# Loop through presets
for preset in "${presets[@]}"; do
    echo "Building preset: $preset"

    # Determine build type and architecture
    build_type=$( [[ "$preset" == *"debug"* ]] && echo "Debug" || echo "Release" )
    atrc_arch=$( echo "x64" )
    toolchain_file="${PROJECT_ROOT}/cmake/toolchain-linux-${atrc_arch}.cmake"
    
    # Generate build files
    mkdir -p "${OUT_DIR}/${preset}/build"
    echo "Generating build files for preset: $preset"

    command="cmake -G Ninja --preset $preset -B${OUT_DIR}/${preset}/build -DCMAKE_BUILD_TYPE=$build_type -DCMAKE_TOOLCHAIN_FILE=$toolchain_file -DATRC_BUILD_TESTS=OFF "
    echo "Running: $command"
    eval $command
    if [ $? -ne 0 ]; then
        echo "Error encountered with: $command"
        ERROR_ENCOUNTERED=1
        break
    fi

    # Build the project
    command="cmake --build ${OUT_DIR}/${preset}/build --config $build_type --parallel"
    echo "Running: $command"
    eval $command
    if [ $? -ne 0 ]; then
        echo "Error encountered with: $command"
        ERROR_ENCOUNTERED=1
        break
    fi

    # Copy built library files to the output directory
    echo "Copying .so files to ${ATRC_DIR}/Linux/${atrc_arch}/${build_type}"

    cp -r "${OUT_DIR}/${preset}/build/ATRC/libATRC.so" "${ATRC_DIR}/Linux/${atrc_arch}/${build_type}/libATRC.so"
done

# Handle build errors
if [ "$ERROR_ENCOUNTERED" -eq 1 ]; then
    echo "Build process failed. Attempt: $counter"
    counter=$((counter + 1))
    if [ "$counter" -lt $ATTEMPT_COUNT ]; then
        echo "Retrying the build process... Attempt: $counter"
        exec "$0" "$counter"
    fi
    echo "Build process failed after $counter attempts."
    exit 1
else
    echo "All builds completed successfully!"
fi

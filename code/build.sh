original_directory=$(pwd)
script_directory=$(dirname "$0")

cd "$script_directory"

# Clean up previous build artifacts and create a new build directory
rm -rf build
mkdir -p build
cd build

# Run CMake and log errors
cmake -G "Unix Makefiles" .. 2>&1 | tee cmake_errors.log
if [ $? -ne 0 ]; then
    echo "CMake configuration failed. Check cmake_errors.log"
    exit 1
fi

make 2>&1 | tee make_errors.log
if [ $? -ne 0 ]; then
    echo "Build failed. Check make_errors.log"
    exit 1
fi

cd "$original_directory"

# Check if the application binary exists before printing success message
if [ -f "build/harmoniser_pedal" ]; then
    echo "Application built successfully"
else
    echo "Build completed, but executable not found. Check make_errors.log"
    exit 1
fi

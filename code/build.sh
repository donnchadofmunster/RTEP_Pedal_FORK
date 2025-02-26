original_directory=$(pwd)

script_directory=$(dirname "$0")

cd "$script_directory"

mkdir -p build

cd build

cmake -G "Unix Makefiles" ..
make

cd "$original_directory"

echo "Application built"
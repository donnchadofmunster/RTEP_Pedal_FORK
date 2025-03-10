original_directory=$(pwd)
script_directory=$(dirname "$0")

cd "$script_directory"
cd "lib"

git clone https://github.com/Signalsmith-Audio/signalsmith-stretch.git

cd "$original_directory"
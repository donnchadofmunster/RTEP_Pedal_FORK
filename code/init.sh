original_directory=$(pwd)
script_directory=$(dirname "$0")

apt update
apt upgrade
apt install build-essential
apt install cmake
apt install clang clang-tidy
apt install libsndfile1-dev pkg-config
apt install libasound2-dev

cd "$script_directory"
cd "lib"

git clone https://github.com/Signalsmith-Audio/signalsmith-stretch.git

cd "$original_directory"


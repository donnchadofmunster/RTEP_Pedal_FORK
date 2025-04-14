original_directory=$(pwd)
script_directory=$(dirname "$0")

sudo apt update
sudo apt upgrade
sudo apt install build-essential
sudo apt install cmake
sudo apt install gdb
sudo apt install clang clang-tidy
sudo apt install libgtest-dev
sudo apt install libsndfile1-dev pkg-config
sudo apt install sndfile-programs
sudo apt install sox
sudo apt install libasound2-dev
sudo apt autoremove

cd "$script_directory/lib"

# If directory exists and is not a git repo, remove it
if [ -d "signalsmith-stretch" ]; then
  if [ ! -d "signalsmith-stretch/.git" ]; then
    rm -rf signalsmith-stretch
  fi
fi

# Clone if not already present
if [ ! -d "signalsmith-stretch" ]; then
  git submodule add -f https://github.com/Signalsmith-Audio/signalsmith-stretch.git ./signalsmith-stretch
fi

cd "$original_directory"


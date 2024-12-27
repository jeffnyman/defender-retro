# Retro Defender

The `source` directory contains the [Historical Source code](https://github.com/historicalsource/defender). This includes the single sound file from the [Historical source sound ROMs](https://github.com/historicalsource/williams-soundroms). The `source\labels` directory contains the binaries for the different labels of the game that were produced.

The `src` directory is the original contents with certain modifications to make it possible to compile the source code as well as fill in any gaps.

## Build Instructions (WORK IN PROGRESS)

First, make sure you clone the repo:

```sh
git clone https://github.com/jeffnyman/retro-defender.git
```

### Setup Build Chain

You need to set up the Git submodules that provide the assembler toolchain.

```sh
git submodule init
git submodule update
```

### Setup Tooling

There are some Python scripts used as part of the building process, so you should have a version of Python 3 installed. On Linux:

```sh
sudo apt install python3
```

On Mac:

```sh
brew install python3
```

You can use whateever installation method you want, of course. On Windows, you can install the a binary of your choice. In any environment, make sure Python is on your PATH.

There is a Makefile present to help run the various commands of the build toolchain. Technically, you could just run those commands individualy but having Make installed would not hurt. On a POSIX system you will have this already, most likely.

If on a Mac, you will need the following:

```sh
brew install md5sha1sum
brew install automake
```

On a Mac, you will likely have Bison but I recommend making sure it's up-to-date.

```sh
brew install bison
export PATH="/usr/local/opt/bison/bin:$PATH"
```

If on a Linux variant, make sure you have the following:

```sh
sudo apt install build-essential flex
```

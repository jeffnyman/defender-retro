<h1 align="center">

<img src="https://raw.githubusercontent.com/jeffnyman/retro-defender/master/assets/defender-title.jpg" alt="Retro Defender"/>

</h1>

This project is related to my [Defender Red Label](https://github.com/jeffnyman/defender-redlabel) project, which attempt to create a generated _Defender_ clone from the ROM version generated in this repository.

The `source` directory contains the [Historical Source Defender source code](https://github.com/historicalsource/defender). This includes the single sound file from the [Historical Source Sound ROMS for Williams](https://github.com/historicalsource/williams-soundroms). The `source\labels` directory contains the binaries for the different labels of the game that were produced.

The `src` directory is the original source code with certain modifications to make it possible to compile that code on a relatively modern development chain.

In terms of the provenance of the code, ex-Williams engineers had previously donated material to the Strong Museum of Play. Some of that was the work of [Sam Dickers on Defender](https://archives.museumofplay.org/repositories/3/resources/228).

## Running Defender

You can use some ROM files to play _Defender_ via [MAME](https://www.mamedev.org/release.html). On Windows you can install this via a binary. On most POSIX systems:

```sh
sudo apt install mame
```

On Mac:

```sh
brew install mame
```

However, to be able to do this you have to generate the ROM files. And to do that, you have to build from source, which is what the next section is about.

## Build Instructions

On Windows, I would highly recommend using Windows Subsystem for Linux. Cygwin or other POSIX-based implementations might work as well.

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

The tools used here are [ASM6809](https://www.6809.org.uk/asm6809/) and [VASM](http://www.compilers.de/vasm.html).

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

### Build ASM6809 Assembler

```sh
cd asm6809
./autogen.sh
./configure
```

The `autogen.sh` script generates the configure script (from `configure.ac`, using autoconf) and any files it needs (like creating Makefile.in from Makefile.am using automake). This requires autotools to be installed on your system. The configure script generates `Makefile` and other files needed to build. If the above steps complete succesfully, you should be able to make everything.

```sh
make
```

### Build VASM Assembler

```sh
cd vasm
make CPU=6800 SYNTAX=oldstyle
```

### Build Red Label Version

With both ASM and VASM built, you should be able to build the binaries for <em>Defender</em>. This will require access to Python as it will use the included scripts in this repo.

```sh
make redlabel
```

This will create the `defend.[x]` numbered binaries (excluding 5) as well as the `defend.snd` file.

## Reference Implementation

An executable <em>Defender</em> version is provided in the `reference` directory. This was the basis for determining the correct implementation of building from the sources. Technically there is no reason you need to worry about this, but should you want to generate the reference implementation ROM, you can first create the relevant executable:

```sh
make extract
```

Then run the results script against the provided Defender executable.

```sh
./reference/rom_extract -d -v -i ./reference/DEFENDER.EXE -o ./reference/defender.rom
```

I should note that my `rom_extract.c` is a modified version of a C program that's existed since January of 1996, called `romgrab.c`, written by Jonathan Wolff. I don't have a whole lot of details but Jonathan's program was originally written as a utility to grab the ROM files from the Williams Arcade executables.

## Credits

The Historical Source repository provided the [original source code for _Defender_](https://github.com/historicalsource/defender).

The [_Defender_ section of Computer Archeology](http://www.computerarcheology.com/Arcade/Defender/) provides a great deal of info on the internals.

The [Williams hardware identification repository](https://www.robotron-2084.co.uk/williams/hardware) was extremely helpful in untangling the PCB information.

The [_Defender_ entry at Museum of the Game](https://www.arcade-museum.com/Videogame/defender--williams) is useful for lots of historical information.

This project benefitted massively from Rob Hogan's <a href="https://github.com/mwenge/defender">defender</a> repository. What I have done is clean up a lot of the source, provide consistency to the scripts, and include the reference implementations so that all work could be entirely recreated.

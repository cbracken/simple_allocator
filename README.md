# Simple allocator

A simple, poorly-performing memory allocator.

## Prerequisites

To build, you'll need [cmake](https://cmake.org),
[ninja](https://github.com/ninja-build/ninja), and a clang build toolchain
installed on your system.

## Obtaining the source

First, clone the repo:

```shell
git clone git@gitlab.com:cbracken/simple_allocator.git
```

Next, initialise and fetch git submodules:

```shell
# Initialise local configuration file.
git submodule init

# Fetch submodules (googletest).
git submodule update
```

## Updating git submodules

To update the git submodules to a newer commit, simply run:

```shell
git submodule update --remote
```

## Building and running

First, generate the ninja build files:

```shell
# For debug build:
cmake -DCMAKE_BUILD_TYPE=Debug -GNinja

# For release build:
cmake -DCMAKE_BUILD_TYPE=Release -GNinja
```

### Executable binary

To build and run the binary:

```shell
ninja
./bin/main
```

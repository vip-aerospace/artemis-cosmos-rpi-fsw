<img src="images/artemis.png" alt="Artemis" width="190" height="180"> <img src="images/university-of-hawaii-manoa.png" alt="University of Hawaii Manoa" width="180" height="180"> <br>

# Artemis Cubesat Kit Flight Software (Raspberry Pi Zero W)
This repository contains the HSFL COSMOS flight software for the Artemis Cubesat's Raspberry Pi Zero.

Clone Artemis-RPi-Flight-Software into:
`~/cosmos/source/projects`

For documentation and tutorials, visit our [project website](https://sites.google.com/hawaii.edu/artemiscubesatkit).

Current Functionality of the software:
* Receives CameraCapture command from Teensy, takes photograph, and stores it to the SD card. 

## Setup and Installation
This flight software is meant to be used as a project within a COSMOS development enviornment. Follow these [installation instructions](https://hsfl.github.io/cosmos-docs/pages/2-getting_started/development-environment.html), then open a terminal within the `~/cosmos` directory. Navigate to `~/cosmos/source/projects`, then run
```
git clone [this repo's Git URL]
```
where the URL is found by pressing the green "Code" button above.

## Compilation
This flight software can be compiled for two targets: a standard x86 architecture, or the ARM architecture. These correspond to a COSMOS installation on a regular computer or Raspberry Pi respectively.

### x86 Compilation
To compile for the x86 architecture, navigate to `~/cosmos/source/projects/artemis-cosmos-rpi-fsw/source/build/`. Then run
```
./do_cmake_linux
```
After it completes successfully, navigate to the newly-created `linux` directory, and run
```
make install
```
The agents will compile, and the binaries will be written to `~/cosmos/artemis-flight/bin/`

### Raspberry Pi Compilation
To compile for the ARM architecture (Raspberry Pi), first ensure you have the cross-compiler installed. This only has to be done if you manually installed COSMOS. If you are using COSMOS within a Docker container, the cross-compiler should already be installed within that container.

If you do not have the cross-compiler installed, install it by running the following:
```
wget https://github.com/Pro/raspi-toolchain/releases/latest/download/raspi-toolchain.tar.gz
tar xfz raspi-toolchain.tar.gz --strip-components=1 -C /opt
```

With the cross-compiler installed, navigate to `~/cosmos/source/projects/artemis-cosmos-rpi-fsw/source/build/`. Then run
```
./do_cmake_rpi
```
After it completes successfully, navigate to the newly-created `rpi_` directory, and run
```
make install
```
The agents will compile, and the binaries will be written to `~/cosmos/rpi/artemis-flight/bin/`

## Flight Software Architetcture 

![Flight Software Architetcture](/images/FSWArchitecture.png)

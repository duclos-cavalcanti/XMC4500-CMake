# Developing on Infineon's XMC4500 Relax Lite Kit

## Dependencies
Will be using [jlink](https://www.segger.com/products/debug-probes/j-link/technology/flash-download/) to flash the device.
`make cmake arm-none-eabi-gcc arm-none-eabi-newlib arm-none-eabi-gdb jlink debtap gcc`

## Install
Dependencies are based on a Arch Linux system, should work on any Linux distro as long as
the dependencies are installed accordingly.
``` 
sudo pacman -S arm-none-eabi-gcc arm-none-eabi-newlib
yay -S jlink debtap # can be done with any other AUR helper
mkdir ~/xmc4500
git clone git@github.com:duclos-cavalcanti/XMC4500_Cmake.git
``` 
## Udev Rules
For access to serial interface add your user to the group `uucp`. 
```
 gpasswd -a username uucp
```
Information on how to communicate with the device can be found in the serial interface article on the arch wiki.

## Downloading XMClib Software
Either
1. Install the XMClib .deb package and convert it.
```
debtap xmclib-<version>.deb
sudo pacman -U xmclib-<version>.pkg.tar.zst
sudo mv /opt/XMClib/ ~/xmc4500/
```
2. Go [here](https://github.com/PromyLOPh/xmclib) and clone the repo which should make the
   necessary library available.
```
git clone https://github.com/PromyLOPh/xmclib
sudo pacman -U xmclib-<version>.pkg.tar.zst
sudo mv /opt/XMClib/ ~/xmc4500/
```
Versions may vary.

## Usage
```
cd ~/xmc4500/XMC4500-Cmake
mkdir build; cd build
cmake ..
make # Compiles
make flash # flashes
make debug # debugs using gdb
```


# fusee-tools
A set of essential tools for Fusee Gelee on T20 / T30 / T114

## Preparation
To successfully build payloads you need to install several dependencies:
```
apt install build-essential gcc-arm-none-eabi python3-usb git
```

## Building
Open the terminal in the directory you want to contain fusee-tools and perform:
```
git clone --recursive https://gitlab.com/grate-driver/fusee-tools.git
cd fusee-tools/payloads
make ARCH=arm CROSS_COMPILE=arm-none-eabi-
```

## Executing
To execute the exploit plug in device via usb to host linux PC using usb 3 port (blue one) and enter RCM with method your device uses (usually by holding `power` + `volume up`).

Fusee-tools provide scripts for 3 main actions:
 - dumping SBK
 - getting BCT
 - loading bootloader into RAM

All 3 scripts share similar set of keys:
 - `s` defines Tegra SOC generation. Valid values are `T20`, `T30` and `T114`. This key is mandatory. 
 - `t` defines path to and name of BCT. If not provided `current.bct` name is used at the root of fusee-tools folder.
 - `b` defines path to and name of bootloader. If not provided `u-boot-dtb-tegra.bin` name is used at the root of fusee-tools folder. 

### Dumping SBK
SBK can be dumped using `dump_sbk.sh` script. Valid key is `-s`. Example:
```
./dump_sbk.sh -s T30
```

### Getting BCT

**NOTE:**
If your BCT is corrupted, you may use one which matches your device model from /bct folder.

BCT can be dumped using `get_bct.sh` script. Valid keys are `-s` and `-t` (optional). Example:
```
./get_bct.sh -s T30 -t ./bct/tf201.bct
```

### Loading bootloader
Any compatible bootloader can be uploaded into RAM and it will act as primary bootloader. To achieve this `run_bootloader.sh` script is used. Valid keys are `-s`, `-t` (optional) and `-b` (optional). Example:
```
./run_bootloader.sh -s T30 -t ./bct/tf201.bct -b android_bootloader.bin
```

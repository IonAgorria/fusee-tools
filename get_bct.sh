#!/bin/bash

while getopts "s:t:" option; do
    case "${option}" in
        s)
            gen=${OPTARG}
            ;;
        t)
            bct=${OPTARG}
            ;;
        *)
            echo "     With -s you MUST specify tegra SOC generation"
            echo "     valid options are T20 T30 and T114 (T capital!)"
            exit 2
            ;;
    esac
done

if [ "$gen" = "" ]; then
    echo "     No SOC generation provided!"
    echo "     Use -s to specify tegra SOC generation."
    echo "     Valid options are T20 T30 and T114 (T capital!)"
    exit 3
fi

if [ "$gen" = "T114" ]; then
    nvflash="nvflash_v3.08.1700"
else
    nvflash="nvflash_v1.13.87205"
fi

if [ "$bct" = "" ]; then
    bct="current.bct"
fi

./fusee-launcher-new/fusee-launcher.py ./payloads/out/"$gen"/patch_irom.bin

sleep 1

./utils/"$nvflash" --getbct --bct "$bct" --configfile ./utils/flash.cfg

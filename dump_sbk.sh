#!/bin/bash

while getopts "s:" option; do
    case "${option}" in
        s)
            gen=${OPTARG}
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

./fusee-launcher-new/fusee-launcher.py --tty ./payloads/out/"$gen"/dump_sbk_usb.bin

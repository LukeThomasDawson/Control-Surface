#!/bin/bash

parent_path=$( cd "$(dirname "${BASH_SOURCE}")" ; pwd -P )
cd "$parent_path"

read -p "Are you sure you want to flash your ATmega16U2 with the Arduino Uno Serial firmware? [Y/n]: " confirm
if [ "$confirm" = y ] || [ "$confirm" = Y ]
then 
if [ ! -e Arduino-usbserial-atmega16u2-Uno-Rev3.hex ]
then
echo "Downloading firmware: Arduino-usbserial-atmega16u2-Uno-Rev3.hex ..."
wget --quiet "https://raw.githubusercontent.com/arduino/Arduino/master/hardware/arduino/avr/firmwares/atmegaxxu2/arduino-usbserial/Arduino-usbserial-atmega16u2-Uno-Rev3.hex"
fi
echo "Erasing ..."
sudo dfu-programmer atmega16u2 erase
echo "Flashing arduino_midi.hex ..."
sudo dfu-programmer atmega16u2 flash Arduino-usbserial-atmega16u2-Uno-Rev3.hex
echo "Resetting ..."
sudo dfu-programmer atmega16u2 reset
echo "Done."
fi

#!/bin/bash

TARGET=temp_control
if [ -f bin/$TARGET.elf ]
then
  arm-none-eabi-objcopy --verbose -O ihex bin/$TARGET.elf bin/$TARGET.hex
  arm-none-eabi-objcopy --verbose -O binary bin/$TARGET.elf bin/$TARGET.bin
else
  echo "bin/$TARGET.elf: Not found".
fi

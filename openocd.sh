#!/bin/bash

openocd \
  -f interface/stlink-v2.cfg \
  -c "transport select hla_swd" \
  -f target/stm32f0x.cfg

#!/bin/sh
#arm-softmmu/qemu-system-arm -option-rom /home/cmw/dev/clean/peek-build/build/peek.bin -option-rom srom.bin -serial stdio -gdb tcp::6666 -S &> err
arm-softmmu/qemu-system-arm -option-rom /home/cmw/dev/clean/peek-build/build/peek.bin -option-rom srom.bin -serial stdio &> err
#arm-softmmu/qemu-system-arm -option-rom /home/cmw/dev/clean/peek-build/build/peek.bin -option-rom srom.bin -serial tcp::4444,server &> err
#arm-softmmu/qemu-system-arm -option-rom /home/andrey/dev/peek-build/build/peek.bin -option-rom srom.bin -serial stdio 2> err

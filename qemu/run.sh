#!/bin/sh
#cp ~/norhome.img nor.img
cp ~/temp/norffs/nor.img .
#arm-softmmu/qemu-system-arm -option-rom /home/cmw/dev/clean/non-lcd/peek-build/build/peek.bin -option-rom srom.bin -pflash nor.img -gdb tcp::6666 -S -serial stdio 2> err
arm-softmmu/qemu-system-arm -option-rom /home/cmw/dev/clean/non-lcd/peek-build/build/peek.bin -option-rom srom.bin -pflash nor.img -serial stdio 2> err | strings
#arm-softmmu/qemu-system-arm -option-rom /home/cmw/dev/clean/non-lcd/peek-build/build/peek.bin -option-rom srom.bin -pflash nor.img -serial tcp::4444,server &> err
#arm-softmmu/qemu-system-arm -option-rom /home/andrey/dev/peek-build/build/peek.bin -option-rom srom.bin -serial stdio 2> err

#!/bin/sh
#cp ~/norhome.img nor.img
#cp ~/temp/norffs/nor.img .
cp ~/temp/norffsnetsurf/nor.img .
arm-softmmu/qemu-system-arm -option-rom /home/cmw/dev/clean/non-lcd/peek-build/build/peek.bin -option-rom srom.bin -pflash nor.img -serial stdio 2> out|strings
# | strings
#arm-softmmu/qemu-system-arm -option-rom /home/cmw/dev/clean/non-lcd/peek-build/build/peek.bin -option-rom srom.bin -pflash nor.img -gdb tcp::1234 -S -serial stdio 1> out 
#arm-softmmu/qemu-system-arm -option-rom /home/ryan/linux-server/repo/trunk/peek-build/build/peek.bin -option-rom srom.bin -pflash nor.img -serial stdio 2> err | strings
#arm-softmmu/qemu-system-arm -option-rom /home/cmw/dev/clean/non-lcd/peek-build/build/peek.bin -option-rom srom.bin -pflash nor.img -serial stdio 2> err | strings
#arm-softmmu/qemu-system-arm -option-rom /home/cmw/dev/clean/non-lcd/peek-build/build/peek.bin -option-rom srom.bin -pflash nor.img -serial tcp::4444,server &> err
#arm-softmmu/qemu-system-arm -option-rom /home/andrey/dev/peek-build/build/peek.bin -option-rom srom.bin -serial stdio 2> err

#!/bin/bash

sed "s/IMG_C/landscape.c/" < cvt.c > icvt.c
gcc icvt.c -o cvt -DRUN_MODE=RGB565
./cvt landscape_image > landscape565.c

sed "s/IMG_C/sun.c/" < cvt.c > icvt.c
gcc icvt.c -o cvt -DRUN_MODE=A4
./cvt sun_image > sunA4.c

sed "s/IMG_C/letter.c/" < cvt.c > icvt.c
gcc icvt.c -o cvt -DRUN_MODE=A4
./cvt letter_image > letterA4.c

sed "s/IMG_C/sms.c/" < cvt.c > icvt.c
gcc icvt.c -o cvt -DRUN_MODE=A4
./cvt sms_image > smsA4.c

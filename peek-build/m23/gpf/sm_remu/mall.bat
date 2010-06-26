
cd .\test\
gnumake clean
gnumake DEBUG=1 ARM7=1 MEMSUPER=1 TS3=1
gnumake clean
gnumake DEBUG=1 ARM7=1 MEMSUPER=0 TS3=1
gnumake clean
gnumake DEBUG=0 ARM7=1 MEMSUPER=0 TS3=1
gnumake clean
gnumake DEBUG=0 ARM7=1 MEMSUPER=1 TS3=1
goto end
:end
cd ..
quit 0


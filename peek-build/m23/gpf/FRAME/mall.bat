@echo off
rem ----------------------------------------------------------------------------- 
rem  Project :  
rem  Modul   :  
rem ----------------------------------------------------------------------------- 
rem  Copyright 2004 Texas Instruments Deutschland, AG 
rem                 All rights reserved. 
rem 
rem                 This file is confidential and a trade secret of Texas 
rem                 Instruments Berlin, AG 
rem                 The receipt of or possession of this file does not convey 
rem                 any rights to reproduce or disclose its contents or to 
rem                 manufacture, use, or sell anything it may describe, in 
rem                 whole, or in part, without the specific written consent of 
rem                 Texas Instruments Deutschland, AG. 
rem ----------------------------------------------------------------------------- 
rem |  Purpose :  build all FRAME variants
rem ----------------------------------------------------------------------------- 
setlocal

set name=FRAME
set name_small=frame 
set err_file=%name_small%.err

@if x%_4ver% == x goto need4nt
@if x%1 == x-help goto help
@if x%1 == x-test goto test
@if x%1 == x-guitest goto test

REM default is gnumake:
set M=gnumake -r
set MAKE=gnumake
REM use clearmake if under ClearCase
REM is clearcase installed ?
if "%CLEARCASE_PRIMARY_GROUP%" == "" goto options
REM are we using a clearcase view ?
cleartool lsview -cview >nul 2>>&1
if errorlevel 1 goto options
REM on a clearcase view: use clearmake in gnu mode:
set M=clearmake -V -C gnu SHELL=%TESTDRIVE%/gpf/tools/bin/sh.exe
set MAKE=clearmake

:options
REM set default options
set clean=0
set def_ver=DEV_BUILD
set ver=0
REM parse command line options
:parse_opt
if "%1" == "-clean" set clean=1
iff "%1" == "-l" then
  iff "%2" == "" then
    echo error: no label name specified !
    goto help
  endiff
  set ver=%2
  shift
endiff
shift
if "%1" != "" goto parse_opt 

if %ver% != 0 echo "using label %ver% (not implemented yet) ..."


cd ..\..\gpf
if exist %err_file% del %err_file% >nul

if %clean% == 1 goto clean

echo *** making FRAME/MISC/TIF libs ***              										>>%err_file% 2>>&1

call initvars gprs ms %TESTDRIVE% \g23m\condat  											>>%err_file% 2>>&1


rem nuc arm7 Libraries
echo ********** nuc arm7        **********      											>>%err_file% 2>>&1
cd frame
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7
%M% frame.lib TARGET=nuc PLATFORM=arm7														>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1
%M% frame.lib TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1										>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 RUN_FLASH=1
%M% frame.lib TARGET=nuc PLATFORM=arm7 RUN_FLASH=1											>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1												>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1								>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2		
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_INT_RAM=1	
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_INT_RAM=1						>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_FLASH=1	
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_FLASH=1						>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm7 TOOL_CHOICE=3											>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1 TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 RUN_FLASH=1 TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm7 RUN_FLASH=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 TOOL_CHOICE=3									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1 TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1 TOOL_CHOICE=3						>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1 TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1 TOOL_CHOICE=3						>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3		
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3						>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_INT_RAM=1	TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_INT_RAM=1	TOOL_CHOICE=3			>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_FLASH=1 TOOL_CHOICE=3	
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_FLASH=1 TOOL_CHOICE=3			>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib    TARGET=nuc PLATFORM=arm7 OS_ONLY=1 TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm7 OS_ONLY=1 TOOL_CHOICE=3									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib    TARGET=nuc PLATFORM=arm7 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
if %? != 0 goto err

cd ..\tst
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7					
%M% tif.lib TARGET=nuc PLATFORM=arm7														>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1			
%M% tif.lib TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1											>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 RUN_FLASH=1			
%M% tif.lib TARGET=nuc PLATFORM=arm7 RUN_FLASH=1											>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1				
%M% tif.lib TARGET=nuc PLATFORM=arm7 DEBUG=1												>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1		
%M% tif.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1		
%M% tif.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2			
%M% tif.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2										>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_INT_RAM=1	
%M% tif.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_INT_RAM=1						>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_FLASH=1	
%M% tif.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_FLASH=1							>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 TOOL_CHOICE=3					
%M% tif.lib TARGET=nuc PLATFORM=arm7 TOOL_CHOICE=3												>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1 TOOL_CHOICE=3			
%M% tif.lib TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 RUN_FLASH=1 TOOL_CHOICE=3			
%M% tif.lib TARGET=nuc PLATFORM=arm7 RUN_FLASH=1 TOOL_CHOICE=3									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 TOOL_CHOICE=3				
%M% tif.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 TOOL_CHOICE=3										>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1 TOOL_CHOICE=3		
%M% tif.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1 TOOL_CHOICE=3						>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1 TOOL_CHOICE=3		
%M% tif.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3			
%M% tif.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_INT_RAM=1 TOOL_CHOICE=3	
%M% tif.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_INT_RAM=1 TOOL_CHOICE=3			>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_FLASH=1 TOOL_CHOICE=3	
%M% tif.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_FLASH=1 TOOL_CHOICE=3				>>..\%err_file% 2>>&1
if %? != 0 goto err

cd ..\misc
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm7
%M% misc.lib TARGET=nuc PLATFORM=arm7														>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1
%M% misc.lib TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1											>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm7 RUN_FLASH=1
%M% misc.lib TARGET=nuc PLATFORM=arm7 RUN_FLASH=1											>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm7 DEBUG=1
%M% misc.lib TARGET=nuc PLATFORM=arm7 DEBUG=1												>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1
%M% misc.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1
%M% misc.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm7 TOOL_CHOICE=3
%M% misc.lib TARGET=nuc PLATFORM=arm7 TOOL_CHOICE=3											>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1 TOOL_CHOICE=3
%M% misc.lib TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm7 RUN_FLASH=1 TOOL_CHOICE=3
%M% misc.lib TARGET=nuc PLATFORM=arm7 RUN_FLASH=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm7 DEBUG=1 TOOL_CHOICE=3
%M% misc.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 TOOL_CHOICE=3									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1 TOOL_CHOICE=3
%M% misc.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1 TOOL_CHOICE=3						>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1 TOOL_CHOICE=3
%M% misc.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1 TOOL_CHOICE=3						>>..\%err_file% 2>>&1
if %? != 0 goto err

rem nuc arm9 Libraries
echo ********** nuc arm9        **********      											>>..\%err_file% 2>>&1
cd ..\frame
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=3					
%M% frame.lib TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=3											>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=3				
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=3									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3		
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3						>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm9 ESF=1 TOOL_CHOICE=3					
%M% frame.lib TARGET=nuc PLATFORM=arm9 ESF=1 TOOL_CHOICE=3										>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1 TOOL_CHOICE=3				
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1 TOOL_CHOICE=3		
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1 TOOL_CHOICE=3					>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=3									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib TARGET=nuc PLATFORM=arm9 OS_ONLY=1 ESF=1 TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm9 OS_ONLY=1 ESF=1 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 ESF=1 TOOL_CHOICE=3
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 ESF=1 TOOL_CHOICE=3					>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 ESF=1 TOOL_CHOICE=3 NO_OPT=1
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 ESF=1 TOOL_CHOICE=3 NO_OPT=1			>>..\%err_file% 2>>&1
if %? != 0 goto err
cd ..\tst
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=3					
%M% tif.lib TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=3												>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=3				
%M% tif.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=3										>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3			
%M% tif.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm9 ESF=1 TOOL_CHOICE=3					
%M% tif.lib TARGET=nuc PLATFORM=arm9 ESF=1 TOOL_CHOICE=3										>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1 TOOL_CHOICE=3				
%M% tif.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1 TOOL_CHOICE=3			
%M% tif.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1 TOOL_CHOICE=3					>>..\%err_file% 2>>&1
if %? != 0 goto err
cd ..\misc
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=3					
%M% misc.lib TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=3											>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=3				
%M% misc.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=3									>>..\%err_file% 2>>&1
if %? != 0 goto err
cd ..\frame
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=6					
%M% frame.lib TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=6											>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=6				
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=6									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=6		
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=6						>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm9 ESF=1 TOOL_CHOICE=6					
%M% frame.lib TARGET=nuc PLATFORM=arm9 ESF=1 TOOL_CHOICE=6										>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1 TOOL_CHOICE=6				
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1 TOOL_CHOICE=6								>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1 TOOL_CHOICE=6		
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1 TOOL_CHOICE=6					>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=6
%M% frame.lib TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=6									>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=6
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=6							>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib TARGET=nuc PLATFORM=arm9 OS_ONLY=1 ESF=1 TOOL_CHOICE=6
%M% frame.lib TARGET=nuc PLATFORM=arm9 OS_ONLY=1 ESF=1 TOOL_CHOICE=6							>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 ESF=1 TOOL_CHOICE=6
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 ESF=1 TOOL_CHOICE=6					>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 ESF=1 TOOL_CHOICE=6 NO_OPT=1
%M% frame.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 ESF=1 TOOL_CHOICE=6 NO_OPT=1			>>..\%err_file% 2>>&1
if %? != 0 goto err
cd ..\tst
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=6					
%M% tif.lib TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=6												>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=6				
%M% tif.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=6										>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=6			
%M% tif.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=6							>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm9 ESF=1 TOOL_CHOICE=6					
%M% tif.lib TARGET=nuc PLATFORM=arm9 ESF=1 TOOL_CHOICE=6										>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1 TOOL_CHOICE=6				
%M% tif.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1 TOOL_CHOICE=6								>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1 TOOL_CHOICE=6			
%M% tif.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1 TOOL_CHOICE=6					>>..\%err_file% 2>>&1
if %? != 0 goto err
cd ..\misc
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=6					
%M% misc.lib TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=6											>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=6				
%M% misc.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=6									>>..\%err_file% 2>>&1
if %? != 0 goto err

rem nucwin Libraries
echo ********** nuc pc          **********      											>>..\%err_file% 2>>&1
cd ..\frame
echo %MAKE% frame.lib TARGET=nuc PLATFORM=pc					
%M% frame.lib TARGET=nuc PLATFORM=pc														>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=pc DEBUG=1				
%M% frame.lib TARGET=nuc PLATFORM=pc DEBUG=1												>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.lib TARGET=nuc PLATFORM=pc DEBUG=1 MEMSUPER=2			
%M% frame.lib TARGET=nuc PLATFORM=pc DEBUG=1 MEMSUPER=2										>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib    TARGET=nuc PLATFORM=pc OS_ONLY=1
%M% frame.lib TARGET=nuc PLATFORM=pc OS_ONLY=1												>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% os.lib    TARGET=nuc PLATFORM=pc DEBUG=1 OS_ONLY=1
%M% frame.lib TARGET=nuc PLATFORM=pc DEBUG=1 OS_ONLY=1										>>..\%err_file% 2>>&1
if %? != 0 goto err
cd ..\tst
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=pc					
%M% tif.lib TARGET=nuc PLATFORM=pc															>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=pc DEBUG=1				
%M% tif.lib TARGET=nuc PLATFORM=pc DEBUG=1													>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.lib   TARGET=nuc PLATFORM=pc DEBUG=1 MEMSUPER=2			
%M% tif.lib TARGET=nuc PLATFORM=pc DEBUG=1 MEMSUPER=2										>>..\%err_file% 2>>&1
if %? != 0 goto err
cd ..\misc
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=pc					
%M% misc.lib TARGET=nuc PLATFORM=pc															>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.lib  TARGET=nuc PLATFORM=pc DEBUG=1				
%M% misc.lib TARGET=nuc PLATFORM=pc DEBUG=1													>>..\%err_file% 2>>&1
if %? != 0 goto err

rem win32 Libraries
echo ********** win32           **********      											>>..\%err_file% 2>>&1
cd ..\misc
echo %MAKE% misc.dll  TARGET=win32						
%M% misc.dll TARGET=win32																	>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% misc.dll  TARGET=win32 DEBUG=1					
%M% misc.dll TARGET=win32 DEBUG=1															>>..\%err_file% 2>>&1
if %? != 0 goto err
cd ..\frame
echo %MAKE% frame.dll TARGET=win32						
%M% frame.dll TARGET=win32																	>>..\%err_file% 2>>&1
echo %MAKE% frame.dll TARGET=win32 DEBUG=1					
%M% frame.dll TARGET=win32 DEBUG=1															>>..\%err_file% 2>>&1
cd ..\tst
echo %MAKE% tif.dll   TARGET=win32						
%M% tif.dll TARGET=win32																	>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% tif.dll   TARGET=win32 DEBUG=1					
%M% tif.dll TARGET=win32 DEBUG=1															>>..\%err_file% 2>>&1
if %? != 0 goto err
cd ..\frame
echo %MAKE% frame.dll TARGET=win32						
%M% frame.dll TARGET=win32																	>>..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% frame.dll TARGET=win32 DEBUG=1					
%M% frame.dll TARGET=win32 DEBUG=1															>>..\%err_file% 2>>&1
if %? != 0 goto err

echo ********** OSX             **********      											>>..\%err_file% 2>>&1
cd ..\frame\cust_os
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm7					
%M% osx.lib TARGET=nuc PLATFORM=arm7														>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1				
%M% osx.lib TARGET=nuc PLATFORM=arm7 DEBUG=1												>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2			
%M% osx.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2										>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm7 TOOL_CHOICE=3					
%M% osx.lib TARGET=nuc PLATFORM=arm7 TOOL_CHOICE=3												>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 TOOL_CHOICE=3				
%M% osx.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 TOOL_CHOICE=3										>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3			
%M% osx.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3							>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm7 OS_ONLY=1 TOOL_CHOICE=3					
%M% osx.lib TARGET=nuc PLATFORM=arm7 OS_ONLY=1 TOOL_CHOICE=3									>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm7 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3				
%M% osx.lib TARGET=nuc PLATFORM=arm7 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3							>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=3					
%M% osx.lib TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=3												>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=3				
%M% osx.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=3										>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3			
%M% osx.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2	TOOL_CHOICE=3							>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=3					
%M% osx.lib TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=3								    >>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3				
%M% osx.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3						    >>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3	NO_OPT=1			
%M% osx.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3 NO_OPT=1					>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=6					
%M% osx.lib TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=6												>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=6				
%M% osx.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=6										>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=6			
%M% osx.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2	TOOL_CHOICE=6							>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=6					
%M% osx.lib TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=6								    >>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=6				
%M% osx.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=6						    >>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=6	NO_OPT=1			
%M% osx.lib TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=6 NO_OPT=1					>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=pc OS_ONLY=1					
%M% osx.lib TARGET=nuc PLATFORM=pc OS_ONLY=1												>>..\..\%err_file% 2>>&1
if %? != 0 goto err
echo %MAKE% osx.lib   TARGET=nuc PLATFORM=pc DEBUG=1 OS_ONLY=1				
%M% osx.lib TARGET=nuc PLATFORM=pc DEBUG=1 OS_ONLY=1										>>..\..\%err_file% 2>>&1
if %? != 0 goto err

goto final


:clean

echo *** cleaning FRAME/MISC/TIF libs ***
echo *** cleaning FRAME/MISC/TIF libs ***            								>>%err_file% 2>>&1

echo ********** nuc arm7        **********
echo ********** nuc arm7        **********      									>>%err_file% 2>>&1
cd frame
%M% clean TARGET=nuc PLATFORM=arm7													>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1									>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 RUN_FLASH=1										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1											>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_INT_RAM=1					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_FLASH=1					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 TOOL_CHOICE=3										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 RUN_FLASH=1 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1 TOOL_CHOICE=3					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1 TOOL_CHOICE=3					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_INT_RAM=1 TOOL_CHOICE=3		>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_FLASH=1 TOOL_CHOICE=3		>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 OS_ONLY=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3						>>..\%err_file% 2>>&1
cd ..\tst
%M% clean TARGET=nuc PLATFORM=arm7													>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1									>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 RUN_FLASH=1										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1											>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_INT_RAM=1					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_FLASH=1					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 TOOL_CHOICE=3										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 RUN_FLASH=1 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1 TOOL_CHOICE=3					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1 TOOL_CHOICE=3					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_INT_RAM=1 TOOL_CHOICE=3		>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 RUN_FLASH=1 TOOL_CHOICE=3		>>..\%err_file% 2>>&1
cd ..\misc
%M% clean TARGET=nuc PLATFORM=arm7													>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1									>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 RUN_FLASH=1										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1											>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 TOOL_CHOICE=3										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 RUN_INT_RAM=1 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 RUN_FLASH=1 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_INT_RAM=1 TOOL_CHOICE=3					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 RUN_FLASH=1 TOOL_CHOICE=3					>>..\%err_file% 2>>&1

echo ********** nuc arm9        **********
echo ********** nuc arm9        **********      									>>..\%err_file% 2>>&1
cd ..\frame
%M% clean TARGET=nuc PLATFORM=arm9													>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1											>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 ESF=1											>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1									>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 OS_ONLY=1										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 OS_ONLY=1 ESF=1									>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 ESF=1							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=3										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 ESF=1 TOOL_CHOICE=3									>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1 TOOL_CHOICE=3				>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3						>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=3	ESF=1						>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3	ESF=1				>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3	ESF=1 NO_OPT=1		>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=6										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=6								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=6					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 ESF=1 TOOL_CHOICE=6									>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1 TOOL_CHOICE=6							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1 TOOL_CHOICE=6				>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=6								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=6						>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=6	ESF=1						>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=6	ESF=1				>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=6	ESF=1 NO_OPT=1		>>..\%err_file% 2>>&1
cd ..\tst
%M% clean TARGET=nuc PLATFORM=arm9													>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1											>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 ESF=1											>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1									>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=3										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 ESF=1 TOOL_CHOICE=3									>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1 TOOL_CHOICE=3							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1	TOOL_CHOICE=3				>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=6										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=6								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=6					>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 ESF=1 TOOL_CHOICE=6									>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 ESF=1 TOOL_CHOICE=6							>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 ESF=1	TOOL_CHOICE=6				>>..\%err_file% 2>>&1
cd ..\misc
%M% clean TARGET=nuc PLATFORM=arm9													>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1											>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=3										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=3								>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=6										>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=6								>>..\%err_file% 2>>&1

echo ********** nuc pc          **********
echo ********** nuc pc          **********      									>>..\%err_file% 2>>&1
cd ..\frame
%M% clean TARGET=nuc PLATFORM=pc													>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=pc DEBUG=1											>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=pc DEBUG=1 MEMSUPER=2									>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=pc OS_ONLY=1											>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=pc DEBUG=1 OS_ONLY=1									>>..\%err_file% 2>>&1
cd ..\tst
%M% clean TARGET=nuc PLATFORM=pc													>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=pc DEBUG=1											>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=pc DEBUG=1 MEMSUPER=2									>>..\%err_file% 2>>&1
cd ..\misc
%M% clean TARGET=nuc PLATFORM=pc													>>..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=pc DEBUG=1											>>..\%err_file% 2>>&1

echo ********** win32           **********
echo ********** win32           **********      									>>..\%err_file% 2>>&1
cd ..\frame
%M% clean TARGET=win32																>>..\%err_file% 2>>&1
%M% clean TARGET=win32 DEBUG=1														>>..\%err_file% 2>>&1
cd ..\tst
%M% clean TARGET=win32																>>..\%err_file% 2>>&1
%M% clean TARGET=win32 DEBUG=1														>>..\%err_file% 2>>&1
cd ..\misc
%M% clean TARGET=win32																>>..\%err_file% 2>>&1
%M% clean TARGET=win32 DEBUG=1														>>..\%err_file% 2>>&1

echo ********** OSX				**********
echo ********** OSX				**********      									>>..\%err_file% 2>>&1
cd ..\frame\cust_os
%M% clean TARGET=nuc PLATFORM=arm7													>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1											>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2								>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9													>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1											>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2								>>..\..\%err_file% 2>>&1
rem %M% clean TARGET=nuc PLATFORM=arm9 OS_ONLY=1									>>..\..\%err_file% 2>>&1
rem %M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1							>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 TOOL_CHOICE=3										>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 TOOL_CHOICE=3								>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3					>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 OS_ONLY=1 TOOL_CHOICE=3								>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm7 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3						>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=3										>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=3								>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=3					>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=3								>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3						>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=3	NO_OPT=1			>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 TOOL_CHOICE=6										>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 TOOL_CHOICE=6								>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 MEMSUPER=2 TOOL_CHOICE=6					>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 OS_ONLY=1 TOOL_CHOICE=6								>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=6						>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=arm9 DEBUG=1 OS_ONLY=1 TOOL_CHOICE=6	NO_OPT=1			>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=pc OS_ONLY=1											>>..\..\%err_file% 2>>&1
%M% clean TARGET=nuc PLATFORM=pc DEBUG=1 OS_ONLY=1									>>..\..\%err_file% 2>>&1


goto final


:help
text
usage:
mall {-help | 
      { [-l <LABEL>] | -clean } 
     }
example: mall -l FRAME_2.7.1 
endtext
goto end

:need4nt
@echo Error: 4DOS or 4NT is needed
goto end

:err
set C_DIR=%V122_DIR%
cd..
grep "error " %err_file% 
@echo Errors occured ! See %err_file% for details.
endlocal
quit 1

goto final

:test
rem TEMP HACK
echo no automated tests supported so far :-(
goto final


:final
grep "warning " %err_file% 2> nul

:end
endlocal
quit 0

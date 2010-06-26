@echo off
rem build all CCD variants
setlocal

set name=CCD
set name_small=ccd 
set err_file=..\%name_small%.err

@if x%_4ver% == x goto need4nt
@if x%1 == x-help goto help
@if x%1 == x-guitest goto guitest
@if x%1 == x-test goto test

REM default is gnumake:
set M=gnumake -r
REM use clearmake if under ClearCase
set IN_VOB=FALSE
REM is clearcase installed ?
if "%CLEARCASE_PRIMARY_GROUP%" == "" goto options
REM are we using a clearcase view ?
cleartool lsview -cview >nul 2>>&1
if errorlevel 1 goto options
REM on a clearcase view: use clearmake in gnu mode:
set M=clearmake -V -C gnu
set IN_VOB=TRUE

:options
REM set default options
set clean=0
set def_ver=DEV_BUILD
set ver=0
set maketarget=colibs all
REM parse command line options
:parse_opt
iff "%1" == "-gmake" then
  set M=gnumake -r
elseiff "%1" == "-clean" then
  iff "%maketarget%" == "colibs all cilibs" then
	  echo mall: option -clean ignored when -exp is given
	else
    set maketarget=clean
  endiff
elseiff "%1" == "-l" then
  iff "%2" == "" then
    echo error: no label name specified !
    goto help
  endiff
  set ver=%2
  shift
elseiff "%1" == "-exp" then
  iff %maketarget% == clean then
    echo mall: option -exp ignored when -clean is given
  else
    set maketarget=colibs all cilibs
  endiff
elseiff "%1" == "-n" then
  set M=echo
  shift
elseiff "%1" != "" then
  goto help
endiff
shift
if "%1" != "" goto parse_opt 

if exist %err_file% del %err_file% >nul

if %ver% != 0 echo "using label %ver% (not implemented yet) ..."

iff %maketarget% == clean then
  echo *** cleaning CCD libs *** | tee /a %err_file%
else
  echo *** making CCD libs *** | tee /a %err_file%
endiff

rem nuc Libraries
:arm7
echo ********** nuc arm7        **********        | tee /a %err_file%
echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=0 TRACE=0 MEMSUPER=0 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=0 TRACE=0 MEMSUPER=0 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=0 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=0 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=0 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=0 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err
rem ********** nuc arm7: splitted libraries ir/fl
rem RUN_FLASH=1 RUN_INT_RAM=0
echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=0 TRACE=0 MEMSUPER=0 RUN_FLASH=1 RUN_INT_RAM=0 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=0 TRACE=0 MEMSUPER=0 RUN_FLASH=1 RUN_INT_RAM=0 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=0 RUN_FLASH=1 RUN_INT_RAM=0 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=0 RUN_FLASH=1 RUN_INT_RAM=0 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=0 RUN_FLASH=1 RUN_INT_RAM=0 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=0 RUN_FLASH=1 RUN_INT_RAM=0 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=1 RUN_FLASH=1 RUN_INT_RAM=0 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=1 RUN_FLASH=1 RUN_INT_RAM=0 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=1 RUN_FLASH=1 RUN_INT_RAM=0 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=1 RUN_FLASH=1 RUN_INT_RAM=0 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

rem RUN_FLASH=0 RUN_INT_RAM=1
echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=0 TRACE=0 MEMSUPER=0 RUN_FLASH=0 RUN_INT_RAM=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=0 TRACE=0 MEMSUPER=0 RUN_FLASH=0 RUN_INT_RAM=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=0 RUN_FLASH=0 RUN_INT_RAM=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=0 RUN_FLASH=0 RUN_INT_RAM=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=0 RUN_FLASH=0 RUN_INT_RAM=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=0 RUN_FLASH=0 RUN_INT_RAM=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=1 RUN_FLASH=0 RUN_INT_RAM=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=1 RUN_FLASH=0 RUN_INT_RAM=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=1 RUN_FLASH=0 RUN_INT_RAM=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=1 RUN_FLASH=0 RUN_INT_RAM=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo ********** nuc arm7 ts3    **********        | tee /a %err_file%
echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=0 TRACE=0 MEMSUPER=0 TOOL_CHOICE=3 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=0 TRACE=0 MEMSUPER=0 TOOL_CHOICE=3 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=0 TOOL_CHOICE=3 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=0 TOOL_CHOICE=3 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=0 TOOL_CHOICE=3 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=0 TOOL_CHOICE=3 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=1 TOOL_CHOICE=3 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=1 TOOL_CHOICE=3 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=1 TOOL_CHOICE=3 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=1 TOOL_CHOICE=3 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

rem ********** nuc arm7 ts3 + DYNARR=1
echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=0 TRACE=0 MEMSUPER=0 TOOL_CHOICE=3 DYNARR=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=0 TRACE=0 MEMSUPER=0 TOOL_CHOICE=3 DYNARR=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=0 TOOL_CHOICE=3 DYNARR=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=0 TOOL_CHOICE=3 DYNARR=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=0 TOOL_CHOICE=3 DYNARR=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=0 TOOL_CHOICE=3 DYNARR=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=1 TOOL_CHOICE=3 DYNARR=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=1 MEMSUPER=1 TOOL_CHOICE=3 DYNARR=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=1 TOOL_CHOICE=3 DYNARR=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm7 DEBUG=1 TRACE=0 MEMSUPER=1 TOOL_CHOICE=3 DYNARR=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo ********** nuc arm9 ts3    **********        | tee /a %err_file%
echo %M% TARGET=nuc PLATFORM=arm9 DEBUG=0 TRACE=0 MEMSUPER=0 TOOL_CHOICE=3 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm9 DEBUG=0 TRACE=0 MEMSUPER=0 TOOL_CHOICE=3 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm9 DEBUG=1 TRACE=0 MEMSUPER=0 TOOL_CHOICE=3 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm9 DEBUG=1 TRACE=0 MEMSUPER=0 TOOL_CHOICE=3 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm9 DEBUG=1 TRACE=1 MEMSUPER=1 TOOL_CHOICE=3 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm9 DEBUG=1 TRACE=1 MEMSUPER=1 TOOL_CHOICE=3 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo ********** nuc arm9 ts6    **********        | tee /a %err_file%
echo %M% TARGET=nuc PLATFORM=arm9 DEBUG=0 TRACE=0 MEMSUPER=0 TOOL_CHOICE=6 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm9 DEBUG=0 TRACE=0 MEMSUPER=0 TOOL_CHOICE=6 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm9 DEBUG=1 TRACE=0 MEMSUPER=0 TOOL_CHOICE=6 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm9 DEBUG=1 TRACE=0 MEMSUPER=0 TOOL_CHOICE=6 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=arm9 DEBUG=1 TRACE=1 MEMSUPER=1 TOOL_CHOICE=6 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=arm9 DEBUG=1 TRACE=1 MEMSUPER=1 TOOL_CHOICE=6 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo ********** nuc pc          **********        | tee /a %err_file%
echo %M% TARGET=nuc PLATFORM=pc DEBUG=0 TRACE=0 MEMSUPER=0 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=pc DEBUG=0 TRACE=0 MEMSUPER=0 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=pc DEBUG=1 TRACE=0 MEMSUPER=0 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=pc DEBUG=1 TRACE=0 MEMSUPER=0 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=pc DEBUG=1 TRACE=1 MEMSUPER=0 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=pc DEBUG=1 TRACE=1 MEMSUPER=0 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=nuc PLATFORM=pc DEBUG=1 TRACE=1 MEMSUPER=1 %maketarget% >>%err_file%
%M% TARGET=nuc PLATFORM=pc DEBUG=1 TRACE=1 MEMSUPER=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

rem win32 Libraries
echo ********** win32           **********        | tee /a %err_file%
echo %M% TARGET=win32 DEBUG=0 TRACE=0 %maketarget% >>%err_file%
%M% TARGET=win32 DEBUG=0 TRACE=0 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

echo %M% TARGET=win32 DEBUG=1 TRACE=1 %maketarget% >>%err_file%
%M% TARGET=win32 DEBUG=1 TRACE=1 %maketarget% >>%err_file% 2>>&1
if %? != 0 goto err

goto final

:help
text
usage:
mall {-help | 
      { [-l <LABEL>] | -clean | -exp | -gmake } 
     }
example: mall -l CCD_1.7.1 
-clean: clean objects
-exp  : export, i.e. checkin the built items
-gmake: use gnumake anyhow
endtext
goto end

:guitest
text
Tool without GUI - in consequence no GUI test implemented
endtext
goto end

:test
text
Test CCD functionality with GPF teststack:
- Change to /gpf/util/teststack:
- Call </gpf/util/teststack/mall.bat> to build teststack
- Call </gpf/util/teststack/mall.bat -test> to build and execute testsuite 
endtext
goto end

:need4nt
@echo Error: 4DOS or 4NT is needed
goto end

:err
grep "error " %err_file% 
@echo Errors occured ! See %err_file% for details.
endlocal
quit 1

:final
grep "warning " %err_file% 2> nul 

:end
endlocal
quit 0

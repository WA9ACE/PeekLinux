@echo off
rem build tools
setlocal


set name=TOOLS
set name_small=tools 
set err_file=..\%name_small%.err

@if x%_4ver% == x goto need4nt
@if x%1 == x-help goto help
@if x%1 == x-test goto test

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

if exist %err_file% del %err_file% >nul

if %clean% == 1 goto clean

if %ver% != 0 echo "using label %ver% (versioning not necessary for tools) ..."


echo *** making TOOLS ***
echo *** making TOOLS ***			 >>%err_file% 2>>&1
echo "making not necessary for tools"
goto final


:clean
echo "cleaning not necessary for tools"
goto final

:test
echo "testing  not yet supported for tools"
goto final

:help
text
usage:
mall {-help | -test
      { [-l <LABEL>] | -clean } 
     }
example: mall -l TOOLS_1.7.1 
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


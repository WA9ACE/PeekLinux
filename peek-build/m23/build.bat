set PATH_CC_2_54=C:\dvpt_tools\Codegen\TMS4701x_2.70
set CSST_BIN_DIR=C:\Program Files\Texas Instruments\CSST 2.18.0
set MSVCDIR=C:\Program Files\Microsoft Visual Studio\VC98
set MSVC_BIN=C:\Program Files\Microsoft Visual Studio\VC98\Bin;C:\Program Files\Microsoft Visual Studio\COMMON\MSDev98\Bin
set MSDEVDIR=C:\Program Files\Microsoft Visual Studio\COMMON\MSDev98
sbuild.py --Toolset=LNX_TI --buildThreads=1 --Variant=rel_n5_camera_micron --variantBackwardCompatibility  --ignoreCharacterCaseErrors --scons pre_generation
sbuild.py --Toolset=LNX_TI --buildThreads=1 --noIncDepScan --variantBackwardCompatibility  --ignoreCharacterCaseErrors --Variant=rel_n5_camera_micron


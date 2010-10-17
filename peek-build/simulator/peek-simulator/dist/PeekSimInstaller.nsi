;NSIS Modern User Interface
;Start Menu Folder Selection Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "Emobiix Peek Simulator"
  OutFile "PeekSimInstaller.exe"

  RequestExecutionLevel  "user"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\Emobiix"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Emobiix" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

 ; !insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Emobiix" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Peek Simulator"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Peek Simulator" SecDummy

  SetOutPath "$INSTDIR"
  SetFileAttributes "$INSTDIR" 0 
  
File "battery1.png"
File "battery2.png"
File "battery3.png"
File "battery4.png"
File "battery5.png"
File "battery6.png"
File "batterycase.png"
File "batterycharge.png"
File "DroidSans.ttf"
File "glut32.dll"
File "gprs.png"
File "home-server.ini"
File "letter.png"
File "opengl32.dll"
File "..\Release\peek-simulator.exe"
File "plasma.png"
File "signal0.png"
File "signal1.png"
File "signal2.png"
File "signal3.png"
File "signal4.png"
File "signal5.png"
File "spkr-off.png"
File "spkr-on.png"
File "star.png"
File "street64.png"
File "sun.png"
File "wi-cloud.png"
File "wi-rain.png"
File "wi-snow.png"
File "wi-storm.png"
File "wi-sun.png"
File "xerces-c_2_2_0.dll"

CreateDirectory "$INSTDIR\example-Arrays"
SetOutPath "$INSTDIR\example-Arrays"
File "example-Arrays\mail.xml"
File "example-Arrays\mailview.xml"

CreateDirectory "$INSTDIR\example-Boxes"
SetOutPath "$INSTDIR\example-Boxes"
File "example-Boxes\Boxes.xml"

CreateDirectory "$INSTDIR\example-Calculator"
SetOutPath "$INSTDIR\example-Calculator"
File "example-Calculator\Calculator.xml"

CreateDirectory "$INSTDIR\example-Centering"
SetOutPath "$INSTDIR\example-Centering"
File "example-Centering\Centering.xml"

CreateDirectory "$INSTDIR\example-CheckBox"
SetOutPath "$INSTDIR\example-CheckBox"
File "example-CheckBox\CheckBox.xml"

CreateDirectory "$INSTDIR\example-CircularReferences"
SetOutPath "$INSTDIR\example-CircularReferences"
File "example-CircularReferences\CircularReferences.xml"

CreateDirectory "$INSTDIR\example-Frame"
SetOutPath "$INSTDIR\example-Frame"
File "example-Frame\Frame.xml"

CreateDirectory "$INSTDIR\example-Grids"
SetOutPath "$INSTDIR\example-Grids"
File "example-Grids\HGrid.xml"
File "example-Grids\VGrid.xml"

CreateDirectory "$INSTDIR\example-HorizontalPacking"
SetOutPath "$INSTDIR\example-HorizontalPacking"
File "example-HorizontalPacking\HorizontalPacking.xml"
File "example-Grids\VGrid.xml"

CreateDirectory "$INSTDIR\example-Images"
SetOutPath "$INSTDIR\example-Images"
File "example-Images\GreyscaleImage.xml"
File "example-Images\RGBAimage.xml"
File "example-Images\RGBimage.xml"
File "example-Images\letter.png"
File "example-Images\plasma.png"
File "example-Images\star.png"
File "example-Grids\VGrid.xml"

CreateDirectory "$INSTDIR\example-Input-Buttons"
SetOutPath "$INSTDIR\example-Input-Buttons"
File "example-Input-Buttons\InputButtons.xml"

CreateDirectory "$INSTDIR\example-Margins"
SetOutPath "$INSTDIR\example-Margins"
File "example-Margins\Margins.xml"

CreateDirectory "$INSTDIR\example-MultiScreen"
SetOutPath "$INSTDIR\example-MultiScreen"
File "example-MultiScreen\MultiScreen.xml"

CreateDirectory "$INSTDIR\example-StackAndSet"
SetOutPath "$INSTDIR\example-StackAndSet"
File "example-StackAndSet\CombinedSetStack.xml"
File "example-StackAndSet\facebook-48.png"
File "example-StackAndSet\Set.xml"
File "example-StackAndSet\Stack.xml"

CreateDirectory "$INSTDIR\example-Style"
SetOutPath "$INSTDIR\example-Style"
File "example-Style\Style.xml"

CreateDirectory "$INSTDIR\example-Text"
SetOutPath "$INSTDIR\example-Text"
File "example-Text\Text.xml"

CreateDirectory "$INSTDIR\example-VerticalPacking"
SetOutPath "$INSTDIR\example-VerticalPacking"
File "example-VerticalPacking\VerticalPacking.xml"

CreateDirectory "$INSTDIR\RootApplication"
SetOutPath "$INSTDIR\RootApplication"
File "RootApplication\aim.png"
File "RootApplication\calc.png"
File "RootApplication\facebook.png"
File "RootApplication\hello.png"
File "RootApplication\mail.png"
File "RootApplication\maps.png"
File "RootApplication\netsurf.png"
File "RootApplication\sms.png"
File "RootApplication\twitter.png"
File "RootApplication\BootApplication.xml"
File "RootApplication\RootApplication.xml"
File "RootApplication\RootStyle.xml"
File "RootApplication\WeatherStatus.xml"

SetOutPath "$INSTDIR"
  ;Store installation folder
  WriteRegStr HKCU "Software\Emobiix" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Peek Simulator.lnk" "$INSTDIR\peek-simulator.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "Simulator Binaries."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

Delete "$INSTDIR\battery1.png"
Delete "$INSTDIR\battery2.png"
Delete "$INSTDIR\battery3.png"
Delete "$INSTDIR\battery4.png"
Delete "$INSTDIR\battery5.png"
Delete "$INSTDIR\battery6.png"
Delete "$INSTDIR\batterycase.png"
Delete "$INSTDIR\batterycharge.png"
Delete "$INSTDIR\calc.xml"
Delete "$INSTDIR\DroidSans.ttf"
Delete "$INSTDIR\glut32.dll"
Delete "$INSTDIR\gprs.png"
Delete "$INSTDIR\home-server.ini"
Delete "$INSTDIR\letter.png"
Delete "$INSTDIR\LockApp.xml"
Delete "$INSTDIR\opengl32.dll"
Delete "$INSTDIR\peek-simulator.exe"
Delete "$INSTDIR\plasma.png"
Delete "$INSTDIR\signal0.png"
Delete "$INSTDIR\signal1.png"
Delete "$INSTDIR\signal2.png"
Delete "$INSTDIR\signal3.png"
Delete "$INSTDIR\signal4.png"
Delete "$INSTDIR\signal5.png"
Delete "$INSTDIR\spkr-off.png"
Delete "$INSTDIR\spkr-on.png"
Delete "$INSTDIR\star.png"
Delete "$INSTDIR\street64.png"
Delete "$INSTDIR\sun.png"
Delete "$INSTDIR\wi-cloud.png"
Delete "$INSTDIR\wi-rain.png"
Delete "$INSTDIR\wi-snow.png"
Delete "$INSTDIR\wi-storm.png"
Delete "$INSTDIR\wi-sun.png"
Delete "$INSTDIR\xerces-c_2_2_0.dll"
  Delete "$INSTDIR\Uninstall.exe"
  
  RMDir "$INSTDIR"
  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\Peek Simulator.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  
  DeleteRegKey /ifempty HKCU "Software\Emobiix"

SectionEnd

; Attempt to give the UAC plug-in a user process and an admin process.
Function .OnInit
 
UAC_Elevate:
    UAC::RunElevated 
    StrCmp 1223 $0 UAC_ElevationAborted ; UAC dialog aborted by user?
    StrCmp 0 $0 0 UAC_Err ; Error?
    StrCmp 1 $1 0 UAC_Success ;Are we the real deal or just the wrapper?
    Quit
 
UAC_Err:
    MessageBox mb_iconstop "Unable to elevate, error $0"
    Abort
 
UAC_ElevationAborted:
    # elevation was aborted, run as normal?
    MessageBox mb_iconstop "This installer requires admin access, aborting!"
    Abort
 
UAC_Success:
    StrCmp 1 $3 +4 ;Admin?
    StrCmp 3 $1 0 UAC_ElevationAborted ;Try again?
    MessageBox mb_iconstop "This installer requires admin access, try again"
    goto UAC_Elevate 
 
FunctionEnd

Function .OnInstFailed
    UAC::Unload ;Must call unload!
FunctionEnd
 
Function .OnInstSuccess
    UAC::Unload ;Must call unload!
FunctionEnd


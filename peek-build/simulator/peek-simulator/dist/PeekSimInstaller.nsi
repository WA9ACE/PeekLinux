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
  
File "battery1.png"
File "battery2.png"
File "battery3.png"
File "battery4.png"
File "battery5.png"
File "battery6.png"
File "batterycase.png"
File "batterycharge.png"
File "calc.xml"
File "DroidSans.ttf"
File "glut32.dll"
File "gprs.png"
File "home-server.ini"
File "letter.png"
File "LockApp.xml"
File "opengl32.dll"
File "peek-simulator.exe"
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
  LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

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

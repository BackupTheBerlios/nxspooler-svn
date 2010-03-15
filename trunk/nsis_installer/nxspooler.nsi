;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NSIS script for creating NxSpooler installers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Note: this file is in CP1252 (Windows-1252) encoding.

;--------------------------------
;Let's use the "Modern UI".

  !include "MUI2.nsh"

;--------------------------------
;General

  !ifndef PROG_NAME
    !define PROG_NAME "NxSpooler"
  !endif

  ;Define the default value of VERSION. Because of the "ifndef", if we didn't define VERSION in 
  ;this file, it could be defined in the compiler or in the command-line when compiling this script.
  !ifndef VERSION
    !define VERSION "1.0"
  !endif

  Name "${PROG_NAME}"

  ;Icon to show when installing the program.
  !ifndef MUI_ICON
    !define MUI_ICON "..\nxspooler.ico"
  !endif

  ;Icon to show when uninstalling the program.
  !ifndef MUI_UNICON
    !define MUI_UNICON "uninstall.ico"
  !endif

  !ifndef TARGET
    !define TARGET "win32"
  !endif

  ;Name of the installer. 
  OutFile ${PROG_NAME}-${VERSION}-${TARGET}.exe
  
  ;Place where the Qt dlls are. This value is only used when compiling this script.
  !ifndef QT_DLL_FOLDER
    !define QT_DLL_FOLDER "c:\Qt\2010.02.1\qt"
  !endif

  ;Default installation folder.
  InstallDir "$PROGRAMFILES\${PROG_NAME}"
  
  !ifndef PROGRAM_GROUP_IN_REGISTRY
    !define PROGRAM_GROUP_IN_REGISTRY "Creación y Diseño Ibense"
  !endif

  !ifndef PATH_IN_REGISTRY
    !define PATH_IN_REGISTRY "${PROGRAM_GROUP_IN_REGISTRY}\${PROG_NAME}"
  !endif

  ;Get installation folder from registry if available.
  InstallDirRegKey HKCU "Software\${PATH_IN_REGISTRY}" ""

  ;The caption which the window of the program has. 
  ;In this case it's the same as the name of the program.
  !ifndef WINDOW_CAPTION_OF_THE_PROG
    !define WINDOW_CAPTION_OF_THE_PROG ${PROG_NAME}
  !endif

  ;Request application privileges for Windows Vista.
  RequestExecutionLevel user

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Language Selection Dialog Settings

  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\${PATH_IN_REGISTRY}" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
;Pages

  ;Show the license
  !insertmacro MUI_PAGE_LICENSE "..\gpl.txt"

  ;There the user could choose between a typical installation, a full one, etc
  ;Not needed because we have only one component to install: 
  ;!insertmacro MUI_PAGE_COMPONENTS

  ; Select the folder where to install the program
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${PATH_IN_REGISTRY}" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

  ;Show the process of installing the files
  !insertmacro MUI_PAGE_INSTFILES

  ;At the end, let the user select if the program will be executed or not
  !define MUI_FINISHPAGE_RUN "$INSTDIR\${PROG_NAME}.exe"
  !define MUI_FINISHPAGE_NOAUTOCLOSE
  !insertmacro MUI_PAGE_FINISH
  
  ;Prepare the uninstaller pages
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English" ;first language is the default language
  !insertmacro MUI_LANGUAGE "SpanishInternational"

;--------------------------------
;Reserve Files
  
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.
  
  !insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
;Installer Sections

;Prepare the name of the icon that the user will see in the start menu 
;and can use to uninstall the program.
LangString CaptionOfTheUninstallIcon ${LANG_ENGLISH} "Uninstall"
LangString CaptionOfTheUninstallIcon ${LANG_SPANISHINTERNATIONAL} "Desinstalar"

Section "Section Name 1" Section1

  SetOutPath "$INSTDIR"
  
  ;Files to install
  File ..\release\${PROG_NAME}.exe
  File ..\${PROG_NAME}_es.qm
  File "${QT_DLL_FOLDER}\bin\mingwm10.dll"
  File "${QT_DLL_FOLDER}\bin\libgcc_s_dw2-1.dll"
  File "${QT_DLL_FOLDER}\bin\QtCore4.dll"
  File "${QT_DLL_FOLDER}\bin\QtGui4.dll"
  File "${QT_DLL_FOLDER}\bin\QtNetwork4.dll"
  File "${QT_DLL_FOLDER}\translations\qt_ar.qm"
  File "${QT_DLL_FOLDER}\translations\qt_da.qm"
  File "${QT_DLL_FOLDER}\translations\qt_de.qm"
  File "${QT_DLL_FOLDER}\translations\qt_es.qm"
  File "${QT_DLL_FOLDER}\translations\qt_fr.qm"
  File "${QT_DLL_FOLDER}\translations\qt_iw.qm"
  File "${QT_DLL_FOLDER}\translations\qt_ja_JP.qm"
  File "${QT_DLL_FOLDER}\translations\qt_pl.qm"
  File "${QT_DLL_FOLDER}\translations\qt_pt.qm"
  File "${QT_DLL_FOLDER}\translations\qt_ru.qm"
  File "${QT_DLL_FOLDER}\translations\qt_sk.qm"
  File "${QT_DLL_FOLDER}\translations\qt_sl.qm"
  File "${QT_DLL_FOLDER}\translations\qt_sv.qm"
  File "${QT_DLL_FOLDER}\translations\qt_uk.qm"
  File "${QT_DLL_FOLDER}\translations\qt_zh_CN.qm"
  File "${QT_DLL_FOLDER}\translations\qt_zh_TW.qm"
  
  ;Store installation folder
  WriteRegStr HKCU "Software\${PATH_IN_REGISTRY}" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application

    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(CaptionOfTheUninstallIcon).lnk" "$INSTDIR\Uninstall.exe" 
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\${PROG_NAME}.lnk" "$INSTDIR\${PROG_NAME}.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END

  ;Add entries to the "Add/Remove Programs" section in the Control Panel
  !ifndef DISPLAY_NAME_IN_ADD_REMOVE_PROGRAMS
    !define DISPLAY_NAME_IN_ADD_REMOVE_PROGRAMS "${PROG_NAME} ${VERSION}"
  !endif
  WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROG_NAME}" \
                 "DisplayName" "${DISPLAY_NAME_IN_ADD_REMOVE_PROGRAMS}"
  WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROG_NAME}" \
                 "UninstallString" "$\"$INSTDIR\uninstall.exe$\""

  ;Prepare to run the .exe file on Windows Start
  WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Run" \
                 "${PROG_NAME}" "$INSTDIR\${PROG_NAME}.exe"

SectionEnd

;Note: not needed because the user doesn't choose between components (there's only one component to install): 
;LangString DESC_Section1 ${LANG_ENGLISH} "Description of section 1."
;LangString DESC_Section1 ${LANG_SPANISHINTERNATIONAL} "Descripción de la sección 1."

;--------------------------------
;Installer Functions

LangString CloseProgramBeforeInstallation ${LANG_ENGLISH} "Please close ${PROG_NAME} before you continue with the (re)installation."
LangString CloseProgramBeforeInstallation ${LANG_SPANISHINTERNATIONAL} "Por favor cierre ${PROG_NAME} para continuar con la (re)instalación."

Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

  ; Try to detect if the program to install was running already.
  CheckIfRunning:
      FindWindow $1 "" "${WINDOW_CAPTION_OF_THE_PROG}"
      StrCmp $1 0 ProgramNotFoundRunning
      MessageBox MB_RETRYCANCEL $(CloseProgramBeforeInstallation) IDRETRY CheckIfRunning
      Quit 
  ProgramNotFoundRunning:

FunctionEnd

;--------------------------------
;Descriptions

  ;Assign descriptions to sections
  ;Not needed because we have only one component to install: 
  ;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  ;!insertmacro MUI_DESCRIPTION_TEXT ${Section1} $(DESC_Section1)
  ;!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  Delete "$INSTDIR\${PROG_NAME}.exe"
  Delete "$INSTDIR\Uninstall.exe"
  Delete "$INSTDIR\${PROG_NAME}_es.qm"
  Delete "$INSTDIR\mingwm10.dll"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\QtNetwork4.dll"
  Delete "$INSTDIR\qt_ar.qm"
  Delete "$INSTDIR\qt_da.qm"
  Delete "$INSTDIR\qt_de.qm"
  Delete "$INSTDIR\qt_es.qm"
  Delete "$INSTDIR\qt_fr.qm"
  Delete "$INSTDIR\qt_iw.qm"
  Delete "$INSTDIR\qt_ja_JP.qm"
  Delete "$INSTDIR\qt_pl.qm"
  Delete "$INSTDIR\qt_pt.qm"
  Delete "$INSTDIR\qt_ru.qm"
  Delete "$INSTDIR\qt_sk.qm"
  Delete "$INSTDIR\qt_sl.qm"
  Delete "$INSTDIR\qt_sv.qm"
  Delete "$INSTDIR\qt_uk.qm"
  Delete "$INSTDIR\qt_zh_CN.qm"
  Delete "$INSTDIR\qt_zh_TW.qm"
  
  ;That will not remove all the existing files inside the folder, because 
  ;some could be data files of the user.
  RMDir "$INSTDIR"
  ;That will try to remove all the existing files inside this temporary folder.
  RMDir /r "$PROFILE\.${PROG_NAME}"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  
  Delete "$SMPROGRAMS\$StartMenuFolder\$(CaptionOfTheUninstallIcon).lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\${PROG_NAME}.lnk"  
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  DeleteRegKey /ifempty HKCU "Software\${PATH_IN_REGISTRY}"
  DeleteRegKey /ifempty HKCU "Software\${PROGRAM_GROUP_IN_REGISTRY}"

  ;Stop trying to run the .exe file on Windows Start
  DeleteRegValue HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Run" "${PROG_NAME}"

  ;Remove entries from the "Add/Remove Programs" section in the Control Panel
  DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROG_NAME}"

SectionEnd

;--------------------------------
;Uninstaller Functions

LangString CloseProgramBeforeUninstallation ${LANG_ENGLISH} "Please close ${PROG_NAME} before you continue uninstallation."
LangString CloseProgramBeforeUninstallation ${LANG_SPANISHINTERNATIONAL} "Por favor cierre ${PROG_NAME} para continuar con la desinstalación."

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE

  ; Try to detect if the program to install was running already.
  CheckIfRunning:
      FindWindow $1 "" "${WINDOW_CAPTION_OF_THE_PROG}"
      StrCmp $1 0 ProgramNotFoundRunning
      MessageBox MB_RETRYCANCEL $(CloseProgramBeforeUninstallation) IDRETRY CheckIfRunning
      Quit
  ProgramNotFoundRunning:

FunctionEnd

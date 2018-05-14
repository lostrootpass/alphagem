!define APPNAME "AlphaGem"
!define COMPANYNAME "Pete Murdoch"
!define DESCRIPTION "AlphaGem"

!define VERSIONMAJOR 0
!define VERSIONMINOR 0
!define VERSIONBUILD 2

!define HELPURL 	"http://github.com/lostrootpass/alphagem"
!define UPDATEURL 	"http://github.com/lostrootpass/alphagem"
!define ABOUTURL 	"http://github.com/lostrootpass/alphagem"

!define INSTALLSIZE 25793

RequestExecutionLevel admin
OutFile "AlphaGemInstall0.2.exe"
InstallDir "$PROGRAMFILES\${APPNAME}"
LicenseData "License.txt"
Name "${APPNAME}"

page license
page directory
page instfiles

#-------------------
#Install AlphaGem
#-------------------
function .onInit
	setShellVarContext all
functionEnd

section "install"

	setOutPath $INSTDIR\..
	writeUninstaller $INSTDIR\uninstall.exe
	
	File /r AlphaGem
	
	createDirectory "$SMPROGRAMS\"
	createShortcut "$SMPROGRAMS\${APPNAME}.lnk "$INSTDIR\${APPNAME}.exe" "" ""
	
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "InstallLocation" "$\"$INSTDIR$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayIcon" "$\"$INSTDIR\logo.ico$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "Publisher" "$\"${COMPANYNAME}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "HelpLink" "$\"${HELPURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLUpdateInfo" "$\"${UPDATEURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLInfoAbout" "$\"${ABOUTURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayVersion" "$\"${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}$\""
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMajor" ${VERSIONMAJOR}
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMinor" ${VERSIONMINOR}
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoRepair" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "EstimatedSize" ${INSTALLSIZE}

sectionEnd

#------------------
#Uninstall
#------------------
function un.onInit
	setShellVarContext all
	
	MessageBox MB_OKCANCEL|MB_ICONINFORMATION "Permanently remove ${APPNAME}?$\r$\nDownloaded podcasts will remain in your chosen directory." IDOK next
		Abort
	next:
functionEnd

section "Uninstall"
	
	delete "$SMPROGRAMS\${APPNAME}.lnk"
	rmDir /r "$SMPROGRAMS\${APPNAME}"
	
	rmDir /r $INSTDIR
	rmDir "$PROGRAMFILES\${APPNAME}"
	
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"

sectionEnd
; Installer for MediaControl Flutter App - Runs in background on boot

[Setup]
AppName=MediaControl
AppVersion=1.0
DefaultDirName={pf}\MediaControl
DefaultGroupName=MediaControl
OutputDir=.
OutputBaseFilename=MediaControlSetup
Compression=lzma
SolidCompression=yes
SetupIconFile=assets\app_icon.ico
UninstallDisplayIcon={app}\MediaControl.exe

[Files]
Source: "App\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\MediaControl"; Filename: "{app}\MediaControl.exe"; IconFilename: "{app}\MediaControl.exe"
Name: "{group}\Uninstall MediaControl"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\MediaControl.exe"; Description: "Launch MediaControl"; Flags: nowait postinstall skipifsilent

[Registry]
; Add startup entry to run at Windows login (current user)
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; \
    ValueType: string; ValueName: "MediaControl"; ValueData: """{app}\MediaControl.exe"""; Flags: uninsdeletevalue

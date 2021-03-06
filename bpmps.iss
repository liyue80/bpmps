; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{1884E6D7-A9B8-4011-9E09-2C91922DDA91}
AppName=Anysoft Bpmps
AppVersion=1.5
;AppVerName=Anysoft Bpmps 1.5
AppPublisher=Anysoft Company, Inc.
DefaultDirName={pf}\Anysoft
DefaultGroupName=Anysoft
;OutputDir=
;SourceDir=
OutputBaseFilename=bpmps-setup
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "Release\Bpmps.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "Release\libmysql.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dump.sql"; DestDir: "{app}"; Flags: ignoreversion
Source: "init.bat"; DestDir: "{app}"; Flags: ignoreversion
Source: "mysql\*"; DestDir: "{app}\mysql"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Anysoft Bpmps"; Filename: "{app}\Bpmps.exe"
Name: "{group}\{cm:UninstallProgram,Anysoft Bpmps}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\Anysoft Bpmps"; Filename: "{app}\Bpmps.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\Bpmps.exe"; Description: "{cm:LaunchProgram,Anysoft Bpmps}"; Flags: nowait postinstall skipifsilent
Filename: "{app}\mysql\bin\mysqld.exe"; Parameters: "--install AnysoftMySQL --defaults-file=""{app}\mysql\my.ini"""; Description: "Install MySQL service"; StatusMsg: "Installing service..."
Filename: "net"; Parameters: "start AnysoftMySQL"
;Filename: "{app}\mysql\bin\mysql.exe"; Parameters: "-u root -h localhost test < {app}\dump.sql >> c:\debug.txt"
Filename: "{app}\init.bat"
[UninstallRun]
Filename: "net"; Parameters: "stop AnysoftMySQL"
Filename: "sc"; Parameters: "delete AnysoftMySQL"

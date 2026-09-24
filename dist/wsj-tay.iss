; WSJ-TAY installer - Inno Setup 6
;
; Deliberately a per-user install: no administrator rights, nothing added to
; PATH, no file associations, and the only registry entries are the ones that
; make Windows' own uninstall entry work. The machine this was written on had
; three orphaned WSJT-X entries in Installed Apps pointing at uninstallers that
; had been deleted, and three stale PATH entries, two of them malformed. This
; installer cannot leave that behind: everything it writes, it removes.

#define AppName    "WSJ-TAY"
#define AppVersion "v2.1.0"
#define AppPub     "SV1TAY"
#define AppURL     "https://sv1tay.com"
#define AppExe     "wsj-tay.exe"

[Setup]
; Keep this GUID for the life of the program - it is how Windows recognises an
; upgrade rather than a second, parallel install.
AppId={{8E3C1A64-5B27-4F19-9E4D-3A7C2F5D8B10}
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher={#AppPub}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}
VersionInfoVersion=2.0.0.0
VersionInfoDescription={#AppName} - decode two receive sources at once

; lowest = install for this user only, no UAC prompt. {autopf} then resolves to
; %LOCALAPPDATA%\Programs.
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
DefaultDirName={autopf}\{#AppName}
DefaultGroupName={#AppName}
DisableProgramGroupPage=yes
AllowNoIcons=yes

LicenseFile=stage\WSJ-TAY\COPYING.txt
InfoBeforeFile=stage\WSJ-TAY\README.txt

OutputDir=.
OutputBaseFilename={#AppName}-v2.1.0-setup
SetupIconFile=..\icons\windows-icons\wsj-tay.ico
UninstallDisplayIcon={app}\bin\{#AppExe}
UninstallDisplayName={#AppName} {#AppVersion}

Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

; If it is running, shut it down cleanly rather than failing halfway through.
CloseApplications=yes
RestartApplications=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Shortcuts:"

[Files]
Source: "stage\WSJ-TAY\app\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs ignoreversion
Source: "stage\WSJ-TAY\README.txt";   DestDir: "{app}"; Flags: ignoreversion
Source: "stage\WSJ-TAY\FEATURES.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "stage\WSJ-TAY\TI-PROSTHESAME.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "stage\WSJ-TAY\COPYING.txt";  DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#AppName}";            Filename: "{app}\bin\{#AppExe}"; WorkingDir: "{app}\bin"
Name: "{group}\What differs";          Filename: "{app}\FEATURES.txt"
Name: "{group}\Ti prosthesame (GR)"; Filename: "{app}\TI-PROSTHESAME.txt"
Name: "{group}\Uninstall {#AppName}";  Filename: "{uninstallexe}"
Name: "{autodesktop}\{#AppName}";      Filename: "{app}\bin\{#AppExe}"; WorkingDir: "{app}\bin"; Tasks: desktopicon

[Run]
Filename: "{app}\bin\{#AppExe}"; Description: "Start {#AppName}"; WorkingDir: "{app}\bin"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
; Files the program makes next to itself while it runs. Settings, log and
; ALL.TXT live under %LOCALAPPDATA%\wsj-tay and are deliberately left alone:
; removing a program should not throw away the operator's log.
Type: files;      Name: "{app}\bin\*.log"
Type: dirifempty; Name: "{app}\bin"
Type: dirifempty; Name: "{app}"

[Messages]
WelcomeLabel2=This will install [name/ver] for the current user only.%n%nIt needs no administrator rights, adds nothing to PATH, and does not touch any WSJT-X, JTDX, MSHV or WSJT-Z you already have. WSJ-TAY keeps its own settings, so those programs will not notice it.

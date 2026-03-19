[Setup]
AppName=GPS-Seeder
AppVersion=0.1
DefaultDirName={pf}\GPS-Seeder
DefaultGroupName=GPS-Seeder
OutputBaseFilename=GPSSeeder-Setup
Compression=lzma
SolidCompression=yes

; 使用脚本所在目录的相对路径，避免写死绝对路径
SetupIconFile={#SourcePath}\resource\icon_64x64.ico



[Files]
Source: {#SourcePath}\build\GPSSeeder.exe; DestDir: "{app}"; Flags: ignoreversion
Source: {#SourcePath}\build\*.dll; DestDir: "{app}"; Flags: ignoreversion

; Qt 插件目录：递归复制，确保子目录也被包含
Source: {#SourcePath}\build\platforms\*; DestDir: "{app}\\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: {#SourcePath}\build\iconengines\*; DestDir: "{app}\\iconengines"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: {#SourcePath}\build\imageformats\*; DestDir: "{app}\\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: {#SourcePath}\build\styles\*; DestDir: "{app}\\styles"; Flags: ignoreversion recursesubdirs createallsubdirs

; QtSql 驱动（如使用 SQLite）
Source: {#SourcePath}\build\sqldrivers\*; DestDir: "{app}\\sqldrivers"; Flags: ignoreversion recursesubdirs createallsubdirs

; 资源文件
Source: {#SourcePath}\resource\*; DestDir: "{app}\\resource"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\\GPS-Seeder"; Filename: "{app}\\GPSSeeder.exe"
Name: "{commondesktop}\\GPS-Seeder"; Filename: "{app}\\GPSSeeder.exe"

[Run]
Filename: "{app}\\GPSSeeder.exe"; Description: "启动GPS-Seeder"; Flags: nowait postinstall skipifsilent

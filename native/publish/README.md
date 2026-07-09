# Windows INI Launcher

[简体中文](README.zh-Hans.md) / [繁體中文](README.zh-Hant.md)

## What It Does

Put the EXE beside an INI file with the same base name. The INI decides what the launcher does.

It can:

- run silently with no window;
- open local files or programs;
- import `.reg` files;
- show a small native window with text, buttons, input boxes, text boxes, text-file boxes, and CSV tables;
- show a local PNG splash image;
- use variables so the same config can work in different folders or on different computers.

The program uses common Windows APIs and standard system controls. It is built as one EXE and is intended for Windows 10 and Windows 11.

## Build

Main source:

```text
native/src/XHLauncher.cpp
```

Build command:

```powershell
powershell -ExecutionPolicy Bypass -File native\build.ps1 -Arch x86
```

The default target is x86 so the same EXE can run on both 32-bit and 64-bit Windows.

## Quick Start

1. Put the EXE in the folder you want to use.
2. Run it once. If the matching INI is missing, the program creates a default template.
3. The default template opens editor mode.
4. Choose a mode, fill the settings, then click **Save and Close**.
5. Run the EXE again.

If the EXE is renamed, the INI name follows it. For example, `Tools.exe` reads `Tools.ini`.

## Modes

`Mode=0` opens editor mode. It edits the config only. It does not open files or import registry files.

`Mode=1` runs silent mode. It has no window and no success message. It reads only `Mode1.*` sections and exits after the configured actions are handed to Windows.

`Mode=2` opens window mode. It shows a fixed-width native window and reads only `Mode2.*` sections.

Unknown sections and unknown keys are ignored. Hidden or disabled items are not fully checked because they are not used by the current run.

Values after = are kept as written. Leading and trailing spaces are part of the value. Control fields such as Mode, 0/1 switches, Type, language, and numeric fields may ignore surrounding spaces.

## Language

Use one of these three values: `Language=en-US` for English, `Language=zh-Hans` for Simplified Chinese, or `Language=zh-Hant` for Traditional Chinese.

Editor mode shows the embedded ReadMe for the selected language. English is the default.

## Encoding and Size

The main INI supports:

- UTF-8 without BOM;
- UTF-8 with BOM;
- UTF-16 LE with BOM.

ANSI, GBK, and other local code pages are not supported for the main INI.

The default template uses UTF-8 with BOM so older Windows tools can open it more safely.

The INI limit is about 4 MB. Keep it for launcher settings. Put long notes in a separate text file and show them with a TextFileBox element.

If the config is missing, empty, too large, uses the wrong encoding, or contains invalid numeric state values, the program shows one config-read-failed message.

## Paths

The recommended Windows separator is `\`. The program also accepts `/` and changes it to `\` at run time.

Relative path examples:

```text
App.exe
.\App.exe
Folder\App.exe
Folder/Sub/App.exe
```

Relative paths are based on the EXE folder. They must not use a drive name, a UNC path, a root path, or `..` to leave the EXE folder.

Absolute path examples:

```text
C:\Tools\App.exe
C:/Tools/App.exe
\\Server\Share\File.txt
%USERPROFILE%\Desktop\App.exe
%XHLROOT%\Tools\App.exe
```

Unsupported examples:

```text
C:Folder\App.exe
\Folder\App.exe
/Folder/App.exe
file://C:/Folder/App.exe
```

`UseRelativePath=1` means the field is relative. It only replaces variables from the current mode. It does not expand Windows environment variables or `%XHLROOT%`.

`UseRelativePath=0` means the field is absolute. It supports mode variables, Windows environment variables, and `%XHLROOT%`.

`%XHLROOT%` means the folder that contains the current EXE. It has no trailing backslash for normal folders, so write `%XHLROOT%\Tools\App.exe` when joining a file name.

## Variables

Mode1 variables are `%XHLM1V1%` to `%XHLM1V6%`. Each has `Enable` and `Value`. If a variable is not enabled, its token is treated as normal text.

Mode2 variables are `%XHLM2V1%` to `%XHLM2V6%`. They are written by Input elements in the window. The value is temporary and is not written back to the INI. If a token is used but no Input element writes that variable, the token is treated as normal text.

If several Mode2 Input elements write the same variable, the later element number wins.

Variables can appear next to other text, such as `ABC%XHLM1V1%123`. Variables are for runtime text fields such as paths, arguments, working directory paths, display text, and messages. They are not used in control fields such as `Mode`, `Language`, `Show`, `Type`, `Run`, `UseRelativePath`, `HeightScale`, `Variable`, or `WorkingDirectoryMode`.

One variable, input value, or normal text field supports about 4095 characters.

After variable replacement at run time, the final text used for paths, arguments, working directories, messages, and other runtime fields is limited to 32767 UTF-16 characters. If it is longer, the value is treated as invalid and is skipped or left empty.

## Working Directory

When opening a file, starting a program, or importing a registry file, the default working directory is the target file folder.

Use:

- `WorkingDirectoryMode=TargetDir`
- `WorkingDirectoryMode=LauncherDir`
- `WorkingDirectoryMode=Custom`

`WorkingDirectory` is used only when the mode is `Custom`. The final path must be an existing folder.

## Splash Image

Splash images are optional and are controlled by the `Splash*` keys in `[Launcher]`.

Rules:

- PNG only;
- alpha transparency is supported;
- maximum file size is 12 MB;
- maximum source size is 3840x2160;
- final rendered size is limited to 3840x2160;
- missing or invalid files are skipped silently;
- no animation and no fade effect.

The image is scaled with Windows Imaging Component and the current screen DPI.

## Registry Import

Mode1 can import one registry file. Mode2 has a registry button type.

The registry file must exist and must end with `.reg`.

Registry import waits up to 60 seconds. If it finishes in time, the real `reg.exe` result decides success or failure. If it is still running after 60 seconds, the program stops it and treats it as failure.

The program runs with the current user permissions. HKCU usually works without administrator rights. HKLM, HKCR, and other system-level areas usually require running the EXE as administrator.

Mode1 stays silent. Mode2 shows success, failure, or timeout messages.

## Window Elements

Window mode reads `Mode2.Element1` to `Mode2.Element10`. Display order is fixed by element number.

Supported element types:

- `Text`: one single-line text element. It shows up to 512 characters. Longer text is cut silently.
- `TextBox`: read-only scrolling text written in the INI. Use it for longer notes that are still small enough to keep in the INI.
- `TextFileBox`: read-only scrolling text loaded from a local text file. UTF-8 is tried first. If the file is not valid UTF-8, the system default code page is used. CRLF and LF line breaks are both supported. File size is up to about 4 MB.
- `Input`: a text input box. Its value becomes one Mode2 variable during the current run. If several Input elements write the same variable, the later element number wins.
- `OpenButton`: opens up to five configured files or programs. Set `MinimizeOnClick=1` to also minimize the window after the click.
- `MessageButton`: shows a message. Keep the message on one INI line and use `\n` for real line breaks. Set `MinimizeOnClick=1` to also minimize the window after the click.
- `RegButton`: imports a `.reg` file. The file must exist and the final path must end with `.reg`. Mode2 shows success, failure, or timeout messages. Set `MinimizeOnClick=1` to also minimize the window after the click.
- `Table`: shows a basic CSV file. The first row is the header. Up to 20 columns and 1000 rows are shown. Each cell shows about 256 characters. File size is up to about 4 MB. Commas, quoted fields, escaped quotes, and line breaks inside quoted fields are supported. Invalid quotes make the table blank. Sorting, filtering, editing, formulas, and images are not supported.
- `CloseButton`: closes the launcher window.

`TextBox`, `TextFileBox`, and `Table` support `HeightScale=1`, `2`, or `3`. Higher values use more screen height. On small or high-DPI screens, too many large elements may make the window taller than the visible screen.

## Troubleshooting

If the config cannot be read, check:

- the INI file name matches the EXE file name;
- the file is not empty;
- the file is UTF-8 or UTF-16 LE with BOM;
- the file is not larger than about 4 MB;
- `Mode` is 0, 1, or 2;
- numeric switches are 0 or 1.

If you cannot fix the config, back it up first, then delete it. The program will create a default config next time.

## Default Configuration Template

```ini
##Default configuration template
##If the same-name INI does not exist, the program creates this template.
##Default Mode=0 opens editor mode. Editor mode shows the embedded English ReadMe by default.
##The default template uses UTF-8 with BOM. The program supports UTF-8 without BOM, UTF-8 with BOM, and UTF-16 LE with BOM.
##ANSI, GBK, and other local code pages are not supported for the main INI.
##Comment lines start with # or ;. Lines without = are ignored. Unknown sections and unknown keys are ignored.
##Values after = are kept as written. Spaces at the start or end are part of the value. Control fields may ignore surrounding spaces.
##Mode must be 0, 1, or 2. Numeric switches such as Enable, Run, Show, UseRelativePath, AddReg, and RegImported must be 0 or 1.
##If the config cannot be repaired, back it up first, then delete it. The program will create a default config next time.
##Relative path fields only replace variables from the current mode. Absolute path fields, arguments, and custom working directories also support Windows environment variables and %XHLROOT%.
##Variables are not used in control fields such as Mode, Language, Show, Type, Run, UseRelativePath, HeightScale, Variable, or WorkingDirectoryMode.
##Registry import uses current user permissions. HKCU usually works without administrator rights. HKLM, HKCR, and other system areas usually require running the EXE as administrator.

[Launcher]
Title=Launcher
##Window title for editor mode and window mode. Up to 32 characters.
Version=1.0.3.0
##Text shown after the title. Up to 32 characters. Any text is allowed.
Language=en-US
##UI language. Use one value only: en-US for English, zh-Hans for Simplified Chinese, or zh-Hant for Traditional Chinese.
Mode=0
##Run mode. 0=editor mode, 1=silent mode, 2=window mode.
SplashEnable=0
##Splash switch. 0=off, 1=on. If enabled but the path is empty or invalid, the splash is skipped silently.
SplashUseRelativePath=1
##Splash path type. 1=relative path from the EXE folder, 0=absolute path.
SplashImage=
##PNG splash image path. PNG only. Alpha transparency is supported.
SplashDurationMs=1000
##Splash display time in milliseconds. When SplashEnable=1, this must be 1 to 60000.
SplashScale=100
##Splash scale. When SplashEnable=1, enter 10 to 200 without the percent sign. The editor uses a drop-down list in 5% steps.
SplashPosition=Center
##Splash position. When SplashEnable=1, use Center, TopLeft, TopRight, BottomLeft, or BottomRight.

[Mode1.Variable1]
Enable=0
##Use 0 or 1. 1 enables %XHLM1V1%.
Value=
##Variable value. Supports about 4095 characters. Runtime text after replacement is limited to 32767 UTF-16 characters.

[Mode1.Variable2]
Enable=0
##Use 0 or 1. 1 enables %XHLM1V2%.
Value=
##Variable value. Supports about 4095 characters. Runtime text after replacement is limited to 32767 UTF-16 characters.

[Mode1.Variable3]
Enable=0
##Use 0 or 1. 1 enables %XHLM1V3%.
Value=
##Variable value. Supports about 4095 characters. Runtime text after replacement is limited to 32767 UTF-16 characters.

[Mode1.Variable4]
Enable=0
##Use 0 or 1. 1 enables %XHLM1V4%.
Value=
##Variable value. Supports about 4095 characters. Runtime text after replacement is limited to 32767 UTF-16 characters.

[Mode1.Variable5]
Enable=0
##Use 0 or 1. 1 enables %XHLM1V5%.
Value=
##Variable value. Supports about 4095 characters. Runtime text after replacement is limited to 32767 UTF-16 characters.

[Mode1.Variable6]
Enable=0
##Use 0 or 1. 1 enables %XHLM1V6%.
Value=
##Variable value. Supports about 4095 characters. Runtime text after replacement is limited to 32767 UTF-16 characters.

[Mode1.Registry]
AddReg=0
##Use 0 or 1. 1 lets silent mode import the .reg file from RegFileName.
UseRelativePath=1
##Registry path type. 1=relative path, 0=absolute path.
RegFileName=
##Registry path. It must be an existing .reg file.
RegImported=0
##Mark as imported. 0=allow import, 1=skip import. The program writes 1 after a successful import.
WorkingDirectoryMode=TargetDir
##Registry import working directory. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom. The final path must be an existing folder.

[Mode1.Run1]
Run=0
##Use 0 or 1. 1 runs item 1 in silent mode.
UseRelativePath=1
##Path type. 1=relative path, 0=absolute path.
FileName=
##File path. It must be an existing file. Folder paths are skipped.
Arguments=
##Launch arguments. Mainly for programs. Mode1 variables, Windows variables, and %XHLROOT% are supported.
WorkingDirectoryMode=TargetDir
##Working directory mode. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom. The final path must be an existing folder.

[Mode1.Run2]
Run=0
##Use 0 or 1. 1 runs item 2 in silent mode.
UseRelativePath=1
##Path type. 1=relative path, 0=absolute path.
FileName=
##File path. It must be an existing file. Folder paths are skipped.
Arguments=
##Launch arguments. Mainly for programs. Mode1 variables, Windows variables, and %XHLROOT% are supported.
WorkingDirectoryMode=TargetDir
##Working directory mode. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom. The final path must be an existing folder.

[Mode1.Run3]
Run=0
##Use 0 or 1. 1 runs item 3 in silent mode.
UseRelativePath=1
##Path type. 1=relative path, 0=absolute path.
FileName=
##File path. It must be an existing file. Folder paths are skipped.
Arguments=
##Launch arguments. Mainly for programs. Mode1 variables, Windows variables, and %XHLROOT% are supported.
WorkingDirectoryMode=TargetDir
##Working directory mode. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom. The final path must be an existing folder.

[Mode1.Run4]
Run=0
##Use 0 or 1. 1 runs item 4 in silent mode.
UseRelativePath=1
##Path type. 1=relative path, 0=absolute path.
FileName=
##File path. It must be an existing file. Folder paths are skipped.
Arguments=
##Launch arguments. Mainly for programs. Mode1 variables, Windows variables, and %XHLROOT% are supported.
WorkingDirectoryMode=TargetDir
##Working directory mode. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom. The final path must be an existing folder.

[Mode1.Run5]
Run=0
##Use 0 or 1. 1 runs item 5 in silent mode.
UseRelativePath=1
##Path type. 1=relative path, 0=absolute path.
FileName=
##File path. It must be an existing file. Folder paths are skipped.
Arguments=
##Launch arguments. Mainly for programs. Mode1 variables, Windows variables, and %XHLROOT% are supported.
WorkingDirectoryMode=TargetDir
##Working directory mode. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom. The final path must be an existing folder.

##Window-mode variables are written by Input elements. Use Variable=1 to 6 to choose %XHLM2V1% to %XHLM2V6%. Runtime text after replacement is limited to 32767 UTF-16 characters.
##Only Element1 to Element10 are read. Display order is Element1 to Element10. Element11 and higher are ignored.
##Supported Type values: Text, TextBox, TextFileBox, Input, OpenButton, MessageButton, RegButton, Table, CloseButton.
##Table supports basic CSV only. The first row is the header. Invalid quotes make the table blank.

[Mode2.Element1]
Show=1
##Use 0 or 1. 1 shows this element.
Type=TextBox
##Element type. When Show=1, this must be Text, TextBox, TextFileBox, Input, OpenButton, MessageButton, RegButton, Table, or CloseButton.
HeightScale=1
##Only for TextBox, TextFileBox, and Table. Use 1, 2, or 3. Higher values may make the window too tall on small or high-DPI screens.
Text=Welcome.\nUse editor mode to change this template.
##Display text. TextBox shows long text, Text shows one line up to 512 characters, and buttons use it as button text up to 512 characters. Input, TextFileBox, and Table do not use this key.
MinimizeOnClick=0
##Only for OpenButton, MessageButton, and RegButton. Use 1 to also minimize the window after clicking the button.
Variable=1
##Only for Input. Use 1 to 6 for %XHLM2V1% to %XHLM2V6%. If several Input elements use the same variable, the later element number wins.
Message=This is a message.\nYou can use \n for line breaks.
##Only for MessageButton. Keep it on one INI line. Use \n for real line breaks.
RegFileName=
##Only for RegButton. Enter an existing .reg file. The final path must end with .reg.
UseRelativePath=1
##Only for RegButton. 1=relative path, 0=absolute path.
WorkingDirectoryMode=TargetDir
##Only for RegButton. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom.
RegImportingText=
##Only for RegButton. Button text shown while importing. Empty means default text.
RegSuccessMessage=
##Only for RegButton. Message shown after success. Empty means default message.
RegFailureMessage=
##Only for RegButton. Message shown after failure. Mention possible partial import if you customize it.
RegTimeoutMessage=
##Only for RegButton. Message shown after the 60-second timeout. Mention possible partial import if you customize it.
CsvFileName=
##Only for Table. CSV path. The final path must end with .csv. Up to 20 columns and 1000 rows are shown.
CsvUseRelativePath=1
##Only for Table. 1=relative path, 0=absolute path.
TextFileName=
##Only for TextFileBox. Text file path. UTF-8 is tried first; if it is not valid UTF-8, the system default code page is used. CRLF and LF line breaks are supported.
TextFileUseRelativePath=1
##Only for TextFileBox. 1=relative path, 0=absolute path.
Run1=0
##Only for OpenButton. 1 runs item 1 when the button is clicked.
UseRelativePath1=1
##Only for OpenButton. 1 means FileName1 is relative, 0 means absolute.
FileName1=
##Only for OpenButton. File path for run item 1.
Arguments1=
##Only for OpenButton. Launch arguments for run item 1.
WorkingDirectoryMode1=TargetDir
##Only for OpenButton. Working directory mode for run item 1.
WorkingDirectory1=
##Only for OpenButton. Custom working directory for run item 1.
Run2=0
##Only for OpenButton. 1 runs item 2 when the button is clicked.
UseRelativePath2=1
##Only for OpenButton. 1 means FileName2 is relative, 0 means absolute.
FileName2=
##Only for OpenButton. File path for run item 2.
Arguments2=
##Only for OpenButton. Launch arguments for run item 2.
WorkingDirectoryMode2=TargetDir
##Only for OpenButton. Working directory mode for run item 2.
WorkingDirectory2=
##Only for OpenButton. Custom working directory for run item 2.
Run3=0
##Only for OpenButton. 1 runs item 3 when the button is clicked.
UseRelativePath3=1
##Only for OpenButton. 1 means FileName3 is relative, 0 means absolute.
FileName3=
##Only for OpenButton. File path for run item 3.
Arguments3=
##Only for OpenButton. Launch arguments for run item 3.
WorkingDirectoryMode3=TargetDir
##Only for OpenButton. Working directory mode for run item 3.
WorkingDirectory3=
##Only for OpenButton. Custom working directory for run item 3.
Run4=0
##Only for OpenButton. 1 runs item 4 when the button is clicked.
UseRelativePath4=1
##Only for OpenButton. 1 means FileName4 is relative, 0 means absolute.
FileName4=
##Only for OpenButton. File path for run item 4.
Arguments4=
##Only for OpenButton. Launch arguments for run item 4.
WorkingDirectoryMode4=TargetDir
##Only for OpenButton. Working directory mode for run item 4.
WorkingDirectory4=
##Only for OpenButton. Custom working directory for run item 4.
Run5=0
##Only for OpenButton. 1 runs item 5 when the button is clicked.
UseRelativePath5=1
##Only for OpenButton. 1 means FileName5 is relative, 0 means absolute.
FileName5=
##Only for OpenButton. File path for run item 5.
Arguments5=
##Only for OpenButton. Launch arguments for run item 5.
WorkingDirectoryMode5=TargetDir
##Only for OpenButton. Working directory mode for run item 5.
WorkingDirectory5=
##Only for OpenButton. Custom working directory for run item 5.

[Mode2.Element2]
Show=1
##Use 0 or 1. 1 shows this element.
Type=Text
##Element type. When Show=1, this must be Text, TextBox, TextFileBox, Input, OpenButton, MessageButton, RegButton, Table, or CloseButton.
HeightScale=1
##Only for TextBox, TextFileBox, and Table. Use 1, 2, or 3. Higher values may make the window too tall on small or high-DPI screens.
Text=Single-line text example
##Display text. TextBox shows long text, Text shows one line up to 512 characters, and buttons use it as button text up to 512 characters. Input, TextFileBox, and Table do not use this key.
MinimizeOnClick=0
##Only for OpenButton, MessageButton, and RegButton. Use 1 to also minimize the window after clicking the button.
Variable=1
##Only for Input. Use 1 to 6 for %XHLM2V1% to %XHLM2V6%. If several Input elements use the same variable, the later element number wins.
Message=This is a message.\nYou can use \n for line breaks.
##Only for MessageButton. Keep it on one INI line. Use \n for real line breaks.
RegFileName=
##Only for RegButton. Enter an existing .reg file. The final path must end with .reg.
UseRelativePath=1
##Only for RegButton. 1=relative path, 0=absolute path.
WorkingDirectoryMode=TargetDir
##Only for RegButton. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom.
RegImportingText=
##Only for RegButton. Button text shown while importing. Empty means default text.
RegSuccessMessage=
##Only for RegButton. Message shown after success. Empty means default message.
RegFailureMessage=
##Only for RegButton. Message shown after failure. Mention possible partial import if you customize it.
RegTimeoutMessage=
##Only for RegButton. Message shown after the 60-second timeout. Mention possible partial import if you customize it.
CsvFileName=
##Only for Table. CSV path. The final path must end with .csv. Up to 20 columns and 1000 rows are shown.
CsvUseRelativePath=1
##Only for Table. 1=relative path, 0=absolute path.
TextFileName=
##Only for TextFileBox. Text file path. UTF-8 is tried first; if it is not valid UTF-8, the system default code page is used. CRLF and LF line breaks are supported.
TextFileUseRelativePath=1
##Only for TextFileBox. 1=relative path, 0=absolute path.
Run1=0
##Only for OpenButton. 1 runs item 1 when the button is clicked.
UseRelativePath1=1
##Only for OpenButton. 1 means FileName1 is relative, 0 means absolute.
FileName1=
##Only for OpenButton. File path for run item 1.
Arguments1=
##Only for OpenButton. Launch arguments for run item 1.
WorkingDirectoryMode1=TargetDir
##Only for OpenButton. Working directory mode for run item 1.
WorkingDirectory1=
##Only for OpenButton. Custom working directory for run item 1.
Run2=0
##Only for OpenButton. 1 runs item 2 when the button is clicked.
UseRelativePath2=1
##Only for OpenButton. 1 means FileName2 is relative, 0 means absolute.
FileName2=
##Only for OpenButton. File path for run item 2.
Arguments2=
##Only for OpenButton. Launch arguments for run item 2.
WorkingDirectoryMode2=TargetDir
##Only for OpenButton. Working directory mode for run item 2.
WorkingDirectory2=
##Only for OpenButton. Custom working directory for run item 2.
Run3=0
##Only for OpenButton. 1 runs item 3 when the button is clicked.
UseRelativePath3=1
##Only for OpenButton. 1 means FileName3 is relative, 0 means absolute.
FileName3=
##Only for OpenButton. File path for run item 3.
Arguments3=
##Only for OpenButton. Launch arguments for run item 3.
WorkingDirectoryMode3=TargetDir
##Only for OpenButton. Working directory mode for run item 3.
WorkingDirectory3=
##Only for OpenButton. Custom working directory for run item 3.
Run4=0
##Only for OpenButton. 1 runs item 4 when the button is clicked.
UseRelativePath4=1
##Only for OpenButton. 1 means FileName4 is relative, 0 means absolute.
FileName4=
##Only for OpenButton. File path for run item 4.
Arguments4=
##Only for OpenButton. Launch arguments for run item 4.
WorkingDirectoryMode4=TargetDir
##Only for OpenButton. Working directory mode for run item 4.
WorkingDirectory4=
##Only for OpenButton. Custom working directory for run item 4.
Run5=0
##Only for OpenButton. 1 runs item 5 when the button is clicked.
UseRelativePath5=1
##Only for OpenButton. 1 means FileName5 is relative, 0 means absolute.
FileName5=
##Only for OpenButton. File path for run item 5.
Arguments5=
##Only for OpenButton. Launch arguments for run item 5.
WorkingDirectoryMode5=TargetDir
##Only for OpenButton. Working directory mode for run item 5.
WorkingDirectory5=
##Only for OpenButton. Custom working directory for run item 5.

[Mode2.Element3]
Show=1
##Use 0 or 1. 1 shows this element.
Type=Input
##Element type. When Show=1, this must be Text, TextBox, TextFileBox, Input, OpenButton, MessageButton, RegButton, Table, or CloseButton.
HeightScale=1
##Only for TextBox, TextFileBox, and Table. Use 1, 2, or 3. Higher values may make the window too tall on small or high-DPI screens.
Text=
##Display text. TextBox shows long text, Text shows one line up to 512 characters, and buttons use it as button text up to 512 characters. Input, TextFileBox, and Table do not use this key.
MinimizeOnClick=0
##Only for OpenButton, MessageButton, and RegButton. Use 1 to also minimize the window after clicking the button.
Variable=1
##Only for Input. Use 1 to 6 for %XHLM2V1% to %XHLM2V6%. If several Input elements use the same variable, the later element number wins.
Message=This is a message.\nYou can use \n for line breaks.
##Only for MessageButton. Keep it on one INI line. Use \n for real line breaks.
RegFileName=
##Only for RegButton. Enter an existing .reg file. The final path must end with .reg.
UseRelativePath=1
##Only for RegButton. 1=relative path, 0=absolute path.
WorkingDirectoryMode=TargetDir
##Only for RegButton. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom.
RegImportingText=
##Only for RegButton. Button text shown while importing. Empty means default text.
RegSuccessMessage=
##Only for RegButton. Message shown after success. Empty means default message.
RegFailureMessage=
##Only for RegButton. Message shown after failure. Mention possible partial import if you customize it.
RegTimeoutMessage=
##Only for RegButton. Message shown after the 60-second timeout. Mention possible partial import if you customize it.
CsvFileName=
##Only for Table. CSV path. The final path must end with .csv. Up to 20 columns and 1000 rows are shown.
CsvUseRelativePath=1
##Only for Table. 1=relative path, 0=absolute path.
TextFileName=
##Only for TextFileBox. Text file path. UTF-8 is tried first; if it is not valid UTF-8, the system default code page is used. CRLF and LF line breaks are supported.
TextFileUseRelativePath=1
##Only for TextFileBox. 1=relative path, 0=absolute path.
Run1=0
##Only for OpenButton. 1 runs item 1 when the button is clicked.
UseRelativePath1=1
##Only for OpenButton. 1 means FileName1 is relative, 0 means absolute.
FileName1=
##Only for OpenButton. File path for run item 1.
Arguments1=
##Only for OpenButton. Launch arguments for run item 1.
WorkingDirectoryMode1=TargetDir
##Only for OpenButton. Working directory mode for run item 1.
WorkingDirectory1=
##Only for OpenButton. Custom working directory for run item 1.
Run2=0
##Only for OpenButton. 1 runs item 2 when the button is clicked.
UseRelativePath2=1
##Only for OpenButton. 1 means FileName2 is relative, 0 means absolute.
FileName2=
##Only for OpenButton. File path for run item 2.
Arguments2=
##Only for OpenButton. Launch arguments for run item 2.
WorkingDirectoryMode2=TargetDir
##Only for OpenButton. Working directory mode for run item 2.
WorkingDirectory2=
##Only for OpenButton. Custom working directory for run item 2.
Run3=0
##Only for OpenButton. 1 runs item 3 when the button is clicked.
UseRelativePath3=1
##Only for OpenButton. 1 means FileName3 is relative, 0 means absolute.
FileName3=
##Only for OpenButton. File path for run item 3.
Arguments3=
##Only for OpenButton. Launch arguments for run item 3.
WorkingDirectoryMode3=TargetDir
##Only for OpenButton. Working directory mode for run item 3.
WorkingDirectory3=
##Only for OpenButton. Custom working directory for run item 3.
Run4=0
##Only for OpenButton. 1 runs item 4 when the button is clicked.
UseRelativePath4=1
##Only for OpenButton. 1 means FileName4 is relative, 0 means absolute.
FileName4=
##Only for OpenButton. File path for run item 4.
Arguments4=
##Only for OpenButton. Launch arguments for run item 4.
WorkingDirectoryMode4=TargetDir
##Only for OpenButton. Working directory mode for run item 4.
WorkingDirectory4=
##Only for OpenButton. Custom working directory for run item 4.
Run5=0
##Only for OpenButton. 1 runs item 5 when the button is clicked.
UseRelativePath5=1
##Only for OpenButton. 1 means FileName5 is relative, 0 means absolute.
FileName5=
##Only for OpenButton. File path for run item 5.
Arguments5=
##Only for OpenButton. Launch arguments for run item 5.
WorkingDirectoryMode5=TargetDir
##Only for OpenButton. Working directory mode for run item 5.
WorkingDirectory5=
##Only for OpenButton. Custom working directory for run item 5.

[Mode2.Element4]
Show=1
##Use 0 or 1. 1 shows this element.
Type=OpenButton
##Element type. When Show=1, this must be Text, TextBox, TextFileBox, Input, OpenButton, MessageButton, RegButton, Table, or CloseButton.
HeightScale=1
##Only for TextBox, TextFileBox, and Table. Use 1, 2, or 3. Higher values may make the window too tall on small or high-DPI screens.
Text=Open file
##Display text. TextBox shows long text, Text shows one line up to 512 characters, and buttons use it as button text up to 512 characters. Input, TextFileBox, and Table do not use this key.
MinimizeOnClick=0
##Only for OpenButton, MessageButton, and RegButton. Use 1 to also minimize the window after clicking the button.
Variable=1
##Only for Input. Use 1 to 6 for %XHLM2V1% to %XHLM2V6%. If several Input elements use the same variable, the later element number wins.
Message=This is a message.\nYou can use \n for line breaks.
##Only for MessageButton. Keep it on one INI line. Use \n for real line breaks.
RegFileName=
##Only for RegButton. Enter an existing .reg file. The final path must end with .reg.
UseRelativePath=1
##Only for RegButton. 1=relative path, 0=absolute path.
WorkingDirectoryMode=TargetDir
##Only for RegButton. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom.
RegImportingText=
##Only for RegButton. Button text shown while importing. Empty means default text.
RegSuccessMessage=
##Only for RegButton. Message shown after success. Empty means default message.
RegFailureMessage=
##Only for RegButton. Message shown after failure. Mention possible partial import if you customize it.
RegTimeoutMessage=
##Only for RegButton. Message shown after the 60-second timeout. Mention possible partial import if you customize it.
CsvFileName=
##Only for Table. CSV path. The final path must end with .csv. Up to 20 columns and 1000 rows are shown.
CsvUseRelativePath=1
##Only for Table. 1=relative path, 0=absolute path.
TextFileName=
##Only for TextFileBox. Text file path. UTF-8 is tried first; if it is not valid UTF-8, the system default code page is used. CRLF and LF line breaks are supported.
TextFileUseRelativePath=1
##Only for TextFileBox. 1=relative path, 0=absolute path.
Run1=0
##Only for OpenButton. 1 runs item 1 when the button is clicked.
UseRelativePath1=1
##Only for OpenButton. 1 means FileName1 is relative, 0 means absolute.
FileName1=
##Only for OpenButton. File path for run item 1.
Arguments1=
##Only for OpenButton. Launch arguments for run item 1.
WorkingDirectoryMode1=TargetDir
##Only for OpenButton. Working directory mode for run item 1.
WorkingDirectory1=
##Only for OpenButton. Custom working directory for run item 1.
Run2=0
##Only for OpenButton. 1 runs item 2 when the button is clicked.
UseRelativePath2=1
##Only for OpenButton. 1 means FileName2 is relative, 0 means absolute.
FileName2=
##Only for OpenButton. File path for run item 2.
Arguments2=
##Only for OpenButton. Launch arguments for run item 2.
WorkingDirectoryMode2=TargetDir
##Only for OpenButton. Working directory mode for run item 2.
WorkingDirectory2=
##Only for OpenButton. Custom working directory for run item 2.
Run3=0
##Only for OpenButton. 1 runs item 3 when the button is clicked.
UseRelativePath3=1
##Only for OpenButton. 1 means FileName3 is relative, 0 means absolute.
FileName3=
##Only for OpenButton. File path for run item 3.
Arguments3=
##Only for OpenButton. Launch arguments for run item 3.
WorkingDirectoryMode3=TargetDir
##Only for OpenButton. Working directory mode for run item 3.
WorkingDirectory3=
##Only for OpenButton. Custom working directory for run item 3.
Run4=0
##Only for OpenButton. 1 runs item 4 when the button is clicked.
UseRelativePath4=1
##Only for OpenButton. 1 means FileName4 is relative, 0 means absolute.
FileName4=
##Only for OpenButton. File path for run item 4.
Arguments4=
##Only for OpenButton. Launch arguments for run item 4.
WorkingDirectoryMode4=TargetDir
##Only for OpenButton. Working directory mode for run item 4.
WorkingDirectory4=
##Only for OpenButton. Custom working directory for run item 4.
Run5=0
##Only for OpenButton. 1 runs item 5 when the button is clicked.
UseRelativePath5=1
##Only for OpenButton. 1 means FileName5 is relative, 0 means absolute.
FileName5=
##Only for OpenButton. File path for run item 5.
Arguments5=
##Only for OpenButton. Launch arguments for run item 5.
WorkingDirectoryMode5=TargetDir
##Only for OpenButton. Working directory mode for run item 5.
WorkingDirectory5=
##Only for OpenButton. Custom working directory for run item 5.

[Mode2.Element5]
Show=1
##Use 0 or 1. 1 shows this element.
Type=MessageButton
##Element type. When Show=1, this must be Text, TextBox, TextFileBox, Input, OpenButton, MessageButton, RegButton, Table, or CloseButton.
HeightScale=1
##Only for TextBox, TextFileBox, and Table. Use 1, 2, or 3. Higher values may make the window too tall on small or high-DPI screens.
Text=Message
##Display text. TextBox shows long text, Text shows one line up to 512 characters, and buttons use it as button text up to 512 characters. Input, TextFileBox, and Table do not use this key.
MinimizeOnClick=0
##Only for OpenButton, MessageButton, and RegButton. Use 1 to also minimize the window after clicking the button.
Variable=1
##Only for Input. Use 1 to 6 for %XHLM2V1% to %XHLM2V6%. If several Input elements use the same variable, the later element number wins.
Message=This is a message.\nYou can use \n for line breaks.
##Only for MessageButton. Keep it on one INI line. Use \n for real line breaks.
RegFileName=
##Only for RegButton. Enter an existing .reg file. The final path must end with .reg.
UseRelativePath=1
##Only for RegButton. 1=relative path, 0=absolute path.
WorkingDirectoryMode=TargetDir
##Only for RegButton. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom.
RegImportingText=
##Only for RegButton. Button text shown while importing. Empty means default text.
RegSuccessMessage=
##Only for RegButton. Message shown after success. Empty means default message.
RegFailureMessage=
##Only for RegButton. Message shown after failure. Mention possible partial import if you customize it.
RegTimeoutMessage=
##Only for RegButton. Message shown after the 60-second timeout. Mention possible partial import if you customize it.
CsvFileName=
##Only for Table. CSV path. The final path must end with .csv. Up to 20 columns and 1000 rows are shown.
CsvUseRelativePath=1
##Only for Table. 1=relative path, 0=absolute path.
TextFileName=
##Only for TextFileBox. Text file path. UTF-8 is tried first; if it is not valid UTF-8, the system default code page is used. CRLF and LF line breaks are supported.
TextFileUseRelativePath=1
##Only for TextFileBox. 1=relative path, 0=absolute path.
Run1=0
##Only for OpenButton. 1 runs item 1 when the button is clicked.
UseRelativePath1=1
##Only for OpenButton. 1 means FileName1 is relative, 0 means absolute.
FileName1=
##Only for OpenButton. File path for run item 1.
Arguments1=
##Only for OpenButton. Launch arguments for run item 1.
WorkingDirectoryMode1=TargetDir
##Only for OpenButton. Working directory mode for run item 1.
WorkingDirectory1=
##Only for OpenButton. Custom working directory for run item 1.
Run2=0
##Only for OpenButton. 1 runs item 2 when the button is clicked.
UseRelativePath2=1
##Only for OpenButton. 1 means FileName2 is relative, 0 means absolute.
FileName2=
##Only for OpenButton. File path for run item 2.
Arguments2=
##Only for OpenButton. Launch arguments for run item 2.
WorkingDirectoryMode2=TargetDir
##Only for OpenButton. Working directory mode for run item 2.
WorkingDirectory2=
##Only for OpenButton. Custom working directory for run item 2.
Run3=0
##Only for OpenButton. 1 runs item 3 when the button is clicked.
UseRelativePath3=1
##Only for OpenButton. 1 means FileName3 is relative, 0 means absolute.
FileName3=
##Only for OpenButton. File path for run item 3.
Arguments3=
##Only for OpenButton. Launch arguments for run item 3.
WorkingDirectoryMode3=TargetDir
##Only for OpenButton. Working directory mode for run item 3.
WorkingDirectory3=
##Only for OpenButton. Custom working directory for run item 3.
Run4=0
##Only for OpenButton. 1 runs item 4 when the button is clicked.
UseRelativePath4=1
##Only for OpenButton. 1 means FileName4 is relative, 0 means absolute.
FileName4=
##Only for OpenButton. File path for run item 4.
Arguments4=
##Only for OpenButton. Launch arguments for run item 4.
WorkingDirectoryMode4=TargetDir
##Only for OpenButton. Working directory mode for run item 4.
WorkingDirectory4=
##Only for OpenButton. Custom working directory for run item 4.
Run5=0
##Only for OpenButton. 1 runs item 5 when the button is clicked.
UseRelativePath5=1
##Only for OpenButton. 1 means FileName5 is relative, 0 means absolute.
FileName5=
##Only for OpenButton. File path for run item 5.
Arguments5=
##Only for OpenButton. Launch arguments for run item 5.
WorkingDirectoryMode5=TargetDir
##Only for OpenButton. Working directory mode for run item 5.
WorkingDirectory5=
##Only for OpenButton. Custom working directory for run item 5.

[Mode2.Element6]
Show=1
##Use 0 or 1. 1 shows this element.
Type=RegButton
##Element type. When Show=1, this must be Text, TextBox, TextFileBox, Input, OpenButton, MessageButton, RegButton, Table, or CloseButton.
HeightScale=1
##Only for TextBox, TextFileBox, and Table. Use 1, 2, or 3. Higher values may make the window too tall on small or high-DPI screens.
Text=Import registry
##Display text. TextBox shows long text, Text shows one line up to 512 characters, and buttons use it as button text up to 512 characters. Input, TextFileBox, and Table do not use this key.
MinimizeOnClick=0
##Only for OpenButton, MessageButton, and RegButton. Use 1 to also minimize the window after clicking the button.
Variable=1
##Only for Input. Use 1 to 6 for %XHLM2V1% to %XHLM2V6%. If several Input elements use the same variable, the later element number wins.
Message=This is a message.\nYou can use \n for line breaks.
##Only for MessageButton. Keep it on one INI line. Use \n for real line breaks.
RegFileName=
##Only for RegButton. Enter an existing .reg file. The final path must end with .reg.
UseRelativePath=1
##Only for RegButton. 1=relative path, 0=absolute path.
WorkingDirectoryMode=TargetDir
##Only for RegButton. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom.
RegImportingText=
##Only for RegButton. Button text shown while importing. Empty means default text.
RegSuccessMessage=
##Only for RegButton. Message shown after success. Empty means default message.
RegFailureMessage=
##Only for RegButton. Message shown after failure. Mention possible partial import if you customize it.
RegTimeoutMessage=
##Only for RegButton. Message shown after the 60-second timeout. Mention possible partial import if you customize it.
CsvFileName=
##Only for Table. CSV path. The final path must end with .csv. Up to 20 columns and 1000 rows are shown.
CsvUseRelativePath=1
##Only for Table. 1=relative path, 0=absolute path.
TextFileName=
##Only for TextFileBox. Text file path. UTF-8 is tried first; if it is not valid UTF-8, the system default code page is used. CRLF and LF line breaks are supported.
TextFileUseRelativePath=1
##Only for TextFileBox. 1=relative path, 0=absolute path.
Run1=0
##Only for OpenButton. 1 runs item 1 when the button is clicked.
UseRelativePath1=1
##Only for OpenButton. 1 means FileName1 is relative, 0 means absolute.
FileName1=
##Only for OpenButton. File path for run item 1.
Arguments1=
##Only for OpenButton. Launch arguments for run item 1.
WorkingDirectoryMode1=TargetDir
##Only for OpenButton. Working directory mode for run item 1.
WorkingDirectory1=
##Only for OpenButton. Custom working directory for run item 1.
Run2=0
##Only for OpenButton. 1 runs item 2 when the button is clicked.
UseRelativePath2=1
##Only for OpenButton. 1 means FileName2 is relative, 0 means absolute.
FileName2=
##Only for OpenButton. File path for run item 2.
Arguments2=
##Only for OpenButton. Launch arguments for run item 2.
WorkingDirectoryMode2=TargetDir
##Only for OpenButton. Working directory mode for run item 2.
WorkingDirectory2=
##Only for OpenButton. Custom working directory for run item 2.
Run3=0
##Only for OpenButton. 1 runs item 3 when the button is clicked.
UseRelativePath3=1
##Only for OpenButton. 1 means FileName3 is relative, 0 means absolute.
FileName3=
##Only for OpenButton. File path for run item 3.
Arguments3=
##Only for OpenButton. Launch arguments for run item 3.
WorkingDirectoryMode3=TargetDir
##Only for OpenButton. Working directory mode for run item 3.
WorkingDirectory3=
##Only for OpenButton. Custom working directory for run item 3.
Run4=0
##Only for OpenButton. 1 runs item 4 when the button is clicked.
UseRelativePath4=1
##Only for OpenButton. 1 means FileName4 is relative, 0 means absolute.
FileName4=
##Only for OpenButton. File path for run item 4.
Arguments4=
##Only for OpenButton. Launch arguments for run item 4.
WorkingDirectoryMode4=TargetDir
##Only for OpenButton. Working directory mode for run item 4.
WorkingDirectory4=
##Only for OpenButton. Custom working directory for run item 4.
Run5=0
##Only for OpenButton. 1 runs item 5 when the button is clicked.
UseRelativePath5=1
##Only for OpenButton. 1 means FileName5 is relative, 0 means absolute.
FileName5=
##Only for OpenButton. File path for run item 5.
Arguments5=
##Only for OpenButton. Launch arguments for run item 5.
WorkingDirectoryMode5=TargetDir
##Only for OpenButton. Working directory mode for run item 5.
WorkingDirectory5=
##Only for OpenButton. Custom working directory for run item 5.

[Mode2.Element7]
Show=1
##Use 0 or 1. 1 shows this element.
Type=Table
##Element type. When Show=1, this must be Text, TextBox, TextFileBox, Input, OpenButton, MessageButton, RegButton, Table, or CloseButton.
HeightScale=1
##Only for TextBox, TextFileBox, and Table. Use 1, 2, or 3. Higher values may make the window too tall on small or high-DPI screens.
Text=
##Display text. TextBox shows long text, Text shows one line up to 512 characters, and buttons use it as button text up to 512 characters. Input, TextFileBox, and Table do not use this key.
MinimizeOnClick=0
##Only for OpenButton, MessageButton, and RegButton. Use 1 to also minimize the window after clicking the button.
Variable=1
##Only for Input. Use 1 to 6 for %XHLM2V1% to %XHLM2V6%. If several Input elements use the same variable, the later element number wins.
Message=This is a message.\nYou can use \n for line breaks.
##Only for MessageButton. Keep it on one INI line. Use \n for real line breaks.
RegFileName=
##Only for RegButton. Enter an existing .reg file. The final path must end with .reg.
UseRelativePath=1
##Only for RegButton. 1=relative path, 0=absolute path.
WorkingDirectoryMode=TargetDir
##Only for RegButton. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom.
RegImportingText=
##Only for RegButton. Button text shown while importing. Empty means default text.
RegSuccessMessage=
##Only for RegButton. Message shown after success. Empty means default message.
RegFailureMessage=
##Only for RegButton. Message shown after failure. Mention possible partial import if you customize it.
RegTimeoutMessage=
##Only for RegButton. Message shown after the 60-second timeout. Mention possible partial import if you customize it.
CsvFileName=Example.csv
##Only for Table. CSV path. The final path must end with .csv. Up to 20 columns and 1000 rows are shown.
CsvUseRelativePath=1
##Only for Table. 1=relative path, 0=absolute path.
TextFileName=
##Only for TextFileBox. Text file path. UTF-8 is tried first; if it is not valid UTF-8, the system default code page is used. CRLF and LF line breaks are supported.
TextFileUseRelativePath=1
##Only for TextFileBox. 1=relative path, 0=absolute path.
Run1=0
##Only for OpenButton. 1 runs item 1 when the button is clicked.
UseRelativePath1=1
##Only for OpenButton. 1 means FileName1 is relative, 0 means absolute.
FileName1=
##Only for OpenButton. File path for run item 1.
Arguments1=
##Only for OpenButton. Launch arguments for run item 1.
WorkingDirectoryMode1=TargetDir
##Only for OpenButton. Working directory mode for run item 1.
WorkingDirectory1=
##Only for OpenButton. Custom working directory for run item 1.
Run2=0
##Only for OpenButton. 1 runs item 2 when the button is clicked.
UseRelativePath2=1
##Only for OpenButton. 1 means FileName2 is relative, 0 means absolute.
FileName2=
##Only for OpenButton. File path for run item 2.
Arguments2=
##Only for OpenButton. Launch arguments for run item 2.
WorkingDirectoryMode2=TargetDir
##Only for OpenButton. Working directory mode for run item 2.
WorkingDirectory2=
##Only for OpenButton. Custom working directory for run item 2.
Run3=0
##Only for OpenButton. 1 runs item 3 when the button is clicked.
UseRelativePath3=1
##Only for OpenButton. 1 means FileName3 is relative, 0 means absolute.
FileName3=
##Only for OpenButton. File path for run item 3.
Arguments3=
##Only for OpenButton. Launch arguments for run item 3.
WorkingDirectoryMode3=TargetDir
##Only for OpenButton. Working directory mode for run item 3.
WorkingDirectory3=
##Only for OpenButton. Custom working directory for run item 3.
Run4=0
##Only for OpenButton. 1 runs item 4 when the button is clicked.
UseRelativePath4=1
##Only for OpenButton. 1 means FileName4 is relative, 0 means absolute.
FileName4=
##Only for OpenButton. File path for run item 4.
Arguments4=
##Only for OpenButton. Launch arguments for run item 4.
WorkingDirectoryMode4=TargetDir
##Only for OpenButton. Working directory mode for run item 4.
WorkingDirectory4=
##Only for OpenButton. Custom working directory for run item 4.
Run5=0
##Only for OpenButton. 1 runs item 5 when the button is clicked.
UseRelativePath5=1
##Only for OpenButton. 1 means FileName5 is relative, 0 means absolute.
FileName5=
##Only for OpenButton. File path for run item 5.
Arguments5=
##Only for OpenButton. Launch arguments for run item 5.
WorkingDirectoryMode5=TargetDir
##Only for OpenButton. Working directory mode for run item 5.
WorkingDirectory5=
##Only for OpenButton. Custom working directory for run item 5.

[Mode2.Element8]
Show=1
##Use 0 or 1. 1 shows this element.
Type=TextFileBox
##Element type. When Show=1, this must be Text, TextBox, TextFileBox, Input, OpenButton, MessageButton, RegButton, Table, or CloseButton.
HeightScale=1
##Only for TextBox, TextFileBox, and Table. Use 1, 2, or 3. Higher values may make the window too tall on small or high-DPI screens.
Text=
##Display text. TextBox shows long text, Text shows one line up to 512 characters, and buttons use it as button text up to 512 characters. Input, TextFileBox, and Table do not use this key.
MinimizeOnClick=0
##Only for OpenButton, MessageButton, and RegButton. Use 1 to also minimize the window after clicking the button.
Variable=1
##Only for Input. Use 1 to 6 for %XHLM2V1% to %XHLM2V6%. If several Input elements use the same variable, the later element number wins.
Message=This is a message.\nYou can use \n for line breaks.
##Only for MessageButton. Keep it on one INI line. Use \n for real line breaks.
RegFileName=
##Only for RegButton. Enter an existing .reg file. The final path must end with .reg.
UseRelativePath=1
##Only for RegButton. 1=relative path, 0=absolute path.
WorkingDirectoryMode=TargetDir
##Only for RegButton. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom.
RegImportingText=
##Only for RegButton. Button text shown while importing. Empty means default text.
RegSuccessMessage=
##Only for RegButton. Message shown after success. Empty means default message.
RegFailureMessage=
##Only for RegButton. Message shown after failure. Mention possible partial import if you customize it.
RegTimeoutMessage=
##Only for RegButton. Message shown after the 60-second timeout. Mention possible partial import if you customize it.
CsvFileName=
##Only for Table. CSV path. The final path must end with .csv. Up to 20 columns and 1000 rows are shown.
CsvUseRelativePath=1
##Only for Table. 1=relative path, 0=absolute path.
TextFileName=
##Only for TextFileBox. Text file path. UTF-8 is tried first; if it is not valid UTF-8, the system default code page is used. CRLF and LF line breaks are supported.
TextFileUseRelativePath=1
##Only for TextFileBox. 1=relative path, 0=absolute path.
Run1=0
##Only for OpenButton. 1 runs item 1 when the button is clicked.
UseRelativePath1=1
##Only for OpenButton. 1 means FileName1 is relative, 0 means absolute.
FileName1=
##Only for OpenButton. File path for run item 1.
Arguments1=
##Only for OpenButton. Launch arguments for run item 1.
WorkingDirectoryMode1=TargetDir
##Only for OpenButton. Working directory mode for run item 1.
WorkingDirectory1=
##Only for OpenButton. Custom working directory for run item 1.
Run2=0
##Only for OpenButton. 1 runs item 2 when the button is clicked.
UseRelativePath2=1
##Only for OpenButton. 1 means FileName2 is relative, 0 means absolute.
FileName2=
##Only for OpenButton. File path for run item 2.
Arguments2=
##Only for OpenButton. Launch arguments for run item 2.
WorkingDirectoryMode2=TargetDir
##Only for OpenButton. Working directory mode for run item 2.
WorkingDirectory2=
##Only for OpenButton. Custom working directory for run item 2.
Run3=0
##Only for OpenButton. 1 runs item 3 when the button is clicked.
UseRelativePath3=1
##Only for OpenButton. 1 means FileName3 is relative, 0 means absolute.
FileName3=
##Only for OpenButton. File path for run item 3.
Arguments3=
##Only for OpenButton. Launch arguments for run item 3.
WorkingDirectoryMode3=TargetDir
##Only for OpenButton. Working directory mode for run item 3.
WorkingDirectory3=
##Only for OpenButton. Custom working directory for run item 3.
Run4=0
##Only for OpenButton. 1 runs item 4 when the button is clicked.
UseRelativePath4=1
##Only for OpenButton. 1 means FileName4 is relative, 0 means absolute.
FileName4=
##Only for OpenButton. File path for run item 4.
Arguments4=
##Only for OpenButton. Launch arguments for run item 4.
WorkingDirectoryMode4=TargetDir
##Only for OpenButton. Working directory mode for run item 4.
WorkingDirectory4=
##Only for OpenButton. Custom working directory for run item 4.
Run5=0
##Only for OpenButton. 1 runs item 5 when the button is clicked.
UseRelativePath5=1
##Only for OpenButton. 1 means FileName5 is relative, 0 means absolute.
FileName5=
##Only for OpenButton. File path for run item 5.
Arguments5=
##Only for OpenButton. Launch arguments for run item 5.
WorkingDirectoryMode5=TargetDir
##Only for OpenButton. Working directory mode for run item 5.
WorkingDirectory5=
##Only for OpenButton. Custom working directory for run item 5.

[Mode2.Element9]
Show=1
##Use 0 or 1. 1 shows this element.
Type=Text
##Element type. When Show=1, this must be Text, TextBox, TextFileBox, Input, OpenButton, MessageButton, RegButton, Table, or CloseButton.
HeightScale=1
##Only for TextBox, TextFileBox, and Table. Use 1, 2, or 3. Higher values may make the window too tall on small or high-DPI screens.
Text=Extra text
##Display text. TextBox shows long text, Text shows one line up to 512 characters, and buttons use it as button text up to 512 characters. Input, TextFileBox, and Table do not use this key.
MinimizeOnClick=0
##Only for OpenButton, MessageButton, and RegButton. Use 1 to also minimize the window after clicking the button.
Variable=1
##Only for Input. Use 1 to 6 for %XHLM2V1% to %XHLM2V6%. If several Input elements use the same variable, the later element number wins.
Message=This is a message.\nYou can use \n for line breaks.
##Only for MessageButton. Keep it on one INI line. Use \n for real line breaks.
RegFileName=
##Only for RegButton. Enter an existing .reg file. The final path must end with .reg.
UseRelativePath=1
##Only for RegButton. 1=relative path, 0=absolute path.
WorkingDirectoryMode=TargetDir
##Only for RegButton. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom.
RegImportingText=
##Only for RegButton. Button text shown while importing. Empty means default text.
RegSuccessMessage=
##Only for RegButton. Message shown after success. Empty means default message.
RegFailureMessage=
##Only for RegButton. Message shown after failure. Mention possible partial import if you customize it.
RegTimeoutMessage=
##Only for RegButton. Message shown after the 60-second timeout. Mention possible partial import if you customize it.
CsvFileName=
##Only for Table. CSV path. The final path must end with .csv. Up to 20 columns and 1000 rows are shown.
CsvUseRelativePath=1
##Only for Table. 1=relative path, 0=absolute path.
TextFileName=
##Only for TextFileBox. Text file path. UTF-8 is tried first; if it is not valid UTF-8, the system default code page is used. CRLF and LF line breaks are supported.
TextFileUseRelativePath=1
##Only for TextFileBox. 1=relative path, 0=absolute path.
Run1=0
##Only for OpenButton. 1 runs item 1 when the button is clicked.
UseRelativePath1=1
##Only for OpenButton. 1 means FileName1 is relative, 0 means absolute.
FileName1=
##Only for OpenButton. File path for run item 1.
Arguments1=
##Only for OpenButton. Launch arguments for run item 1.
WorkingDirectoryMode1=TargetDir
##Only for OpenButton. Working directory mode for run item 1.
WorkingDirectory1=
##Only for OpenButton. Custom working directory for run item 1.
Run2=0
##Only for OpenButton. 1 runs item 2 when the button is clicked.
UseRelativePath2=1
##Only for OpenButton. 1 means FileName2 is relative, 0 means absolute.
FileName2=
##Only for OpenButton. File path for run item 2.
Arguments2=
##Only for OpenButton. Launch arguments for run item 2.
WorkingDirectoryMode2=TargetDir
##Only for OpenButton. Working directory mode for run item 2.
WorkingDirectory2=
##Only for OpenButton. Custom working directory for run item 2.
Run3=0
##Only for OpenButton. 1 runs item 3 when the button is clicked.
UseRelativePath3=1
##Only for OpenButton. 1 means FileName3 is relative, 0 means absolute.
FileName3=
##Only for OpenButton. File path for run item 3.
Arguments3=
##Only for OpenButton. Launch arguments for run item 3.
WorkingDirectoryMode3=TargetDir
##Only for OpenButton. Working directory mode for run item 3.
WorkingDirectory3=
##Only for OpenButton. Custom working directory for run item 3.
Run4=0
##Only for OpenButton. 1 runs item 4 when the button is clicked.
UseRelativePath4=1
##Only for OpenButton. 1 means FileName4 is relative, 0 means absolute.
FileName4=
##Only for OpenButton. File path for run item 4.
Arguments4=
##Only for OpenButton. Launch arguments for run item 4.
WorkingDirectoryMode4=TargetDir
##Only for OpenButton. Working directory mode for run item 4.
WorkingDirectory4=
##Only for OpenButton. Custom working directory for run item 4.
Run5=0
##Only for OpenButton. 1 runs item 5 when the button is clicked.
UseRelativePath5=1
##Only for OpenButton. 1 means FileName5 is relative, 0 means absolute.
FileName5=
##Only for OpenButton. File path for run item 5.
Arguments5=
##Only for OpenButton. Launch arguments for run item 5.
WorkingDirectoryMode5=TargetDir
##Only for OpenButton. Working directory mode for run item 5.
WorkingDirectory5=
##Only for OpenButton. Custom working directory for run item 5.

[Mode2.Element10]
Show=1
##Use 0 or 1. 1 shows this element.
Type=CloseButton
##Element type. When Show=1, this must be Text, TextBox, TextFileBox, Input, OpenButton, MessageButton, RegButton, Table, or CloseButton.
HeightScale=1
##Only for TextBox, TextFileBox, and Table. Use 1, 2, or 3. Higher values may make the window too tall on small or high-DPI screens.
Text=Close
##Display text. TextBox shows long text, Text shows one line up to 512 characters, and buttons use it as button text up to 512 characters. Input, TextFileBox, and Table do not use this key.
MinimizeOnClick=0
##Only for OpenButton, MessageButton, and RegButton. Use 1 to also minimize the window after clicking the button.
Variable=1
##Only for Input. Use 1 to 6 for %XHLM2V1% to %XHLM2V6%. If several Input elements use the same variable, the later element number wins.
Message=This is a message.\nYou can use \n for line breaks.
##Only for MessageButton. Keep it on one INI line. Use \n for real line breaks.
RegFileName=
##Only for RegButton. Enter an existing .reg file. The final path must end with .reg.
UseRelativePath=1
##Only for RegButton. 1=relative path, 0=absolute path.
WorkingDirectoryMode=TargetDir
##Only for RegButton. Use TargetDir, LauncherDir, or Custom.
WorkingDirectory=
##Custom working directory. Used only when WorkingDirectoryMode=Custom.
RegImportingText=
##Only for RegButton. Button text shown while importing. Empty means default text.
RegSuccessMessage=
##Only for RegButton. Message shown after success. Empty means default message.
RegFailureMessage=
##Only for RegButton. Message shown after failure. Mention possible partial import if you customize it.
RegTimeoutMessage=
##Only for RegButton. Message shown after the 60-second timeout. Mention possible partial import if you customize it.
CsvFileName=
##Only for Table. CSV path. The final path must end with .csv. Up to 20 columns and 1000 rows are shown.
CsvUseRelativePath=1
##Only for Table. 1=relative path, 0=absolute path.
TextFileName=
##Only for TextFileBox. Text file path. UTF-8 is tried first; if it is not valid UTF-8, the system default code page is used. CRLF and LF line breaks are supported.
TextFileUseRelativePath=1
##Only for TextFileBox. 1=relative path, 0=absolute path.
Run1=0
##Only for OpenButton. 1 runs item 1 when the button is clicked.
UseRelativePath1=1
##Only for OpenButton. 1 means FileName1 is relative, 0 means absolute.
FileName1=
##Only for OpenButton. File path for run item 1.
Arguments1=
##Only for OpenButton. Launch arguments for run item 1.
WorkingDirectoryMode1=TargetDir
##Only for OpenButton. Working directory mode for run item 1.
WorkingDirectory1=
##Only for OpenButton. Custom working directory for run item 1.
Run2=0
##Only for OpenButton. 1 runs item 2 when the button is clicked.
UseRelativePath2=1
##Only for OpenButton. 1 means FileName2 is relative, 0 means absolute.
FileName2=
##Only for OpenButton. File path for run item 2.
Arguments2=
##Only for OpenButton. Launch arguments for run item 2.
WorkingDirectoryMode2=TargetDir
##Only for OpenButton. Working directory mode for run item 2.
WorkingDirectory2=
##Only for OpenButton. Custom working directory for run item 2.
Run3=0
##Only for OpenButton. 1 runs item 3 when the button is clicked.
UseRelativePath3=1
##Only for OpenButton. 1 means FileName3 is relative, 0 means absolute.
FileName3=
##Only for OpenButton. File path for run item 3.
Arguments3=
##Only for OpenButton. Launch arguments for run item 3.
WorkingDirectoryMode3=TargetDir
##Only for OpenButton. Working directory mode for run item 3.
WorkingDirectory3=
##Only for OpenButton. Custom working directory for run item 3.
Run4=0
##Only for OpenButton. 1 runs item 4 when the button is clicked.
UseRelativePath4=1
##Only for OpenButton. 1 means FileName4 is relative, 0 means absolute.
FileName4=
##Only for OpenButton. File path for run item 4.
Arguments4=
##Only for OpenButton. Launch arguments for run item 4.
WorkingDirectoryMode4=TargetDir
##Only for OpenButton. Working directory mode for run item 4.
WorkingDirectory4=
##Only for OpenButton. Custom working directory for run item 4.
Run5=0
##Only for OpenButton. 1 runs item 5 when the button is clicked.
UseRelativePath5=1
##Only for OpenButton. 1 means FileName5 is relative, 0 means absolute.
FileName5=
##Only for OpenButton. File path for run item 5.
Arguments5=
##Only for OpenButton. Launch arguments for run item 5.
WorkingDirectoryMode5=TargetDir
##Only for OpenButton. Working directory mode for run item 5.
WorkingDirectory5=
##Only for OpenButton. Custom working directory for run item 5.
```

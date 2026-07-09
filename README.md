# Xiao Hammer Launcher

[简体中文](README.zh-Hans.md) / [繁體中文](README.zh-Hant.md)

## What Is It

Xiao Hammer Launcher is a native single-file Windows launcher driven by an INI file.

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

See the full default template: [DEFAULT_TEMPLATE.txt](DEFAULT_TEMPLATE.txt).

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

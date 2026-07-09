# Windows INI 启动器

[English README](README.md) / [繁體中文](README.zh-Hant.md)

## 功能简介

把 EXE 放在一个文件夹里，再放一个同名 INI，程序就会按 INI 的内容执行。

它可以：

- 静默运行，没有窗口；
- 打开本地文件或程序；
- 导入 `.reg` 注册表文件；
- 显示一个原生窗口，里面可以放文本、按钮、输入框、文本框、读取文本文件的文本框和 CSV 表格；
- 显示本地 PNG 启动图；
- 使用变量，让同一份配置更容易换目录或换电脑使用。

程序使用常见 Windows API 和系统控件，目标是单文件、少依赖，适用于 Windows 10 和 Windows 11。

## 构建

主源码：

```text
native/src/XHLauncher.cpp
```

构建命令：

```powershell
powershell -ExecutionPolicy Bypass -File native\build.ps1 -Arch x86
```

默认构建 x86，这样同一个 EXE 可以运行在 32 位和 64 位 Windows 上。

## 快速开始

1. 把 EXE 放到要使用的文件夹。
2. 运行一次。如果同名 INI 不存在，程序会释放默认模板。
3. 默认模板会打开编辑模式。
4. 选择模式，填写标题、路径、按钮、文本等配置。
5. 点击“保存并关闭”。
6. 再次运行 EXE。

如果 EXE 改名，INI 名也跟着变。例如 `Tools.exe` 会读取 `Tools.ini`。

## 模式

`Mode=0` 是编辑模式。只修改配置，不打开文件，不导入注册表。

`Mode=1` 是静默模式。没有窗口，没有成功提示。只读取 `Mode1.*` 区域，执行后退出。

`Mode=2` 是窗口模式。显示固定宽度的原生窗口，只读取 `Mode2.*` 区域。

未知区域和未知配置项会被忽略。隐藏或禁用的项目不会做完整检查，因为当前运行不会使用它们。

等号右侧的内容会按原样保留。开头和结尾的空格也属于配置值。Mode、0/1 开关、Type、语言和数字等控制字段会忽略两侧空格。

## 语言

语言只使用三个标准值：`Language=en-US` 表示英文，`Language=zh-Hans` 表示简体中文，`Language=zh-Hant` 表示繁体中文。

编辑模式会根据当前语言显示对应的内置说明。英文是默认说明。

## 编码和大小

主 INI 支持：

- UTF-8 无 BOM；
- UTF-8 with BOM；
- UTF-16 LE with BOM。

主 INI 不支持 ANSI、GBK 或其他本地代码页编码。

默认模板使用 UTF-8 with BOM，方便旧版 Windows 工具打开。

INI 最大约 4 MB。它只建议保存启动配置。大段说明请放到单独文本文件里，再用 TextFileBox 显示。

如果配置缺失、为空、过大、编码错误，或数字状态值非法，程序会统一提示配置读取失败。

## 路径

推荐使用 Windows 常见的 `\` 分隔符。程序也接受 `/`，运行时会转换成 `\`。

相对路径示例：

```text
App.exe
.\App.exe
Folder\App.exe
Folder/Sub/App.exe
```

相对路径以 EXE 所在目录为基准。不能写盘符、UNC 网络路径、根路径，也不能用 `..` 逃出 EXE 目录。

绝对路径示例：

```text
C:\Tools\App.exe
C:/Tools/App.exe
\\Server\Share\File.txt
%USERPROFILE%\Desktop\App.exe
%XHLROOT%\Tools\App.exe
```

不支持：

```text
C:Folder\App.exe
\Folder\App.exe
/Folder/App.exe
file://C:/Folder/App.exe
```

`UseRelativePath=1` 表示相对路径。只替换当前模式变量，不展开 Windows 环境变量，也不展开 `%XHLROOT%`。

`UseRelativePath=0` 表示绝对路径。支持模式变量、Windows 环境变量和 `%XHLROOT%`。

`%XHLROOT%` 表示当前 EXE 所在文件夹。普通文件夹末尾不带反斜杠，拼接文件时请自己加，例如 `%XHLROOT%\Tools\App.exe`。

## 变量

模式 1 变量是 `%XHLM1V1%` 到 `%XHLM1V6%`。每个变量都有 `Enable` 和 `Value`。未启用时，变量标记会按普通文字处理。

模式 2 变量是 `%XHLM2V1%` 到 `%XHLM2V6%`。它们由窗口里的输入框写入，只在本次运行中生效，不写回 INI。某个变量如果没有对应输入框，也会按普通文字处理。

多个输入框写同一个变量时，元素编号靠后的输入框会覆盖前面的值。

变量可以和其他文字连在一起，例如 `ABC%XHLM1V1%123`。变量用于路径、启动参数、工作目录路径、显示文字、弹窗内容等运行文本字段，不用于 Mode、Language、Show、Type、Run、UseRelativePath、HeightScale、Variable、WorkingDirectoryMode 等控制字段。

一个变量值、输入值或普通文本字段大约支持 4095 个字符。

运行时变量替换后的最终文本最多支持 32767 个 UTF-16 字符。路径、启动参数、工作目录、弹窗内容等运行字段超过这个上限时，会被视为无效并跳过或留空。

## 工作目录

打开文件、启动程序或导入注册表时，默认工作目录是目标文件所在目录。

可选值：

- `WorkingDirectoryMode=TargetDir`
- `WorkingDirectoryMode=LauncherDir`
- `WorkingDirectoryMode=Custom`

只有选择 `Custom` 时才会使用 `WorkingDirectory`。最终路径必须是存在的文件夹。

## 启动图

启动图由 `[Launcher]` 里的 `Splash*` 配置控制。

规则：

- 只支持 PNG；
- 支持透明通道；
- 文件最大 12 MB；
- 原图最大 3840x2160；
- 最终渲染尺寸限制在 3840x2160；
- 文件缺失或无效时静默跳过；
- 没有动画和淡入淡出。

图片会用 Windows Imaging Component 按当前屏幕 DPI 高质量缩放。

## 注册表导入

模式 1 可以导入一个注册表文件。模式 2 里有注册表按钮。

注册表文件必须存在，并且必须以 `.reg` 结尾。

注册表导入最多等待 60 秒。60 秒内完成时按 `reg.exe` 的真实结果判断成功或失败；超过 60 秒仍未完成时会停止 `reg.exe`，并按失败处理。

程序默认使用当前用户权限运行。HKCU 通常不需要管理员权限。HKLM、HKCR 等系统级位置通常需要右键以管理员身份运行 EXE。

模式 1 保持静默。模式 2 会显示成功、失败或超时提示。

## 窗口元素

窗口模式只读取 `Mode2.Element1` 到 `Mode2.Element10`。显示顺序固定按元素编号排列。

支持的元素类型：

- `Text`：单行文本，最多显示 512 个字符，超过后会静默截断。
- `TextBox`：直接写在 INI 里的只读滚动文本。适合不太长、可以直接放在 INI 里的说明。
- `TextFileBox`：从本地文本文件读取的只读滚动文本。程序会优先按 UTF-8 读取；如果不是有效 UTF-8，会按系统默认代码页读取。CRLF 和 LF 换行都会正常显示。文件最大约 4 MB。
- `Input`：输入框。用户输入的内容会在本次运行中写入一个模式 2 变量。多个输入框写同一变量时，元素编号靠后的会覆盖前面的值。
- `OpenButton`：打开最多五个文件或程序。设置 `MinimizeOnClick=1` 后，点击时也会最小化窗口。
- `MessageButton`：弹出消息。内容必须写在 INI 的一行里，用 `\n` 表示实际换行。设置 `MinimizeOnClick=1` 后，点击时也会最小化窗口。
- `RegButton`：导入 `.reg` 文件。文件必须存在，最终路径必须以 `.reg` 结尾。模式 2 会显示成功、失败或超时提示。设置 `MinimizeOnClick=1` 后，点击时也会最小化窗口。
- `Table`：显示基础 CSV 文件。第一行是表头，最多显示 20 列、1000 行。每个单元格大约显示 256 个字符。文件最大约 4 MB。支持逗号、规范引号、两个双引号转义、引号内换行；引号不规范时表格显示为空。不支持排序、筛选、编辑、公式和图片。
- `CloseButton`：关闭窗口。

`TextBox`、`TextFileBox` 和 `Table` 支持 `HeightScale=1`、`2`、`3`。倍率越高越占窗口高度。小屏幕或高 DPI 下，太多高倍率元素可能让窗口超过屏幕高度。

## 常见排查

如果配置无法读取，请检查：

- INI 文件名是否和 EXE 文件名对应；
- 文件是否为空；
- 编码是否是 UTF-8 或 UTF-16 LE with BOM；
- 文件是否超过约 4 MB；
- `Mode` 是否为 0、1、2；
- 数字开关是否为 0 或 1。

如果无法修复，请先备份 INI，再删除它。程序下次运行会释放默认配置。

## 默认配置模板

```ini
##默认配置模板
##如果同名 INI 不存在，程序会释放这份模板。
##默认 Mode=0 打开编辑模式。编辑模式会按当前语言显示内置说明。
##默认模板使用 UTF-8 with BOM。程序支持 UTF-8 无 BOM、UTF-8 with BOM 和 UTF-16 LE with BOM。
##主 INI 不支持 ANSI、GBK 或其他本地代码页。
##注释行以 # 或 ; 开头。没有等号的行会被忽略。未知区域和未知配置项会被忽略。
##等号右侧的内容会按原样保留。开头和结尾的空格也属于配置值。控制字段会忽略两侧空格。
##Mode 只能填写 0、1、2。Enable、Run、Show、UseRelativePath、AddReg、RegImported 等数字开关只能填写 0 或 1。
##如果配置无法修复，请先备份它，再删除它。程序下次运行会释放默认配置。
##相对路径字段只替换当前模式变量。绝对路径字段、启动参数和自定义工作目录也支持 Windows 环境变量和 %XHLROOT%。
##变量不用于 Mode、Language、Show、Type、Run、UseRelativePath、HeightScale、Variable、WorkingDirectoryMode 等控制字段。
##注册表导入使用当前用户权限。HKCU 通常不需要管理员权限。HKLM、HKCR 等系统区域通常需要以管理员身份运行 EXE。

[Launcher]
Title=启动器
##编辑模式和窗口模式使用的窗口标题，最多 32 个字符。
Version=1.0.3.0
##显示在标题后面的版本文字，最多 32 个字符，可以填写任意文字。
Language=zh-Hans
##界面语言。en-US 表示英文，zh-Hans 表示简体中文，zh-Hant 表示繁体中文，可选值是 en-US、zh-Hans 和 zh-Hant。
Mode=0
##运行模式。0=编辑模式，1=静默模式，2=窗口模式。
SplashEnable=0
##启动图开关。0=关闭，1=启用。启用但路径为空或无效时会静默跳过。
SplashUseRelativePath=1
##启动图路径类型。1=相对路径，以 EXE 所在目录为基准；0=绝对路径。
SplashImage=
##PNG 启动图路径。只支持 PNG，支持透明通道。
SplashDurationMs=1000
##启动图显示时间，单位毫秒。启用启动图时必须填写 1 到 60000。
SplashScale=100
##启动图缩放比例。启用启动图时必须填写 10 到 200，不写百分号。编辑模式用下拉框选择，每 5% 一个步进。
SplashPosition=Center
##启动图位置。启用启动图时必须填写 Center、TopLeft、TopRight、BottomLeft 或 BottomRight。

[Mode1.Variable1]
Enable=0
##可选 0 或 1。1 表示启用 %XHLM1V1%。
Value=
##变量值，最多约 4095 个字符。替换后的运行时文本最多 32767 个 UTF-16 字符。

[Mode1.Variable2]
Enable=0
##可选 0 或 1。1 表示启用 %XHLM1V2%。
Value=
##变量值，最多约 4095 个字符。替换后的运行时文本最多 32767 个 UTF-16 字符。

[Mode1.Variable3]
Enable=0
##可选 0 或 1。1 表示启用 %XHLM1V3%。
Value=
##变量值，最多约 4095 个字符。替换后的运行时文本最多 32767 个 UTF-16 字符。

[Mode1.Variable4]
Enable=0
##可选 0 或 1。1 表示启用 %XHLM1V4%。
Value=
##变量值，最多约 4095 个字符。替换后的运行时文本最多 32767 个 UTF-16 字符。

[Mode1.Variable5]
Enable=0
##可选 0 或 1。1 表示启用 %XHLM1V5%。
Value=
##变量值，最多约 4095 个字符。替换后的运行时文本最多 32767 个 UTF-16 字符。

[Mode1.Variable6]
Enable=0
##可选 0 或 1。1 表示启用 %XHLM1V6%。
Value=
##变量值，最多约 4095 个字符。替换后的运行时文本最多 32767 个 UTF-16 字符。

[Mode1.Registry]
AddReg=0
##可选 0 或 1。1 表示静默模式导入 RegFileName 指定的 .reg 文件。
UseRelativePath=1
##注册表路径类型。1=相对路径，0=绝对路径。
RegFileName=
##注册表路径，必须是存在的 .reg 文件。
RegImported=0
##标记为已导入。0=允许导入，1=跳过导入。成功导入后程序会写成 1。
WorkingDirectoryMode=TargetDir
##注册表导入的工作目录。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用，最终必须是存在的文件夹。

[Mode1.Run1]
Run=0
##可选 0 或 1。1 表示静默模式执行运行项 1。
UseRelativePath=1
##路径类型。1=相对路径，0=绝对路径。
FileName=
##文件路径，必须是存在的文件。文件夹路径会跳过。
Arguments=
##启动参数，主要用于程序。支持模式 1 变量、Windows 环境变量和 %XHLROOT%。
WorkingDirectoryMode=TargetDir
##工作目录模式。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用，最终必须是存在的文件夹。

[Mode1.Run2]
Run=0
##可选 0 或 1。1 表示静默模式执行运行项 2。
UseRelativePath=1
##路径类型。1=相对路径，0=绝对路径。
FileName=
##文件路径，必须是存在的文件。文件夹路径会跳过。
Arguments=
##启动参数，主要用于程序。支持模式 1 变量、Windows 环境变量和 %XHLROOT%。
WorkingDirectoryMode=TargetDir
##工作目录模式。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用，最终必须是存在的文件夹。

[Mode1.Run3]
Run=0
##可选 0 或 1。1 表示静默模式执行运行项 3。
UseRelativePath=1
##路径类型。1=相对路径，0=绝对路径。
FileName=
##文件路径，必须是存在的文件。文件夹路径会跳过。
Arguments=
##启动参数，主要用于程序。支持模式 1 变量、Windows 环境变量和 %XHLROOT%。
WorkingDirectoryMode=TargetDir
##工作目录模式。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用，最终必须是存在的文件夹。

[Mode1.Run4]
Run=0
##可选 0 或 1。1 表示静默模式执行运行项 4。
UseRelativePath=1
##路径类型。1=相对路径，0=绝对路径。
FileName=
##文件路径，必须是存在的文件。文件夹路径会跳过。
Arguments=
##启动参数，主要用于程序。支持模式 1 变量、Windows 环境变量和 %XHLROOT%。
WorkingDirectoryMode=TargetDir
##工作目录模式。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用，最终必须是存在的文件夹。

[Mode1.Run5]
Run=0
##可选 0 或 1。1 表示静默模式执行运行项 5。
UseRelativePath=1
##路径类型。1=相对路径，0=绝对路径。
FileName=
##文件路径，必须是存在的文件。文件夹路径会跳过。
Arguments=
##启动参数，主要用于程序。支持模式 1 变量、Windows 环境变量和 %XHLROOT%。
WorkingDirectoryMode=TargetDir
##工作目录模式。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用，最终必须是存在的文件夹。

##窗口模式变量由输入框元素写入。Variable=1 到 6 对应 %XHLM2V1% 到 %XHLM2V6%。替换后的运行时文本最多 32767 个 UTF-16 字符。
##只读取 Element1 到 Element10。显示顺序固定为 Element1 到 Element10。Element11 和更高编号会被忽略。
##支持的 Type 值：Text、TextBox、TextFileBox、Input、OpenButton、MessageButton、RegButton、Table、CloseButton。
##表格只支持基础 CSV。第一行是表头。引号格式不正确时表格会显示为空。

[Mode2.Element1]
Show=1
##可选 0 或 1。1 表示显示此元素。
Type=TextBox
##元素类型。Show=1 时必须选择有效类型。建议在编辑模式中选择。
HeightScale=1
##仅 TextBox、TextFileBox 和 Table 使用。可选 1、2、3。高倍率元素太多时窗口可能超过屏幕高度。
Text=欢迎使用。\n请在编辑模式中修改这份模板。
##显示文字。TextBox 显示长文本，Text 显示一行，最多 512 个字符；按钮显示按钮文字，最多 512 个字符。Input、TextFileBox 和 Table 不使用此项。
Variable=1
##仅 Input 使用。可选 1 到 6，对应 %XHLM2V1% 到 %XHLM2V6%。多个输入框写同一变量时，元素编号靠后的会覆盖前面的值。
Message=这是一条弹窗消息。\n可以用 \n 表示换行。
##仅 MessageButton 使用。必须写在一行里，用 \n 表示实际换行。
RegFileName=
##仅 RegButton 使用。填写存在的 .reg 文件，最终路径必须以 .reg 结尾。
UseRelativePath=1
##仅 RegButton 使用。1=相对路径，0=绝对路径。
WorkingDirectoryMode=TargetDir
##仅 RegButton 使用。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用。
RegImportingText=
##仅 RegButton 使用。导入中显示在按钮上的文字。留空使用默认文字。
RegSuccessMessage=
##仅 RegButton 使用。导入成功后的提示，留空使用默认提示。
RegFailureMessage=
##仅 RegButton 使用。导入失败后的提示。自定义时建议提醒可能已部分导入。
RegTimeoutMessage=
##仅 RegButton 使用。导入超过 60 秒后的提示。自定义时建议提醒可能已部分导入。
CsvFileName=
##仅 Table 使用。CSV 路径，最终路径必须以 .csv 结尾。最多显示 20 列、1000 行。
CsvUseRelativePath=1
##仅 Table 使用。1=相对路径，0=绝对路径。
TextFileName=
##仅 TextFileBox 使用。文本文件路径。优先按 UTF-8 读取；不是有效 UTF-8 时按系统默认代码页读取。支持 CRLF 和 LF 换行。
TextFileUseRelativePath=1
##仅 TextFileBox 使用。1=相对路径，0=绝对路径。
Run1=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 1。
UseRelativePath1=1
##仅 OpenButton 使用。1 表示 FileName1 是相对路径，0 表示绝对路径。
FileName1=
##仅 OpenButton 使用。运行项 1 的文件路径。
Arguments1=
##仅 OpenButton 使用。运行项 1 的启动参数。
WorkingDirectoryMode1=TargetDir
##仅 OpenButton 使用。运行项 1 的工作目录模式。
WorkingDirectory1=
##仅 OpenButton 使用。运行项 1 的自定义工作目录。
Run2=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 2。
UseRelativePath2=1
##仅 OpenButton 使用。1 表示 FileName2 是相对路径，0 表示绝对路径。
FileName2=
##仅 OpenButton 使用。运行项 2 的文件路径。
Arguments2=
##仅 OpenButton 使用。运行项 2 的启动参数。
WorkingDirectoryMode2=TargetDir
##仅 OpenButton 使用。运行项 2 的工作目录模式。
WorkingDirectory2=
##仅 OpenButton 使用。运行项 2 的自定义工作目录。
Run3=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 3。
UseRelativePath3=1
##仅 OpenButton 使用。1 表示 FileName3 是相对路径，0 表示绝对路径。
FileName3=
##仅 OpenButton 使用。运行项 3 的文件路径。
Arguments3=
##仅 OpenButton 使用。运行项 3 的启动参数。
WorkingDirectoryMode3=TargetDir
##仅 OpenButton 使用。运行项 3 的工作目录模式。
WorkingDirectory3=
##仅 OpenButton 使用。运行项 3 的自定义工作目录。
Run4=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 4。
UseRelativePath4=1
##仅 OpenButton 使用。1 表示 FileName4 是相对路径，0 表示绝对路径。
FileName4=
##仅 OpenButton 使用。运行项 4 的文件路径。
Arguments4=
##仅 OpenButton 使用。运行项 4 的启动参数。
WorkingDirectoryMode4=TargetDir
##仅 OpenButton 使用。运行项 4 的工作目录模式。
WorkingDirectory4=
##仅 OpenButton 使用。运行项 4 的自定义工作目录。
Run5=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 5。
UseRelativePath5=1
##仅 OpenButton 使用。1 表示 FileName5 是相对路径，0 表示绝对路径。
FileName5=
##仅 OpenButton 使用。运行项 5 的文件路径。
Arguments5=
##仅 OpenButton 使用。运行项 5 的启动参数。
WorkingDirectoryMode5=TargetDir
##仅 OpenButton 使用。运行项 5 的工作目录模式。
WorkingDirectory5=
##仅 OpenButton 使用。运行项 5 的自定义工作目录。

[Mode2.Element2]
Show=1
##可选 0 或 1。1 表示显示此元素。
Type=Text
##元素类型。Show=1 时必须选择有效类型。建议在编辑模式中选择。
HeightScale=1
##仅 TextBox、TextFileBox 和 Table 使用。可选 1、2、3。高倍率元素太多时窗口可能超过屏幕高度。
Text=单行文本示例
##显示文字。TextBox 显示长文本，Text 显示一行，最多 512 个字符；按钮显示按钮文字，最多 512 个字符。Input、TextFileBox 和 Table 不使用此项。
Variable=1
##仅 Input 使用。可选 1 到 6，对应 %XHLM2V1% 到 %XHLM2V6%。多个输入框写同一变量时，元素编号靠后的会覆盖前面的值。
Message=这是一条弹窗消息。\n可以用 \n 表示换行。
##仅 MessageButton 使用。必须写在一行里，用 \n 表示实际换行。
RegFileName=
##仅 RegButton 使用。填写存在的 .reg 文件，最终路径必须以 .reg 结尾。
UseRelativePath=1
##仅 RegButton 使用。1=相对路径，0=绝对路径。
WorkingDirectoryMode=TargetDir
##仅 RegButton 使用。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用。
RegImportingText=
##仅 RegButton 使用。导入中显示在按钮上的文字。留空使用默认文字。
RegSuccessMessage=
##仅 RegButton 使用。导入成功后的提示，留空使用默认提示。
RegFailureMessage=
##仅 RegButton 使用。导入失败后的提示。自定义时建议提醒可能已部分导入。
RegTimeoutMessage=
##仅 RegButton 使用。导入超过 60 秒后的提示。自定义时建议提醒可能已部分导入。
CsvFileName=
##仅 Table 使用。CSV 路径，最终路径必须以 .csv 结尾。最多显示 20 列、1000 行。
CsvUseRelativePath=1
##仅 Table 使用。1=相对路径，0=绝对路径。
TextFileName=
##仅 TextFileBox 使用。文本文件路径。优先按 UTF-8 读取；不是有效 UTF-8 时按系统默认代码页读取。支持 CRLF 和 LF 换行。
TextFileUseRelativePath=1
##仅 TextFileBox 使用。1=相对路径，0=绝对路径。
Run1=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 1。
UseRelativePath1=1
##仅 OpenButton 使用。1 表示 FileName1 是相对路径，0 表示绝对路径。
FileName1=
##仅 OpenButton 使用。运行项 1 的文件路径。
Arguments1=
##仅 OpenButton 使用。运行项 1 的启动参数。
WorkingDirectoryMode1=TargetDir
##仅 OpenButton 使用。运行项 1 的工作目录模式。
WorkingDirectory1=
##仅 OpenButton 使用。运行项 1 的自定义工作目录。
Run2=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 2。
UseRelativePath2=1
##仅 OpenButton 使用。1 表示 FileName2 是相对路径，0 表示绝对路径。
FileName2=
##仅 OpenButton 使用。运行项 2 的文件路径。
Arguments2=
##仅 OpenButton 使用。运行项 2 的启动参数。
WorkingDirectoryMode2=TargetDir
##仅 OpenButton 使用。运行项 2 的工作目录模式。
WorkingDirectory2=
##仅 OpenButton 使用。运行项 2 的自定义工作目录。
Run3=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 3。
UseRelativePath3=1
##仅 OpenButton 使用。1 表示 FileName3 是相对路径，0 表示绝对路径。
FileName3=
##仅 OpenButton 使用。运行项 3 的文件路径。
Arguments3=
##仅 OpenButton 使用。运行项 3 的启动参数。
WorkingDirectoryMode3=TargetDir
##仅 OpenButton 使用。运行项 3 的工作目录模式。
WorkingDirectory3=
##仅 OpenButton 使用。运行项 3 的自定义工作目录。
Run4=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 4。
UseRelativePath4=1
##仅 OpenButton 使用。1 表示 FileName4 是相对路径，0 表示绝对路径。
FileName4=
##仅 OpenButton 使用。运行项 4 的文件路径。
Arguments4=
##仅 OpenButton 使用。运行项 4 的启动参数。
WorkingDirectoryMode4=TargetDir
##仅 OpenButton 使用。运行项 4 的工作目录模式。
WorkingDirectory4=
##仅 OpenButton 使用。运行项 4 的自定义工作目录。
Run5=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 5。
UseRelativePath5=1
##仅 OpenButton 使用。1 表示 FileName5 是相对路径，0 表示绝对路径。
FileName5=
##仅 OpenButton 使用。运行项 5 的文件路径。
Arguments5=
##仅 OpenButton 使用。运行项 5 的启动参数。
WorkingDirectoryMode5=TargetDir
##仅 OpenButton 使用。运行项 5 的工作目录模式。
WorkingDirectory5=
##仅 OpenButton 使用。运行项 5 的自定义工作目录。

[Mode2.Element3]
Show=1
##可选 0 或 1。1 表示显示此元素。
Type=Input
##元素类型。Show=1 时必须选择有效类型。建议在编辑模式中选择。
HeightScale=1
##仅 TextBox、TextFileBox 和 Table 使用。可选 1、2、3。高倍率元素太多时窗口可能超过屏幕高度。
Text=
##显示文字。TextBox 显示长文本，Text 显示一行，最多 512 个字符；按钮显示按钮文字，最多 512 个字符。Input、TextFileBox 和 Table 不使用此项。
Variable=1
##仅 Input 使用。可选 1 到 6，对应 %XHLM2V1% 到 %XHLM2V6%。多个输入框写同一变量时，元素编号靠后的会覆盖前面的值。
Message=这是一条弹窗消息。\n可以用 \n 表示换行。
##仅 MessageButton 使用。必须写在一行里，用 \n 表示实际换行。
RegFileName=
##仅 RegButton 使用。填写存在的 .reg 文件，最终路径必须以 .reg 结尾。
UseRelativePath=1
##仅 RegButton 使用。1=相对路径，0=绝对路径。
WorkingDirectoryMode=TargetDir
##仅 RegButton 使用。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用。
RegImportingText=
##仅 RegButton 使用。导入中显示在按钮上的文字。留空使用默认文字。
RegSuccessMessage=
##仅 RegButton 使用。导入成功后的提示，留空使用默认提示。
RegFailureMessage=
##仅 RegButton 使用。导入失败后的提示。自定义时建议提醒可能已部分导入。
RegTimeoutMessage=
##仅 RegButton 使用。导入超过 60 秒后的提示。自定义时建议提醒可能已部分导入。
CsvFileName=
##仅 Table 使用。CSV 路径，最终路径必须以 .csv 结尾。最多显示 20 列、1000 行。
CsvUseRelativePath=1
##仅 Table 使用。1=相对路径，0=绝对路径。
TextFileName=
##仅 TextFileBox 使用。文本文件路径。优先按 UTF-8 读取；不是有效 UTF-8 时按系统默认代码页读取。支持 CRLF 和 LF 换行。
TextFileUseRelativePath=1
##仅 TextFileBox 使用。1=相对路径，0=绝对路径。
Run1=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 1。
UseRelativePath1=1
##仅 OpenButton 使用。1 表示 FileName1 是相对路径，0 表示绝对路径。
FileName1=
##仅 OpenButton 使用。运行项 1 的文件路径。
Arguments1=
##仅 OpenButton 使用。运行项 1 的启动参数。
WorkingDirectoryMode1=TargetDir
##仅 OpenButton 使用。运行项 1 的工作目录模式。
WorkingDirectory1=
##仅 OpenButton 使用。运行项 1 的自定义工作目录。
Run2=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 2。
UseRelativePath2=1
##仅 OpenButton 使用。1 表示 FileName2 是相对路径，0 表示绝对路径。
FileName2=
##仅 OpenButton 使用。运行项 2 的文件路径。
Arguments2=
##仅 OpenButton 使用。运行项 2 的启动参数。
WorkingDirectoryMode2=TargetDir
##仅 OpenButton 使用。运行项 2 的工作目录模式。
WorkingDirectory2=
##仅 OpenButton 使用。运行项 2 的自定义工作目录。
Run3=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 3。
UseRelativePath3=1
##仅 OpenButton 使用。1 表示 FileName3 是相对路径，0 表示绝对路径。
FileName3=
##仅 OpenButton 使用。运行项 3 的文件路径。
Arguments3=
##仅 OpenButton 使用。运行项 3 的启动参数。
WorkingDirectoryMode3=TargetDir
##仅 OpenButton 使用。运行项 3 的工作目录模式。
WorkingDirectory3=
##仅 OpenButton 使用。运行项 3 的自定义工作目录。
Run4=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 4。
UseRelativePath4=1
##仅 OpenButton 使用。1 表示 FileName4 是相对路径，0 表示绝对路径。
FileName4=
##仅 OpenButton 使用。运行项 4 的文件路径。
Arguments4=
##仅 OpenButton 使用。运行项 4 的启动参数。
WorkingDirectoryMode4=TargetDir
##仅 OpenButton 使用。运行项 4 的工作目录模式。
WorkingDirectory4=
##仅 OpenButton 使用。运行项 4 的自定义工作目录。
Run5=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 5。
UseRelativePath5=1
##仅 OpenButton 使用。1 表示 FileName5 是相对路径，0 表示绝对路径。
FileName5=
##仅 OpenButton 使用。运行项 5 的文件路径。
Arguments5=
##仅 OpenButton 使用。运行项 5 的启动参数。
WorkingDirectoryMode5=TargetDir
##仅 OpenButton 使用。运行项 5 的工作目录模式。
WorkingDirectory5=
##仅 OpenButton 使用。运行项 5 的自定义工作目录。

[Mode2.Element4]
Show=1
##可选 0 或 1。1 表示显示此元素。
Type=OpenButton
##元素类型。Show=1 时必须选择有效类型。建议在编辑模式中选择。
HeightScale=1
##仅 TextBox、TextFileBox 和 Table 使用。可选 1、2、3。高倍率元素太多时窗口可能超过屏幕高度。
Text=打开文件
##显示文字。TextBox 显示长文本，Text 显示一行，最多 512 个字符；按钮显示按钮文字，最多 512 个字符。Input、TextFileBox 和 Table 不使用此项。
Variable=1
##仅 Input 使用。可选 1 到 6，对应 %XHLM2V1% 到 %XHLM2V6%。多个输入框写同一变量时，元素编号靠后的会覆盖前面的值。
Message=这是一条弹窗消息。\n可以用 \n 表示换行。
##仅 MessageButton 使用。必须写在一行里，用 \n 表示实际换行。
RegFileName=
##仅 RegButton 使用。填写存在的 .reg 文件，最终路径必须以 .reg 结尾。
UseRelativePath=1
##仅 RegButton 使用。1=相对路径，0=绝对路径。
WorkingDirectoryMode=TargetDir
##仅 RegButton 使用。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用。
RegImportingText=
##仅 RegButton 使用。导入中显示在按钮上的文字。留空使用默认文字。
RegSuccessMessage=
##仅 RegButton 使用。导入成功后的提示，留空使用默认提示。
RegFailureMessage=
##仅 RegButton 使用。导入失败后的提示。自定义时建议提醒可能已部分导入。
RegTimeoutMessage=
##仅 RegButton 使用。导入超过 60 秒后的提示。自定义时建议提醒可能已部分导入。
CsvFileName=
##仅 Table 使用。CSV 路径，最终路径必须以 .csv 结尾。最多显示 20 列、1000 行。
CsvUseRelativePath=1
##仅 Table 使用。1=相对路径，0=绝对路径。
TextFileName=
##仅 TextFileBox 使用。文本文件路径。优先按 UTF-8 读取；不是有效 UTF-8 时按系统默认代码页读取。支持 CRLF 和 LF 换行。
TextFileUseRelativePath=1
##仅 TextFileBox 使用。1=相对路径，0=绝对路径。
Run1=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 1。
UseRelativePath1=1
##仅 OpenButton 使用。1 表示 FileName1 是相对路径，0 表示绝对路径。
FileName1=
##仅 OpenButton 使用。运行项 1 的文件路径。
Arguments1=
##仅 OpenButton 使用。运行项 1 的启动参数。
WorkingDirectoryMode1=TargetDir
##仅 OpenButton 使用。运行项 1 的工作目录模式。
WorkingDirectory1=
##仅 OpenButton 使用。运行项 1 的自定义工作目录。
Run2=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 2。
UseRelativePath2=1
##仅 OpenButton 使用。1 表示 FileName2 是相对路径，0 表示绝对路径。
FileName2=
##仅 OpenButton 使用。运行项 2 的文件路径。
Arguments2=
##仅 OpenButton 使用。运行项 2 的启动参数。
WorkingDirectoryMode2=TargetDir
##仅 OpenButton 使用。运行项 2 的工作目录模式。
WorkingDirectory2=
##仅 OpenButton 使用。运行项 2 的自定义工作目录。
Run3=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 3。
UseRelativePath3=1
##仅 OpenButton 使用。1 表示 FileName3 是相对路径，0 表示绝对路径。
FileName3=
##仅 OpenButton 使用。运行项 3 的文件路径。
Arguments3=
##仅 OpenButton 使用。运行项 3 的启动参数。
WorkingDirectoryMode3=TargetDir
##仅 OpenButton 使用。运行项 3 的工作目录模式。
WorkingDirectory3=
##仅 OpenButton 使用。运行项 3 的自定义工作目录。
Run4=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 4。
UseRelativePath4=1
##仅 OpenButton 使用。1 表示 FileName4 是相对路径，0 表示绝对路径。
FileName4=
##仅 OpenButton 使用。运行项 4 的文件路径。
Arguments4=
##仅 OpenButton 使用。运行项 4 的启动参数。
WorkingDirectoryMode4=TargetDir
##仅 OpenButton 使用。运行项 4 的工作目录模式。
WorkingDirectory4=
##仅 OpenButton 使用。运行项 4 的自定义工作目录。
Run5=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 5。
UseRelativePath5=1
##仅 OpenButton 使用。1 表示 FileName5 是相对路径，0 表示绝对路径。
FileName5=
##仅 OpenButton 使用。运行项 5 的文件路径。
Arguments5=
##仅 OpenButton 使用。运行项 5 的启动参数。
WorkingDirectoryMode5=TargetDir
##仅 OpenButton 使用。运行项 5 的工作目录模式。
WorkingDirectory5=
##仅 OpenButton 使用。运行项 5 的自定义工作目录。

[Mode2.Element5]
Show=1
##可选 0 或 1。1 表示显示此元素。
Type=MessageButton
##元素类型。Show=1 时必须选择有效类型。建议在编辑模式中选择。
HeightScale=1
##仅 TextBox、TextFileBox 和 Table 使用。可选 1、2、3。高倍率元素太多时窗口可能超过屏幕高度。
Text=弹窗
##显示文字。TextBox 显示长文本，Text 显示一行，最多 512 个字符；按钮显示按钮文字，最多 512 个字符。Input、TextFileBox 和 Table 不使用此项。
Variable=1
##仅 Input 使用。可选 1 到 6，对应 %XHLM2V1% 到 %XHLM2V6%。多个输入框写同一变量时，元素编号靠后的会覆盖前面的值。
Message=这是一条弹窗消息。\n可以用 \n 表示换行。
##仅 MessageButton 使用。必须写在一行里，用 \n 表示实际换行。
RegFileName=
##仅 RegButton 使用。填写存在的 .reg 文件，最终路径必须以 .reg 结尾。
UseRelativePath=1
##仅 RegButton 使用。1=相对路径，0=绝对路径。
WorkingDirectoryMode=TargetDir
##仅 RegButton 使用。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用。
RegImportingText=
##仅 RegButton 使用。导入中显示在按钮上的文字。留空使用默认文字。
RegSuccessMessage=
##仅 RegButton 使用。导入成功后的提示，留空使用默认提示。
RegFailureMessage=
##仅 RegButton 使用。导入失败后的提示。自定义时建议提醒可能已部分导入。
RegTimeoutMessage=
##仅 RegButton 使用。导入超过 60 秒后的提示。自定义时建议提醒可能已部分导入。
CsvFileName=
##仅 Table 使用。CSV 路径，最终路径必须以 .csv 结尾。最多显示 20 列、1000 行。
CsvUseRelativePath=1
##仅 Table 使用。1=相对路径，0=绝对路径。
TextFileName=
##仅 TextFileBox 使用。文本文件路径。优先按 UTF-8 读取；不是有效 UTF-8 时按系统默认代码页读取。支持 CRLF 和 LF 换行。
TextFileUseRelativePath=1
##仅 TextFileBox 使用。1=相对路径，0=绝对路径。
Run1=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 1。
UseRelativePath1=1
##仅 OpenButton 使用。1 表示 FileName1 是相对路径，0 表示绝对路径。
FileName1=
##仅 OpenButton 使用。运行项 1 的文件路径。
Arguments1=
##仅 OpenButton 使用。运行项 1 的启动参数。
WorkingDirectoryMode1=TargetDir
##仅 OpenButton 使用。运行项 1 的工作目录模式。
WorkingDirectory1=
##仅 OpenButton 使用。运行项 1 的自定义工作目录。
Run2=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 2。
UseRelativePath2=1
##仅 OpenButton 使用。1 表示 FileName2 是相对路径，0 表示绝对路径。
FileName2=
##仅 OpenButton 使用。运行项 2 的文件路径。
Arguments2=
##仅 OpenButton 使用。运行项 2 的启动参数。
WorkingDirectoryMode2=TargetDir
##仅 OpenButton 使用。运行项 2 的工作目录模式。
WorkingDirectory2=
##仅 OpenButton 使用。运行项 2 的自定义工作目录。
Run3=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 3。
UseRelativePath3=1
##仅 OpenButton 使用。1 表示 FileName3 是相对路径，0 表示绝对路径。
FileName3=
##仅 OpenButton 使用。运行项 3 的文件路径。
Arguments3=
##仅 OpenButton 使用。运行项 3 的启动参数。
WorkingDirectoryMode3=TargetDir
##仅 OpenButton 使用。运行项 3 的工作目录模式。
WorkingDirectory3=
##仅 OpenButton 使用。运行项 3 的自定义工作目录。
Run4=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 4。
UseRelativePath4=1
##仅 OpenButton 使用。1 表示 FileName4 是相对路径，0 表示绝对路径。
FileName4=
##仅 OpenButton 使用。运行项 4 的文件路径。
Arguments4=
##仅 OpenButton 使用。运行项 4 的启动参数。
WorkingDirectoryMode4=TargetDir
##仅 OpenButton 使用。运行项 4 的工作目录模式。
WorkingDirectory4=
##仅 OpenButton 使用。运行项 4 的自定义工作目录。
Run5=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 5。
UseRelativePath5=1
##仅 OpenButton 使用。1 表示 FileName5 是相对路径，0 表示绝对路径。
FileName5=
##仅 OpenButton 使用。运行项 5 的文件路径。
Arguments5=
##仅 OpenButton 使用。运行项 5 的启动参数。
WorkingDirectoryMode5=TargetDir
##仅 OpenButton 使用。运行项 5 的工作目录模式。
WorkingDirectory5=
##仅 OpenButton 使用。运行项 5 的自定义工作目录。

[Mode2.Element6]
Show=1
##可选 0 或 1。1 表示显示此元素。
Type=RegButton
##元素类型。Show=1 时必须选择有效类型。建议在编辑模式中选择。
HeightScale=1
##仅 TextBox、TextFileBox 和 Table 使用。可选 1、2、3。高倍率元素太多时窗口可能超过屏幕高度。
Text=导入注册表
##显示文字。TextBox 显示长文本，Text 显示一行，最多 512 个字符；按钮显示按钮文字，最多 512 个字符。Input、TextFileBox 和 Table 不使用此项。
Variable=1
##仅 Input 使用。可选 1 到 6，对应 %XHLM2V1% 到 %XHLM2V6%。多个输入框写同一变量时，元素编号靠后的会覆盖前面的值。
Message=这是一条弹窗消息。\n可以用 \n 表示换行。
##仅 MessageButton 使用。必须写在一行里，用 \n 表示实际换行。
RegFileName=
##仅 RegButton 使用。填写存在的 .reg 文件，最终路径必须以 .reg 结尾。
UseRelativePath=1
##仅 RegButton 使用。1=相对路径，0=绝对路径。
WorkingDirectoryMode=TargetDir
##仅 RegButton 使用。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用。
RegImportingText=
##仅 RegButton 使用。导入中显示在按钮上的文字。留空使用默认文字。
RegSuccessMessage=
##仅 RegButton 使用。导入成功后的提示，留空使用默认提示。
RegFailureMessage=
##仅 RegButton 使用。导入失败后的提示。自定义时建议提醒可能已部分导入。
RegTimeoutMessage=
##仅 RegButton 使用。导入超过 60 秒后的提示。自定义时建议提醒可能已部分导入。
CsvFileName=
##仅 Table 使用。CSV 路径，最终路径必须以 .csv 结尾。最多显示 20 列、1000 行。
CsvUseRelativePath=1
##仅 Table 使用。1=相对路径，0=绝对路径。
TextFileName=
##仅 TextFileBox 使用。文本文件路径。优先按 UTF-8 读取；不是有效 UTF-8 时按系统默认代码页读取。支持 CRLF 和 LF 换行。
TextFileUseRelativePath=1
##仅 TextFileBox 使用。1=相对路径，0=绝对路径。
Run1=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 1。
UseRelativePath1=1
##仅 OpenButton 使用。1 表示 FileName1 是相对路径，0 表示绝对路径。
FileName1=
##仅 OpenButton 使用。运行项 1 的文件路径。
Arguments1=
##仅 OpenButton 使用。运行项 1 的启动参数。
WorkingDirectoryMode1=TargetDir
##仅 OpenButton 使用。运行项 1 的工作目录模式。
WorkingDirectory1=
##仅 OpenButton 使用。运行项 1 的自定义工作目录。
Run2=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 2。
UseRelativePath2=1
##仅 OpenButton 使用。1 表示 FileName2 是相对路径，0 表示绝对路径。
FileName2=
##仅 OpenButton 使用。运行项 2 的文件路径。
Arguments2=
##仅 OpenButton 使用。运行项 2 的启动参数。
WorkingDirectoryMode2=TargetDir
##仅 OpenButton 使用。运行项 2 的工作目录模式。
WorkingDirectory2=
##仅 OpenButton 使用。运行项 2 的自定义工作目录。
Run3=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 3。
UseRelativePath3=1
##仅 OpenButton 使用。1 表示 FileName3 是相对路径，0 表示绝对路径。
FileName3=
##仅 OpenButton 使用。运行项 3 的文件路径。
Arguments3=
##仅 OpenButton 使用。运行项 3 的启动参数。
WorkingDirectoryMode3=TargetDir
##仅 OpenButton 使用。运行项 3 的工作目录模式。
WorkingDirectory3=
##仅 OpenButton 使用。运行项 3 的自定义工作目录。
Run4=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 4。
UseRelativePath4=1
##仅 OpenButton 使用。1 表示 FileName4 是相对路径，0 表示绝对路径。
FileName4=
##仅 OpenButton 使用。运行项 4 的文件路径。
Arguments4=
##仅 OpenButton 使用。运行项 4 的启动参数。
WorkingDirectoryMode4=TargetDir
##仅 OpenButton 使用。运行项 4 的工作目录模式。
WorkingDirectory4=
##仅 OpenButton 使用。运行项 4 的自定义工作目录。
Run5=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 5。
UseRelativePath5=1
##仅 OpenButton 使用。1 表示 FileName5 是相对路径，0 表示绝对路径。
FileName5=
##仅 OpenButton 使用。运行项 5 的文件路径。
Arguments5=
##仅 OpenButton 使用。运行项 5 的启动参数。
WorkingDirectoryMode5=TargetDir
##仅 OpenButton 使用。运行项 5 的工作目录模式。
WorkingDirectory5=
##仅 OpenButton 使用。运行项 5 的自定义工作目录。

[Mode2.Element7]
Show=1
##可选 0 或 1。1 表示显示此元素。
Type=Table
##元素类型。Show=1 时必须选择有效类型。建议在编辑模式中选择。
HeightScale=1
##仅 TextBox、TextFileBox 和 Table 使用。可选 1、2、3。高倍率元素太多时窗口可能超过屏幕高度。
Text=
##显示文字。TextBox 显示长文本，Text 显示一行，最多 512 个字符；按钮显示按钮文字，最多 512 个字符。Input、TextFileBox 和 Table 不使用此项。
Variable=1
##仅 Input 使用。可选 1 到 6，对应 %XHLM2V1% 到 %XHLM2V6%。多个输入框写同一变量时，元素编号靠后的会覆盖前面的值。
Message=这是一条弹窗消息。\n可以用 \n 表示换行。
##仅 MessageButton 使用。必须写在一行里，用 \n 表示实际换行。
RegFileName=
##仅 RegButton 使用。填写存在的 .reg 文件，最终路径必须以 .reg 结尾。
UseRelativePath=1
##仅 RegButton 使用。1=相对路径，0=绝对路径。
WorkingDirectoryMode=TargetDir
##仅 RegButton 使用。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用。
RegImportingText=
##仅 RegButton 使用。导入中显示在按钮上的文字。留空使用默认文字。
RegSuccessMessage=
##仅 RegButton 使用。导入成功后的提示，留空使用默认提示。
RegFailureMessage=
##仅 RegButton 使用。导入失败后的提示。自定义时建议提醒可能已部分导入。
RegTimeoutMessage=
##仅 RegButton 使用。导入超过 60 秒后的提示。自定义时建议提醒可能已部分导入。
CsvFileName=Example.csv
##仅 Table 使用。CSV 路径，最终路径必须以 .csv 结尾。最多显示 20 列、1000 行。
CsvUseRelativePath=1
##仅 Table 使用。1=相对路径，0=绝对路径。
TextFileName=
##仅 TextFileBox 使用。文本文件路径。优先按 UTF-8 读取；不是有效 UTF-8 时按系统默认代码页读取。支持 CRLF 和 LF 换行。
TextFileUseRelativePath=1
##仅 TextFileBox 使用。1=相对路径，0=绝对路径。
Run1=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 1。
UseRelativePath1=1
##仅 OpenButton 使用。1 表示 FileName1 是相对路径，0 表示绝对路径。
FileName1=
##仅 OpenButton 使用。运行项 1 的文件路径。
Arguments1=
##仅 OpenButton 使用。运行项 1 的启动参数。
WorkingDirectoryMode1=TargetDir
##仅 OpenButton 使用。运行项 1 的工作目录模式。
WorkingDirectory1=
##仅 OpenButton 使用。运行项 1 的自定义工作目录。
Run2=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 2。
UseRelativePath2=1
##仅 OpenButton 使用。1 表示 FileName2 是相对路径，0 表示绝对路径。
FileName2=
##仅 OpenButton 使用。运行项 2 的文件路径。
Arguments2=
##仅 OpenButton 使用。运行项 2 的启动参数。
WorkingDirectoryMode2=TargetDir
##仅 OpenButton 使用。运行项 2 的工作目录模式。
WorkingDirectory2=
##仅 OpenButton 使用。运行项 2 的自定义工作目录。
Run3=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 3。
UseRelativePath3=1
##仅 OpenButton 使用。1 表示 FileName3 是相对路径，0 表示绝对路径。
FileName3=
##仅 OpenButton 使用。运行项 3 的文件路径。
Arguments3=
##仅 OpenButton 使用。运行项 3 的启动参数。
WorkingDirectoryMode3=TargetDir
##仅 OpenButton 使用。运行项 3 的工作目录模式。
WorkingDirectory3=
##仅 OpenButton 使用。运行项 3 的自定义工作目录。
Run4=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 4。
UseRelativePath4=1
##仅 OpenButton 使用。1 表示 FileName4 是相对路径，0 表示绝对路径。
FileName4=
##仅 OpenButton 使用。运行项 4 的文件路径。
Arguments4=
##仅 OpenButton 使用。运行项 4 的启动参数。
WorkingDirectoryMode4=TargetDir
##仅 OpenButton 使用。运行项 4 的工作目录模式。
WorkingDirectory4=
##仅 OpenButton 使用。运行项 4 的自定义工作目录。
Run5=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 5。
UseRelativePath5=1
##仅 OpenButton 使用。1 表示 FileName5 是相对路径，0 表示绝对路径。
FileName5=
##仅 OpenButton 使用。运行项 5 的文件路径。
Arguments5=
##仅 OpenButton 使用。运行项 5 的启动参数。
WorkingDirectoryMode5=TargetDir
##仅 OpenButton 使用。运行项 5 的工作目录模式。
WorkingDirectory5=
##仅 OpenButton 使用。运行项 5 的自定义工作目录。

[Mode2.Element8]
Show=1
##可选 0 或 1。1 表示显示此元素。
Type=TextFileBox
##元素类型。Show=1 时必须选择有效类型。建议在编辑模式中选择。
HeightScale=1
##仅 TextBox、TextFileBox 和 Table 使用。可选 1、2、3。高倍率元素太多时窗口可能超过屏幕高度。
Text=
##显示文字。TextBox 显示长文本，Text 显示一行，最多 512 个字符；按钮显示按钮文字，最多 512 个字符。Input、TextFileBox 和 Table 不使用此项。
Variable=1
##仅 Input 使用。可选 1 到 6，对应 %XHLM2V1% 到 %XHLM2V6%。多个输入框写同一变量时，元素编号靠后的会覆盖前面的值。
Message=这是一条弹窗消息。\n可以用 \n 表示换行。
##仅 MessageButton 使用。必须写在一行里，用 \n 表示实际换行。
RegFileName=
##仅 RegButton 使用。填写存在的 .reg 文件，最终路径必须以 .reg 结尾。
UseRelativePath=1
##仅 RegButton 使用。1=相对路径，0=绝对路径。
WorkingDirectoryMode=TargetDir
##仅 RegButton 使用。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用。
RegImportingText=
##仅 RegButton 使用。导入中显示在按钮上的文字。留空使用默认文字。
RegSuccessMessage=
##仅 RegButton 使用。导入成功后的提示，留空使用默认提示。
RegFailureMessage=
##仅 RegButton 使用。导入失败后的提示。自定义时建议提醒可能已部分导入。
RegTimeoutMessage=
##仅 RegButton 使用。导入超过 60 秒后的提示。自定义时建议提醒可能已部分导入。
CsvFileName=
##仅 Table 使用。CSV 路径，最终路径必须以 .csv 结尾。最多显示 20 列、1000 行。
CsvUseRelativePath=1
##仅 Table 使用。1=相对路径，0=绝对路径。
TextFileName=
##仅 TextFileBox 使用。文本文件路径。优先按 UTF-8 读取；不是有效 UTF-8 时按系统默认代码页读取。支持 CRLF 和 LF 换行。
TextFileUseRelativePath=1
##仅 TextFileBox 使用。1=相对路径，0=绝对路径。
Run1=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 1。
UseRelativePath1=1
##仅 OpenButton 使用。1 表示 FileName1 是相对路径，0 表示绝对路径。
FileName1=
##仅 OpenButton 使用。运行项 1 的文件路径。
Arguments1=
##仅 OpenButton 使用。运行项 1 的启动参数。
WorkingDirectoryMode1=TargetDir
##仅 OpenButton 使用。运行项 1 的工作目录模式。
WorkingDirectory1=
##仅 OpenButton 使用。运行项 1 的自定义工作目录。
Run2=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 2。
UseRelativePath2=1
##仅 OpenButton 使用。1 表示 FileName2 是相对路径，0 表示绝对路径。
FileName2=
##仅 OpenButton 使用。运行项 2 的文件路径。
Arguments2=
##仅 OpenButton 使用。运行项 2 的启动参数。
WorkingDirectoryMode2=TargetDir
##仅 OpenButton 使用。运行项 2 的工作目录模式。
WorkingDirectory2=
##仅 OpenButton 使用。运行项 2 的自定义工作目录。
Run3=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 3。
UseRelativePath3=1
##仅 OpenButton 使用。1 表示 FileName3 是相对路径，0 表示绝对路径。
FileName3=
##仅 OpenButton 使用。运行项 3 的文件路径。
Arguments3=
##仅 OpenButton 使用。运行项 3 的启动参数。
WorkingDirectoryMode3=TargetDir
##仅 OpenButton 使用。运行项 3 的工作目录模式。
WorkingDirectory3=
##仅 OpenButton 使用。运行项 3 的自定义工作目录。
Run4=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 4。
UseRelativePath4=1
##仅 OpenButton 使用。1 表示 FileName4 是相对路径，0 表示绝对路径。
FileName4=
##仅 OpenButton 使用。运行项 4 的文件路径。
Arguments4=
##仅 OpenButton 使用。运行项 4 的启动参数。
WorkingDirectoryMode4=TargetDir
##仅 OpenButton 使用。运行项 4 的工作目录模式。
WorkingDirectory4=
##仅 OpenButton 使用。运行项 4 的自定义工作目录。
Run5=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 5。
UseRelativePath5=1
##仅 OpenButton 使用。1 表示 FileName5 是相对路径，0 表示绝对路径。
FileName5=
##仅 OpenButton 使用。运行项 5 的文件路径。
Arguments5=
##仅 OpenButton 使用。运行项 5 的启动参数。
WorkingDirectoryMode5=TargetDir
##仅 OpenButton 使用。运行项 5 的工作目录模式。
WorkingDirectory5=
##仅 OpenButton 使用。运行项 5 的自定义工作目录。

[Mode2.Element9]
Show=1
##可选 0 或 1。1 表示显示此元素。
Type=Text
##元素类型。Show=1 时必须选择有效类型。建议在编辑模式中选择。
HeightScale=1
##仅 TextBox、TextFileBox 和 Table 使用。可选 1、2、3。高倍率元素太多时窗口可能超过屏幕高度。
Text=最小化
##显示文字。TextBox 显示长文本，Text 显示一行，最多 512 个字符；按钮显示按钮文字，最多 512 个字符。Input、TextFileBox 和 Table 不使用此项。
Variable=1
##仅 Input 使用。可选 1 到 6，对应 %XHLM2V1% 到 %XHLM2V6%。多个输入框写同一变量时，元素编号靠后的会覆盖前面的值。
Message=这是一条弹窗消息。\n可以用 \n 表示换行。
##仅 MessageButton 使用。必须写在一行里，用 \n 表示实际换行。
RegFileName=
##仅 RegButton 使用。填写存在的 .reg 文件，最终路径必须以 .reg 结尾。
UseRelativePath=1
##仅 RegButton 使用。1=相对路径，0=绝对路径。
WorkingDirectoryMode=TargetDir
##仅 RegButton 使用。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用。
RegImportingText=
##仅 RegButton 使用。导入中显示在按钮上的文字。留空使用默认文字。
RegSuccessMessage=
##仅 RegButton 使用。导入成功后的提示，留空使用默认提示。
RegFailureMessage=
##仅 RegButton 使用。导入失败后的提示。自定义时建议提醒可能已部分导入。
RegTimeoutMessage=
##仅 RegButton 使用。导入超过 60 秒后的提示。自定义时建议提醒可能已部分导入。
CsvFileName=
##仅 Table 使用。CSV 路径，最终路径必须以 .csv 结尾。最多显示 20 列、1000 行。
CsvUseRelativePath=1
##仅 Table 使用。1=相对路径，0=绝对路径。
TextFileName=
##仅 TextFileBox 使用。文本文件路径。优先按 UTF-8 读取；不是有效 UTF-8 时按系统默认代码页读取。支持 CRLF 和 LF 换行。
TextFileUseRelativePath=1
##仅 TextFileBox 使用。1=相对路径，0=绝对路径。
Run1=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 1。
UseRelativePath1=1
##仅 OpenButton 使用。1 表示 FileName1 是相对路径，0 表示绝对路径。
FileName1=
##仅 OpenButton 使用。运行项 1 的文件路径。
Arguments1=
##仅 OpenButton 使用。运行项 1 的启动参数。
WorkingDirectoryMode1=TargetDir
##仅 OpenButton 使用。运行项 1 的工作目录模式。
WorkingDirectory1=
##仅 OpenButton 使用。运行项 1 的自定义工作目录。
Run2=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 2。
UseRelativePath2=1
##仅 OpenButton 使用。1 表示 FileName2 是相对路径，0 表示绝对路径。
FileName2=
##仅 OpenButton 使用。运行项 2 的文件路径。
Arguments2=
##仅 OpenButton 使用。运行项 2 的启动参数。
WorkingDirectoryMode2=TargetDir
##仅 OpenButton 使用。运行项 2 的工作目录模式。
WorkingDirectory2=
##仅 OpenButton 使用。运行项 2 的自定义工作目录。
Run3=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 3。
UseRelativePath3=1
##仅 OpenButton 使用。1 表示 FileName3 是相对路径，0 表示绝对路径。
FileName3=
##仅 OpenButton 使用。运行项 3 的文件路径。
Arguments3=
##仅 OpenButton 使用。运行项 3 的启动参数。
WorkingDirectoryMode3=TargetDir
##仅 OpenButton 使用。运行项 3 的工作目录模式。
WorkingDirectory3=
##仅 OpenButton 使用。运行项 3 的自定义工作目录。
Run4=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 4。
UseRelativePath4=1
##仅 OpenButton 使用。1 表示 FileName4 是相对路径，0 表示绝对路径。
FileName4=
##仅 OpenButton 使用。运行项 4 的文件路径。
Arguments4=
##仅 OpenButton 使用。运行项 4 的启动参数。
WorkingDirectoryMode4=TargetDir
##仅 OpenButton 使用。运行项 4 的工作目录模式。
WorkingDirectory4=
##仅 OpenButton 使用。运行项 4 的自定义工作目录。
Run5=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 5。
UseRelativePath5=1
##仅 OpenButton 使用。1 表示 FileName5 是相对路径，0 表示绝对路径。
FileName5=
##仅 OpenButton 使用。运行项 5 的文件路径。
Arguments5=
##仅 OpenButton 使用。运行项 5 的启动参数。
WorkingDirectoryMode5=TargetDir
##仅 OpenButton 使用。运行项 5 的工作目录模式。
WorkingDirectory5=
##仅 OpenButton 使用。运行项 5 的自定义工作目录。

[Mode2.Element10]
Show=1
##可选 0 或 1。1 表示显示此元素。
Type=CloseButton
##元素类型。Show=1 时必须选择有效类型。建议在编辑模式中选择。
HeightScale=1
##仅 TextBox、TextFileBox 和 Table 使用。可选 1、2、3。高倍率元素太多时窗口可能超过屏幕高度。
Text=关闭
##显示文字。TextBox 显示长文本，Text 显示一行，最多 512 个字符；按钮显示按钮文字，最多 512 个字符。Input、TextFileBox 和 Table 不使用此项。
Variable=1
##仅 Input 使用。可选 1 到 6，对应 %XHLM2V1% 到 %XHLM2V6%。多个输入框写同一变量时，元素编号靠后的会覆盖前面的值。
Message=这是一条弹窗消息。\n可以用 \n 表示换行。
##仅 MessageButton 使用。必须写在一行里，用 \n 表示实际换行。
RegFileName=
##仅 RegButton 使用。填写存在的 .reg 文件，最终路径必须以 .reg 结尾。
UseRelativePath=1
##仅 RegButton 使用。1=相对路径，0=绝对路径。
WorkingDirectoryMode=TargetDir
##仅 RegButton 使用。可选 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定义工作目录，仅 WorkingDirectoryMode=Custom 时使用。
RegImportingText=
##仅 RegButton 使用。导入中显示在按钮上的文字。留空使用默认文字。
RegSuccessMessage=
##仅 RegButton 使用。导入成功后的提示，留空使用默认提示。
RegFailureMessage=
##仅 RegButton 使用。导入失败后的提示。自定义时建议提醒可能已部分导入。
RegTimeoutMessage=
##仅 RegButton 使用。导入超过 60 秒后的提示。自定义时建议提醒可能已部分导入。
CsvFileName=
##仅 Table 使用。CSV 路径，最终路径必须以 .csv 结尾。最多显示 20 列、1000 行。
CsvUseRelativePath=1
##仅 Table 使用。1=相对路径，0=绝对路径。
TextFileName=
##仅 TextFileBox 使用。文本文件路径。优先按 UTF-8 读取；不是有效 UTF-8 时按系统默认代码页读取。支持 CRLF 和 LF 换行。
TextFileUseRelativePath=1
##仅 TextFileBox 使用。1=相对路径，0=绝对路径。
Run1=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 1。
UseRelativePath1=1
##仅 OpenButton 使用。1 表示 FileName1 是相对路径，0 表示绝对路径。
FileName1=
##仅 OpenButton 使用。运行项 1 的文件路径。
Arguments1=
##仅 OpenButton 使用。运行项 1 的启动参数。
WorkingDirectoryMode1=TargetDir
##仅 OpenButton 使用。运行项 1 的工作目录模式。
WorkingDirectory1=
##仅 OpenButton 使用。运行项 1 的自定义工作目录。
Run2=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 2。
UseRelativePath2=1
##仅 OpenButton 使用。1 表示 FileName2 是相对路径，0 表示绝对路径。
FileName2=
##仅 OpenButton 使用。运行项 2 的文件路径。
Arguments2=
##仅 OpenButton 使用。运行项 2 的启动参数。
WorkingDirectoryMode2=TargetDir
##仅 OpenButton 使用。运行项 2 的工作目录模式。
WorkingDirectory2=
##仅 OpenButton 使用。运行项 2 的自定义工作目录。
Run3=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 3。
UseRelativePath3=1
##仅 OpenButton 使用。1 表示 FileName3 是相对路径，0 表示绝对路径。
FileName3=
##仅 OpenButton 使用。运行项 3 的文件路径。
Arguments3=
##仅 OpenButton 使用。运行项 3 的启动参数。
WorkingDirectoryMode3=TargetDir
##仅 OpenButton 使用。运行项 3 的工作目录模式。
WorkingDirectory3=
##仅 OpenButton 使用。运行项 3 的自定义工作目录。
Run4=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 4。
UseRelativePath4=1
##仅 OpenButton 使用。1 表示 FileName4 是相对路径，0 表示绝对路径。
FileName4=
##仅 OpenButton 使用。运行项 4 的文件路径。
Arguments4=
##仅 OpenButton 使用。运行项 4 的启动参数。
WorkingDirectoryMode4=TargetDir
##仅 OpenButton 使用。运行项 4 的工作目录模式。
WorkingDirectory4=
##仅 OpenButton 使用。运行项 4 的自定义工作目录。
Run5=0
##仅 OpenButton 使用。1 表示点击按钮时执行运行项 5。
UseRelativePath5=1
##仅 OpenButton 使用。1 表示 FileName5 是相对路径，0 表示绝对路径。
FileName5=
##仅 OpenButton 使用。运行项 5 的文件路径。
Arguments5=
##仅 OpenButton 使用。运行项 5 的启动参数。
WorkingDirectoryMode5=TargetDir
##仅 OpenButton 使用。运行项 5 的工作目录模式。
WorkingDirectory5=
##仅 OpenButton 使用。运行项 5 的自定义工作目录。
```

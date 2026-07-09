# 小锤启动器

[English README](README.md) / [繁體中文](README.zh-Hant.md)

## 这是什么

小锤启动器是一个由 INI 配置驱动的 Windows 原生单文件启动器。

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

完整默认模板见：[DEFAULT_TEMPLATE.zh-Hans.txt](DEFAULT_TEMPLATE.zh-Hans.txt)。

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

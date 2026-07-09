# Windows INI 啟動器

[English README](README.md) / [簡體中文](README.zh-Hans.md)

## 功能簡介

把 EXE 放在一個文件夾里，再放一個同名 INI，程序就會按 INI 的內容執行。

它可以：

- 靜默運行，沒有窗口；
- 打開本地文件或程序；
- 導入 `.reg` 註冊表文件；
- 顯示一個原生窗口，里面可以放文本、按鈕、輸入框、文本框、讀取文本文件的文本框和 CSV 表格；
- 顯示本地 PNG 啟動圖；
- 使用變量，讓同一份配置更容易換目錄或換電腦使用。

程序使用常見 Windows API 和系統控件，目標是單文件、少依賴，適用於 Windows 10 和 Windows 11。

## 構建

主源碼：

```text
native/src/XHLauncher.cpp
```

構建命令：

```powershell
powershell -ExecutionPolicy Bypass -File native\build.ps1 -Arch x86
```

預設構建 x86，這樣同一個 EXE 可以運行在 32 位和 64 位 Windows 上。

## 快速開始

1. 把 EXE 放到要使用的文件夾。
2. 運行一次。如果同名 INI 不存在，程序會釋放預設模板。
3. 預設模板會打開編輯模式。
4. 選擇模式，填寫標題、路徑、按鈕、文本等配置。
5. 點擊“保存并關閉”。
6. 再次運行 EXE。

如果 EXE 改名，INI 名也跟著變。例如 `Tools.exe` 會讀取 `Tools.ini`。

## 模式

`Mode=0` 是編輯模式。只修改配置，不打開文件，不導入註冊表。

`Mode=1` 是靜默模式。沒有窗口，沒有成功提示。只讀取 `Mode1.*` 區域，執行后退出。

`Mode=2` 是窗口模式。顯示固定寬度的原生窗口，只讀取 `Mode2.*` 區域。

未知區域和未知配置項會被忽略。隱藏或禁用的項目不會做完整檢查，因為當前運行不會使用它們。

等號右側的內容會按原樣保留。開頭和結尾的空格也屬於配置值。Mode、0/1 開關、Type、語言和數字等控制字段會忽略兩側空格。

## 語言

語言只使用三個標準值：`Language=en-US` 表示英文，`Language=zh-Hans` 表示簡體中文，`Language=zh-Hant` 表示繁體中文。

編輯模式會根據目前語言顯示對應的內建說明。英文是預設說明。

## 編碼和大小

主 INI 支持：

- UTF-8 無 BOM；
- UTF-8 with BOM；
- UTF-16 LE with BOM。

主 INI 不支持 ANSI、GBK 或其他本地代碼頁編碼。

預設模板使用 UTF-8 with BOM，方便舊版 Windows 工具打開。

INI 最大約 4 MB。它只建議保存啟動配置。大段說明請放到單獨文本文件里，再用 TextFileBox 顯示。

如果配置缺失、為空、過大、編碼錯誤，或數字狀態值非法，程序會統一提示配置讀取失敗。

## 路徑

推薦使用 Windows 常見的 `\` 分隔符。程序也接受 `/`，運行時會轉換成 `\`。

相對路徑示例：

```text
App.exe
.\App.exe
Folder\App.exe
Folder/Sub/App.exe
```

相對路徑以 EXE 所在目錄為基準。不能寫盤符、UNC 網絡路徑、根路徑，也不能用 `..` 逃出 EXE 目錄。

絕對路徑示例：

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

`UseRelativePath=1` 表示相對路徑。只替換當前模式變量，不展開 Windows 環境變量，也不展開 `%XHLROOT%`。

`UseRelativePath=0` 表示絕對路徑。支持模式變量、Windows 環境變量和 `%XHLROOT%`。

`%XHLROOT%` 表示當前 EXE 所在文件夾。普通文件夾末尾不帶反斜杠，拼接文件時請自己加，例如 `%XHLROOT%\Tools\App.exe`。

## 變量

模式 1 變量是 `%XHLM1V1%` 到 `%XHLM1V6%`。每個變量都有 `Enable` 和 `Value`。未啟用時，變量標記會按普通文字處理。

模式 2 變量是 `%XHLM2V1%` 到 `%XHLM2V6%`。它們由窗口里的輸入框寫入，只在本次運行中生效，不寫回 INI。某個變量如果沒有對應輸入框，也會按普通文字處理。

多個輸入框寫同一個變量時，元素編號靠後的輸入框會覆蓋前面的值。

變量可以和其他文字連在一起，例如 `ABC%XHLM1V1%123`。變量用於路徑、啟動參數、工作目錄路徑、顯示文字、彈窗內容等運行文本字段，不用於 Mode、Language、Show、Type、Run、UseRelativePath、HeightScale、Variable、WorkingDirectoryMode 等控制字段。

一個變量值、輸入值或普通文本字段大約支持 4095 個字符。

運行時變量替換後的最終文本最多支持 32767 個 UTF-16 字符。路徑、啟動參數、工作目錄、彈窗內容等運行字段超過這個上限時，會被視為無效並跳過或留空。

## 工作目錄

打開文件、啟動程序或導入註冊表時，預設工作目錄是目標文件所在目錄。

可選值：

- `WorkingDirectoryMode=TargetDir`
- `WorkingDirectoryMode=LauncherDir`
- `WorkingDirectoryMode=Custom`

只有選擇 `Custom` 時才會使用 `WorkingDirectory`。最終路徑必須是存在的文件夾。

## 啟動圖

啟動圖由 `[Launcher]` 里的 `Splash*` 配置控制。

規則：

- 只支持 PNG；
- 支持透明通道；
- 文件最大 12 MB；
- 原圖最大 3840x2160；
- 最終渲染尺寸限制在 3840x2160；
- 文件缺失或無效時靜默跳過；
- 沒有動畫和淡入淡出。

圖片會用 Windows Imaging Component 按當前屏幕 DPI 高質量縮放。

## 註冊表導入

模式 1 可以導入一個註冊表文件。模式 2 里有註冊表按鈕。

註冊表文件必須存在，並且必須以 `.reg` 結尾。

註冊表導入最多等待 60 秒。60 秒內完成時按 `reg.exe` 的真實結果判斷成功或失敗；超過 60 秒仍未完成時會停止 `reg.exe`，並按失敗處理。

程序預設使用當前用戶權限運行。HKCU 通常不需要管理員權限。HKLM、HKCR 等系統級位置通常需要右鍵以管理員身份運行 EXE。

模式 1 保持靜默。模式 2 會顯示成功、失敗或超時提示。

## 窗口元素

窗口模式只讀取 `Mode2.Element1` 到 `Mode2.Element10`。顯示順序固定按元素編號排列。

支持的元素類型：

- `Text`：單行文本，最多顯示 512 個字符，超過後會靜默截斷。
- `TextBox`：直接寫在 INI 裡的只讀滾動文本。適合不太長、可以直接放在 INI 裡的說明。
- `TextFileBox`：從本地文本文件讀取的只讀滾動文本。程序會優先按 UTF-8 讀取；如果不是有效 UTF-8，會按系統預設代碼頁讀取。CRLF 和 LF 換行都會正常顯示。文件最大約 4 MB。
- `Input`：輸入框。用戶輸入的內容會在本次運行中寫入一個模式 2 變量。多個輸入框寫同一變量時，元素編號靠後的會覆蓋前面的值。
- `OpenButton`：打開最多五個文件或程序。設置 `MinimizeOnClick=1` 後，點擊時也會最小化窗口。
- `MessageButton`：彈出消息。內容必須寫在 INI 的一行裡，用 `\n` 表示實際換行。設置 `MinimizeOnClick=1` 後，點擊時也會最小化窗口。
- `RegButton`：導入 `.reg` 文件。文件必須存在，最終路徑必須以 `.reg` 結尾。模式 2 會顯示成功、失敗或超時提示。設置 `MinimizeOnClick=1` 後，點擊時也會最小化窗口。
- `Table`：顯示基礎 CSV 文件。第一行是表頭，最多顯示 20 列、1000 行。每個單元格大約顯示 256 個字符。文件最大約 4 MB。支持逗號、規範引號、兩個雙引號轉義、引號內換行；引號不規範時表格顯示為空。不支持排序、篩選、編輯、公式和圖片。
- `CloseButton`：關閉窗口。

`TextBox`、`TextFileBox` 和 `Table` 支持 `HeightScale=1`、`2`、`3`。倍率越高越佔窗口高度。小屏幕或高 DPI 下，太多高倍率元素可能讓窗口超過屏幕高度。

## 常見排查

如果配置無法讀取，請檢查：

- INI 文件名是否和 EXE 文件名對應；
- 文件是否為空；
- 編碼是否是 UTF-8 或 UTF-16 LE with BOM；
- 文件是否超過約 4 MB；
- `Mode` 是否為 0、1、2；
- 數字開關是否為 0 或 1。

如果無法修復，請先備份 INI，再刪除它。程序下次運行會釋放預設配置。

## 預設配置模板

```ini
##預設配置模板
##如果同名 INI 不存在，程序會釋放這份模板。
##預設 Mode=0 打開編輯模式。編輯模式會按當前語言顯示內置說明。
##預設模板使用 UTF-8 with BOM。程序支持 UTF-8 無 BOM、UTF-8 with BOM 和 UTF-16 LE with BOM。
##主 INI 不支持 ANSI、GBK 或其他本地代碼頁。
##注釋行以 # 或 ; 開頭。沒有等號的行會被忽略。未知區域和未知配置項會被忽略。
##等號右側的內容會按原樣保留。開頭和結尾的空格也屬於配置值。控制字段會忽略兩側空格。
##Mode 只能填寫 0、1、2。Enable、Run、Show、UseRelativePath、AddReg、RegImported 等數字開關只能填寫 0 或 1。
##如果配置無法修復，請先備份它，再刪除它。程序下次運行會釋放預設配置。
##相對路徑字段只替換當前模式變量。絕對路徑字段、啟動參數和自定義工作目錄也支持 Windows 環境變量和 %XHLROOT%。
##變量不用於 Mode、Language、Show、Type、Run、UseRelativePath、HeightScale、Variable、WorkingDirectoryMode 等控制字段。
##註冊表導入使用當前用戶權限。HKCU 通常不需要管理員權限。HKLM、HKCR 等系統區域通常需要以管理員身份運行 EXE。

[Launcher]
Title=啟動器
##編輯模式和窗口模式使用的窗口標題，最多 32 個字符。
Version=1.0.3.0
##顯示在標題后面的版本文字，最多 32 個字符，可以填寫任意文字。
Language=zh-Hant
##界面語言。en-US 表示英文，zh-Hans 表示簡體中文，zh-Hant 表示繁體中文，可選值是 en-US、zh-Hans 和 zh-Hant。
Mode=0
##運行模式。0=編輯模式，1=靜默模式，2=窗口模式。
SplashEnable=0
##啟動圖開關。0=關閉，1=啟用。啟用但路徑為空或無效時會靜默跳過。
SplashUseRelativePath=1
##啟動圖路徑類型。1=相對路徑，以 EXE 所在目錄為基準；0=絕對路徑。
SplashImage=
##PNG 啟動圖路徑。只支持 PNG，支持透明通道。
SplashDurationMs=1000
##啟動圖顯示時間，單位毫秒。啟用啟動圖時必須填寫 1 到 60000。
SplashScale=100
##啟動圖縮放比例。啟用啟動圖時必須填寫 10 到 200，不寫百分號。編輯模式用下拉框選擇，每 5% 一個步進。
SplashPosition=Center
##啟動圖位置。啟用啟動圖時必須填寫 Center、TopLeft、TopRight、BottomLeft 或 BottomRight。

[Mode1.Variable1]
Enable=0
##可選 0 或 1。1 表示啟用 %XHLM1V1%。
Value=
##變量值，最多約 4095 個字符。替換後的運行時文本最多 32767 個 UTF-16 字符。

[Mode1.Variable2]
Enable=0
##可選 0 或 1。1 表示啟用 %XHLM1V2%。
Value=
##變量值，最多約 4095 個字符。替換後的運行時文本最多 32767 個 UTF-16 字符。

[Mode1.Variable3]
Enable=0
##可選 0 或 1。1 表示啟用 %XHLM1V3%。
Value=
##變量值，最多約 4095 個字符。替換後的運行時文本最多 32767 個 UTF-16 字符。

[Mode1.Variable4]
Enable=0
##可選 0 或 1。1 表示啟用 %XHLM1V4%。
Value=
##變量值，最多約 4095 個字符。替換後的運行時文本最多 32767 個 UTF-16 字符。

[Mode1.Variable5]
Enable=0
##可選 0 或 1。1 表示啟用 %XHLM1V5%。
Value=
##變量值，最多約 4095 個字符。替換後的運行時文本最多 32767 個 UTF-16 字符。

[Mode1.Variable6]
Enable=0
##可選 0 或 1。1 表示啟用 %XHLM1V6%。
Value=
##變量值，最多約 4095 個字符。替換後的運行時文本最多 32767 個 UTF-16 字符。

[Mode1.Registry]
AddReg=0
##可選 0 或 1。1 表示靜默模式導入 RegFileName 指定的 .reg 文件。
UseRelativePath=1
##註冊表路徑類型。1=相對路徑，0=絕對路徑。
RegFileName=
##註冊表路徑，必須是存在的 .reg 文件。
RegImported=0
##標記為已導入。0=允許導入，1=跳過導入。成功導入後程序會寫成 1。
WorkingDirectoryMode=TargetDir
##註冊表導入的工作目錄。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用，最終必須是存在的文件夾。

[Mode1.Run1]
Run=0
##可選 0 或 1。1 表示靜默模式執行運行項 1。
UseRelativePath=1
##路徑類型。1=相對路徑，0=絕對路徑。
FileName=
##文件路徑，必須是存在的文件。文件夾路徑會跳過。
Arguments=
##啟動參數，主要用於程序。支持模式 1 變量、Windows 環境變量和 %XHLROOT%。
WorkingDirectoryMode=TargetDir
##工作目錄模式。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用，最終必須是存在的文件夾。

[Mode1.Run2]
Run=0
##可選 0 或 1。1 表示靜默模式執行運行項 2。
UseRelativePath=1
##路徑類型。1=相對路徑，0=絕對路徑。
FileName=
##文件路徑，必須是存在的文件。文件夾路徑會跳過。
Arguments=
##啟動參數，主要用於程序。支持模式 1 變量、Windows 環境變量和 %XHLROOT%。
WorkingDirectoryMode=TargetDir
##工作目錄模式。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用，最終必須是存在的文件夾。

[Mode1.Run3]
Run=0
##可選 0 或 1。1 表示靜默模式執行運行項 3。
UseRelativePath=1
##路徑類型。1=相對路徑，0=絕對路徑。
FileName=
##文件路徑，必須是存在的文件。文件夾路徑會跳過。
Arguments=
##啟動參數，主要用於程序。支持模式 1 變量、Windows 環境變量和 %XHLROOT%。
WorkingDirectoryMode=TargetDir
##工作目錄模式。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用，最終必須是存在的文件夾。

[Mode1.Run4]
Run=0
##可選 0 或 1。1 表示靜默模式執行運行項 4。
UseRelativePath=1
##路徑類型。1=相對路徑，0=絕對路徑。
FileName=
##文件路徑，必須是存在的文件。文件夾路徑會跳過。
Arguments=
##啟動參數，主要用於程序。支持模式 1 變量、Windows 環境變量和 %XHLROOT%。
WorkingDirectoryMode=TargetDir
##工作目錄模式。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用，最終必須是存在的文件夾。

[Mode1.Run5]
Run=0
##可選 0 或 1。1 表示靜默模式執行運行項 5。
UseRelativePath=1
##路徑類型。1=相對路徑，0=絕對路徑。
FileName=
##文件路徑，必須是存在的文件。文件夾路徑會跳過。
Arguments=
##啟動參數，主要用於程序。支持模式 1 變量、Windows 環境變量和 %XHLROOT%。
WorkingDirectoryMode=TargetDir
##工作目錄模式。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用，最終必須是存在的文件夾。

##窗口模式變量由輸入框元素寫入。Variable=1 到 6 對應 %XHLM2V1% 到 %XHLM2V6%。替換後的運行時文本最多 32767 個 UTF-16 字符。
##只讀取 Element1 到 Element10。顯示順序固定為 Element1 到 Element10。Element11 和更高編號會被忽略。
##支持的 Type 值：Text、TextBox、TextFileBox、Input、OpenButton、MessageButton、RegButton、Table、CloseButton。
##表格只支持基礎 CSV。第一行是表頭。引號格式不正確時表格會顯示為空。

[Mode2.Element1]
Show=1
##可選 0 或 1。1 表示顯示此元素。
Type=TextBox
##元素類型。Show=1 時必須選擇有效類型。建議在編輯模式中選擇。
HeightScale=1
##僅 TextBox、TextFileBox 和 Table 使用。可選 1、2、3。高倍率元素太多時窗口可能超過屏幕高度。
Text=歡迎使用。\n請在編輯模式中修改這份模板。
##顯示文字。TextBox 顯示長文本，Text 顯示一行，最多 512 個字符；按鈕顯示按鈕文字，最多 512 個字符。Input、TextFileBox 和 Table 不使用此項。
Variable=1
##僅 Input 使用。可選 1 到 6，對應 %XHLM2V1% 到 %XHLM2V6%。多個輸入框寫同一變量時，元素編號靠後的會覆蓋前面的值。
Message=這是一條彈窗消息。\n可以用 \n 表示換行。
##僅 MessageButton 使用。必須寫在一行里，用 \n 表示實際換行。
RegFileName=
##僅 RegButton 使用。填寫存在的 .reg 文件，最終路徑必須以 .reg 結尾。
UseRelativePath=1
##僅 RegButton 使用。1=相對路徑，0=絕對路徑。
WorkingDirectoryMode=TargetDir
##僅 RegButton 使用。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用。
RegImportingText=
##僅 RegButton 使用。導入中顯示在按鈕上的文字。留空使用預設文字。
RegSuccessMessage=
##僅 RegButton 使用。導入成功後的提示，留空使用預設提示。
RegFailureMessage=
##僅 RegButton 使用。導入失敗後的提示。自定義時建議提醒可能已部分導入。
RegTimeoutMessage=
##僅 RegButton 使用。導入超過 60 秒後的提示。自定義時建議提醒可能已部分導入。
CsvFileName=
##僅 Table 使用。CSV 路徑，最終路徑必須以 .csv 結尾。最多顯示 20 列、1000 行。
CsvUseRelativePath=1
##僅 Table 使用。1=相對路徑，0=絕對路徑。
TextFileName=
##僅 TextFileBox 使用。文本文件路徑。優先按 UTF-8 讀取；不是有效 UTF-8 時按系統預設代碼頁讀取。支持 CRLF 和 LF 換行。
TextFileUseRelativePath=1
##僅 TextFileBox 使用。1=相對路徑，0=絕對路徑。
Run1=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 1。
UseRelativePath1=1
##僅 OpenButton 使用。1 表示 FileName1 是相對路徑，0 表示絕對路徑。
FileName1=
##僅 OpenButton 使用。運行項 1 的文件路徑。
Arguments1=
##僅 OpenButton 使用。運行項 1 的啟動參數。
WorkingDirectoryMode1=TargetDir
##僅 OpenButton 使用。運行項 1 的工作目錄模式。
WorkingDirectory1=
##僅 OpenButton 使用。運行項 1 的自定義工作目錄。
Run2=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 2。
UseRelativePath2=1
##僅 OpenButton 使用。1 表示 FileName2 是相對路徑，0 表示絕對路徑。
FileName2=
##僅 OpenButton 使用。運行項 2 的文件路徑。
Arguments2=
##僅 OpenButton 使用。運行項 2 的啟動參數。
WorkingDirectoryMode2=TargetDir
##僅 OpenButton 使用。運行項 2 的工作目錄模式。
WorkingDirectory2=
##僅 OpenButton 使用。運行項 2 的自定義工作目錄。
Run3=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 3。
UseRelativePath3=1
##僅 OpenButton 使用。1 表示 FileName3 是相對路徑，0 表示絕對路徑。
FileName3=
##僅 OpenButton 使用。運行項 3 的文件路徑。
Arguments3=
##僅 OpenButton 使用。運行項 3 的啟動參數。
WorkingDirectoryMode3=TargetDir
##僅 OpenButton 使用。運行項 3 的工作目錄模式。
WorkingDirectory3=
##僅 OpenButton 使用。運行項 3 的自定義工作目錄。
Run4=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 4。
UseRelativePath4=1
##僅 OpenButton 使用。1 表示 FileName4 是相對路徑，0 表示絕對路徑。
FileName4=
##僅 OpenButton 使用。運行項 4 的文件路徑。
Arguments4=
##僅 OpenButton 使用。運行項 4 的啟動參數。
WorkingDirectoryMode4=TargetDir
##僅 OpenButton 使用。運行項 4 的工作目錄模式。
WorkingDirectory4=
##僅 OpenButton 使用。運行項 4 的自定義工作目錄。
Run5=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 5。
UseRelativePath5=1
##僅 OpenButton 使用。1 表示 FileName5 是相對路徑，0 表示絕對路徑。
FileName5=
##僅 OpenButton 使用。運行項 5 的文件路徑。
Arguments5=
##僅 OpenButton 使用。運行項 5 的啟動參數。
WorkingDirectoryMode5=TargetDir
##僅 OpenButton 使用。運行項 5 的工作目錄模式。
WorkingDirectory5=
##僅 OpenButton 使用。運行項 5 的自定義工作目錄。

[Mode2.Element2]
Show=1
##可選 0 或 1。1 表示顯示此元素。
Type=Text
##元素類型。Show=1 時必須選擇有效類型。建議在編輯模式中選擇。
HeightScale=1
##僅 TextBox、TextFileBox 和 Table 使用。可選 1、2、3。高倍率元素太多時窗口可能超過屏幕高度。
Text=單行文本示例
##顯示文字。TextBox 顯示長文本，Text 顯示一行，最多 512 個字符；按鈕顯示按鈕文字，最多 512 個字符。Input、TextFileBox 和 Table 不使用此項。
Variable=1
##僅 Input 使用。可選 1 到 6，對應 %XHLM2V1% 到 %XHLM2V6%。多個輸入框寫同一變量時，元素編號靠後的會覆蓋前面的值。
Message=這是一條彈窗消息。\n可以用 \n 表示換行。
##僅 MessageButton 使用。必須寫在一行里，用 \n 表示實際換行。
RegFileName=
##僅 RegButton 使用。填寫存在的 .reg 文件，最終路徑必須以 .reg 結尾。
UseRelativePath=1
##僅 RegButton 使用。1=相對路徑，0=絕對路徑。
WorkingDirectoryMode=TargetDir
##僅 RegButton 使用。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用。
RegImportingText=
##僅 RegButton 使用。導入中顯示在按鈕上的文字。留空使用預設文字。
RegSuccessMessage=
##僅 RegButton 使用。導入成功後的提示，留空使用預設提示。
RegFailureMessage=
##僅 RegButton 使用。導入失敗後的提示。自定義時建議提醒可能已部分導入。
RegTimeoutMessage=
##僅 RegButton 使用。導入超過 60 秒後的提示。自定義時建議提醒可能已部分導入。
CsvFileName=
##僅 Table 使用。CSV 路徑，最終路徑必須以 .csv 結尾。最多顯示 20 列、1000 行。
CsvUseRelativePath=1
##僅 Table 使用。1=相對路徑，0=絕對路徑。
TextFileName=
##僅 TextFileBox 使用。文本文件路徑。優先按 UTF-8 讀取；不是有效 UTF-8 時按系統預設代碼頁讀取。支持 CRLF 和 LF 換行。
TextFileUseRelativePath=1
##僅 TextFileBox 使用。1=相對路徑，0=絕對路徑。
Run1=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 1。
UseRelativePath1=1
##僅 OpenButton 使用。1 表示 FileName1 是相對路徑，0 表示絕對路徑。
FileName1=
##僅 OpenButton 使用。運行項 1 的文件路徑。
Arguments1=
##僅 OpenButton 使用。運行項 1 的啟動參數。
WorkingDirectoryMode1=TargetDir
##僅 OpenButton 使用。運行項 1 的工作目錄模式。
WorkingDirectory1=
##僅 OpenButton 使用。運行項 1 的自定義工作目錄。
Run2=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 2。
UseRelativePath2=1
##僅 OpenButton 使用。1 表示 FileName2 是相對路徑，0 表示絕對路徑。
FileName2=
##僅 OpenButton 使用。運行項 2 的文件路徑。
Arguments2=
##僅 OpenButton 使用。運行項 2 的啟動參數。
WorkingDirectoryMode2=TargetDir
##僅 OpenButton 使用。運行項 2 的工作目錄模式。
WorkingDirectory2=
##僅 OpenButton 使用。運行項 2 的自定義工作目錄。
Run3=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 3。
UseRelativePath3=1
##僅 OpenButton 使用。1 表示 FileName3 是相對路徑，0 表示絕對路徑。
FileName3=
##僅 OpenButton 使用。運行項 3 的文件路徑。
Arguments3=
##僅 OpenButton 使用。運行項 3 的啟動參數。
WorkingDirectoryMode3=TargetDir
##僅 OpenButton 使用。運行項 3 的工作目錄模式。
WorkingDirectory3=
##僅 OpenButton 使用。運行項 3 的自定義工作目錄。
Run4=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 4。
UseRelativePath4=1
##僅 OpenButton 使用。1 表示 FileName4 是相對路徑，0 表示絕對路徑。
FileName4=
##僅 OpenButton 使用。運行項 4 的文件路徑。
Arguments4=
##僅 OpenButton 使用。運行項 4 的啟動參數。
WorkingDirectoryMode4=TargetDir
##僅 OpenButton 使用。運行項 4 的工作目錄模式。
WorkingDirectory4=
##僅 OpenButton 使用。運行項 4 的自定義工作目錄。
Run5=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 5。
UseRelativePath5=1
##僅 OpenButton 使用。1 表示 FileName5 是相對路徑，0 表示絕對路徑。
FileName5=
##僅 OpenButton 使用。運行項 5 的文件路徑。
Arguments5=
##僅 OpenButton 使用。運行項 5 的啟動參數。
WorkingDirectoryMode5=TargetDir
##僅 OpenButton 使用。運行項 5 的工作目錄模式。
WorkingDirectory5=
##僅 OpenButton 使用。運行項 5 的自定義工作目錄。

[Mode2.Element3]
Show=1
##可選 0 或 1。1 表示顯示此元素。
Type=Input
##元素類型。Show=1 時必須選擇有效類型。建議在編輯模式中選擇。
HeightScale=1
##僅 TextBox、TextFileBox 和 Table 使用。可選 1、2、3。高倍率元素太多時窗口可能超過屏幕高度。
Text=
##顯示文字。TextBox 顯示長文本，Text 顯示一行，最多 512 個字符；按鈕顯示按鈕文字，最多 512 個字符。Input、TextFileBox 和 Table 不使用此項。
Variable=1
##僅 Input 使用。可選 1 到 6，對應 %XHLM2V1% 到 %XHLM2V6%。多個輸入框寫同一變量時，元素編號靠後的會覆蓋前面的值。
Message=這是一條彈窗消息。\n可以用 \n 表示換行。
##僅 MessageButton 使用。必須寫在一行里，用 \n 表示實際換行。
RegFileName=
##僅 RegButton 使用。填寫存在的 .reg 文件，最終路徑必須以 .reg 結尾。
UseRelativePath=1
##僅 RegButton 使用。1=相對路徑，0=絕對路徑。
WorkingDirectoryMode=TargetDir
##僅 RegButton 使用。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用。
RegImportingText=
##僅 RegButton 使用。導入中顯示在按鈕上的文字。留空使用預設文字。
RegSuccessMessage=
##僅 RegButton 使用。導入成功後的提示，留空使用預設提示。
RegFailureMessage=
##僅 RegButton 使用。導入失敗後的提示。自定義時建議提醒可能已部分導入。
RegTimeoutMessage=
##僅 RegButton 使用。導入超過 60 秒後的提示。自定義時建議提醒可能已部分導入。
CsvFileName=
##僅 Table 使用。CSV 路徑，最終路徑必須以 .csv 結尾。最多顯示 20 列、1000 行。
CsvUseRelativePath=1
##僅 Table 使用。1=相對路徑，0=絕對路徑。
TextFileName=
##僅 TextFileBox 使用。文本文件路徑。優先按 UTF-8 讀取；不是有效 UTF-8 時按系統預設代碼頁讀取。支持 CRLF 和 LF 換行。
TextFileUseRelativePath=1
##僅 TextFileBox 使用。1=相對路徑，0=絕對路徑。
Run1=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 1。
UseRelativePath1=1
##僅 OpenButton 使用。1 表示 FileName1 是相對路徑，0 表示絕對路徑。
FileName1=
##僅 OpenButton 使用。運行項 1 的文件路徑。
Arguments1=
##僅 OpenButton 使用。運行項 1 的啟動參數。
WorkingDirectoryMode1=TargetDir
##僅 OpenButton 使用。運行項 1 的工作目錄模式。
WorkingDirectory1=
##僅 OpenButton 使用。運行項 1 的自定義工作目錄。
Run2=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 2。
UseRelativePath2=1
##僅 OpenButton 使用。1 表示 FileName2 是相對路徑，0 表示絕對路徑。
FileName2=
##僅 OpenButton 使用。運行項 2 的文件路徑。
Arguments2=
##僅 OpenButton 使用。運行項 2 的啟動參數。
WorkingDirectoryMode2=TargetDir
##僅 OpenButton 使用。運行項 2 的工作目錄模式。
WorkingDirectory2=
##僅 OpenButton 使用。運行項 2 的自定義工作目錄。
Run3=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 3。
UseRelativePath3=1
##僅 OpenButton 使用。1 表示 FileName3 是相對路徑，0 表示絕對路徑。
FileName3=
##僅 OpenButton 使用。運行項 3 的文件路徑。
Arguments3=
##僅 OpenButton 使用。運行項 3 的啟動參數。
WorkingDirectoryMode3=TargetDir
##僅 OpenButton 使用。運行項 3 的工作目錄模式。
WorkingDirectory3=
##僅 OpenButton 使用。運行項 3 的自定義工作目錄。
Run4=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 4。
UseRelativePath4=1
##僅 OpenButton 使用。1 表示 FileName4 是相對路徑，0 表示絕對路徑。
FileName4=
##僅 OpenButton 使用。運行項 4 的文件路徑。
Arguments4=
##僅 OpenButton 使用。運行項 4 的啟動參數。
WorkingDirectoryMode4=TargetDir
##僅 OpenButton 使用。運行項 4 的工作目錄模式。
WorkingDirectory4=
##僅 OpenButton 使用。運行項 4 的自定義工作目錄。
Run5=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 5。
UseRelativePath5=1
##僅 OpenButton 使用。1 表示 FileName5 是相對路徑，0 表示絕對路徑。
FileName5=
##僅 OpenButton 使用。運行項 5 的文件路徑。
Arguments5=
##僅 OpenButton 使用。運行項 5 的啟動參數。
WorkingDirectoryMode5=TargetDir
##僅 OpenButton 使用。運行項 5 的工作目錄模式。
WorkingDirectory5=
##僅 OpenButton 使用。運行項 5 的自定義工作目錄。

[Mode2.Element4]
Show=1
##可選 0 或 1。1 表示顯示此元素。
Type=OpenButton
##元素類型。Show=1 時必須選擇有效類型。建議在編輯模式中選擇。
HeightScale=1
##僅 TextBox、TextFileBox 和 Table 使用。可選 1、2、3。高倍率元素太多時窗口可能超過屏幕高度。
Text=打開文件
##顯示文字。TextBox 顯示長文本，Text 顯示一行，最多 512 個字符；按鈕顯示按鈕文字，最多 512 個字符。Input、TextFileBox 和 Table 不使用此項。
Variable=1
##僅 Input 使用。可選 1 到 6，對應 %XHLM2V1% 到 %XHLM2V6%。多個輸入框寫同一變量時，元素編號靠後的會覆蓋前面的值。
Message=這是一條彈窗消息。\n可以用 \n 表示換行。
##僅 MessageButton 使用。必須寫在一行里，用 \n 表示實際換行。
RegFileName=
##僅 RegButton 使用。填寫存在的 .reg 文件，最終路徑必須以 .reg 結尾。
UseRelativePath=1
##僅 RegButton 使用。1=相對路徑，0=絕對路徑。
WorkingDirectoryMode=TargetDir
##僅 RegButton 使用。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用。
RegImportingText=
##僅 RegButton 使用。導入中顯示在按鈕上的文字。留空使用預設文字。
RegSuccessMessage=
##僅 RegButton 使用。導入成功後的提示，留空使用預設提示。
RegFailureMessage=
##僅 RegButton 使用。導入失敗後的提示。自定義時建議提醒可能已部分導入。
RegTimeoutMessage=
##僅 RegButton 使用。導入超過 60 秒後的提示。自定義時建議提醒可能已部分導入。
CsvFileName=
##僅 Table 使用。CSV 路徑，最終路徑必須以 .csv 結尾。最多顯示 20 列、1000 行。
CsvUseRelativePath=1
##僅 Table 使用。1=相對路徑，0=絕對路徑。
TextFileName=
##僅 TextFileBox 使用。文本文件路徑。優先按 UTF-8 讀取；不是有效 UTF-8 時按系統預設代碼頁讀取。支持 CRLF 和 LF 換行。
TextFileUseRelativePath=1
##僅 TextFileBox 使用。1=相對路徑，0=絕對路徑。
Run1=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 1。
UseRelativePath1=1
##僅 OpenButton 使用。1 表示 FileName1 是相對路徑，0 表示絕對路徑。
FileName1=
##僅 OpenButton 使用。運行項 1 的文件路徑。
Arguments1=
##僅 OpenButton 使用。運行項 1 的啟動參數。
WorkingDirectoryMode1=TargetDir
##僅 OpenButton 使用。運行項 1 的工作目錄模式。
WorkingDirectory1=
##僅 OpenButton 使用。運行項 1 的自定義工作目錄。
Run2=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 2。
UseRelativePath2=1
##僅 OpenButton 使用。1 表示 FileName2 是相對路徑，0 表示絕對路徑。
FileName2=
##僅 OpenButton 使用。運行項 2 的文件路徑。
Arguments2=
##僅 OpenButton 使用。運行項 2 的啟動參數。
WorkingDirectoryMode2=TargetDir
##僅 OpenButton 使用。運行項 2 的工作目錄模式。
WorkingDirectory2=
##僅 OpenButton 使用。運行項 2 的自定義工作目錄。
Run3=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 3。
UseRelativePath3=1
##僅 OpenButton 使用。1 表示 FileName3 是相對路徑，0 表示絕對路徑。
FileName3=
##僅 OpenButton 使用。運行項 3 的文件路徑。
Arguments3=
##僅 OpenButton 使用。運行項 3 的啟動參數。
WorkingDirectoryMode3=TargetDir
##僅 OpenButton 使用。運行項 3 的工作目錄模式。
WorkingDirectory3=
##僅 OpenButton 使用。運行項 3 的自定義工作目錄。
Run4=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 4。
UseRelativePath4=1
##僅 OpenButton 使用。1 表示 FileName4 是相對路徑，0 表示絕對路徑。
FileName4=
##僅 OpenButton 使用。運行項 4 的文件路徑。
Arguments4=
##僅 OpenButton 使用。運行項 4 的啟動參數。
WorkingDirectoryMode4=TargetDir
##僅 OpenButton 使用。運行項 4 的工作目錄模式。
WorkingDirectory4=
##僅 OpenButton 使用。運行項 4 的自定義工作目錄。
Run5=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 5。
UseRelativePath5=1
##僅 OpenButton 使用。1 表示 FileName5 是相對路徑，0 表示絕對路徑。
FileName5=
##僅 OpenButton 使用。運行項 5 的文件路徑。
Arguments5=
##僅 OpenButton 使用。運行項 5 的啟動參數。
WorkingDirectoryMode5=TargetDir
##僅 OpenButton 使用。運行項 5 的工作目錄模式。
WorkingDirectory5=
##僅 OpenButton 使用。運行項 5 的自定義工作目錄。

[Mode2.Element5]
Show=1
##可選 0 或 1。1 表示顯示此元素。
Type=MessageButton
##元素類型。Show=1 時必須選擇有效類型。建議在編輯模式中選擇。
HeightScale=1
##僅 TextBox、TextFileBox 和 Table 使用。可選 1、2、3。高倍率元素太多時窗口可能超過屏幕高度。
Text=彈窗
##顯示文字。TextBox 顯示長文本，Text 顯示一行，最多 512 個字符；按鈕顯示按鈕文字，最多 512 個字符。Input、TextFileBox 和 Table 不使用此項。
Variable=1
##僅 Input 使用。可選 1 到 6，對應 %XHLM2V1% 到 %XHLM2V6%。多個輸入框寫同一變量時，元素編號靠後的會覆蓋前面的值。
Message=這是一條彈窗消息。\n可以用 \n 表示換行。
##僅 MessageButton 使用。必須寫在一行里，用 \n 表示實際換行。
RegFileName=
##僅 RegButton 使用。填寫存在的 .reg 文件，最終路徑必須以 .reg 結尾。
UseRelativePath=1
##僅 RegButton 使用。1=相對路徑，0=絕對路徑。
WorkingDirectoryMode=TargetDir
##僅 RegButton 使用。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用。
RegImportingText=
##僅 RegButton 使用。導入中顯示在按鈕上的文字。留空使用預設文字。
RegSuccessMessage=
##僅 RegButton 使用。導入成功後的提示，留空使用預設提示。
RegFailureMessage=
##僅 RegButton 使用。導入失敗後的提示。自定義時建議提醒可能已部分導入。
RegTimeoutMessage=
##僅 RegButton 使用。導入超過 60 秒後的提示。自定義時建議提醒可能已部分導入。
CsvFileName=
##僅 Table 使用。CSV 路徑，最終路徑必須以 .csv 結尾。最多顯示 20 列、1000 行。
CsvUseRelativePath=1
##僅 Table 使用。1=相對路徑，0=絕對路徑。
TextFileName=
##僅 TextFileBox 使用。文本文件路徑。優先按 UTF-8 讀取；不是有效 UTF-8 時按系統預設代碼頁讀取。支持 CRLF 和 LF 換行。
TextFileUseRelativePath=1
##僅 TextFileBox 使用。1=相對路徑，0=絕對路徑。
Run1=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 1。
UseRelativePath1=1
##僅 OpenButton 使用。1 表示 FileName1 是相對路徑，0 表示絕對路徑。
FileName1=
##僅 OpenButton 使用。運行項 1 的文件路徑。
Arguments1=
##僅 OpenButton 使用。運行項 1 的啟動參數。
WorkingDirectoryMode1=TargetDir
##僅 OpenButton 使用。運行項 1 的工作目錄模式。
WorkingDirectory1=
##僅 OpenButton 使用。運行項 1 的自定義工作目錄。
Run2=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 2。
UseRelativePath2=1
##僅 OpenButton 使用。1 表示 FileName2 是相對路徑，0 表示絕對路徑。
FileName2=
##僅 OpenButton 使用。運行項 2 的文件路徑。
Arguments2=
##僅 OpenButton 使用。運行項 2 的啟動參數。
WorkingDirectoryMode2=TargetDir
##僅 OpenButton 使用。運行項 2 的工作目錄模式。
WorkingDirectory2=
##僅 OpenButton 使用。運行項 2 的自定義工作目錄。
Run3=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 3。
UseRelativePath3=1
##僅 OpenButton 使用。1 表示 FileName3 是相對路徑，0 表示絕對路徑。
FileName3=
##僅 OpenButton 使用。運行項 3 的文件路徑。
Arguments3=
##僅 OpenButton 使用。運行項 3 的啟動參數。
WorkingDirectoryMode3=TargetDir
##僅 OpenButton 使用。運行項 3 的工作目錄模式。
WorkingDirectory3=
##僅 OpenButton 使用。運行項 3 的自定義工作目錄。
Run4=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 4。
UseRelativePath4=1
##僅 OpenButton 使用。1 表示 FileName4 是相對路徑，0 表示絕對路徑。
FileName4=
##僅 OpenButton 使用。運行項 4 的文件路徑。
Arguments4=
##僅 OpenButton 使用。運行項 4 的啟動參數。
WorkingDirectoryMode4=TargetDir
##僅 OpenButton 使用。運行項 4 的工作目錄模式。
WorkingDirectory4=
##僅 OpenButton 使用。運行項 4 的自定義工作目錄。
Run5=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 5。
UseRelativePath5=1
##僅 OpenButton 使用。1 表示 FileName5 是相對路徑，0 表示絕對路徑。
FileName5=
##僅 OpenButton 使用。運行項 5 的文件路徑。
Arguments5=
##僅 OpenButton 使用。運行項 5 的啟動參數。
WorkingDirectoryMode5=TargetDir
##僅 OpenButton 使用。運行項 5 的工作目錄模式。
WorkingDirectory5=
##僅 OpenButton 使用。運行項 5 的自定義工作目錄。

[Mode2.Element6]
Show=1
##可選 0 或 1。1 表示顯示此元素。
Type=RegButton
##元素類型。Show=1 時必須選擇有效類型。建議在編輯模式中選擇。
HeightScale=1
##僅 TextBox、TextFileBox 和 Table 使用。可選 1、2、3。高倍率元素太多時窗口可能超過屏幕高度。
Text=導入註冊表
##顯示文字。TextBox 顯示長文本，Text 顯示一行，最多 512 個字符；按鈕顯示按鈕文字，最多 512 個字符。Input、TextFileBox 和 Table 不使用此項。
Variable=1
##僅 Input 使用。可選 1 到 6，對應 %XHLM2V1% 到 %XHLM2V6%。多個輸入框寫同一變量時，元素編號靠後的會覆蓋前面的值。
Message=這是一條彈窗消息。\n可以用 \n 表示換行。
##僅 MessageButton 使用。必須寫在一行里，用 \n 表示實際換行。
RegFileName=
##僅 RegButton 使用。填寫存在的 .reg 文件，最終路徑必須以 .reg 結尾。
UseRelativePath=1
##僅 RegButton 使用。1=相對路徑，0=絕對路徑。
WorkingDirectoryMode=TargetDir
##僅 RegButton 使用。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用。
RegImportingText=
##僅 RegButton 使用。導入中顯示在按鈕上的文字。留空使用預設文字。
RegSuccessMessage=
##僅 RegButton 使用。導入成功後的提示，留空使用預設提示。
RegFailureMessage=
##僅 RegButton 使用。導入失敗後的提示。自定義時建議提醒可能已部分導入。
RegTimeoutMessage=
##僅 RegButton 使用。導入超過 60 秒後的提示。自定義時建議提醒可能已部分導入。
CsvFileName=
##僅 Table 使用。CSV 路徑，最終路徑必須以 .csv 結尾。最多顯示 20 列、1000 行。
CsvUseRelativePath=1
##僅 Table 使用。1=相對路徑，0=絕對路徑。
TextFileName=
##僅 TextFileBox 使用。文本文件路徑。優先按 UTF-8 讀取；不是有效 UTF-8 時按系統預設代碼頁讀取。支持 CRLF 和 LF 換行。
TextFileUseRelativePath=1
##僅 TextFileBox 使用。1=相對路徑，0=絕對路徑。
Run1=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 1。
UseRelativePath1=1
##僅 OpenButton 使用。1 表示 FileName1 是相對路徑，0 表示絕對路徑。
FileName1=
##僅 OpenButton 使用。運行項 1 的文件路徑。
Arguments1=
##僅 OpenButton 使用。運行項 1 的啟動參數。
WorkingDirectoryMode1=TargetDir
##僅 OpenButton 使用。運行項 1 的工作目錄模式。
WorkingDirectory1=
##僅 OpenButton 使用。運行項 1 的自定義工作目錄。
Run2=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 2。
UseRelativePath2=1
##僅 OpenButton 使用。1 表示 FileName2 是相對路徑，0 表示絕對路徑。
FileName2=
##僅 OpenButton 使用。運行項 2 的文件路徑。
Arguments2=
##僅 OpenButton 使用。運行項 2 的啟動參數。
WorkingDirectoryMode2=TargetDir
##僅 OpenButton 使用。運行項 2 的工作目錄模式。
WorkingDirectory2=
##僅 OpenButton 使用。運行項 2 的自定義工作目錄。
Run3=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 3。
UseRelativePath3=1
##僅 OpenButton 使用。1 表示 FileName3 是相對路徑，0 表示絕對路徑。
FileName3=
##僅 OpenButton 使用。運行項 3 的文件路徑。
Arguments3=
##僅 OpenButton 使用。運行項 3 的啟動參數。
WorkingDirectoryMode3=TargetDir
##僅 OpenButton 使用。運行項 3 的工作目錄模式。
WorkingDirectory3=
##僅 OpenButton 使用。運行項 3 的自定義工作目錄。
Run4=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 4。
UseRelativePath4=1
##僅 OpenButton 使用。1 表示 FileName4 是相對路徑，0 表示絕對路徑。
FileName4=
##僅 OpenButton 使用。運行項 4 的文件路徑。
Arguments4=
##僅 OpenButton 使用。運行項 4 的啟動參數。
WorkingDirectoryMode4=TargetDir
##僅 OpenButton 使用。運行項 4 的工作目錄模式。
WorkingDirectory4=
##僅 OpenButton 使用。運行項 4 的自定義工作目錄。
Run5=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 5。
UseRelativePath5=1
##僅 OpenButton 使用。1 表示 FileName5 是相對路徑，0 表示絕對路徑。
FileName5=
##僅 OpenButton 使用。運行項 5 的文件路徑。
Arguments5=
##僅 OpenButton 使用。運行項 5 的啟動參數。
WorkingDirectoryMode5=TargetDir
##僅 OpenButton 使用。運行項 5 的工作目錄模式。
WorkingDirectory5=
##僅 OpenButton 使用。運行項 5 的自定義工作目錄。

[Mode2.Element7]
Show=1
##可選 0 或 1。1 表示顯示此元素。
Type=Table
##元素類型。Show=1 時必須選擇有效類型。建議在編輯模式中選擇。
HeightScale=1
##僅 TextBox、TextFileBox 和 Table 使用。可選 1、2、3。高倍率元素太多時窗口可能超過屏幕高度。
Text=
##顯示文字。TextBox 顯示長文本，Text 顯示一行，最多 512 個字符；按鈕顯示按鈕文字，最多 512 個字符。Input、TextFileBox 和 Table 不使用此項。
Variable=1
##僅 Input 使用。可選 1 到 6，對應 %XHLM2V1% 到 %XHLM2V6%。多個輸入框寫同一變量時，元素編號靠後的會覆蓋前面的值。
Message=這是一條彈窗消息。\n可以用 \n 表示換行。
##僅 MessageButton 使用。必須寫在一行里，用 \n 表示實際換行。
RegFileName=
##僅 RegButton 使用。填寫存在的 .reg 文件，最終路徑必須以 .reg 結尾。
UseRelativePath=1
##僅 RegButton 使用。1=相對路徑，0=絕對路徑。
WorkingDirectoryMode=TargetDir
##僅 RegButton 使用。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用。
RegImportingText=
##僅 RegButton 使用。導入中顯示在按鈕上的文字。留空使用預設文字。
RegSuccessMessage=
##僅 RegButton 使用。導入成功後的提示，留空使用預設提示。
RegFailureMessage=
##僅 RegButton 使用。導入失敗後的提示。自定義時建議提醒可能已部分導入。
RegTimeoutMessage=
##僅 RegButton 使用。導入超過 60 秒後的提示。自定義時建議提醒可能已部分導入。
CsvFileName=Example.csv
##僅 Table 使用。CSV 路徑，最終路徑必須以 .csv 結尾。最多顯示 20 列、1000 行。
CsvUseRelativePath=1
##僅 Table 使用。1=相對路徑，0=絕對路徑。
TextFileName=
##僅 TextFileBox 使用。文本文件路徑。優先按 UTF-8 讀取；不是有效 UTF-8 時按系統預設代碼頁讀取。支持 CRLF 和 LF 換行。
TextFileUseRelativePath=1
##僅 TextFileBox 使用。1=相對路徑，0=絕對路徑。
Run1=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 1。
UseRelativePath1=1
##僅 OpenButton 使用。1 表示 FileName1 是相對路徑，0 表示絕對路徑。
FileName1=
##僅 OpenButton 使用。運行項 1 的文件路徑。
Arguments1=
##僅 OpenButton 使用。運行項 1 的啟動參數。
WorkingDirectoryMode1=TargetDir
##僅 OpenButton 使用。運行項 1 的工作目錄模式。
WorkingDirectory1=
##僅 OpenButton 使用。運行項 1 的自定義工作目錄。
Run2=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 2。
UseRelativePath2=1
##僅 OpenButton 使用。1 表示 FileName2 是相對路徑，0 表示絕對路徑。
FileName2=
##僅 OpenButton 使用。運行項 2 的文件路徑。
Arguments2=
##僅 OpenButton 使用。運行項 2 的啟動參數。
WorkingDirectoryMode2=TargetDir
##僅 OpenButton 使用。運行項 2 的工作目錄模式。
WorkingDirectory2=
##僅 OpenButton 使用。運行項 2 的自定義工作目錄。
Run3=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 3。
UseRelativePath3=1
##僅 OpenButton 使用。1 表示 FileName3 是相對路徑，0 表示絕對路徑。
FileName3=
##僅 OpenButton 使用。運行項 3 的文件路徑。
Arguments3=
##僅 OpenButton 使用。運行項 3 的啟動參數。
WorkingDirectoryMode3=TargetDir
##僅 OpenButton 使用。運行項 3 的工作目錄模式。
WorkingDirectory3=
##僅 OpenButton 使用。運行項 3 的自定義工作目錄。
Run4=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 4。
UseRelativePath4=1
##僅 OpenButton 使用。1 表示 FileName4 是相對路徑，0 表示絕對路徑。
FileName4=
##僅 OpenButton 使用。運行項 4 的文件路徑。
Arguments4=
##僅 OpenButton 使用。運行項 4 的啟動參數。
WorkingDirectoryMode4=TargetDir
##僅 OpenButton 使用。運行項 4 的工作目錄模式。
WorkingDirectory4=
##僅 OpenButton 使用。運行項 4 的自定義工作目錄。
Run5=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 5。
UseRelativePath5=1
##僅 OpenButton 使用。1 表示 FileName5 是相對路徑，0 表示絕對路徑。
FileName5=
##僅 OpenButton 使用。運行項 5 的文件路徑。
Arguments5=
##僅 OpenButton 使用。運行項 5 的啟動參數。
WorkingDirectoryMode5=TargetDir
##僅 OpenButton 使用。運行項 5 的工作目錄模式。
WorkingDirectory5=
##僅 OpenButton 使用。運行項 5 的自定義工作目錄。

[Mode2.Element8]
Show=1
##可選 0 或 1。1 表示顯示此元素。
Type=TextFileBox
##元素類型。Show=1 時必須選擇有效類型。建議在編輯模式中選擇。
HeightScale=1
##僅 TextBox、TextFileBox 和 Table 使用。可選 1、2、3。高倍率元素太多時窗口可能超過屏幕高度。
Text=
##顯示文字。TextBox 顯示長文本，Text 顯示一行，最多 512 個字符；按鈕顯示按鈕文字，最多 512 個字符。Input、TextFileBox 和 Table 不使用此項。
Variable=1
##僅 Input 使用。可選 1 到 6，對應 %XHLM2V1% 到 %XHLM2V6%。多個輸入框寫同一變量時，元素編號靠後的會覆蓋前面的值。
Message=這是一條彈窗消息。\n可以用 \n 表示換行。
##僅 MessageButton 使用。必須寫在一行里，用 \n 表示實際換行。
RegFileName=
##僅 RegButton 使用。填寫存在的 .reg 文件，最終路徑必須以 .reg 結尾。
UseRelativePath=1
##僅 RegButton 使用。1=相對路徑，0=絕對路徑。
WorkingDirectoryMode=TargetDir
##僅 RegButton 使用。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用。
RegImportingText=
##僅 RegButton 使用。導入中顯示在按鈕上的文字。留空使用預設文字。
RegSuccessMessage=
##僅 RegButton 使用。導入成功後的提示，留空使用預設提示。
RegFailureMessage=
##僅 RegButton 使用。導入失敗後的提示。自定義時建議提醒可能已部分導入。
RegTimeoutMessage=
##僅 RegButton 使用。導入超過 60 秒後的提示。自定義時建議提醒可能已部分導入。
CsvFileName=
##僅 Table 使用。CSV 路徑，最終路徑必須以 .csv 結尾。最多顯示 20 列、1000 行。
CsvUseRelativePath=1
##僅 Table 使用。1=相對路徑，0=絕對路徑。
TextFileName=
##僅 TextFileBox 使用。文本文件路徑。優先按 UTF-8 讀取；不是有效 UTF-8 時按系統預設代碼頁讀取。支持 CRLF 和 LF 換行。
TextFileUseRelativePath=1
##僅 TextFileBox 使用。1=相對路徑，0=絕對路徑。
Run1=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 1。
UseRelativePath1=1
##僅 OpenButton 使用。1 表示 FileName1 是相對路徑，0 表示絕對路徑。
FileName1=
##僅 OpenButton 使用。運行項 1 的文件路徑。
Arguments1=
##僅 OpenButton 使用。運行項 1 的啟動參數。
WorkingDirectoryMode1=TargetDir
##僅 OpenButton 使用。運行項 1 的工作目錄模式。
WorkingDirectory1=
##僅 OpenButton 使用。運行項 1 的自定義工作目錄。
Run2=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 2。
UseRelativePath2=1
##僅 OpenButton 使用。1 表示 FileName2 是相對路徑，0 表示絕對路徑。
FileName2=
##僅 OpenButton 使用。運行項 2 的文件路徑。
Arguments2=
##僅 OpenButton 使用。運行項 2 的啟動參數。
WorkingDirectoryMode2=TargetDir
##僅 OpenButton 使用。運行項 2 的工作目錄模式。
WorkingDirectory2=
##僅 OpenButton 使用。運行項 2 的自定義工作目錄。
Run3=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 3。
UseRelativePath3=1
##僅 OpenButton 使用。1 表示 FileName3 是相對路徑，0 表示絕對路徑。
FileName3=
##僅 OpenButton 使用。運行項 3 的文件路徑。
Arguments3=
##僅 OpenButton 使用。運行項 3 的啟動參數。
WorkingDirectoryMode3=TargetDir
##僅 OpenButton 使用。運行項 3 的工作目錄模式。
WorkingDirectory3=
##僅 OpenButton 使用。運行項 3 的自定義工作目錄。
Run4=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 4。
UseRelativePath4=1
##僅 OpenButton 使用。1 表示 FileName4 是相對路徑，0 表示絕對路徑。
FileName4=
##僅 OpenButton 使用。運行項 4 的文件路徑。
Arguments4=
##僅 OpenButton 使用。運行項 4 的啟動參數。
WorkingDirectoryMode4=TargetDir
##僅 OpenButton 使用。運行項 4 的工作目錄模式。
WorkingDirectory4=
##僅 OpenButton 使用。運行項 4 的自定義工作目錄。
Run5=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 5。
UseRelativePath5=1
##僅 OpenButton 使用。1 表示 FileName5 是相對路徑，0 表示絕對路徑。
FileName5=
##僅 OpenButton 使用。運行項 5 的文件路徑。
Arguments5=
##僅 OpenButton 使用。運行項 5 的啟動參數。
WorkingDirectoryMode5=TargetDir
##僅 OpenButton 使用。運行項 5 的工作目錄模式。
WorkingDirectory5=
##僅 OpenButton 使用。運行項 5 的自定義工作目錄。

[Mode2.Element9]
Show=1
##可選 0 或 1。1 表示顯示此元素。
Type=Text
##元素類型。Show=1 時必須選擇有效類型。建議在編輯模式中選擇。
HeightScale=1
##僅 TextBox、TextFileBox 和 Table 使用。可選 1、2、3。高倍率元素太多時窗口可能超過屏幕高度。
Text=最小化
##顯示文字。TextBox 顯示長文本，Text 顯示一行，最多 512 個字符；按鈕顯示按鈕文字，最多 512 個字符。Input、TextFileBox 和 Table 不使用此項。
Variable=1
##僅 Input 使用。可選 1 到 6，對應 %XHLM2V1% 到 %XHLM2V6%。多個輸入框寫同一變量時，元素編號靠後的會覆蓋前面的值。
Message=這是一條彈窗消息。\n可以用 \n 表示換行。
##僅 MessageButton 使用。必須寫在一行里，用 \n 表示實際換行。
RegFileName=
##僅 RegButton 使用。填寫存在的 .reg 文件，最終路徑必須以 .reg 結尾。
UseRelativePath=1
##僅 RegButton 使用。1=相對路徑，0=絕對路徑。
WorkingDirectoryMode=TargetDir
##僅 RegButton 使用。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用。
RegImportingText=
##僅 RegButton 使用。導入中顯示在按鈕上的文字。留空使用預設文字。
RegSuccessMessage=
##僅 RegButton 使用。導入成功後的提示，留空使用預設提示。
RegFailureMessage=
##僅 RegButton 使用。導入失敗後的提示。自定義時建議提醒可能已部分導入。
RegTimeoutMessage=
##僅 RegButton 使用。導入超過 60 秒後的提示。自定義時建議提醒可能已部分導入。
CsvFileName=
##僅 Table 使用。CSV 路徑，最終路徑必須以 .csv 結尾。最多顯示 20 列、1000 行。
CsvUseRelativePath=1
##僅 Table 使用。1=相對路徑，0=絕對路徑。
TextFileName=
##僅 TextFileBox 使用。文本文件路徑。優先按 UTF-8 讀取；不是有效 UTF-8 時按系統預設代碼頁讀取。支持 CRLF 和 LF 換行。
TextFileUseRelativePath=1
##僅 TextFileBox 使用。1=相對路徑，0=絕對路徑。
Run1=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 1。
UseRelativePath1=1
##僅 OpenButton 使用。1 表示 FileName1 是相對路徑，0 表示絕對路徑。
FileName1=
##僅 OpenButton 使用。運行項 1 的文件路徑。
Arguments1=
##僅 OpenButton 使用。運行項 1 的啟動參數。
WorkingDirectoryMode1=TargetDir
##僅 OpenButton 使用。運行項 1 的工作目錄模式。
WorkingDirectory1=
##僅 OpenButton 使用。運行項 1 的自定義工作目錄。
Run2=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 2。
UseRelativePath2=1
##僅 OpenButton 使用。1 表示 FileName2 是相對路徑，0 表示絕對路徑。
FileName2=
##僅 OpenButton 使用。運行項 2 的文件路徑。
Arguments2=
##僅 OpenButton 使用。運行項 2 的啟動參數。
WorkingDirectoryMode2=TargetDir
##僅 OpenButton 使用。運行項 2 的工作目錄模式。
WorkingDirectory2=
##僅 OpenButton 使用。運行項 2 的自定義工作目錄。
Run3=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 3。
UseRelativePath3=1
##僅 OpenButton 使用。1 表示 FileName3 是相對路徑，0 表示絕對路徑。
FileName3=
##僅 OpenButton 使用。運行項 3 的文件路徑。
Arguments3=
##僅 OpenButton 使用。運行項 3 的啟動參數。
WorkingDirectoryMode3=TargetDir
##僅 OpenButton 使用。運行項 3 的工作目錄模式。
WorkingDirectory3=
##僅 OpenButton 使用。運行項 3 的自定義工作目錄。
Run4=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 4。
UseRelativePath4=1
##僅 OpenButton 使用。1 表示 FileName4 是相對路徑，0 表示絕對路徑。
FileName4=
##僅 OpenButton 使用。運行項 4 的文件路徑。
Arguments4=
##僅 OpenButton 使用。運行項 4 的啟動參數。
WorkingDirectoryMode4=TargetDir
##僅 OpenButton 使用。運行項 4 的工作目錄模式。
WorkingDirectory4=
##僅 OpenButton 使用。運行項 4 的自定義工作目錄。
Run5=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 5。
UseRelativePath5=1
##僅 OpenButton 使用。1 表示 FileName5 是相對路徑，0 表示絕對路徑。
FileName5=
##僅 OpenButton 使用。運行項 5 的文件路徑。
Arguments5=
##僅 OpenButton 使用。運行項 5 的啟動參數。
WorkingDirectoryMode5=TargetDir
##僅 OpenButton 使用。運行項 5 的工作目錄模式。
WorkingDirectory5=
##僅 OpenButton 使用。運行項 5 的自定義工作目錄。

[Mode2.Element10]
Show=1
##可選 0 或 1。1 表示顯示此元素。
Type=CloseButton
##元素類型。Show=1 時必須選擇有效類型。建議在編輯模式中選擇。
HeightScale=1
##僅 TextBox、TextFileBox 和 Table 使用。可選 1、2、3。高倍率元素太多時窗口可能超過屏幕高度。
Text=關閉
##顯示文字。TextBox 顯示長文本，Text 顯示一行，最多 512 個字符；按鈕顯示按鈕文字，最多 512 個字符。Input、TextFileBox 和 Table 不使用此項。
Variable=1
##僅 Input 使用。可選 1 到 6，對應 %XHLM2V1% 到 %XHLM2V6%。多個輸入框寫同一變量時，元素編號靠後的會覆蓋前面的值。
Message=這是一條彈窗消息。\n可以用 \n 表示換行。
##僅 MessageButton 使用。必須寫在一行里，用 \n 表示實際換行。
RegFileName=
##僅 RegButton 使用。填寫存在的 .reg 文件，最終路徑必須以 .reg 結尾。
UseRelativePath=1
##僅 RegButton 使用。1=相對路徑，0=絕對路徑。
WorkingDirectoryMode=TargetDir
##僅 RegButton 使用。可選 TargetDir、LauncherDir、Custom。
WorkingDirectory=
##自定義工作目錄，僅 WorkingDirectoryMode=Custom 時使用。
RegImportingText=
##僅 RegButton 使用。導入中顯示在按鈕上的文字。留空使用預設文字。
RegSuccessMessage=
##僅 RegButton 使用。導入成功後的提示，留空使用預設提示。
RegFailureMessage=
##僅 RegButton 使用。導入失敗後的提示。自定義時建議提醒可能已部分導入。
RegTimeoutMessage=
##僅 RegButton 使用。導入超過 60 秒後的提示。自定義時建議提醒可能已部分導入。
CsvFileName=
##僅 Table 使用。CSV 路徑，最終路徑必須以 .csv 結尾。最多顯示 20 列、1000 行。
CsvUseRelativePath=1
##僅 Table 使用。1=相對路徑，0=絕對路徑。
TextFileName=
##僅 TextFileBox 使用。文本文件路徑。優先按 UTF-8 讀取；不是有效 UTF-8 時按系統預設代碼頁讀取。支持 CRLF 和 LF 換行。
TextFileUseRelativePath=1
##僅 TextFileBox 使用。1=相對路徑，0=絕對路徑。
Run1=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 1。
UseRelativePath1=1
##僅 OpenButton 使用。1 表示 FileName1 是相對路徑，0 表示絕對路徑。
FileName1=
##僅 OpenButton 使用。運行項 1 的文件路徑。
Arguments1=
##僅 OpenButton 使用。運行項 1 的啟動參數。
WorkingDirectoryMode1=TargetDir
##僅 OpenButton 使用。運行項 1 的工作目錄模式。
WorkingDirectory1=
##僅 OpenButton 使用。運行項 1 的自定義工作目錄。
Run2=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 2。
UseRelativePath2=1
##僅 OpenButton 使用。1 表示 FileName2 是相對路徑，0 表示絕對路徑。
FileName2=
##僅 OpenButton 使用。運行項 2 的文件路徑。
Arguments2=
##僅 OpenButton 使用。運行項 2 的啟動參數。
WorkingDirectoryMode2=TargetDir
##僅 OpenButton 使用。運行項 2 的工作目錄模式。
WorkingDirectory2=
##僅 OpenButton 使用。運行項 2 的自定義工作目錄。
Run3=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 3。
UseRelativePath3=1
##僅 OpenButton 使用。1 表示 FileName3 是相對路徑，0 表示絕對路徑。
FileName3=
##僅 OpenButton 使用。運行項 3 的文件路徑。
Arguments3=
##僅 OpenButton 使用。運行項 3 的啟動參數。
WorkingDirectoryMode3=TargetDir
##僅 OpenButton 使用。運行項 3 的工作目錄模式。
WorkingDirectory3=
##僅 OpenButton 使用。運行項 3 的自定義工作目錄。
Run4=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 4。
UseRelativePath4=1
##僅 OpenButton 使用。1 表示 FileName4 是相對路徑，0 表示絕對路徑。
FileName4=
##僅 OpenButton 使用。運行項 4 的文件路徑。
Arguments4=
##僅 OpenButton 使用。運行項 4 的啟動參數。
WorkingDirectoryMode4=TargetDir
##僅 OpenButton 使用。運行項 4 的工作目錄模式。
WorkingDirectory4=
##僅 OpenButton 使用。運行項 4 的自定義工作目錄。
Run5=0
##僅 OpenButton 使用。1 表示點擊按鈕時執行運行項 5。
UseRelativePath5=1
##僅 OpenButton 使用。1 表示 FileName5 是相對路徑，0 表示絕對路徑。
FileName5=
##僅 OpenButton 使用。運行項 5 的文件路徑。
Arguments5=
##僅 OpenButton 使用。運行項 5 的啟動參數。
WorkingDirectoryMode5=TargetDir
##僅 OpenButton 使用。運行項 5 的工作目錄模式。
WorkingDirectory5=
##僅 OpenButton 使用。運行項 5 的自定義工作目錄。
```

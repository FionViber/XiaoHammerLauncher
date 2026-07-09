# 小錘啟動器

## 這是什麼

小錘啟動器是一個由 INI 配置驅動的 Windows 原生單文件啟動器。

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

完整預設模板見：[DEFAULT_TEMPLATE.zh-Hant.txt](DEFAULT_TEMPLATE.zh-Hant.txt)。

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

本項目使用 MIT License。

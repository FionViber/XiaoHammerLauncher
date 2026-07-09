#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <wincodec.h>
#include <string>
#include <vector>
#include <list>

#define XHL_MAX_TEXT 4096
#define XHL_EXPANDED_TEXT_MAX 32767
#define XHL_INI_BYTES (4 * 1024 * 1024)
#define XHL_INI_CHARS (XHL_INI_BYTES + 1)
#define XHL_SPLASH_MAX_BYTES (12LL * 1024LL * 1024LL)
#define XHL_SPLASH_MAX_WIDTH 3840
#define XHL_SPLASH_MAX_HEIGHT 2160
#define XHL_SPLASH_MAX_RENDER_WIDTH 3840
#define XHL_SPLASH_MAX_RENDER_HEIGHT 2160
#define XHL_SPLASH_MAX_DIB_BYTES (64ULL * 1024ULL * 1024ULL)
#define XHL_SPLASH_DEFAULT_DURATION_MS 1000
#define XHL_SPLASH_DEFAULT_SCALE 100
#define XHL_SPLASH_MIN_SCALE 10
#define XHL_SPLASH_MAX_SCALE 200
#define XHL_CSV_MAX_BYTES (4 * 1024 * 1024)
#define XHL_TEXT_FILE_MAX_BYTES (4 * 1024 * 1024)
#define XHL_CSV_MAX_COLUMNS 20
#define XHL_CSV_MAX_ROWS 1000
#define XHL_CSV_CELL_MAX_CHARS 256
#define XHL_MODE2_CONTROL_ID_BASE 2000
#define XHL_CONFIG_RESOURCE_ID 3
#define XHL_README_EN_RESOURCE_ID 4
#define XHL_README_ZH_HANS_RESOURCE_ID 5
#define XHL_README_ZH_HANT_RESOURCE_ID 6
#define XHL_APP_ICON_ID 101
#define XHL_TITLE_MAX_CHARS 32
#define XHL_CONFIG_VERSION_MAX_CHARS 32
#define XHL_BUTTON_DISPLAY_MAX_CHARS 512
#define XHL_TEXT_DISPLAY_MAX_CHARS 512
#define XHL_MODE1_RUN_COUNT 5
#define XHL_MODE1_VARIABLE_COUNT 6
#define XHL_MODE2_VARIABLE_COUNT 6
#define XHL_MODE2_BUTTON_RUN_COUNT 5
#define XHL_MODE2_ELEMENT_COUNT 10
#define XHL_MODE2_WINDOW_WIDTH 480
#define XHL_REG_IMPORT_TIMEOUT_MS 60000
#define XHL_EDITOR_ID_TITLE 3001
#define XHL_EDITOR_ID_LANGUAGE_COMBO 3002
#define XHL_EDITOR_ID_MODE_COMBO 3004
#define XHL_EDITOR_ID_SAVE 3008
#define XHL_EDITOR_ID_CANCEL 3009
#define XHL_EDITOR_ID_PANEL 3010
#define XHL_EDITOR_ID_TITLE_LABEL 3011
#define XHL_EDITOR_ID_LANGUAGE_LABEL 3012
#define XHL_EDITOR_ID_MODE_LABEL 3013
#define XHL_EDITOR_ID_VERSION 3014
#define XHL_EDITOR_ID_VERSION_LABEL 3015
#define XHL_EDITOR_ID_ABOUT 3016
#define XHL_ABOUT_ID_OK 3020
#define XHL_EDITOR_ID_MODE0_INFO 3100
#define XHL_EDITOR_ID_MODE0_README 3101
#define XHL_EDITOR_ID_MODE0_SPLASH_IMAGE 3102
#define XHL_EDITOR_ID_MODE0_SPLASH_DURATION 3103
#define XHL_EDITOR_ID_MODE0_SPLASH_SCALE 3104
#define XHL_EDITOR_ID_MODE0_SPLASH_POSITION 3105
#define XHL_EDITOR_ID_MODE0_SPLASH_ENABLE 3106
#define XHL_EDITOR_ID_MODE0_SPLASH_PATH 3107
#define XHL_EDITOR_ID_M1_RUN_SELECTOR 3110
#define XHL_EDITOR_ID_M2_ELEMENT_SELECTOR 3111
#define XHL_EDITOR_ID_M2_RUN_SELECTOR 3112
#define XHL_EDITOR_ID_M1_VAR_BASE 4000
#define XHL_EDITOR_ID_M1_REG_BASE 4100
#define XHL_EDITOR_ID_M1_RUN_BASE 4200
#define XHL_EDITOR_ID_M2_ELEMENT_BASE 5100
#define XHL_WM_REG_IMPORT_DONE (WM_APP + 1)
#define XHL_EDITOR_PAGE_WIDTH 660
#define XHL_EDITOR_WINDOW_WIDTH 700
#define XHL_EDITOR_WINDOW_HEIGHT 720
#define XHL_ABOUT_WINDOW_WIDTH 390
#define XHL_ABOUT_WINDOW_HEIGHT 210
#define XHL_EDITOR_COMBO_WIDTH 126
#define XHL_EDITOR_FIELD_WIDTH 200
#define XHL_EDITOR_FIELD_RIGHT 640

enum LauncherMode {
    ModeDisabled = 0,
    ModeSilent = 1,
    ModeWindow = 2
};

enum AppLanguage {
    AppLanguageEnglish = 0,
    AppLanguageSimplifiedChinese = 1,
    AppLanguageTraditionalChinese = 2
};

enum Mode2ElementKind {
    Mode2ElementNone = 0,
    Mode2ElementTextBox = 1,
    Mode2ElementText = 2,
    Mode2ElementOpenButton = 3,
    Mode2ElementMessageButton = 4,
    Mode2ElementRegButton = 5,
    Mode2ElementTable = 6,
    Mode2ElementInput = 7,
    Mode2ElementCloseButton = 8,
    Mode2ElementTextFileBox = 9
};

enum IniLoadError {
    IniLoadNone = 0,
    IniLoadOpenFailed = 1,
    IniLoadInvalidSize = 2,
    IniLoadReadFailed = 3,
    IniLoadInvalidEncoding = 4,
    IniLoadTooLarge = 5,
    IniLoadInvalidConfig = 6
};

enum WorkingDirectoryMode {
    WorkingDirectoryTargetDir = 0,
    WorkingDirectoryLauncherDir = 1,
    WorkingDirectoryCustom = 2
};

enum PathResolveResult {
    PathResolveOk = 0,
    PathResolveEmpty = 1,
    PathResolveWrongPathType = 2,
    PathResolveInvalid = 3,
    PathResolveOutsideLauncher = 4,
    PathResolveWrongExtension = 5,
    PathResolveMissingFile = 6,
    PathResolveMissingDirectory = 7
};

enum RegImportResult {
    RegImportSuccess = 0,
    RegImportFailed = 1,
    RegImportTimeout = 2
};

enum RegImportStage {
    RegImportStageNone = 0,
    RegImportStageWindowsDirectory = 1,
    RegImportStageLocateRegExe = 2,
    RegImportStageCreateProcess = 3,
    RegImportStageAssignJob = 4,
    RegImportStageResumeThread = 5,
    RegImportStageWait = 6,
    RegImportStageExitCode = 7,
    RegImportStageProcessExit = 8
};

enum SplashPosition {
    SplashCenter = 0,
    SplashTopLeft = 1,
    SplashTopRight = 2,
    SplashBottomLeft = 3,
    SplashBottomRight = 4
};

struct LauncherContext {
    std::wstring modulePath;
    std::wstring baseDir;
    std::wstring fullBaseDir;
    std::wstring configName;
    std::wstring configPath;
};

struct Mode2RunItem {
    BOOL run;
    BOOL useRelativePath;
    WorkingDirectoryMode workingDirectoryMode;
    WCHAR fileName[XHL_MAX_TEXT];
    WCHAR arguments[XHL_MAX_TEXT];
    WCHAR workingDirectory[XHL_MAX_TEXT];
};

struct Mode2Element {
    BOOL visible;
    Mode2ElementKind kind;
    UINT index;
    UINT heightScale;
    BOOL minimizeOnClick;
    HWND control;
    WCHAR text[XHL_MAX_TEXT];
    WCHAR displayText[XHL_MAX_TEXT];
    WCHAR message[XHL_MAX_TEXT];
    WCHAR regFileName[XHL_MAX_TEXT];
    BOOL regUseRelativePath;
    WorkingDirectoryMode regWorkingDirectoryMode;
    WCHAR regWorkingDirectory[XHL_MAX_TEXT];
    WCHAR regImportingText[XHL_MAX_TEXT];
    WCHAR regSuccessMessage[XHL_MAX_TEXT];
    WCHAR regFailureMessage[XHL_MAX_TEXT];
    WCHAR regTimeoutMessage[XHL_MAX_TEXT];
    WCHAR csvFileName[XHL_MAX_TEXT];
    BOOL csvUseRelativePath;
    WCHAR textFileName[XHL_MAX_TEXT];
    BOOL textFileUseRelativePath;
    UINT inputVariableIndex;
    Mode2RunItem runs[XHL_MODE2_BUTTON_RUN_COUNT];
};

struct EditorVariableConfig {
    BOOL enable;
    WCHAR value[XHL_MAX_TEXT];
};

struct RegImportTask {
    HWND window;
    HWND button;
    std::wstring regPath;
    std::wstring workingDirectory;
    std::wstring originalText;
    std::wstring importingText;
    std::wstring successMessage;
    std::wstring failureMessage;
    std::wstring timeoutMessage;
    RegImportStage stage;
    DWORD lastError;
    DWORD exitCode;
    BOOL jobAssigned;
};

struct EditorRunConfig {
    BOOL run;
    BOOL useRelativePath;
    WorkingDirectoryMode workingDirectoryMode;
    WCHAR fileName[XHL_MAX_TEXT];
    WCHAR arguments[XHL_MAX_TEXT];
    WCHAR workingDirectory[XHL_MAX_TEXT];
};

struct EditorRegistryConfig {
    BOOL addReg;
    BOOL useRelativePath;
    WorkingDirectoryMode workingDirectoryMode;
    WCHAR regFileName[XHL_MAX_TEXT];
    WCHAR workingDirectory[XHL_MAX_TEXT];
    BOOL regImported;
};

struct EditorMode2ElementConfig {
    BOOL show;
    WCHAR type[32];
    UINT heightScale;
    BOOL minimizeOnClick;
    WCHAR text[XHL_MAX_TEXT];
    WCHAR message[XHL_MAX_TEXT];
    WCHAR regFileName[XHL_MAX_TEXT];
    BOOL regUseRelativePath;
    WorkingDirectoryMode regWorkingDirectoryMode;
    WCHAR regWorkingDirectory[XHL_MAX_TEXT];
    WCHAR regImportingText[XHL_MAX_TEXT];
    WCHAR regSuccessMessage[XHL_MAX_TEXT];
    WCHAR regFailureMessage[XHL_MAX_TEXT];
    WCHAR regTimeoutMessage[XHL_MAX_TEXT];
    WCHAR csvFileName[XHL_MAX_TEXT];
    BOOL csvUseRelativePath;
    WCHAR textFileName[XHL_MAX_TEXT];
    BOOL textFileUseRelativePath;
    UINT inputVariableIndex;
    EditorRunConfig runs[XHL_MODE2_BUTTON_RUN_COUNT];
};

struct EditorConfig {
    WCHAR title[XHL_MAX_TEXT];
    WCHAR version[XHL_MAX_TEXT];
    BOOL splashEnable;
    BOOL splashUseRelativePath;
    WCHAR splashImage[XHL_MAX_TEXT];
    WCHAR splashDurationMs[32];
    WCHAR splashScale[32];
    SplashPosition splashPosition;
    AppLanguage language;
    BOOL chinese;
    LauncherMode mode;
    EditorVariableConfig mode1Variables[XHL_MODE1_VARIABLE_COUNT];
    EditorRegistryConfig mode1Registry;
    EditorRunConfig mode1Runs[XHL_MODE1_RUN_COUNT];
    EditorMode2ElementConfig mode2Elements[XHL_MODE2_ELEMENT_COUNT];
};

static LauncherContext g_ctx;
static BYTE* g_iniBytes;
static WCHAR* g_iniText;
static WCHAR* g_writeText;
static BOOL g_iniLoaded;
static DWORD g_iniBytesRead;
static IniLoadError g_iniLoadError;
static WCHAR g_substitutionBuffer[XHL_MAX_TEXT];
static WCHAR g_displayTitle[XHL_MAX_TEXT];
static WCHAR g_mode1VariableValues[XHL_MODE1_VARIABLE_COUNT][XHL_MAX_TEXT];
static BOOL g_mode1VariableEnabled[XHL_MODE1_VARIABLE_COUNT];
static WCHAR g_mode2VariableValues[XHL_MODE2_VARIABLE_COUNT][XHL_MAX_TEXT];
static BOOL g_mode2VariableEnabled[XHL_MODE2_VARIABLE_COUNT];
static Mode2Element g_mode2Elements[XHL_MODE2_ELEMENT_COUNT];
static std::wstring g_mode2TextFileDisplayTexts[XHL_MODE2_ELEMENT_COUNT];
static HFONT g_uiFont;
static HFONT g_uiBoldFont;
static EditorConfig g_editorConfig;
static HWND g_editorTitleLabel;
static HWND g_editorVersionLabel;
static HWND g_editorLanguageLabel;
static HWND g_editorModeLabel;
static HWND g_editorTitleEdit;
static HWND g_editorVersionEdit;
static HWND g_editorModeCombo;
static HWND g_editorLanguageCombo;
static HWND g_editorAboutButton;
static HWND g_editorPanel;
static HWND g_editorTooltip;
static HWND g_editorSaveButton;
static HWND g_editorCancelButton;
static LauncherMode g_editorPageMode;
static UINT g_editorMode1RunIndex;
static UINT g_editorMode2ElementIndex;
static UINT g_editorMode2RunIndex;
static std::wstring* g_graphSaveText;
static std::list<std::wstring> g_editorTooltipTexts;
static volatile LONG g_regImportInProgress;
static volatile PVOID g_regImportProcess;

struct SplashImageData {
    HBITMAP bitmap;
    UINT width;
    UINT height;
    int drawWidth;
    int drawHeight;
    DWORD durationMs;
};

struct AboutDialogState {
    HWND owner;
    HWND title;
    HWND body;
    HWND link;
    HWND okButton;
    HFONT font;
    HFONT boldFont;
    UINT dpi;
    BOOL done;
    AppLanguage language;
    BOOL chinese;
    BOOL ownerRestored;
};

static BOOL IsChineseLanguage();
static BOOL GraphEditorUsesChinese();
static AppLanguage ReadConfiguredLanguage();
static AppLanguage GraphEditorLanguage();
static const WCHAR* XHL_GITHUB_URL = L"https://github.com/FionViber/XiaoHammerLauncher";

__declspec(noreturn) static void ExitApplication(HWND window, UINT exitCode) {
    if (window) {
        ShowWindow(window, SW_HIDE);
        DestroyWindow(window);
    }
    ExitProcess(exitCode);
}

static UINT StrLen(const WCHAR* text) {
    UINT length = 0;
    if (!text) {
        return 0;
    }
    while (text[length] != L'\0') {
        ++length;
    }
    return length;
}

static WCHAR Lower(WCHAR ch) {
    if (ch >= L'A' && ch <= L'Z') {
        return ch + 32;
    }
    return ch;
}

static BOOL SameTextI(const WCHAR* left, const WCHAR* right) {
    if (!left || !right) {
        return FALSE;
    }
    while (*left && *right) {
        if (Lower(*left) != Lower(*right)) {
            return FALSE;
        }
        ++left;
        ++right;
    }
    return *left == L'\0' && *right == L'\0';
}

static BOOL StartsWithI(const WCHAR* text, const WCHAR* prefix) {
    if (!text || !prefix) {
        return FALSE;
    }
    while (*prefix) {
        if (Lower(*text) != Lower(*prefix)) {
            return FALSE;
        }
        ++text;
        ++prefix;
    }
    return TRUE;
}

static void CopyText(WCHAR* dest, UINT capacity, const WCHAR* source) {
    if (!dest || capacity == 0) {
        return;
    }
    UINT i = 0;
    if (source) {
        while (source[i] && i + 1 < capacity) {
            dest[i] = source[i];
            ++i;
        }
    }
    dest[i] = L'\0';
}

static BOOL LanguageIsChinese(AppLanguage language) {
    return language == AppLanguageSimplifiedChinese || language == AppLanguageTraditionalChinese;
}

static AppLanguage AppLanguageFromText(const WCHAR* text) {
    if (!text || !text[0]) {
        return AppLanguageEnglish;
    }
    if (SameTextI(text, L"zh-Hant")) {
        return AppLanguageTraditionalChinese;
    }
    if (SameTextI(text, L"zh-Hans")) {
        return AppLanguageSimplifiedChinese;
    }
    if (SameTextI(text, L"en-US")) {
        return AppLanguageEnglish;
    }
    return AppLanguageEnglish;
}

static const WCHAR* AppLanguageToConfigText(AppLanguage language) {
    if (language == AppLanguageTraditionalChinese) {
        return L"zh-Hant";
    }
    if (language == AppLanguageSimplifiedChinese) {
        return L"zh-Hans";
    }
    return L"en-US";
}

static void AppendText(WCHAR* dest, UINT capacity, const WCHAR* source) {
    UINT length = StrLen(dest);
    if (length >= capacity) {
        return;
    }
    CopyText(dest + length, capacity - length, source);
}

static void AppendUInt(WCHAR* dest, UINT capacity, UINT value) {
    WCHAR digits[12];
    UINT index = 0;
    do {
        digits[index++] = (WCHAR)(L'0' + (value % 10));
        value /= 10;
    } while (value && index < 12);

    UINT length = StrLen(dest);
    while (index > 0 && length + 1 < capacity) {
        dest[length++] = digits[--index];
    }
    dest[length] = L'\0';
}

static const WCHAR* LocalizedText(AppLanguage language, const WCHAR* zhHans, const WCHAR* zhHant, const WCHAR* enUs) {
    if (language == AppLanguageTraditionalChinese) {
        return zhHant;
    }
    if (language == AppLanguageSimplifiedChinese) {
        return zhHans;
    }
    return enUs;
}

static void AppendSegmentString(std::wstring& dest, const WCHAR* start, const WCHAR* end) {
    if (start && end && end > start) {
        dest.append(start, end - start);
    }
}

static BOOL EndsWithLineBreakString(const std::wstring& text) {
    return !text.empty() && (text[text.length() - 1] == L'\r' || text[text.length() - 1] == L'\n');
}

static void NormalizeLineBreaksToCrLf(std::wstring& text) {
    if (text.find_first_of(L"\r\n") == std::wstring::npos) {
        return;
    }

    std::wstring normalized;
    normalized.reserve(text.length());
    for (size_t i = 0; i < text.length(); ++i) {
        if (text[i] == L'\r') {
            normalized.append(L"\r\n");
            if (i + 1 < text.length() && text[i + 1] == L'\n') {
                ++i;
            }
        } else if (text[i] == L'\n') {
            normalized.append(L"\r\n");
        } else {
            normalized.push_back(text[i]);
        }
    }
    text.swap(normalized);
}

static void TruncateText(WCHAR* text, UINT maxChars) {
    UINT length = StrLen(text);
    if (length <= maxChars) {
        return;
    }

    if (maxChars > 0 && text[maxChars - 1] >= 0xD800 && text[maxChars - 1] <= 0xDBFF) {
        --maxChars;
    }
    text[maxChars] = L'\0';
}

static int ScaleForDpi(int value, UINT dpi) {
    return MulDiv(value, dpi ? dpi : 96, 96);
}

static UINT NormalizeHeightScale(UINT scale) {
    if (scale < 1 || scale > 3) {
        return 1;
    }
    return scale;
}

static const WCHAR* HeightScaleToText(UINT scale) {
    if (scale == 2) {
        return L"2";
    }
    if (scale == 3) {
        return L"3";
    }
    return L"1";
}

static void InitCommonControlsForApp() {
    INITCOMMONCONTROLSEX controls;
    ZeroMemory(&controls, sizeof(controls));
    controls.dwSize = sizeof(controls);
    controls.dwICC = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES | ICC_LINK_CLASS;
    InitCommonControlsEx(&controls);
}

static UINT GetDpi(HWND window) {
    typedef UINT(WINAPI* GetDpiForWindowProc)(HWND);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        GetDpiForWindowProc getDpiForWindow = (GetDpiForWindowProc)GetProcAddress(user32, "GetDpiForWindow");
        if (getDpiForWindow && window) {
            UINT dpi = getDpiForWindow(window);
            if (dpi) {
                return dpi;
            }
        }
    }

    HWND dcWindow = window ? window : 0;
    HDC dc = GetDC(dcWindow);
    if (!dc) {
        return 96;
    }

    int dpi = GetDeviceCaps(dc, LOGPIXELSX);
    ReleaseDC(dcWindow, dc);
    return dpi > 0 ? (UINT)dpi : 96;
}

static void CenterWindowOnMonitor(HWND window) {
    if (!window) {
        return;
    }

    RECT windowRect;
    if (!GetWindowRect(window, &windowRect)) {
        return;
    }

    RECT workRect;
    HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo;
    ZeroMemory(&monitorInfo, sizeof(monitorInfo));
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
        workRect = monitorInfo.rcWork;
    } else if (!SystemParametersInfoW(SPI_GETWORKAREA, 0, &workRect, 0)) {
        return;
    }

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    int workWidth = workRect.right - workRect.left;
    int workHeight = workRect.bottom - workRect.top;

    int x = windowWidth >= workWidth ? workRect.left : workRect.left + (workWidth - windowWidth) / 2;
    int y = windowHeight >= workHeight ? workRect.top : workRect.top + (workHeight - windowHeight) / 2;

    SetWindowPos(window, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

static BOOL IsMode2ButtonKind(Mode2ElementKind kind);

static void ApplyElementFont(Mode2Element* element) {
    if (!element || !element->control) {
        return;
    }

    if (g_uiFont) {
        SendMessageW(element->control, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
    }
}

static void FitMode2DisplayText(Mode2Element* element, int width, UINT dpi) {
    (void)width;
    (void)dpi;
    if (!element || !element->control || element->kind != Mode2ElementText) {
        return;
    }
    SetWindowTextW(element->control, element->displayText);
}

static void UpdateUIFont(UINT dpi) {
    HFONT newFont = CreateFontW(
        -MulDiv(10, dpi ? dpi : 96, 72),
        0,
        0,
        0,
        FW_NORMAL,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Segoe UI");
    HFONT newBoldFont = CreateFontW(
        -MulDiv(10, dpi ? dpi : 96, 72),
        0,
        0,
        0,
        FW_BOLD,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Segoe UI");
    if (!newFont || !newBoldFont) {
        if (newFont) DeleteObject(newFont);
        if (newBoldFont) DeleteObject(newBoldFont);
        return;
    }

    HFONT oldFont = g_uiFont;
    HFONT oldBoldFont = g_uiBoldFont;
    g_uiFont = newFont;
    g_uiBoldFont = newBoldFont;

    for (UINT i = 0; i < XHL_MODE2_ELEMENT_COUNT; ++i) {
        ApplyElementFont(&g_mode2Elements[i]);
    }

    if (oldFont) {
        DeleteObject(oldFont);
    }
    if (oldBoldFont) {
        DeleteObject(oldBoldFont);
    }
}

static int Mode2ElementHeight(const Mode2Element* element, UINT dpi) {
    int buttonHeight = ScaleForDpi(34, dpi);
    if (element && (element->kind == Mode2ElementTextBox || element->kind == Mode2ElementTextFileBox || element->kind == Mode2ElementTable)) {
        return buttonHeight * 2 * (int)NormalizeHeightScale(element->heightScale);
    }
    return buttonHeight;
}

static int Mode2ClientHeight(UINT dpi) {
    int margin = ScaleForDpi(20, dpi);
    int rowGap = ScaleForDpi(10, dpi);
    int count = 0;
    int height = margin * 2;

    for (UINT i = 0; i < XHL_MODE2_ELEMENT_COUNT; ++i) {
        Mode2Element* element = &g_mode2Elements[i];
        if (!element->visible) {
            continue;
        }

        if (count > 0) {
            height += rowGap;
        }
        height += Mode2ElementHeight(element, dpi);
        ++count;
    }

    return height;
}

static void LayoutWindow(HWND window, UINT dpi) {
    UpdateUIFont(dpi);
    RECT client;
    GetClientRect(window, &client);
    int margin = ScaleForDpi(20, dpi);
    int y = ScaleForDpi(20, dpi);
    int width = client.right - client.left - margin * 2;
    if (width < ScaleForDpi(80, dpi)) {
        width = ScaleForDpi(80, dpi);
    }
    int rowGap = ScaleForDpi(10, dpi);

    for (UINT i = 0; i < XHL_MODE2_ELEMENT_COUNT; ++i) {
        Mode2Element* element = &g_mode2Elements[i];
        if (!element->visible || !element->control) {
            continue;
        }

        int height = Mode2ElementHeight(element, dpi);
        SetWindowPos(element->control, 0, margin, y, width, height, SWP_NOZORDER);
        FitMode2DisplayText(element, width, dpi);
        y += height + rowGap;
    }

    InvalidateRect(window, 0, TRUE);
}

static BOOL IsSpace(WCHAR ch) {
    return ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n';
}

static void TrimInPlace(WCHAR* text) {
    if (!text) {
        return;
    }

    UINT start = 0;
    UINT length = StrLen(text);
    while (start < length && IsSpace(text[start])) {
        ++start;
    }

    UINT end = length;
    while (end > start && IsSpace(text[end - 1])) {
        --end;
    }

    UINT out = 0;
    while (start < end) {
        text[out++] = text[start++];
    }
    text[out] = L'\0';
}

static void ExpandLineBreakEscapes(WCHAR* text, UINT capacity) {
    if (!text || capacity == 0) {
        return;
    }

    WCHAR source[XHL_MAX_TEXT];
    CopyText(source, XHL_MAX_TEXT, text);
    UINT in = 0;
    UINT out = 0;

    while (source[in] && out + 1 < capacity) {
        if (source[in] == L'\\' && source[in + 1] == L'n') {
            if (out + 2 >= capacity) {
                break;
            }
            text[out++] = L'\r';
            text[out++] = L'\n';
            in += 2;
            continue;
        }

        text[out++] = source[in++];
    }
    text[out] = L'\0';
}

static BOOL SegmentEqualsI(const WCHAR* start, const WCHAR* end, const WCHAR* text) {
    while (start < end && *text) {
        if (Lower(*start) != Lower(*text)) {
            return FALSE;
        }
        ++start;
        ++text;
    }
    return start == end && *text == L'\0';
}

static void CopyTrimmedSegment(WCHAR* out, UINT capacity, const WCHAR* start, const WCHAR* end) {
    while (start < end && IsSpace(*start)) {
        ++start;
    }
    while (end > start && IsSpace(*(end - 1))) {
        --end;
    }

    UINT i = 0;
    while (start < end && i + 1 < capacity) {
        out[i++] = *start++;
    }
    out[i] = L'\0';
}

static void CopySegment(WCHAR* out, UINT capacity, const WCHAR* start, const WCHAR* end) {
    UINT i = 0;
    while (start < end && i + 1 < capacity) {
        out[i++] = *start++;
    }
    out[i] = L'\0';
}

static BOOL IsRootedPath(const WCHAR* path) {
    if (!path || !path[0]) {
        return FALSE;
    }
    if (path[0] == L'\\' || path[0] == L'/') {
        return TRUE;
    }
    return path[0] && path[1] == L':';
}

static BOOL IsAbsolutePath(const WCHAR* path) {
    if (!path || !path[0]) {
        return FALSE;
    }
    if (path[0] == L'\\' && path[1] == L'\\') {
        return TRUE;
    }
    return path[0] && path[1] == L':' && path[2] == L'\\';
}

static BOOL IsFile(const WCHAR* path) {
    DWORD attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

static BOOL IsDirectory(const WCHAR* path) {
    DWORD attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

static BOOL GetFileSizeBytes(const WCHAR* path, LONGLONG* out) {
    if (!path || !out) {
        return FALSE;
    }

    WIN32_FILE_ATTRIBUTE_DATA data;
    if (!GetFileAttributesExW(path, GetFileExInfoStandard, &data)) {
        return FALSE;
    }
    if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        return FALSE;
    }

    ULARGE_INTEGER size;
    size.HighPart = data.nFileSizeHigh;
    size.LowPart = data.nFileSizeLow;
    *out = (LONGLONG)size.QuadPart;
    return TRUE;
}

static void EnsureTrailingSlashString(std::wstring& path) {
    if (path.empty()) {
        return;
    }
    WCHAR last = path[path.length() - 1];
    if (last != L'\\' && last != L'/') {
        path.push_back(L'\\');
    }
}

static void ReplaceForwardSlashString(std::wstring& text) {
    for (size_t i = 0; i < text.length(); ++i) {
        if (text[i] == L'/') {
            text[i] = L'\\';
        }
    }
}

static void TrimString(std::wstring& text) {
    size_t start = 0;
    while (start < text.length() && IsSpace(text[start])) {
        ++start;
    }
    size_t end = text.length();
    while (end > start && IsSpace(text[end - 1])) {
        --end;
    }
    text = text.substr(start, end - start);
}

static BOOL GetFullPathString(const std::wstring& path, std::wstring& out) {
    DWORD needed = GetFullPathNameW(path.c_str(), 0, 0, 0);
    if (needed == 0) {
        return FALSE;
    }

    std::wstring buffer;
    buffer.resize(needed + 1);
    DWORD copied = GetFullPathNameW(path.c_str(), (DWORD)buffer.size(), &buffer[0], 0);
    if (copied == 0 || copied >= buffer.size()) {
        return FALSE;
    }

    buffer.resize(copied);
    out = buffer;
    return TRUE;
}

static BOOL GetModuleFileNameString(std::wstring& out) {
    DWORD capacity = 260;
    for (;;) {
        std::wstring buffer;
        buffer.resize(capacity);
        DWORD copied = GetModuleFileNameW(0, &buffer[0], capacity);
        if (copied == 0) {
            return FALSE;
        }
        if (copied < capacity - 1) {
            buffer.resize(copied);
            out = buffer;
            return TRUE;
        }
        capacity *= 2;
        if (capacity > 32768) {
            return FALSE;
        }
    }
}

static BOOL GetWindowsDirectoryString(std::wstring& out) {
    out.clear();
    std::vector<WCHAR> buffer(MAX_PATH);
    for (;;) {
        UINT copied = GetWindowsDirectoryW(buffer.data(), (UINT)buffer.size());
        if (copied == 0) {
            return FALSE;
        }
        if (copied < buffer.size()) {
            out.assign(buffer.data(), copied);
            return TRUE;
        }
        buffer.resize((size_t)copied + 1);
    }
}

static BOOL InitContext() {
    if (!GetModuleFileNameString(g_ctx.modulePath)) {
        return FALSE;
    }

    size_t lastSlash = g_ctx.modulePath.find_last_of(L"\\/");
    if (lastSlash == std::wstring::npos) {
        g_ctx.baseDir.clear();
        g_ctx.configName = g_ctx.modulePath;
    } else {
        g_ctx.baseDir = g_ctx.modulePath.substr(0, lastSlash + 1);
        g_ctx.configName = g_ctx.modulePath.substr(lastSlash + 1);
    }

    size_t dot = g_ctx.configName.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        g_ctx.configName.resize(dot);
    }
    if (g_ctx.configName.empty()) {
        g_ctx.configName = L"XHLauncher";
    }
    g_ctx.configName += L".ini";
    g_ctx.configPath = g_ctx.baseDir + g_ctx.configName;

    if (!GetFullPathString(g_ctx.baseDir.empty() ? L"." : g_ctx.baseDir, g_ctx.fullBaseDir)) {
        g_ctx.fullBaseDir = g_ctx.baseDir;
    }
    EnsureTrailingSlashString(g_ctx.fullBaseDir);
    return TRUE;
}

static BOOL BuildTempPathForTarget(const WCHAR* path, WCHAR* out, UINT capacity) {
    if (!path || !out || capacity == 0) {
        return FALSE;
    }

    CopyText(out, capacity, path);
    AppendText(out, capacity, L".tmp.");
    AppendUInt(out, capacity, GetCurrentProcessId());
    if (StrLen(out) + 1 >= capacity) {
        out[0] = L'\0';
        return FALSE;
    }
    return TRUE;
}

static BOOL BuildTempPathForTargetString(const std::wstring& path, std::wstring& out) {
    if (path.empty()) {
        return FALSE;
    }

    WCHAR suffix[32];
    CopyText(suffix, 32, L".tmp.");
    AppendUInt(suffix, 32, GetCurrentProcessId());
    out = path;
    out += suffix;
    return !out.empty();
}

static BOOL WriteResourceToFileIfMissing(UINT resourceId, const WCHAR* path) {
    if (!path || IsFile(path)) {
        return TRUE;
    }

    HMODULE module = GetModuleHandleW(0);
    HRSRC resource = FindResourceW(module, MAKEINTRESOURCEW(resourceId), RT_RCDATA);
    if (!resource) {
        return FALSE;
    }

    HGLOBAL loaded = LoadResource(module, resource);
    DWORD size = SizeofResource(module, resource);
    void* data = loaded ? LockResource(loaded) : 0;
    if (!data || !size) {
        return FALSE;
    }

    WCHAR tempPath[XHL_MAX_TEXT];
    if (!BuildTempPathForTarget(path, tempPath, XHL_MAX_TEXT)) {
        return FALSE;
    }

    DeleteFileW(tempPath);
    HANDLE file = CreateFileW(tempPath, GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    DWORD written = 0;
    BOOL writeOk = WriteFile(file, data, size, &written, 0);
    BOOL flushOk = writeOk && written == size && FlushFileBuffers(file);
    CloseHandle(file);
    if (!flushOk) {
        DeleteFileW(tempPath);
        return FALSE;
    }

    if (!MoveFileExW(tempPath, path, MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(tempPath);
        return FALSE;
    }

    return TRUE;
}

static BOOL ReleaseConfigIfMissing() {
    return WriteResourceToFileIfMissing(XHL_CONFIG_RESOURCE_ID, g_ctx.configPath.c_str());
}

static BOOL DecodeTextBytesToString(const BYTE* bytes, DWORD byteCount, std::wstring& out) {
    out.clear();
    if (!bytes || byteCount == 0) {
        return FALSE;
    }

    if (byteCount >= 2 && bytes[0] == 0xFF && bytes[1] == 0xFE) {
        if (((byteCount - 2) % 2) != 0) {
            return FALSE;
        }
        DWORD charCount = (byteCount - 2) / 2;
        out.resize(charCount);
        for (DWORD i = 0; i < charCount; ++i) {
            out[i] = (WCHAR)(bytes[2 + i * 2] | (bytes[3 + i * 2] << 8));
        }
        return TRUE;
    }

    DWORD offset = 0;
    if (byteCount >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF) {
        offset = 3;
    }
    DWORD utf8Bytes = byteCount - offset;
    if (utf8Bytes == 0) {
        return TRUE;
    }

    int chars = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCCH)(bytes + offset), utf8Bytes, 0, 0);
    if (chars <= 0) {
        return FALSE;
    }

    out.resize(chars);
    int copied = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCCH)(bytes + offset), utf8Bytes, &out[0], chars);
    if (copied != chars) {
        out.clear();
        return FALSE;
    }
    return TRUE;
}

static BOOL LoadTextResourceString(UINT resourceId, std::wstring& out) {
    HMODULE module = GetModuleHandleW(0);
    HRSRC resource = FindResourceW(module, MAKEINTRESOURCEW(resourceId), RT_RCDATA);
    if (!resource) {
        return FALSE;
    }

    HGLOBAL loaded = LoadResource(module, resource);
    DWORD size = SizeofResource(module, resource);
    void* data = loaded ? LockResource(loaded) : 0;
    if (!data || !size) {
        return FALSE;
    }

    return DecodeTextBytesToString((const BYTE*)data, size, out);
}

typedef std::vector<std::wstring> CsvRow;
typedef std::vector<CsvRow> CsvTable;

static BOOL LoadTextFileStringLimited(const WCHAR* path, LONGLONG maxBytes, std::wstring& out) {
    out.clear();
    if (!path || !path[0] || maxBytes <= 0) {
        return FALSE;
    }

    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(file, &fileSize) || fileSize.QuadPart <= 0 || fileSize.QuadPart > maxBytes || fileSize.QuadPart > 0x7FFFFFFF) {
        CloseHandle(file);
        return FALSE;
    }

    DWORD byteCount = (DWORD)fileSize.QuadPart;
    std::vector<BYTE> bytes(byteCount);
    DWORD bytesRead = 0;
    BOOL ok = ReadFile(file, bytes.data(), byteCount, &bytesRead, 0);
    CloseHandle(file);
    if (!ok || bytesRead != byteCount) {
        return FALSE;
    }

    return DecodeTextBytesToString(bytes.data(), byteCount, out);
}

static BOOL DecodeTextBytesToStringLenient(const BYTE* bytes, DWORD byteCount, std::wstring& out) {
    if (DecodeTextBytesToString(bytes, byteCount, out)) {
        return TRUE;
    }

    out.clear();
    if (!bytes || byteCount == 0) {
        return TRUE;
    }

    int chars = MultiByteToWideChar(CP_ACP, 0, (LPCCH)bytes, byteCount, 0, 0);
    if (chars <= 0) {
        return FALSE;
    }

    out.resize(chars);
    int copied = MultiByteToWideChar(CP_ACP, 0, (LPCCH)bytes, byteCount, &out[0], chars);
    if (copied != chars) {
        out.clear();
        return FALSE;
    }
    return TRUE;
}

static BOOL LoadDisplayTextFileStringLimited(const WCHAR* path, LONGLONG maxBytes, std::wstring& out) {
    out.clear();
    if (!path || !path[0] || maxBytes <= 0) {
        return FALSE;
    }

    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(file, &fileSize) || fileSize.QuadPart < 0 || fileSize.QuadPart > maxBytes || fileSize.QuadPart > 0x7FFFFFFF) {
        CloseHandle(file);
        return FALSE;
    }

    DWORD byteCount = (DWORD)fileSize.QuadPart;
    if (byteCount == 0) {
        CloseHandle(file);
        return TRUE;
    }

    std::vector<BYTE> bytes(byteCount);
    DWORD bytesRead = 0;
    BOOL ok = ReadFile(file, bytes.data(), byteCount, &bytesRead, 0);
    CloseHandle(file);
    if (!ok || bytesRead != byteCount) {
        return FALSE;
    }

    if (!DecodeTextBytesToStringLenient(bytes.data(), byteCount, out)) {
        return FALSE;
    }

    NormalizeLineBreaksToCrLf(out);
    return TRUE;
}

static void TruncateString(std::wstring& text, UINT maxChars) {
    if (text.length() <= maxChars) {
        return;
    }
    if (maxChars > 0 && text[maxChars - 1] >= 0xD800 && text[maxChars - 1] <= 0xDBFF) {
        --maxChars;
    }
    text.resize(maxChars);
}

static void CsvPushCell(CsvRow& row, std::wstring& field) {
    TruncateString(field, XHL_CSV_CELL_MAX_CHARS);
    row.push_back(field);
    field.clear();
}

static void CsvPushRow(CsvTable& table, CsvRow& row, UINT* maxColumns) {
    if (!maxColumns || row.empty() || table.size() >= XHL_CSV_MAX_ROWS + 1) {
        row.clear();
        return;
    }
    if (row.size() > XHL_CSV_MAX_COLUMNS) {
        row.resize(XHL_CSV_MAX_COLUMNS);
    }
    if (row.size() > *maxColumns) {
        *maxColumns = (UINT)row.size();
    }
    table.push_back(row);
    row.clear();
}

enum CsvParseState {
    CsvFieldStart,
    CsvUnquotedField,
    CsvQuotedField,
    CsvAfterQuote
};

static BOOL IsCsvNewline(WCHAR ch) {
    return ch == L'\r' || ch == L'\n';
}

static BOOL IsCsvPostQuoteSpace(WCHAR ch) {
    return ch == L' ' || ch == L'\t';
}

static void CsvAdvanceNewline(const std::wstring& text, size_t* index) {
    if (!index) {
        return;
    }
    if (text[*index] == L'\r' && *index + 1 < text.length() && text[*index + 1] == L'\n') {
        ++(*index);
    }
}

static void CsvFinishRow(CsvTable& table, CsvRow& row, UINT* maxColumns, std::wstring& field, BOOL pushField) {
    if (pushField) {
        CsvPushCell(row, field);
    } else {
        field.clear();
    }
    CsvPushRow(table, row, maxColumns);
}

static BOOL ParseCsvText(const std::wstring& text, CsvTable& table, UINT* maxColumns) {
    table.clear();
    if (maxColumns) {
        *maxColumns = 0;
    }

    CsvRow row;
    std::wstring field;
    CsvParseState state = CsvFieldStart;
    BOOL pendingEmptyField = FALSE;

    for (size_t i = 0; i < text.length(); ++i) {
        WCHAR ch = text[i];

        if (state == CsvQuotedField) {
            if (ch == L'"') {
                if (i + 1 < text.length() && text[i + 1] == L'"') {
                    field.push_back(L'"');
                    ++i;
                } else {
                    state = CsvAfterQuote;
                }
            } else {
                field.push_back(ch);
            }
            continue;
        }

        if (state == CsvAfterQuote) {
            if (ch == L',') {
                CsvPushCell(row, field);
                pendingEmptyField = TRUE;
                state = CsvFieldStart;
            } else if (IsCsvNewline(ch)) {
                CsvFinishRow(table, row, maxColumns, field, TRUE);
                pendingEmptyField = FALSE;
                state = CsvFieldStart;
                CsvAdvanceNewline(text, &i);
            } else if (!IsCsvPostQuoteSpace(ch)) {
                table.clear();
                if (maxColumns) {
                    *maxColumns = 0;
                }
                return FALSE;
            }
            continue;
        }

        if (state == CsvUnquotedField) {
            if (ch == L'"') {
                table.clear();
                if (maxColumns) {
                    *maxColumns = 0;
                }
                return FALSE;
            }
            if (ch == L',') {
                CsvPushCell(row, field);
                pendingEmptyField = TRUE;
                state = CsvFieldStart;
            } else if (IsCsvNewline(ch)) {
                CsvFinishRow(table, row, maxColumns, field, TRUE);
                pendingEmptyField = FALSE;
                state = CsvFieldStart;
                CsvAdvanceNewline(text, &i);
            } else {
                field.push_back(ch);
            }
            continue;
        }

        if (ch == L'"') {
            pendingEmptyField = FALSE;
            state = CsvQuotedField;
        } else if (ch == L',') {
            CsvPushCell(row, field);
            pendingEmptyField = TRUE;
        } else if (IsCsvNewline(ch)) {
            if (pendingEmptyField) {
                CsvFinishRow(table, row, maxColumns, field, TRUE);
            } else {
                CsvFinishRow(table, row, maxColumns, field, !row.empty());
            }
            pendingEmptyField = FALSE;
            CsvAdvanceNewline(text, &i);
        } else {
            pendingEmptyField = FALSE;
            state = CsvUnquotedField;
            field.push_back(ch);
        }
    }

    if (state == CsvQuotedField) {
        table.clear();
        if (maxColumns) {
            *maxColumns = 0;
        }
        return FALSE;
    }
    if (state == CsvAfterQuote || state == CsvUnquotedField || pendingEmptyField || !row.empty()) {
        CsvFinishRow(table, row, maxColumns, field, TRUE);
    }

    return TRUE;
}

static void PopulateTableControlFromCsv(HWND listView, const std::wstring& csvPath) {
    if (!listView || csvPath.empty()) {
        return;
    }

    std::wstring csvText;
    if (!LoadTextFileStringLimited(csvPath.c_str(), XHL_CSV_MAX_BYTES, csvText)) {
        return;
    }

    CsvTable table;
    UINT maxColumns = 0;
    if (!ParseCsvText(csvText, table, &maxColumns)) {
        return;
    }
    if (table.empty() || maxColumns == 0) {
        return;
    }
    if (maxColumns > XHL_CSV_MAX_COLUMNS) {
        maxColumns = XHL_CSV_MAX_COLUMNS;
    }

    RECT rect;
    GetClientRect(listView, &rect);
    int totalWidth = rect.right - rect.left;
    int columnWidth = maxColumns ? totalWidth / (int)maxColumns : 100;
    if (columnWidth < 90) {
        columnWidth = 90;
    }

    for (UINT column = 0; column < maxColumns; ++column) {
        std::wstring heading;
        if (column < table[0].size()) {
            heading = table[0][column];
        }
        if (heading.empty()) {
            heading = L"Column ";
            WCHAR number[12];
            number[0] = L'\0';
            AppendUInt(number, 12, column + 1);
            heading += number;
        }

        LVCOLUMNW listColumn;
        ZeroMemory(&listColumn, sizeof(listColumn));
        listColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        listColumn.pszText = const_cast<LPWSTR>(heading.c_str());
        listColumn.cx = columnWidth;
        listColumn.iSubItem = (int)column;
        SendMessageW(listView, LVM_INSERTCOLUMNW, column, (LPARAM)&listColumn);
    }

    UINT rows = (UINT)table.size();
    if (rows > XHL_CSV_MAX_ROWS + 1) {
        rows = XHL_CSV_MAX_ROWS + 1;
    }
    for (UINT rowIndex = 1; rowIndex < rows; ++rowIndex) {
        CsvRow& row = table[rowIndex];
        std::wstring firstCell = row.empty() ? L"" : row[0];
        LVITEMW item;
        ZeroMemory(&item, sizeof(item));
        item.mask = LVIF_TEXT;
        item.iItem = (int)rowIndex - 1;
        item.iSubItem = 0;
        item.pszText = const_cast<LPWSTR>(firstCell.c_str());
        SendMessageW(listView, LVM_INSERTITEMW, 0, (LPARAM)&item);

        for (UINT column = 1; column < maxColumns; ++column) {
            std::wstring cell;
            if (column < row.size()) {
                cell = row[column];
            }
            LVITEMW subItem;
            ZeroMemory(&subItem, sizeof(subItem));
            subItem.iSubItem = (int)column;
            subItem.pszText = const_cast<LPWSTR>(cell.c_str());
            SendMessageW(listView, LVM_SETITEMTEXTW, (WPARAM)(rowIndex - 1), (LPARAM)&subItem);
        }
    }
}

static BOOL InitIniBuffers() {
    if (g_iniBytes && g_iniText && g_writeText) {
        return TRUE;
    }

    HANDLE heap = GetProcessHeap();
    g_iniBytes = (BYTE*)HeapAlloc(heap, HEAP_ZERO_MEMORY, XHL_INI_BYTES);
    g_iniText = (WCHAR*)HeapAlloc(heap, HEAP_ZERO_MEMORY, XHL_INI_CHARS * sizeof(WCHAR));
    g_writeText = (WCHAR*)HeapAlloc(heap, HEAP_ZERO_MEMORY, XHL_INI_CHARS * sizeof(WCHAR));
    if (!g_iniBytes || !g_iniText || !g_writeText) {
        if (g_iniBytes) {
            HeapFree(heap, 0, g_iniBytes);
        }
        if (g_iniText) {
            HeapFree(heap, 0, g_iniText);
        }
        if (g_writeText) {
            HeapFree(heap, 0, g_writeText);
        }
        g_iniBytes = 0;
        g_iniText = 0;
        g_writeText = 0;
        return FALSE;
    }

    return TRUE;
}

static void FreeIniBuffers() {
    HANDLE heap = GetProcessHeap();
    if (g_iniBytes) {
        HeapFree(heap, 0, g_iniBytes);
    }
    if (g_iniText) {
        HeapFree(heap, 0, g_iniText);
    }
    if (g_writeText) {
        HeapFree(heap, 0, g_writeText);
    }
    g_iniBytes = 0;
    g_iniText = 0;
    g_writeText = 0;
    g_iniBytesRead = 0;
    g_iniLoaded = FALSE;
    g_iniLoadError = IniLoadNone;
}

static int ShowTaskDialogWithButtons(HWND owner, const WCHAR* text, const WCHAR* title, PCWSTR icon, const TASKDIALOG_BUTTON* buttons, UINT buttonCount, int defaultButton) {
    TASKDIALOGCONFIG config;
    ZeroMemory(&config, sizeof(config));
    config.cbSize = sizeof(config);
    config.hwndParent = owner;
    config.dwFlags = TDF_SIZE_TO_CONTENT | TDF_ALLOW_DIALOG_CANCELLATION;
    config.pszWindowTitle = title ? title : g_displayTitle;
    config.pszContent = text ? text : L"";
    config.pszMainIcon = icon;
    config.cButtons = buttonCount;
    config.pButtons = buttons;
    config.nDefaultButton = defaultButton;

    int pressedButton = 0;
    HRESULT result = TaskDialogIndirect(&config, &pressedButton, 0, 0);
    return SUCCEEDED(result) ? pressedButton : 0;
}

static void ShowAppMessageForLanguage(HWND owner, const WCHAR* text, const WCHAR* title, PCWSTR icon, UINT fallbackFlags, BOOL chinese) {
    AppLanguage language = chinese ? (g_editorLanguageCombo ? GraphEditorLanguage() : ReadConfiguredLanguage()) : AppLanguageEnglish;
    TASKDIALOG_BUTTON button;
    button.nButtonID = IDOK;
    button.pszButtonText = LocalizedText(language, L"确定", L"確定", L"OK");

    if (!ShowTaskDialogWithButtons(owner, text, title, icon, &button, 1, IDOK)) {
        MessageBoxW(owner, text, title, MB_OK | fallbackFlags);
    }
}

static void ShowAppMessage(HWND owner, const WCHAR* text, const WCHAR* title, PCWSTR icon, UINT fallbackFlags) {
    ShowAppMessageForLanguage(owner, text, title, icon, fallbackFlags, IsChineseLanguage());
}

static int ShowAppYesNo(HWND owner, const WCHAR* text, const WCHAR* title, PCWSTR icon, UINT fallbackFlags) {
    AppLanguage language = ReadConfiguredLanguage();
    TASKDIALOG_BUTTON buttons[2];
    buttons[0].nButtonID = IDYES;
    buttons[0].pszButtonText = LocalizedText(language, L"是", L"是", L"Yes");
    buttons[1].nButtonID = IDNO;
    buttons[1].pszButtonText = LocalizedText(language, L"否", L"否", L"No");

    int pressed = ShowTaskDialogWithButtons(owner, text, title, icon, buttons, 2, IDYES);
    if (pressed == IDYES || pressed == IDNO) {
        return pressed;
    }
    return MessageBoxW(owner, text, title, MB_YESNO | fallbackFlags | MB_DEFBUTTON1);
}

static void ShowConfigReadFailedMessage() {
    AppLanguage language = ReadConfiguredLanguage();
    std::wstring message = LocalizedText(language,
        L"配置文件读取失败或内容无效。\r\n\r\n"
        L"常见原因：\r\n"
        L"1. 同名 INI 不存在、为空，或不是普通文件。\r\n"
        L"2. INI 过大。当前最多支持约 4 MB。\r\n"
        L"3. 编码错误。请使用 UTF-8，或 UTF-16 LE with BOM。\r\n"
        L"4. 当前目录没有读取权限，文件被占用，或磁盘/路径异常。\r\n"
        L"5. 配置项填写错误。Mode 只能是 0、1、2；数字开关只能是 0 或 1。\r\n\r\n"
        L"如果无法修复，请先备份同目录下的配置文件，再删除它。程序下次运行会自动释放一份默认配置。\r\n\r\n"
        L"配置文件路径：",
        L"配置文件讀取失敗或內容無效。\r\n\r\n"
        L"常見原因：\r\n"
        L"1. 同名 INI 不存在、為空，或不是普通文件。\r\n"
        L"2. INI 過大。目前最多支持約 4 MB。\r\n"
        L"3. 編碼錯誤。請使用 UTF-8，或 UTF-16 LE with BOM。\r\n"
        L"4. 目前目錄沒有讀取權限，文件被佔用，或磁碟/路徑異常。\r\n"
        L"5. 配置項填寫錯誤。Mode 只能是 0、1、2；數字開關只能是 0 或 1。\r\n\r\n"
        L"如果無法修復，請先備份同目錄下的配置文件，再刪除它。程序下次運行會自動釋放一份預設配置。\r\n\r\n"
        L"配置文件路徑：",
        L"Failed to read the configuration file, or its content is invalid.\r\n\r\n"
        L"Common causes:\r\n"
        L"1. The same-name INI is missing, empty, or not a normal file.\r\n"
        L"2. The INI is too large. The current limit is about 4 MB.\r\n"
        L"3. The encoding is wrong. Use UTF-8, or UTF-16 LE with BOM.\r\n"
        L"4. The folder has no read permission, the file is in use, or the disk/path has a problem.\r\n"
        L"5. A config value is invalid. Mode must be 0, 1, or 2. Number switches must be 0 or 1.\r\n\r\n"
        L"If you cannot fix it, back up the INI file in this folder first, then delete it. The program will create a default config next time.\r\n\r\n"
        L"Configuration file path: ");
    message += g_ctx.configPath;
    ShowAppMessage(0, message.c_str(), g_displayTitle, TD_ERROR_ICON, MB_ICONERROR);
}

static void ShowSaveFailedMessage(HWND owner) {
    AppLanguage language = g_editorLanguageCombo ? GraphEditorLanguage() : ReadConfiguredLanguage();
    BOOL chinese = LanguageIsChinese(language);
    ShowAppMessageForLanguage(owner,
        LocalizedText(language,
            L"配置保存失败。\r\n\r\n请检查 INI 文件是否只读、目录是否有写入权限、磁盘空间是否充足，或文件是否被其他程序占用。", L"配置保存失敗。\r\n\r\n請檢查 INI 文件是否只讀、目錄是否有寫入權限、磁碟空間是否充足，或文件是否被其他程序佔用。",
            L"Failed to save the configuration.\r\n\r\nPlease check whether the INI file is read-only, whether the folder allows writing, whether disk space is sufficient, or whether another program is using the file."),
        g_displayTitle,
        TD_ERROR_ICON,
        MB_ICONERROR,
        chinese);
}

static HFONT CreateAboutFont(UINT dpi, int weight) {
    return CreateFontW(
        -MulDiv(10, dpi ? dpi : 96, 72),
        0,
        0,
        0,
        weight,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Segoe UI");
}

static void AdjustWindowRectForDpiCompat(RECT* rect, DWORD style, BOOL menu, DWORD exStyle, UINT dpi) {
    typedef BOOL(WINAPI* AdjustWindowRectExForDpiProc)(LPRECT, DWORD, BOOL, DWORD, UINT);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    AdjustWindowRectExForDpiProc adjustForDpi = user32 ? (AdjustWindowRectExForDpiProc)GetProcAddress(user32, "AdjustWindowRectExForDpi") : 0;
    if (!adjustForDpi || !adjustForDpi(rect, style, menu, exStyle, dpi)) {
        AdjustWindowRectEx(rect, style, menu, exStyle);
    }
}

static void ApplyAboutFonts(AboutDialogState* state) {
    if (!state) {
        return;
    }
    if (state->title && state->boldFont) {
        SendMessageW(state->title, WM_SETFONT, (WPARAM)state->boldFont, TRUE);
    }
    if (state->body && state->font) {
        SendMessageW(state->body, WM_SETFONT, (WPARAM)state->font, TRUE);
    }
    if (state->link && state->font) {
        SendMessageW(state->link, WM_SETFONT, (WPARAM)state->font, TRUE);
    }
    if (state->okButton && state->font) {
        SendMessageW(state->okButton, WM_SETFONT, (WPARAM)state->font, TRUE);
    }
}

static void RecreateAboutFonts(AboutDialogState* state, UINT dpi) {
    if (!state) {
        return;
    }

    HFONT font = CreateAboutFont(dpi, FW_NORMAL);
    HFONT boldFont = CreateAboutFont(dpi, FW_BOLD);
    if (!font || !boldFont) {
        if (font) {
            DeleteObject(font);
        }
        if (boldFont) {
            DeleteObject(boldFont);
        }
        return;
    }

    if (state->font) {
        DeleteObject(state->font);
    }
    if (state->boldFont) {
        DeleteObject(state->boldFont);
    }
    state->font = font;
    state->boldFont = boldFont;
    state->dpi = dpi;
    ApplyAboutFonts(state);
}

static void LayoutAboutWindow(HWND window, AboutDialogState* state) {
    if (!window || !state) {
        return;
    }

    UINT dpi = state->dpi ? state->dpi : GetDpi(window);
    MoveWindow(state->title, ScaleForDpi(22, dpi), ScaleForDpi(18, dpi), ScaleForDpi(340, dpi), ScaleForDpi(26, dpi), TRUE);
    MoveWindow(state->body, ScaleForDpi(22, dpi), ScaleForDpi(54, dpi), ScaleForDpi(340, dpi), ScaleForDpi(54, dpi), TRUE);
    MoveWindow(state->link, ScaleForDpi(22, dpi), ScaleForDpi(116, dpi), ScaleForDpi(340, dpi), ScaleForDpi(26, dpi), TRUE);
    MoveWindow(state->okButton, ScaleForDpi((XHL_ABOUT_WINDOW_WIDTH - 82) / 2, dpi), ScaleForDpi(158, dpi), ScaleForDpi(82, dpi), ScaleForDpi(28, dpi), TRUE);
}

static void CenterWindowOverOwner(HWND window, HWND owner) {
    if (!window || !owner || !IsWindow(owner)) {
        CenterWindowOnMonitor(window);
        return;
    }

    RECT windowRect;
    RECT ownerRect;
    if (!GetWindowRect(window, &windowRect) || !GetWindowRect(owner, &ownerRect)) {
        CenterWindowOnMonitor(window);
        return;
    }

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    int ownerWidth = ownerRect.right - ownerRect.left;
    int ownerHeight = ownerRect.bottom - ownerRect.top;
    int x = ownerRect.left + (ownerWidth - windowWidth) / 2;
    int y = ownerRect.top + (ownerHeight - windowHeight) / 2;

    RECT workRect;
    HMONITOR monitor = MonitorFromWindow(owner, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo;
    ZeroMemory(&monitorInfo, sizeof(monitorInfo));
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
        workRect = monitorInfo.rcWork;
        if (x < workRect.left) x = workRect.left;
        if (y < workRect.top) y = workRect.top;
        if (x + windowWidth > workRect.right) x = workRect.right - windowWidth;
        if (y + windowHeight > workRect.bottom) y = workRect.bottom - windowHeight;
    }

    SetWindowPos(window, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

static void RestoreAboutOwner(AboutDialogState* state) {
    if (!state || state->ownerRestored) {
        return;
    }
    state->ownerRestored = TRUE;

    HWND owner = state->owner;
    if (!owner || !IsWindow(owner)) {
        return;
    }

    if (!IsWindowEnabled(owner)) {
        EnableWindow(owner, TRUE);
    }

    ShowWindow(owner, SW_SHOWNORMAL);
    SetWindowPos(owner, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetForegroundWindow(owner);
    SetActiveWindow(owner);
    SetFocus(owner);
}

static void CloseAboutWindow(HWND window, AboutDialogState* state) {
    RestoreAboutOwner(state);
    DestroyWindow(window);
}

static LRESULT CALLBACK AboutWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    AboutDialogState* state = (AboutDialogState*)GetWindowLongPtrW(window, GWLP_USERDATA);
    switch (message) {
    case WM_CREATE: {
        CREATESTRUCTW* create = (CREATESTRUCTW*)lParam;
        state = create ? (AboutDialogState*)create->lpCreateParams : 0;
        if (!state) {
            return -1;
        }
        SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR)state);
        RecreateAboutFonts(state, GetDpi(window));

        const WCHAR* bodyText = LocalizedText(state->language,
            L"小锤启动器是一个由 INI 配置驱动的 Windows 原生单文件启动器。", L"小錘啟動器是一個由 INI 配置驅動的 Windows 原生單文件啟動器。",
            L"Xiao Hammer Launcher is a native single-file Windows launcher driven by an INI file.");

        state->title = CreateWindowExW(0, L"STATIC", LocalizedText(state->language, L"小锤启动器", L"小錘啟動器", L"Xiao Hammer Launcher"), WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE | SS_NOPREFIX,
            0, 0, 0, 0, window, 0, GetModuleHandleW(0), 0);
        state->body = CreateWindowExW(0, L"STATIC", bodyText, WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
            0, 0, 0, 0, window, 0, GetModuleHandleW(0), 0);
        state->link = CreateWindowExW(0, L"SysLink", L"<a href=\"https://github.com/FionViber/XiaoHammerLauncher\">https://github.com/FionViber/XiaoHammerLauncher</a>",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, 0, 0, 0, window, 0, GetModuleHandleW(0), 0);
        if (!state->link) {
            state->link = CreateWindowExW(0, L"STATIC", XHL_GITHUB_URL, WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
                0, 0, 0, 0, window, 0, GetModuleHandleW(0), 0);
        }
        state->okButton = CreateWindowExW(0, L"BUTTON", LocalizedText(state->language, L"关闭", L"關閉", L"Close"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
            0, 0, 0, 0, window, (HMENU)(UINT_PTR)XHL_ABOUT_ID_OK, GetModuleHandleW(0), 0);
        ApplyAboutFonts(state);
        LayoutAboutWindow(window, state);
        return 0;
    }
    case WM_DPICHANGED: {
        UINT dpi = HIWORD(wParam);
        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = ScaleForDpi(XHL_ABOUT_WINDOW_WIDTH, dpi);
        rect.bottom = ScaleForDpi(XHL_ABOUT_WINDOW_HEIGHT, dpi);
        DWORD style = (DWORD)GetWindowLongPtrW(window, GWL_STYLE);
        DWORD exStyle = (DWORD)GetWindowLongPtrW(window, GWL_EXSTYLE);
        AdjustWindowRectForDpiCompat(&rect, style, FALSE, exStyle, dpi);

        RECT* suggested = (RECT*)lParam;
        int x = suggested ? suggested->left : 0;
        int y = suggested ? suggested->top : 0;
        SetWindowPos(window, 0, x, y, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOACTIVATE);
        RecreateAboutFonts(state, dpi);
        LayoutAboutWindow(window, state);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == XHL_ABOUT_ID_OK && HIWORD(wParam) == BN_CLICKED) {
            CloseAboutWindow(window, state);
            return 0;
        }
        break;
    case WM_NOTIFY: {
        LPNMHDR header = (LPNMHDR)lParam;
        if (state && header && header->hwndFrom == state->link
            && (header->code == NM_CLICK || header->code == NM_RETURN)) {
            ShellExecuteW(window, L"open", XHL_GITHUB_URL, 0, 0, SW_SHOWNORMAL);
            return TRUE;
        }
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC dc = (HDC)wParam;
        SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));
        SetBkMode(dc, TRANSPARENT);
        return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
    }
    case WM_CTLCOLORBTN:
        return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
    case WM_CLOSE:
        CloseAboutWindow(window, state);
        return 0;
    case WM_DESTROY:
        if (state) {
            RestoreAboutOwner(state);
            if (state->font) {
                DeleteObject(state->font);
                state->font = 0;
            }
            if (state->boldFont) {
                DeleteObject(state->boldFont);
                state->boldFont = 0;
            }
            state->done = TRUE;
        }
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

static void ShowAboutMessage(HWND owner) {
    AppLanguage language = g_editorLanguageCombo ? GraphEditorLanguage() : ReadConfiguredLanguage();
    BOOL chinese = LanguageIsChinese(language);
    HINSTANCE instance = GetModuleHandleW(0);
    WNDCLASSEXW windowClass;
    ZeroMemory(&windowClass, sizeof(windowClass));
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = AboutWindowProc;
    windowClass.hInstance = instance;
    windowClass.hIcon = (HICON)LoadImageW(instance, MAKEINTRESOURCEW(XHL_APP_ICON_ID), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR | LR_SHARED);
    windowClass.hIconSm = (HICON)LoadImageW(instance, MAKEINTRESOURCEW(XHL_APP_ICON_ID), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED);
    windowClass.hCursor = LoadCursorW(0, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.lpszClassName = L"XHLauncherAboutWindow";
    if (!RegisterClassExW(&windowClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        ShowAppMessageForLanguage(owner, LocalizedText(language, L"小锤启动器", L"小錘啟動器", L"Xiao Hammer Launcher"), LocalizedText(language, L"关于", L"關於", L"About"), TD_INFORMATION_ICON, MB_ICONINFORMATION, chinese);
        return;
    }

    AboutDialogState state;
    ZeroMemory(&state, sizeof(state));
    state.owner = owner;
    state.language = language;
    state.chinese = chinese;
    state.dpi = GetDpi(owner);

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    DWORD exStyle = WS_EX_DLGMODALFRAME;
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = ScaleForDpi(XHL_ABOUT_WINDOW_WIDTH, state.dpi);
    rect.bottom = ScaleForDpi(XHL_ABOUT_WINDOW_HEIGHT, state.dpi);
    AdjustWindowRectForDpiCompat(&rect, style, FALSE, exStyle, state.dpi);

    HWND window = CreateWindowExW(
        exStyle,
        L"XHLauncherAboutWindow",
        LocalizedText(state.language, L"关于", L"關於", L"About"),
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        owner,
        0,
        instance,
        &state);
    if (!window) {
        ShowAppMessageForLanguage(owner, LocalizedText(language, L"小锤启动器", L"小錘啟動器", L"Xiao Hammer Launcher"), LocalizedText(language, L"关于", L"關於", L"About"), TD_INFORMATION_ICON, MB_ICONINFORMATION, chinese);
        return;
    }

    if (owner && IsWindow(owner)) {
        EnableWindow(owner, FALSE);
    }
    CenterWindowOverOwner(window, owner);
    ShowWindow(window, SW_SHOWNORMAL);
    UpdateWindow(window);

    MSG message;
    while (!state.done && GetMessageW(&message, 0, 0, 0) > 0) {
        if (!IsDialogMessageW(window, &message)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    if (owner && IsWindow(owner)) {
        EnableWindow(owner, TRUE);
        if (!state.ownerRestored) {
            SetActiveWindow(owner);
        }
    }
}

static BOOL LoadIniText() {
    if (!g_iniBytes || !g_iniText) {
        return FALSE;
    }
    if (g_iniLoaded) {
        return TRUE;
    }

    g_iniLoadError = IniLoadNone;
    g_iniBytesRead = 0;
    g_iniText[0] = L'\0';

    HANDLE file = CreateFileW(g_ctx.configPath.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE) {
        g_iniLoadError = IniLoadOpenFailed;
        return FALSE;
    }

    LARGE_INTEGER fileSize;
    fileSize.QuadPart = 0;
    if (!GetFileSizeEx(file, &fileSize) || fileSize.QuadPart <= 0) {
        CloseHandle(file);
        g_iniLoadError = IniLoadInvalidSize;
        return FALSE;
    }
    if (fileSize.QuadPart > XHL_INI_BYTES) {
        CloseHandle(file);
        g_iniLoadError = IniLoadTooLarge;
        return FALSE;
    }

    DWORD bytesRead = 0;
    BOOL readOk = ReadFile(file, g_iniBytes, (DWORD)fileSize.QuadPart, &bytesRead, 0);
    CloseHandle(file);
    if (!readOk || bytesRead != (DWORD)fileSize.QuadPart || bytesRead == 0) {
        g_iniLoadError = IniLoadReadFailed;
        return FALSE;
    }
    g_iniBytesRead = bytesRead;

    if (bytesRead >= 2 && g_iniBytes[0] == 0xFF && g_iniBytes[1] == 0xFE) {
        if (((bytesRead - 2) % 2) != 0) {
            g_iniLoadError = IniLoadInvalidEncoding;
            g_iniText[0] = L'\0';
            return FALSE;
        }
        UINT chars = (bytesRead - 2) / 2;
        if (chars >= XHL_INI_CHARS) {
            chars = XHL_INI_CHARS - 1;
        }
        if (chars > 0) {
            CopyMemory(g_iniText, g_iniBytes + 2, chars * sizeof(WCHAR));
        }
        g_iniText[chars] = L'\0';
        g_iniLoaded = TRUE;
        return TRUE;
    }

    DWORD offset = 0;
    if (bytesRead >= 3 && g_iniBytes[0] == 0xEF && g_iniBytes[1] == 0xBB && g_iniBytes[2] == 0xBF) {
        offset = 3;
    }

    int chars = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCCH)(g_iniBytes + offset), bytesRead - offset, g_iniText, XHL_INI_CHARS - 1);
    if (chars <= 0) {
        g_iniLoadError = IniLoadInvalidEncoding;
        g_iniText[0] = L'\0';
        return FALSE;
    }

    g_iniText[chars] = L'\0';
    g_iniLoaded = TRUE;
    return TRUE;
}

static void ReadIniText(const WCHAR* section, const WCHAR* key, const WCHAR* defaultValue, WCHAR* out, UINT capacity) {
    CopyText(out, capacity, defaultValue);
    if (!LoadIniText()) {
        return;
    }

    WCHAR currentSection[128];
    currentSection[0] = L'\0';

    const WCHAR* cursor = g_iniText;
    while (*cursor) {
        const WCHAR* lineStart = cursor;
        while (*cursor && *cursor != L'\r' && *cursor != L'\n') {
            ++cursor;
        }
        const WCHAR* rawLineEnd = cursor;
        const WCHAR* lineEnd = rawLineEnd;
        while (*cursor == L'\r' || *cursor == L'\n') {
            ++cursor;
        }

        while (lineStart < lineEnd && IsSpace(*lineStart)) {
            ++lineStart;
        }
        while (lineEnd > lineStart && IsSpace(*(lineEnd - 1))) {
            --lineEnd;
        }

        if (lineStart == lineEnd || *lineStart == L';' || *lineStart == L'#') {
            continue;
        }

        if (*lineStart == L'[' && lineEnd > lineStart + 2 && *(lineEnd - 1) == L']') {
            CopyTrimmedSegment(currentSection, 128, lineStart + 1, lineEnd - 1);
            continue;
        }

        if (!SameTextI(currentSection, section)) {
            continue;
        }

        const WCHAR* equals = lineStart;
        while (equals < lineEnd && *equals != L'=') {
            ++equals;
        }
        if (equals == lineEnd) {
            continue;
        }

        const WCHAR* keyStart = lineStart;
        const WCHAR* keyEnd = equals;
        while (keyStart < keyEnd && IsSpace(*keyStart)) {
            ++keyStart;
        }
        while (keyEnd > keyStart && IsSpace(*(keyEnd - 1))) {
            --keyEnd;
        }

        if (SegmentEqualsI(keyStart, keyEnd, key)) {
            CopySegment(out, capacity, equals + 1, rawLineEnd);
            break;
        }
    }
}

static BOOL TryReadIniText(const WCHAR* section, const WCHAR* key, WCHAR* out, UINT capacity) {
    if (capacity > 0) {
        out[0] = L'\0';
    }
    if (!LoadIniText()) {
        return FALSE;
    }

    WCHAR currentSection[128];
    currentSection[0] = L'\0';

    const WCHAR* cursor = g_iniText;
    while (*cursor) {
        const WCHAR* lineStart = cursor;
        while (*cursor && *cursor != L'\r' && *cursor != L'\n') {
            ++cursor;
        }
        const WCHAR* rawLineEnd = cursor;
        const WCHAR* lineEnd = rawLineEnd;
        while (*cursor == L'\r' || *cursor == L'\n') {
            ++cursor;
        }

        while (lineStart < lineEnd && IsSpace(*lineStart)) {
            ++lineStart;
        }
        while (lineEnd > lineStart && IsSpace(*(lineEnd - 1))) {
            --lineEnd;
        }

        if (lineStart == lineEnd || *lineStart == L';' || *lineStart == L'#') {
            continue;
        }

        if (*lineStart == L'[' && lineEnd > lineStart + 2 && *(lineEnd - 1) == L']') {
            CopyTrimmedSegment(currentSection, 128, lineStart + 1, lineEnd - 1);
            continue;
        }

        if (!SameTextI(currentSection, section)) {
            continue;
        }

        const WCHAR* equals = lineStart;
        while (equals < lineEnd && *equals != L'=') {
            ++equals;
        }
        if (equals == lineEnd) {
            continue;
        }

        const WCHAR* keyStart = lineStart;
        const WCHAR* keyEnd = equals;
        while (keyStart < keyEnd && IsSpace(*keyStart)) {
            ++keyStart;
        }
        while (keyEnd > keyStart && IsSpace(*(keyEnd - 1))) {
            --keyEnd;
        }

        if (SegmentEqualsI(keyStart, keyEnd, key)) {
            CopySegment(out, capacity, equals + 1, rawLineEnd);
            return TRUE;
        }
    }

    return FALSE;
}

static std::wstring SegmentString(const WCHAR* start, const WCHAR* end) {
    return std::wstring(start, end);
}

static std::wstring ReadIniTextString(const WCHAR* section, const WCHAR* key, const WCHAR* defaultValue) {
    if (!LoadIniText()) {
        return defaultValue ? std::wstring(defaultValue) : std::wstring();
    }

    WCHAR currentSection[128];
    currentSection[0] = L'\0';

    const WCHAR* cursor = g_iniText;
    while (*cursor) {
        const WCHAR* lineStart = cursor;
        while (*cursor && *cursor != L'\r' && *cursor != L'\n') {
            ++cursor;
        }
        const WCHAR* rawLineEnd = cursor;
        const WCHAR* lineEnd = rawLineEnd;
        while (*cursor == L'\r' || *cursor == L'\n') {
            ++cursor;
        }

        while (lineStart < lineEnd && IsSpace(*lineStart)) {
            ++lineStart;
        }
        while (lineEnd > lineStart && IsSpace(*(lineEnd - 1))) {
            --lineEnd;
        }

        if (lineStart == lineEnd || *lineStart == L';' || *lineStart == L'#') {
            continue;
        }

        if (*lineStart == L'[' && lineEnd > lineStart + 2 && *(lineEnd - 1) == L']') {
            CopyTrimmedSegment(currentSection, 128, lineStart + 1, lineEnd - 1);
            continue;
        }

        if (!SameTextI(currentSection, section)) {
            continue;
        }

        const WCHAR* equals = lineStart;
        while (equals < lineEnd && *equals != L'=') {
            ++equals;
        }
        if (equals == lineEnd) {
            continue;
        }

        const WCHAR* keyStart = lineStart;
        const WCHAR* keyEnd = equals;
        while (keyStart < keyEnd && IsSpace(*keyStart)) {
            ++keyStart;
        }
        while (keyEnd > keyStart && IsSpace(*(keyEnd - 1))) {
            --keyEnd;
        }

        if (SegmentEqualsI(keyStart, keyEnd, key)) {
            return SegmentString(equals + 1, rawLineEnd);
        }
    }

    return defaultValue ? std::wstring(defaultValue) : std::wstring();
}

static BOOL ReadTrimmedFlagValue(const WCHAR* value, BOOL defaultValue) {
    if (!value) {
        return defaultValue;
    }
    while (*value && IsSpace(*value)) {
        ++value;
    }
    const WCHAR* end = value + StrLen(value);
    while (end > value && IsSpace(*(end - 1))) {
        --end;
    }
    if (end - value == 1) {
        if (*value == L'0') {
            return FALSE;
        }
        if (*value == L'1') {
            return TRUE;
        }
    }
    return defaultValue;
}

static BOOL ReadIniFlag(const WCHAR* section, const WCHAR* key, BOOL defaultValue) {
    WCHAR value[16];
    ReadIniText(section, key, defaultValue ? L"1" : L"0", value, 16);
    return ReadTrimmedFlagValue(value, defaultValue);
}

static UINT ParseUIntInRange(const WCHAR* text, UINT defaultValue, UINT minValue, UINT maxValue) {
    if (!text || !text[0]) {
        return defaultValue;
    }

    while (*text && IsSpace(*text)) {
        ++text;
    }
    const WCHAR* end = text + StrLen(text);
    while (end > text && IsSpace(*(end - 1))) {
        --end;
    }
    if (end == text) {
        return defaultValue;
    }

    unsigned long long value = 0;
    for (const WCHAR* cursor = text; cursor < end; ++cursor) {
        if (*cursor < L'0' || *cursor > L'9') {
            return defaultValue;
        }
        value = value * 10 + (unsigned)(*cursor - L'0');
        if (value > maxValue) {
            return maxValue;
        }
    }

    if (value < minValue) {
        return minValue;
    }
    return (UINT)value;
}

static const WCHAR* Mode1RegistrySection();
static void Mode1RunItemSection(UINT index, WCHAR* out, UINT capacity);
static void Mode1VariableSection(UINT index, WCHAR* out, UINT capacity);
static void Mode2ElementSection(UINT index, WCHAR* out, UINT capacity);
static Mode2ElementKind Mode2KindFromType(const WCHAR* type);

static BOOL IsZeroOrOneText(const WCHAR* value) {
    while (*value && IsSpace(*value)) {
        ++value;
    }
    const WCHAR* end = value + StrLen(value);
    while (end > value && IsSpace(*(end - 1))) {
        --end;
    }
    if (end - value != 1) {
        return FALSE;
    }
    return *value == L'0' || *value == L'1';
}

static BOOL ValidateIniFlagValue(const WCHAR* section, const WCHAR* key) {
    WCHAR value[64];
    if (!TryReadIniText(section, key, value, 64)) {
        return TRUE;
    }
    return IsZeroOrOneText(value);
}

static BOOL ValidateIniUIntRangeValue(const WCHAR* section, const WCHAR* key, UINT minValue, UINT maxValue) {
    WCHAR value[64];
    if (!TryReadIniText(section, key, value, 64)) {
        return TRUE;
    }
    TrimInPlace(value);
    if (!value[0]) {
        return FALSE;
    }

    unsigned long long number = 0;
    for (UINT i = 0; value[i]; ++i) {
        if (value[i] < L'0' || value[i] > L'9') {
            return FALSE;
        }
        number = number * 10 + (unsigned)(value[i] - L'0');
        if (number > maxValue) {
            return FALSE;
        }
    }

    return number >= minValue && number <= maxValue;
}

static BOOL ValidateSplashPositionValue() {
    WCHAR value[64];
    if (!TryReadIniText(L"Launcher", L"SplashPosition", value, 64)) {
        return TRUE;
    }
    TrimInPlace(value);
    if (!value[0]) {
        return FALSE;
    }
    return SameTextI(value, L"Center")
        || SameTextI(value, L"TopLeft")
        || SameTextI(value, L"TopRight")
        || SameTextI(value, L"BottomLeft")
        || SameTextI(value, L"BottomRight");
}

static BOOL IsValidLanguageText(WCHAR* value) {
    TrimInPlace(value);
    return SameTextI(value, L"en-US")
        || SameTextI(value, L"zh-Hans")
        || SameTextI(value, L"zh-Hant");
}

static BOOL ValidateLanguageValue() {
    WCHAR value[64];
    if (!TryReadIniText(L"Launcher", L"Language", value, 64)) {
        return TRUE;
    }
    return IsValidLanguageText(value);
}

static BOOL IsValidWorkingDirectoryModeText(WCHAR* value) {
    TrimInPlace(value);
    return SameTextI(value, L"TargetDir")
        || SameTextI(value, L"LauncherDir")
        || SameTextI(value, L"Custom");
}

static BOOL ValidateWorkingDirectoryModeValue(const WCHAR* section, const WCHAR* key) {
    WCHAR value[64];
    if (!TryReadIniText(section, key, value, 64)) {
        return TRUE;
    }
    return IsValidWorkingDirectoryModeText(value);
}

static BOOL FailInvalidConfig() {
    g_iniLoadError = IniLoadInvalidConfig;
    return FALSE;
}

static BOOL ReadIniFlagForValidation(const WCHAR* section, const WCHAR* key, BOOL defaultValue, BOOL* out) {
    WCHAR value[64];
    if (!TryReadIniText(section, key, value, 64)) {
        if (out) {
            *out = defaultValue;
        }
        return TRUE;
    }
    if (!IsZeroOrOneText(value)) {
        return FALSE;
    }
    TrimInPlace(value);
    if (out) {
        *out = value[0] == L'1';
    }
    return TRUE;
}

static BOOL ValidateLauncherNumericConfig(LauncherMode* mode) {
    if (!ValidateLanguageValue()) {
        return FailInvalidConfig();
    }

    WCHAR value[64];
    if (!TryReadIniText(L"Launcher", L"Mode", value, 64)) {
        return FailInvalidConfig();
    }
    TrimInPlace(value);

    if (value[0] == L'0' && value[1] == L'\0') {
        if (mode) *mode = ModeDisabled;
    } else if (value[0] == L'1' && value[1] == L'\0') {
        if (mode) *mode = ModeSilent;
    } else if (value[0] == L'2' && value[1] == L'\0') {
        if (mode) *mode = ModeWindow;
    } else {
        return FailInvalidConfig();
    }

    BOOL splashEnabled = FALSE;
    if (!ReadIniFlagForValidation(L"Launcher", L"SplashEnable", FALSE, &splashEnabled)) {
        return FailInvalidConfig();
    }
    if (splashEnabled) {
        if (!ValidateIniFlagValue(L"Launcher", L"SplashUseRelativePath")) {
            return FailInvalidConfig();
        }
        if (!ValidateIniUIntRangeValue(L"Launcher", L"SplashDurationMs", 1, 60000)) {
            return FailInvalidConfig();
        }
        if (!ValidateIniUIntRangeValue(L"Launcher", L"SplashScale", XHL_SPLASH_MIN_SCALE, XHL_SPLASH_MAX_SCALE)) {
            return FailInvalidConfig();
        }
        if (!ValidateSplashPositionValue()) {
            return FailInvalidConfig();
        }
    }

    return TRUE;
}

static BOOL ValidateMode1NumericConfig() {
    for (UINT i = 1; i <= XHL_MODE1_VARIABLE_COUNT; ++i) {
        WCHAR section[32];
        Mode1VariableSection(i, section, 32);
        if (!ValidateIniFlagValue(section, L"Enable")) {
            return FailInvalidConfig();
        }
    }

    const WCHAR* registrySection = Mode1RegistrySection();
    BOOL addReg = FALSE;
    if (!ReadIniFlagForValidation(registrySection, L"AddReg", FALSE, &addReg)) {
        return FailInvalidConfig();
    }
    if (addReg) {
        BOOL regImported = FALSE;
        if (!ReadIniFlagForValidation(registrySection, L"RegImported", FALSE, &regImported)) {
            return FailInvalidConfig();
        }
        if (!regImported && !ValidateIniFlagValue(registrySection, L"UseRelativePath")) {
            return FailInvalidConfig();
        }
        if (!regImported && !ValidateWorkingDirectoryModeValue(registrySection, L"WorkingDirectoryMode")) {
            return FailInvalidConfig();
        }
    }

    for (UINT i = 1; i <= XHL_MODE1_RUN_COUNT; ++i) {
        WCHAR section[32];
        Mode1RunItemSection(i, section, 32);
        BOOL runEnabled = FALSE;
        if (!ReadIniFlagForValidation(section, L"Run", FALSE, &runEnabled)) {
            return FailInvalidConfig();
        }
        if (runEnabled && !ValidateIniFlagValue(section, L"UseRelativePath")) {
            return FailInvalidConfig();
        }
        if (runEnabled && !ValidateWorkingDirectoryModeValue(section, L"WorkingDirectoryMode")) {
            return FailInvalidConfig();
        }
    }

    return TRUE;
}

static BOOL ValidateMode2NumericConfig() {
    for (UINT i = 1; i <= XHL_MODE2_ELEMENT_COUNT; ++i) {
        WCHAR section[32];
        Mode2ElementSection(i, section, 32);
        BOOL show = FALSE;
        if (!ReadIniFlagForValidation(section, L"Show", FALSE, &show)) {
            return FailInvalidConfig();
        }
        if (!show) {
            continue;
        }

        WCHAR type[32];
        ReadIniText(section, L"Type", L"", type, 32);
        Mode2ElementKind kind = Mode2KindFromType(type);
        if (kind == Mode2ElementNone) {
            return FailInvalidConfig();
        }

        if ((kind == Mode2ElementOpenButton || kind == Mode2ElementMessageButton || kind == Mode2ElementRegButton)
            && !ValidateIniFlagValue(section, L"MinimizeOnClick")) {
            return FailInvalidConfig();
        }

        if ((kind == Mode2ElementTextBox || kind == Mode2ElementTextFileBox || kind == Mode2ElementTable)
            && !ValidateIniUIntRangeValue(section, L"HeightScale", 1, 3)) {
            return FailInvalidConfig();
        }

        if (kind == Mode2ElementInput
            && !ValidateIniUIntRangeValue(section, L"Variable", 1, XHL_MODE2_VARIABLE_COUNT)) {
            return FailInvalidConfig();
        }

        if (kind == Mode2ElementRegButton
            && !ValidateIniFlagValue(section, L"UseRelativePath")) {
            return FailInvalidConfig();
        }
        if (kind == Mode2ElementRegButton
            && !ValidateWorkingDirectoryModeValue(section, L"WorkingDirectoryMode")) {
            return FailInvalidConfig();
        }

        if (kind == Mode2ElementTable
            && !ValidateIniFlagValue(section, L"CsvUseRelativePath")) {
            return FailInvalidConfig();
        }

        if (kind == Mode2ElementTextFileBox
            && !ValidateIniFlagValue(section, L"TextFileUseRelativePath")) {
            return FailInvalidConfig();
        }

        if (kind == Mode2ElementOpenButton) {
            for (UINT run = 1; run <= XHL_MODE2_BUTTON_RUN_COUNT; ++run) {
                WCHAR key[32];
                CopyText(key, 32, L"Run");
                AppendUInt(key, 32, run);

                BOOL runEnabled = FALSE;
                if (!ReadIniFlagForValidation(section, key, FALSE, &runEnabled)) {
                    return FailInvalidConfig();
                }
                if (!runEnabled) {
                    continue;
                }

                CopyText(key, 32, L"UseRelativePath");
                AppendUInt(key, 32, run);
                if (!ValidateIniFlagValue(section, key)) {
                    return FailInvalidConfig();
                }

                CopyText(key, 32, L"WorkingDirectoryMode");
                AppendUInt(key, 32, run);
                if (!ValidateWorkingDirectoryModeValue(section, key)) {
                    return FailInvalidConfig();
                }
            }
        }
    }

    return TRUE;
}

static BOOL ValidateModeNumericConfig(LauncherMode mode) {
    if (mode == ModeSilent) {
        return ValidateMode1NumericConfig();
    }
    if (mode == ModeWindow) {
        return ValidateMode2NumericConfig();
    }
    return TRUE;
}

static BOOL UpdateIniTextValue(std::wstring& iniText, const WCHAR* section, const WCHAR* key, const WCHAR* value) {
    if (!section || !section[0] || !key || !key[0]) {
        return FALSE;
    }

    const WCHAR* writeValue = value ? value : L"";
    std::wstring output;
    output.reserve(iniText.length() + StrLen(section) + StrLen(key) + StrLen(writeValue) + 16);

    WCHAR currentSection[128];
    currentSection[0] = L'\0';

    BOOL foundSection = FALSE;
    BOOL wroteKey = FALSE;

    const WCHAR* cursor = iniText.c_str();
    while (*cursor) {
        const WCHAR* lineStart = cursor;
        while (*cursor && *cursor != L'\r' && *cursor != L'\n') {
            ++cursor;
        }
        const WCHAR* lineEnd = cursor;
        while (*cursor == L'\r' || *cursor == L'\n') {
            ++cursor;
        }
        const WCHAR* nextLine = cursor;

        const WCHAR* trimmedStart = lineStart;
        const WCHAR* trimmedEnd = lineEnd;
        while (trimmedStart < trimmedEnd && IsSpace(*trimmedStart)) {
            ++trimmedStart;
        }
        while (trimmedEnd > trimmedStart && IsSpace(*(trimmedEnd - 1))) {
            --trimmedEnd;
        }

        BOOL isSectionLine = *trimmedStart == L'[' && trimmedEnd > trimmedStart + 2 && *(trimmedEnd - 1) == L']';
        if (isSectionLine && foundSection && !wroteKey) {
            output += key;
            output += L"=";
            output += writeValue;
            output += L"\r\n";
            wroteKey = TRUE;
        }

        if (isSectionLine) {
            CopyTrimmedSegment(currentSection, 128, trimmedStart + 1, trimmedEnd - 1);
            if (SameTextI(currentSection, section)) {
                foundSection = TRUE;
            }
        } else if (foundSection && !wroteKey && SameTextI(currentSection, section) && trimmedStart < trimmedEnd && *trimmedStart != L';' && *trimmedStart != L'#') {
            const WCHAR* equals = trimmedStart;
            while (equals < trimmedEnd && *equals != L'=') {
                ++equals;
            }

            if (equals < trimmedEnd) {
                const WCHAR* keyStart = trimmedStart;
                const WCHAR* keyEnd = equals;
                while (keyStart < keyEnd && IsSpace(*keyStart)) {
                    ++keyStart;
                }
                while (keyEnd > keyStart && IsSpace(*(keyEnd - 1))) {
                    --keyEnd;
                }

                if (SegmentEqualsI(keyStart, keyEnd, key)) {
                    output += key;
                    output += L"=";
                    output += writeValue;
                    AppendSegmentString(output, lineEnd, nextLine);
                    wroteKey = TRUE;
                    continue;
                }
            }
        }

        AppendSegmentString(output, lineStart, nextLine);
    }

    if (!foundSection) {
        if (!output.empty() && !EndsWithLineBreakString(output)) {
            output += L"\r\n";
        }
        output += L"[";
        output += section;
        output += L"]\r\n";
    }

    if (!wroteKey) {
        if (!output.empty() && !EndsWithLineBreakString(output)) {
            output += L"\r\n";
        }
        output += key;
        output += L"=";
        output += writeValue;
        output += L"\r\n";
    }

    if (output.length() >= XHL_INI_CHARS) {
        return FALSE;
    }

    iniText.swap(output);
    return TRUE;
}

static BOOL WriteUtf8TextFileAtomic(const WCHAR* path, const std::wstring& text) {
    if (!path || !path[0] || text.length() >= XHL_INI_CHARS) {
        return FALSE;
    }

    int byteCount = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.length(), 0, 0, 0, 0);
    if ((byteCount == 0 && !text.empty()) || byteCount > XHL_INI_BYTES) {
        return FALSE;
    }

    std::string bytes;
    if (byteCount > 0) {
        bytes.resize(byteCount);
        int converted = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.length(), &bytes[0], byteCount, 0, 0);
        if (converted != byteCount) {
            return FALSE;
        }
    }

    std::wstring tempPath;
    if (!BuildTempPathForTargetString(path, tempPath)) {
        return FALSE;
    }

    DeleteFileW(tempPath.c_str());
    HANDLE file = CreateFileW(tempPath.c_str(), GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    DWORD expectedBytes = (DWORD)bytes.size();
    DWORD bytesWritten = 0;
    BOOL writeOk = expectedBytes == 0 ? TRUE : WriteFile(file, bytes.data(), expectedBytes, &bytesWritten, 0);
    BOOL flushOk = writeOk && bytesWritten == expectedBytes && FlushFileBuffers(file);
    CloseHandle(file);
    if (!flushOk) {
        DeleteFileW(tempPath.c_str());
        return FALSE;
    }

    if (!MoveFileExW(tempPath.c_str(), path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(tempPath.c_str());
        return FALSE;
    }

    g_iniLoaded = FALSE;
    return TRUE;
}

static BOOL WriteIniTextUtf8(const WCHAR* section, const WCHAR* key, const WCHAR* value) {
    if (!g_iniBytes || !g_iniText) {
        return FALSE;
    }

    g_iniLoaded = FALSE;
    if (!LoadIniText()) {
        return FALSE;
    }

    std::wstring updated = g_iniText;
    if (!UpdateIniTextValue(updated, section, key, value)) {
        return FALSE;
    }

    return WriteUtf8TextFileAtomic(g_ctx.configPath.c_str(), updated);
}

static LauncherMode ReadMode() {
    WCHAR value[16];
    ReadIniText(L"Launcher", L"Mode", L"0", value, 16);
    TrimInPlace(value);
    if (value[0] == L'0' && value[1] == L'\0') {
        return ModeDisabled;
    }
    if (value[0] == L'1' && value[1] == L'\0') {
        return ModeSilent;
    }
    if (value[0] == L'2' && value[1] == L'\0') {
        return ModeWindow;
    }
    return ModeDisabled;
}

static void ApplyModeVariables(LauncherMode mode, WCHAR* text, UINT capacity);
static BOOL ApplyModeVariablesString(LauncherMode mode, std::wstring& text);

static void SetDefaultDisplayTitle() {
    CopyText(g_displayTitle, XHL_MAX_TEXT, L"Xiao Hammer Launcher");
}

static void ComposeDisplayTitle(const WCHAR* title, const WCHAR* version, WCHAR* out, UINT capacity) {
    if (title && title[0]) {
        CopyText(out, capacity, title);
    } else {
        CopyText(out, capacity, L"Xiao Hammer Launcher");
    }

    if (version && version[0]) {
        AppendText(out, capacity, L" ");
        AppendText(out, capacity, version);
    }
}

static void ReadDisplayTitle(LauncherMode mode) {
    std::wstring title = ReadIniTextString(L"Launcher", L"Title", L"Xiao Hammer Launcher");
    std::wstring version = ReadIniTextString(L"Launcher", L"Version", L"");
    if (mode == ModeSilent || mode == ModeWindow) {
        (void)ApplyModeVariablesString(mode, title);
        (void)ApplyModeVariablesString(mode, version);
    }

    TruncateString(title, XHL_TITLE_MAX_CHARS);
    TruncateString(version, XHL_CONFIG_VERSION_MAX_CHARS);
    ComposeDisplayTitle(title.c_str(), version.c_str(), g_displayTitle, XHL_MAX_TEXT);
}

static const WCHAR* Mode1RegistrySection() {
    return L"Mode1.Registry";
}

static void Mode1RunItemSection(UINT index, WCHAR* out, UINT capacity) {
    CopyText(out, capacity, L"Mode1.Run");
    AppendUInt(out, capacity, index);
}

static void Mode1VariableSection(UINT index, WCHAR* out, UINT capacity) {
    CopyText(out, capacity, L"Mode1.Variable");
    AppendUInt(out, capacity, index);
}

static void Mode2ElementSection(UINT index, WCHAR* out, UINT capacity) {
    CopyText(out, capacity, L"Mode2.Element");
    AppendUInt(out, capacity, index);
}

static BOOL IsMode2ButtonKind(Mode2ElementKind kind) {
    return kind == Mode2ElementOpenButton
        || kind == Mode2ElementMessageButton
        || kind == Mode2ElementRegButton
        || kind == Mode2ElementCloseButton;
}

static Mode2ElementKind Mode2KindFromType(const WCHAR* type) {
    std::wstring value = type ? std::wstring(type) : std::wstring();
    TrimString(value);
    const WCHAR* trimmedType = value.c_str();
    if (SameTextI(trimmedType, L"TextBox")) {
        return Mode2ElementTextBox;
    }
    if (SameTextI(trimmedType, L"TextFileBox")) {
        return Mode2ElementTextFileBox;
    }
    if (SameTextI(trimmedType, L"Text")) {
        return Mode2ElementText;
    }
    if (SameTextI(trimmedType, L"OpenButton")) {
        return Mode2ElementOpenButton;
    }
    if (SameTextI(trimmedType, L"MessageButton")) {
        return Mode2ElementMessageButton;
    }
    if (SameTextI(trimmedType, L"RegButton")) {
        return Mode2ElementRegButton;
    }
    if (SameTextI(trimmedType, L"Table")) {
        return Mode2ElementTable;
    }
    if (SameTextI(trimmedType, L"Input")) {
        return Mode2ElementInput;
    }
    if (SameTextI(trimmedType, L"CloseButton")) {
        return Mode2ElementCloseButton;
    }
    return Mode2ElementNone;
}

static BOOL IsMode1RuntimeSection(const WCHAR* section) {
    return StartsWithI(section, L"Mode1.") && !StartsWithI(section, L"Mode1.Variable");
}

static BOOL IsMode2RuntimeSection(const WCHAR* section) {
    return StartsWithI(section, L"Mode2.") && !StartsWithI(section, L"Mode2.Variable");
}

static void LoadMode1Variables() {
    for (UINT i = 0; i < XHL_MODE1_VARIABLE_COUNT; ++i) {
        WCHAR section[32];
        Mode1VariableSection(i + 1, section, 32);
        g_mode1VariableEnabled[i] = ReadIniFlag(section, L"Enable", FALSE);
        ReadIniText(section, L"Value", L"", g_mode1VariableValues[i], XHL_MAX_TEXT);
    }
}

static void LoadMode2Variables() {
    for (UINT i = 0; i < XHL_MODE2_VARIABLE_COUNT; ++i) {
        g_mode2VariableEnabled[i] = FALSE;
        g_mode2VariableValues[i][0] = L'\0';
    }
}

static void AppendChar(WCHAR* dest, UINT capacity, WCHAR ch) {
    UINT length = StrLen(dest);
    if (length + 1 >= capacity) {
        return;
    }
    dest[length] = ch;
    dest[length + 1] = L'\0';
}

static void ApplyModeVariables(LauncherMode mode, WCHAR* text, UINT capacity) {
    CopyText(g_substitutionBuffer, XHL_MAX_TEXT, text);
    text[0] = L'\0';

    const WCHAR* cursor = g_substitutionBuffer;
    while (*cursor) {
        BOOL replaced = FALSE;
        UINT variableCount = mode == ModeWindow ? XHL_MODE2_VARIABLE_COUNT : XHL_MODE1_VARIABLE_COUNT;
        for (UINT i = 0; i < variableCount; ++i) {
            BOOL enabled = mode == ModeWindow ? g_mode2VariableEnabled[i] : g_mode1VariableEnabled[i];
            const WCHAR* value = mode == ModeWindow ? g_mode2VariableValues[i] : g_mode1VariableValues[i];
            if (!enabled) {
                continue;
            }

            WCHAR token[16];
            CopyText(token, 16, mode == ModeWindow ? L"%XHLM2V" : L"%XHLM1V");
            AppendUInt(token, 16, i + 1);
            AppendText(token, 16, L"%");

            if (StartsWithI(cursor, token)) {
                AppendText(text, capacity, value);
                cursor += StrLen(token);
                replaced = TRUE;
                break;
            }
        }

        if (!replaced) {
            AppendChar(text, capacity, *cursor);
            ++cursor;
        }
    }
}

static void ReadModeIniText(LauncherMode mode, const WCHAR* section, const WCHAR* key, const WCHAR* defaultValue, WCHAR* out, UINT capacity) {
    ReadIniText(section, key, defaultValue, out, capacity);
    if (mode == ModeSilent && IsMode1RuntimeSection(section)) {
        ApplyModeVariables(mode, out, capacity);
    }
    if (mode == ModeWindow && IsMode2RuntimeSection(section)) {
        ApplyModeVariables(mode, out, capacity);
    }
}

static BOOL ApplyModeVariablesString(LauncherMode mode, std::wstring& text) {
    if (text.length() > XHL_EXPANDED_TEXT_MAX) {
        text.clear();
        return FALSE;
    }
    if (text.find(L'%') == std::wstring::npos) {
        return TRUE;
    }

    std::wstring result;
    size_t cursor = 0;
    while (cursor < text.length()) {
        BOOL replaced = FALSE;
        UINT variableCount = mode == ModeWindow ? XHL_MODE2_VARIABLE_COUNT : XHL_MODE1_VARIABLE_COUNT;
        for (UINT i = 0; i < variableCount; ++i) {
            BOOL enabled = mode == ModeWindow ? g_mode2VariableEnabled[i] : g_mode1VariableEnabled[i];
            const WCHAR* value = mode == ModeWindow ? g_mode2VariableValues[i] : g_mode1VariableValues[i];
            if (!enabled) {
                continue;
            }

            WCHAR token[16];
            CopyText(token, 16, mode == ModeWindow ? L"%XHLM2V" : L"%XHLM1V");
            AppendUInt(token, 16, i + 1);
            AppendText(token, 16, L"%");

            if (StartsWithI(text.c_str() + cursor, token)) {
                size_t valueLength = StrLen(value);
                if (valueLength > XHL_EXPANDED_TEXT_MAX || result.length() > XHL_EXPANDED_TEXT_MAX - valueLength) {
                    text.clear();
                    return FALSE;
                }
                result.append(value, valueLength);
                cursor += StrLen(token);
                replaced = TRUE;
                break;
            }
        }

        if (!replaced) {
            if (result.length() >= XHL_EXPANDED_TEXT_MAX) {
                text.clear();
                return FALSE;
            }
            result.push_back(text[cursor]);
            ++cursor;
        }
    }
    text = result;
    return TRUE;
}

static BOOL ReadModeIniTextStringChecked(LauncherMode mode, const WCHAR* section, const WCHAR* key, const WCHAR* defaultValue, std::wstring& value) {
    value = ReadIniTextString(section, key, defaultValue);
    if (mode == ModeSilent && IsMode1RuntimeSection(section)) {
        if (!ApplyModeVariablesString(mode, value)) {
            return FALSE;
        }
    }
    if (mode == ModeWindow && IsMode2RuntimeSection(section)) {
        if (!ApplyModeVariablesString(mode, value)) {
            return FALSE;
        }
    }
    return TRUE;
}

static BOOL ReadModeIniFlag(LauncherMode mode, const WCHAR* section, const WCHAR* key, BOOL defaultValue) {
    (void)mode;
    WCHAR value[64];
    ReadIniText(section, key, defaultValue ? L"1" : L"0", value, 64);
    return ReadTrimmedFlagValue(value, defaultValue);
}

static BOOL HasExtensionI(const WCHAR* path, const WCHAR* extension) {
    if (!extension || !extension[0]) {
        return TRUE;
    }

    UINT length = StrLen(path);
    UINT extLength = StrLen(extension);
    if (length < extLength) {
        return FALSE;
    }

    return SameTextI(path + length - extLength, extension);
}

static BOOL IsSlash(WCHAR ch) {
    return ch == L'\\' || ch == L'/';
}

static BOOL IsRootPathWithTrailingSlash(const WCHAR* path) {
    UINT length = StrLen(path);
    if (length == 3 && path[1] == L':' && IsSlash(path[2])) {
        return TRUE;
    }

    if (length >= 3 && IsSlash(path[0]) && IsSlash(path[1])) {
        UINT slashCount = 0;
        for (UINT i = 2; path[i]; ++i) {
            if (IsSlash(path[i])) {
                ++slashCount;
                if (i == length - 1 && slashCount == 2) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

static BOOL HasEnvironmentMarkerString(const std::wstring& text) {
    return text.find(L'%') != std::wstring::npos;
}

static std::wstring GetXhlRootPathString() {
    std::wstring root = g_ctx.fullBaseDir;
    if (!root.empty() && IsSlash(root[root.length() - 1]) && !IsRootPathWithTrailingSlash(root.c_str())) {
        root.resize(root.length() - 1);
    }
    return root;
}

static BOOL ExpandInternalVariablesString(std::wstring& text) {
    if (!HasEnvironmentMarkerString(text)) {
        return TRUE;
    }

    std::wstring result;
    std::wstring root = GetXhlRootPathString();
    size_t cursor = 0;
    while (cursor < text.length()) {
        if (StartsWithI(text.c_str() + cursor, L"%XHLROOT%")) {
            result += root;
            cursor += 9;
        } else {
            result.push_back(text[cursor]);
            ++cursor;
        }
    }

    text = result;
    return TRUE;
}

struct ProtectedTextToken {
    std::wstring placeholder;
    std::wstring token;
};

static BOOL TryGetXhlModeVariableTokenAt(const std::wstring& text, size_t position, std::wstring& token) {
    if (position >= text.length() || text[position] != L'%') {
        return FALSE;
    }

    for (UINT modeNumber = 1; modeNumber <= 2; ++modeNumber) {
        UINT variableCount = modeNumber == 1 ? XHL_MODE1_VARIABLE_COUNT : XHL_MODE2_VARIABLE_COUNT;
        for (UINT variableNumber = 1; variableNumber <= variableCount; ++variableNumber) {
            std::wstring candidate = L"%XHLM";
            candidate.push_back((WCHAR)(L'0' + modeNumber));
            candidate.push_back(L'V');
            candidate.push_back((WCHAR)(L'0' + variableNumber));
            candidate.push_back(L'%');

            if (StartsWithI(text.c_str() + position, candidate.c_str())) {
                token = text.substr(position, candidate.length());
                return TRUE;
            }
        }
    }

    return FALSE;
}

static void ProtectUnresolvedXhlModeVariableTokens(std::wstring& text, std::vector<ProtectedTextToken>& protectedTokens) {
    if (!HasEnvironmentMarkerString(text)) {
        return;
    }

    std::wstring result;
    size_t cursor = 0;
    while (cursor < text.length()) {
        std::wstring token;
        if (TryGetXhlModeVariableTokenAt(text, cursor, token)) {
            ProtectedTextToken protectedToken;
            protectedToken.placeholder = L"\xE000XHL";
            protectedToken.placeholder += std::to_wstring(protectedTokens.size());
            protectedToken.placeholder += L"\xE001";
            protectedToken.token = token;
            result += protectedToken.placeholder;
            protectedTokens.push_back(protectedToken);
            cursor += token.length();
        } else {
            result.push_back(text[cursor]);
            ++cursor;
        }
    }

    text = result;
}

static void RestoreProtectedTextTokens(std::wstring& text, const std::vector<ProtectedTextToken>& protectedTokens) {
    for (UINT i = 0; i < protectedTokens.size(); ++i) {
        const ProtectedTextToken& protectedToken = protectedTokens[i];
        size_t position = 0;
        while ((position = text.find(protectedToken.placeholder, position)) != std::wstring::npos) {
            text.replace(position, protectedToken.placeholder.length(), protectedToken.token);
            position += protectedToken.token.length();
        }
    }
}

static BOOL ExpandEnvironmentVariablesString(std::wstring& text) {
    if (!HasEnvironmentMarkerString(text)) {
        return TRUE;
    }

    std::wstring working = text;
    if (!ExpandInternalVariablesString(working)) {
        return FALSE;
    }

    std::vector<ProtectedTextToken> protectedTokens;
    ProtectUnresolvedXhlModeVariableTokens(working, protectedTokens);
    if (!HasEnvironmentMarkerString(working)) {
        RestoreProtectedTextTokens(working, protectedTokens);
        text = working;
        return TRUE;
    }

    DWORD needed = ExpandEnvironmentStringsW(working.c_str(), 0, 0);
    if (needed == 0) {
        return FALSE;
    }

    std::wstring expanded;
    expanded.resize(needed);
    DWORD copied = ExpandEnvironmentStringsW(working.c_str(), &expanded[0], needed);
    if (copied == 0 || copied > needed) {
        return FALSE;
    }

    if (copied > 0) {
        expanded.resize(copied - 1);
    }
    RestoreProtectedTextTokens(expanded, protectedTokens);
    text = expanded;
    return TRUE;
}

static PathResolveResult ResolveRelativeFileString(const std::wstring& configuredPath, const WCHAR* requiredExtension, std::wstring& out) {
    std::wstring relative = configuredPath;
    ReplaceForwardSlashString(relative);

    if (relative.empty()) {
        return PathResolveEmpty;
    }
    if (IsRootedPath(relative.c_str())) {
        return PathResolveWrongPathType;
    }

    std::wstring fullPath;
    if (!GetFullPathString(g_ctx.fullBaseDir + relative, fullPath)) {
        return PathResolveInvalid;
    }
    if (!StartsWithI(fullPath.c_str(), g_ctx.fullBaseDir.c_str())) {
        return PathResolveOutsideLauncher;
    }
    if (!HasExtensionI(fullPath.c_str(), requiredExtension)) {
        return PathResolveWrongExtension;
    }
    if (!IsFile(fullPath.c_str())) {
        return PathResolveMissingFile;
    }

    out = fullPath;
    return PathResolveOk;
}

static PathResolveResult ResolveAbsoluteFileString(const std::wstring& configuredPath, const WCHAR* requiredExtension, std::wstring& out) {
    std::wstring absolute = configuredPath;
    ReplaceForwardSlashString(absolute);

    if (absolute.empty()) {
        return PathResolveEmpty;
    }
    if (!IsAbsolutePath(absolute.c_str())) {
        return PathResolveWrongPathType;
    }

    std::wstring fullPath;
    if (!GetFullPathString(absolute, fullPath)) {
        return PathResolveInvalid;
    }
    if (!HasExtensionI(fullPath.c_str(), requiredExtension)) {
        return PathResolveWrongExtension;
    }
    if (!IsFile(fullPath.c_str())) {
        return PathResolveMissingFile;
    }

    out = fullPath;
    return PathResolveOk;
}

static PathResolveResult ResolveConfiguredFileString(const std::wstring& configuredPath, const WCHAR* requiredExtension, BOOL useRelativePath, std::wstring& out) {
    std::wstring expandedPath = configuredPath;
    if (useRelativePath) {
        return ResolveRelativeFileString(expandedPath, requiredExtension, out);
    }

    if (!ExpandEnvironmentVariablesString(expandedPath)) {
        return PathResolveInvalid;
    }
    ReplaceForwardSlashString(expandedPath);
    return ResolveAbsoluteFileString(expandedPath, requiredExtension, out);
}

static SplashPosition SplashPositionFromText(const WCHAR* text) {
    std::wstring value = text ? std::wstring(text) : std::wstring();
    TrimString(value);
    const WCHAR* trimmedText = value.c_str();
    if (SameTextI(trimmedText, L"TopLeft")) {
        return SplashTopLeft;
    }
    if (SameTextI(trimmedText, L"TopRight")) {
        return SplashTopRight;
    }
    if (SameTextI(trimmedText, L"BottomLeft")) {
        return SplashBottomLeft;
    }
    if (SameTextI(trimmedText, L"BottomRight")) {
        return SplashBottomRight;
    }
    return SplashCenter;
}

static const WCHAR* SplashPositionToText(SplashPosition position) {
    if (position == SplashTopLeft) {
        return L"TopLeft";
    }
    if (position == SplashTopRight) {
        return L"TopRight";
    }
    if (position == SplashBottomLeft) {
        return L"BottomLeft";
    }
    if (position == SplashBottomRight) {
        return L"BottomRight";
    }
    return L"Center";
}

static PathResolveResult ResolveSplashImagePathString(LauncherMode mode, std::wstring& out) {
    std::wstring path = ReadIniTextString(L"Launcher", L"SplashImage", L"");
    BOOL useRelativePath = ReadIniFlag(L"Launcher", L"SplashUseRelativePath", TRUE);
    if (mode == ModeSilent || mode == ModeWindow) {
        (void)ApplyModeVariablesString(mode, path);
    }
    if (path.empty()) {
        return PathResolveEmpty;
    }
    return ResolveConfiguredFileString(path, L".png", useRelativePath, out);
}

static UINT GetMonitorDpiValue(HMONITOR monitor) {
    typedef HRESULT(WINAPI* GetDpiForMonitorProc)(HMONITOR, int, UINT*, UINT*);
    HMODULE shcore = LoadLibraryW(L"shcore.dll");
    if (shcore) {
        GetDpiForMonitorProc getDpiForMonitor = (GetDpiForMonitorProc)GetProcAddress(shcore, "GetDpiForMonitor");
        if (getDpiForMonitor && monitor) {
            UINT dpiX = 0;
            UINT dpiY = 0;
            if (SUCCEEDED(getDpiForMonitor(monitor, 0, &dpiX, &dpiY)) && dpiX) {
                FreeLibrary(shcore);
                return dpiX;
            }
        }
        FreeLibrary(shcore);
    }
    return GetDpi(0);
}

static HMONITOR GetSplashMonitor(RECT* workRect) {
    HWND foreground = GetForegroundWindow();
    HMONITOR monitor = foreground ? MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST) : 0;
    if (!monitor) {
        POINT point;
        point.x = 0;
        point.y = 0;
        monitor = MonitorFromPoint(point, MONITOR_DEFAULTTOPRIMARY);
    }

    MONITORINFO info;
    ZeroMemory(&info, sizeof(info));
    info.cbSize = sizeof(info);
    if (monitor && GetMonitorInfoW(monitor, &info)) {
        if (workRect) {
            *workRect = info.rcWork;
        }
        return monitor;
    }

    if (workRect && !SystemParametersInfoW(SPI_GETWORKAREA, 0, workRect, 0)) {
        SetRect(workRect, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    }
    return monitor;
}

static BOOL CalculateSplashDrawSize(UINT width, UINT height, UINT dpi, UINT scale, UINT* drawWidth, UINT* drawHeight) {
    if (!drawWidth || !drawHeight || width == 0 || height == 0) {
        return FALSE;
    }
    if (dpi == 0) {
        dpi = 96;
    }
    if (scale < XHL_SPLASH_MIN_SCALE) {
        scale = XHL_SPLASH_DEFAULT_SCALE;
    }
    if (scale > XHL_SPLASH_MAX_SCALE) {
        scale = XHL_SPLASH_MAX_SCALE;
    }

    const unsigned long long denominator = 100ULL * 96ULL;
    unsigned long long scaledWidth = ((unsigned long long)width * (unsigned long long)scale * (unsigned long long)dpi + denominator / 2ULL) / denominator;
    unsigned long long scaledHeight = ((unsigned long long)height * (unsigned long long)scale * (unsigned long long)dpi + denominator / 2ULL) / denominator;
    if (scaledWidth == 0) {
        scaledWidth = 1;
    }
    if (scaledHeight == 0) {
        scaledHeight = 1;
    }

    if (scaledWidth > XHL_SPLASH_MAX_RENDER_WIDTH) {
        scaledHeight = (scaledHeight * XHL_SPLASH_MAX_RENDER_WIDTH + scaledWidth / 2ULL) / scaledWidth;
        scaledWidth = XHL_SPLASH_MAX_RENDER_WIDTH;
        if (scaledHeight == 0) {
            scaledHeight = 1;
        }
    }
    if (scaledHeight > XHL_SPLASH_MAX_RENDER_HEIGHT) {
        scaledWidth = (scaledWidth * XHL_SPLASH_MAX_RENDER_HEIGHT + scaledHeight / 2ULL) / scaledHeight;
        scaledHeight = XHL_SPLASH_MAX_RENDER_HEIGHT;
        if (scaledWidth == 0) {
            scaledWidth = 1;
        }
    }

    unsigned long long dibBytes = scaledWidth * scaledHeight * 4ULL;
    if (dibBytes == 0 || dibBytes > XHL_SPLASH_MAX_DIB_BYTES || scaledWidth > 0x7fffffffULL || scaledHeight > 0x7fffffffULL) {
        return FALSE;
    }

    *drawWidth = (UINT)scaledWidth;
    *drawHeight = (UINT)scaledHeight;
    return TRUE;
}

static BOOL LoadPngSplashBitmap(const std::wstring& path, UINT dpi, UINT scale, DWORD durationMs, SplashImageData* out) {
    if (!out) {
        return FALSE;
    }
    ZeroMemory(out, sizeof(*out));

    LONGLONG fileSize = 0;
    if (!GetFileSizeBytes(path.c_str(), &fileSize) || fileSize <= 0 || fileSize > XHL_SPLASH_MAX_BYTES) {
        return FALSE;
    }

    HRESULT coResult = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
    if (FAILED(coResult) && coResult != RPC_E_CHANGED_MODE) {
        return FALSE;
    }
    BOOL shouldUninitialize = SUCCEEDED(coResult);

    IWICImagingFactory* factory = 0;
    IWICBitmapDecoder* decoder = 0;
    IWICBitmapFrameDecode* frame = 0;
    IWICBitmapScaler* scalerSource = 0;
    IWICFormatConverter* converter = 0;
    HBITMAP bitmap = 0;
    BOOL ok = FALSE;

    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
    if (SUCCEEDED(hr)) {
        hr = factory->CreateDecoderFromFilename(path.c_str(), 0, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
    }
    GUID containerFormat;
    ZeroMemory(&containerFormat, sizeof(containerFormat));
    if (SUCCEEDED(hr)) {
        hr = decoder->GetContainerFormat(&containerFormat);
    }
    if (SUCCEEDED(hr) && !IsEqualGUID(containerFormat, GUID_ContainerFormatPng)) {
        hr = E_FAIL;
    }
    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &frame);
    }

    UINT width = 0;
    UINT height = 0;
    if (SUCCEEDED(hr)) {
        hr = frame->GetSize(&width, &height);
    }
    if (SUCCEEDED(hr) && (width == 0 || height == 0 || width > XHL_SPLASH_MAX_WIDTH || height > XHL_SPLASH_MAX_HEIGHT)) {
        hr = E_FAIL;
    }
    UINT drawWidth = 0;
    UINT drawHeight = 0;
    if (SUCCEEDED(hr) && !CalculateSplashDrawSize(width, height, dpi, scale, &drawWidth, &drawHeight)) {
        hr = E_FAIL;
    }
    IWICBitmapSource* bitmapSource = frame;
    if (SUCCEEDED(hr) && (drawWidth != width || drawHeight != height)) {
        hr = factory->CreateBitmapScaler(&scalerSource);
        if (SUCCEEDED(hr)) {
            WICBitmapInterpolationMode mode = (drawWidth > width || drawHeight > height) ? WICBitmapInterpolationModeCubic : WICBitmapInterpolationModeFant;
            hr = scalerSource->Initialize(frame, drawWidth, drawHeight, mode);
        }
        bitmapSource = scalerSource;
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateFormatConverter(&converter);
    }
    if (SUCCEEDED(hr)) {
        hr = converter->Initialize(bitmapSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, 0, 0.0, WICBitmapPaletteTypeCustom);
    }
    if (SUCCEEDED(hr)) {
        BITMAPINFO bitmapInfo;
        ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
        bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmapInfo.bmiHeader.biWidth = (LONG)drawWidth;
        bitmapInfo.bmiHeader.biHeight = -(LONG)drawHeight;
        bitmapInfo.bmiHeader.biPlanes = 1;
        bitmapInfo.bmiHeader.biBitCount = 32;
        bitmapInfo.bmiHeader.biCompression = BI_RGB;

        void* bits = 0;
        bitmap = CreateDIBSection(0, &bitmapInfo, DIB_RGB_COLORS, &bits, 0, 0);
        if (!bitmap || !bits) {
            hr = E_FAIL;
        } else {
            unsigned long long stride64 = (unsigned long long)drawWidth * 4ULL;
            unsigned long long bufferSize64 = stride64 * (unsigned long long)drawHeight;
            if (stride64 > 0xffffffffULL || bufferSize64 > 0xffffffffULL || bufferSize64 > XHL_SPLASH_MAX_DIB_BYTES) {
                hr = E_FAIL;
            } else {
                hr = converter->CopyPixels(0, (UINT)stride64, (UINT)bufferSize64, (BYTE*)bits);
            }
        }
    }

    if (SUCCEEDED(hr) && bitmap) {
        out->bitmap = bitmap;
        out->width = drawWidth;
        out->height = drawHeight;
        out->durationMs = durationMs;
        out->drawWidth = (int)drawWidth;
        out->drawHeight = (int)drawHeight;
        bitmap = 0;
        ok = TRUE;
    }

    if (bitmap) {
        DeleteObject(bitmap);
    }
    if (converter) {
        converter->Release();
    }
    if (scalerSource) {
        scalerSource->Release();
    }
    if (frame) {
        frame->Release();
    }
    if (decoder) {
        decoder->Release();
    }
    if (factory) {
        factory->Release();
    }
    if (shouldUninitialize) {
        CoUninitialize();
    }
    return ok;
}

static LRESULT CALLBACK SplashWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        CREATESTRUCTW* create = (CREATESTRUCTW*)lParam;
        SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR)(create ? create->lpCreateParams : 0));
        SplashImageData* data = (SplashImageData*)(create ? create->lpCreateParams : 0);
        SetTimer(window, 1, data && data->durationMs ? data->durationMs : XHL_SPLASH_DEFAULT_DURATION_MS, 0);
        return 0;
    }
    case WM_TIMER:
        DestroyWindow(window);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

static BOOL UpdateSplashLayeredWindow(HWND window, SplashImageData* data, int x, int y) {
    if (!window || !data || !data->bitmap || data->drawWidth <= 0 || data->drawHeight <= 0) {
        return FALSE;
    }

    HDC screenDc = GetDC(0);
    if (!screenDc) {
        return FALSE;
    }
    HDC memoryDc = CreateCompatibleDC(screenDc);
    if (!memoryDc) {
        ReleaseDC(0, screenDc);
        return FALSE;
    }

    HGDIOBJ oldBitmap = SelectObject(memoryDc, data->bitmap);
    POINT destination;
    destination.x = x;
    destination.y = y;
    SIZE size;
    size.cx = data->drawWidth;
    size.cy = data->drawHeight;
    POINT source;
    source.x = 0;
    source.y = 0;
    BLENDFUNCTION blend;
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    BOOL ok = UpdateLayeredWindow(window, screenDc, &destination, &size, memoryDc, &source, 0, &blend, ULW_ALPHA);

    if (oldBitmap) {
        SelectObject(memoryDc, oldBitmap);
    }
    DeleteDC(memoryDc);
    ReleaseDC(0, screenDc);
    return ok;
}

static void RunSplashWindow(SplashImageData* data, SplashPosition position, const RECT& workRect) {
    if (!data || !data->bitmap || data->drawWidth <= 0 || data->drawHeight <= 0) {
        return;
    }

    WNDCLASSEXW windowClass;
    ZeroMemory(&windowClass, sizeof(windowClass));
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = SplashWindowProc;
    windowClass.hInstance = GetModuleHandleW(0);
    windowClass.hCursor = LoadCursorW(0, IDC_ARROW);
    windowClass.lpszClassName = L"XHLauncherSplashWindow";
    RegisterClassExW(&windowClass);

    int workWidth = workRect.right - workRect.left;
    int workHeight = workRect.bottom - workRect.top;
    int x = workRect.left + (workWidth - data->drawWidth) / 2;
    int y = workRect.top + (workHeight - data->drawHeight) / 2;
    if (position == SplashTopLeft) {
        x = workRect.left;
        y = workRect.top;
    } else if (position == SplashTopRight) {
        x = workRect.right - data->drawWidth;
        y = workRect.top;
    } else if (position == SplashBottomLeft) {
        x = workRect.left;
        y = workRect.bottom - data->drawHeight;
    } else if (position == SplashBottomRight) {
        x = workRect.right - data->drawWidth;
        y = workRect.bottom - data->drawHeight;
    }

    HWND window = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,
        L"XHLauncherSplashWindow",
        L"",
        WS_POPUP,
        x,
        y,
        data->drawWidth,
        data->drawHeight,
        0,
        0,
        GetModuleHandleW(0),
        data);
    if (!window) {
        return;
    }

    BOOL canShow = UpdateSplashLayeredWindow(window, data, x, y);
    if (canShow) {
        ShowWindow(window, SW_SHOWNOACTIVATE);
        UpdateWindow(window);
    } else {
        DestroyWindow(window);
    }

    MSG message;
    while (GetMessageW(&message, 0, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
}

static void ShowSplashIfConfigured(LauncherMode mode) {
    if (!ReadIniFlag(L"Launcher", L"SplashEnable", FALSE)) {
        return;
    }

    std::wstring imagePath;
    if (ResolveSplashImagePathString(mode, imagePath) != PathResolveOk) {
        return;
    }

    WCHAR value[32];
    ReadIniText(L"Launcher", L"SplashDurationMs", L"1000", value, 32);
    DWORD durationMs = (DWORD)ParseUIntInRange(value, XHL_SPLASH_DEFAULT_DURATION_MS, 1, 60000);
    ReadIniText(L"Launcher", L"SplashScale", L"100", value, 32);
    UINT scale = ParseUIntInRange(value, XHL_SPLASH_DEFAULT_SCALE, XHL_SPLASH_MIN_SCALE, XHL_SPLASH_MAX_SCALE);
    ReadIniText(L"Launcher", L"SplashPosition", L"Center", value, 32);
    SplashPosition position = SplashPositionFromText(value);

    RECT workRect;
    SetRect(&workRect, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    HMONITOR monitor = GetSplashMonitor(&workRect);
    UINT dpi = GetMonitorDpiValue(monitor);

    SplashImageData data;
    if (!LoadPngSplashBitmap(imagePath, dpi, scale, durationMs, &data)) {
        return;
    }

    RunSplashWindow(&data, position, workRect);
    if (data.bitmap) {
        DeleteObject(data.bitmap);
    }
}

static PathResolveResult ResolveRelativeDirectoryString(const std::wstring& configuredPath, std::wstring& out) {
    std::wstring relative = configuredPath;
    ReplaceForwardSlashString(relative);
    if (relative.empty()) {
        return PathResolveEmpty;
    }
    if (IsRootedPath(relative.c_str())) {
        return PathResolveWrongPathType;
    }

    std::wstring fullPath;
    if (!GetFullPathString(g_ctx.fullBaseDir + relative, fullPath)) {
        return PathResolveInvalid;
    }
    if (!StartsWithI(fullPath.c_str(), g_ctx.fullBaseDir.c_str())) {
        return PathResolveOutsideLauncher;
    }
    if (!IsDirectory(fullPath.c_str())) {
        return PathResolveMissingDirectory;
    }

    EnsureTrailingSlashString(fullPath);
    out = fullPath;
    return PathResolveOk;
}

static PathResolveResult ResolveAbsoluteDirectoryString(const std::wstring& configuredPath, std::wstring& out) {
    std::wstring absolute = configuredPath;
    ReplaceForwardSlashString(absolute);
    if (absolute.empty()) {
        return PathResolveEmpty;
    }
    if (!IsAbsolutePath(absolute.c_str())) {
        return PathResolveWrongPathType;
    }

    std::wstring fullPath;
    if (!GetFullPathString(absolute, fullPath)) {
        return PathResolveInvalid;
    }
    if (!IsDirectory(fullPath.c_str())) {
        return PathResolveMissingDirectory;
    }

    EnsureTrailingSlashString(fullPath);
    out = fullPath;
    return PathResolveOk;
}

static PathResolveResult ResolveConfiguredDirectoryString(const std::wstring& configuredPath, std::wstring& out) {
    std::wstring expandedPath = configuredPath;
    if (!ExpandEnvironmentVariablesString(expandedPath)) {
        return PathResolveInvalid;
    }
    ReplaceForwardSlashString(expandedPath);

    if (IsAbsolutePath(expandedPath.c_str())) {
        return ResolveAbsoluteDirectoryString(expandedPath, out);
    }
    if (IsRootedPath(expandedPath.c_str())) {
        return PathResolveWrongPathType;
    }
    return ResolveRelativeDirectoryString(expandedPath, out);
}

static WorkingDirectoryMode WorkingDirectoryModeFromText(const WCHAR* text) {
    std::wstring value = text ? std::wstring(text) : std::wstring();
    TrimString(value);
    const WCHAR* trimmedText = value.c_str();
    if (SameTextI(trimmedText, L"LauncherDir")) {
        return WorkingDirectoryLauncherDir;
    }
    if (SameTextI(trimmedText, L"Custom")) {
        return WorkingDirectoryCustom;
    }
    return WorkingDirectoryTargetDir;
}

static const WCHAR* WorkingDirectoryModeToText(WorkingDirectoryMode mode) {
    if (mode == WorkingDirectoryLauncherDir) {
        return L"LauncherDir";
    }
    if (mode == WorkingDirectoryCustom) {
        return L"Custom";
    }
    return L"TargetDir";
}

static void GetParentDirectoryString(const std::wstring& filePath, std::wstring& out) {
    size_t lastSlash = filePath.find_last_of(L"\\/");
    if (lastSlash == std::wstring::npos) {
        out = g_ctx.fullBaseDir;
        return;
    }
    out = filePath.substr(0, lastSlash + 1);
}

static PathResolveResult ResolveWorkingDirectoryFromValuesString(WorkingDirectoryMode mode, const std::wstring& configuredPath, const std::wstring& targetPath, std::wstring& out) {
    if (mode == WorkingDirectoryLauncherDir) {
        out = g_ctx.fullBaseDir;
        return PathResolveOk;
    }
    if (mode == WorkingDirectoryCustom) {
        return ResolveConfiguredDirectoryString(configuredPath, out);
    }

    GetParentDirectoryString(targetPath, out);
    return IsDirectory(out.c_str()) ? PathResolveOk : PathResolveMissingDirectory;
}

static PathResolveResult ResolveWorkingDirectoryString(LauncherMode mode, const WCHAR* section, const std::wstring& targetPath, const WCHAR* modeKey, const WCHAR* dirKey, std::wstring& out) {
    std::wstring modeText = ReadIniTextString(section, modeKey, L"TargetDir");
    std::wstring directoryText;
    if (!ReadModeIniTextStringChecked(mode, section, dirKey, L"", directoryText)) {
        return PathResolveInvalid;
    }
    return ResolveWorkingDirectoryFromValuesString(WorkingDirectoryModeFromText(modeText.c_str()), directoryText, targetPath, out);
}

static HANDLE CreateKillOnCloseJob() {
    HANDLE job = CreateJobObjectW(0, 0);
    if (!job) {
        return 0;
    }

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION info;
    ZeroMemory(&info, sizeof(info));
    info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, &info, sizeof(info))) {
        CloseHandle(job);
        return 0;
    }

    return job;
}

static void StoreActiveRegistryProcess(HANDLE process) {
    if (!process) {
        return;
    }

    HANDLE duplicate = 0;
    if (DuplicateHandle(GetCurrentProcess(), process, GetCurrentProcess(), &duplicate, PROCESS_TERMINATE | SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, 0)) {
        HANDLE old = (HANDLE)InterlockedExchangePointer(&g_regImportProcess, duplicate);
        if (old) {
            CloseHandle(old);
        }
    }
}

static void ClearActiveRegistryProcess() {
    HANDLE old = (HANDLE)InterlockedExchangePointer(&g_regImportProcess, 0);
    if (old) {
        CloseHandle(old);
    }
}

static void CancelActiveRegistryImport() {
    HANDLE process = (HANDLE)InterlockedExchangePointer(&g_regImportProcess, 0);
    if (process) {
        TerminateProcess(process, 1);
        CloseHandle(process);
    }
}

static void SetRegImportDetail(RegImportTask* task, RegImportStage stage, DWORD lastError, DWORD exitCode) {
    if (!task) {
        return;
    }
    task->stage = stage;
    task->lastError = lastError;
    task->exitCode = exitCode;
}

static RegImportResult ImportRegistryFileString(const std::wstring& regPath, const std::wstring& workingDirectory, RegImportTask* task) {
    if (task) {
        task->stage = RegImportStageNone;
        task->lastError = 0;
        task->exitCode = 1;
        task->jobAssigned = FALSE;
    }

    std::wstring windowsDir;
    if (!GetWindowsDirectoryString(windowsDir)) {
        SetRegImportDetail(task, RegImportStageWindowsDirectory, GetLastError(), 1);
        return RegImportFailed;
    }
    EnsureTrailingSlashString(windowsDir);

    std::wstring regExe = windowsDir + L"Sysnative\\reg.exe";
    if (!IsFile(regExe.c_str())) {
        regExe = windowsDir + L"System32\\reg.exe";
    }
    if (!IsFile(regExe.c_str())) {
        SetRegImportDetail(task, RegImportStageLocateRegExe, ERROR_FILE_NOT_FOUND, 1);
        return RegImportFailed;
    }

    std::wstring commandLine = L"\"";
    commandLine += regExe;
    commandLine += L"\" import \"";
    commandLine += regPath;
    commandLine += L"\"";

    STARTUPINFOW startupInfo;
    PROCESS_INFORMATION processInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    ZeroMemory(&processInfo, sizeof(processInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_HIDE;

    const WCHAR* currentDirectory = workingDirectory.empty() ? g_ctx.fullBaseDir.c_str() : workingDirectory.c_str();
    if (!CreateProcessW(regExe.c_str(), &commandLine[0], 0, 0, FALSE, CREATE_NO_WINDOW | CREATE_SUSPENDED, 0, currentDirectory, &startupInfo, &processInfo)) {
        SetRegImportDetail(task, RegImportStageCreateProcess, GetLastError(), 1);
        return RegImportFailed;
    }
    StoreActiveRegistryProcess(processInfo.hProcess);

    HANDLE job = CreateKillOnCloseJob();
    if (job) {
        if (AssignProcessToJobObject(job, processInfo.hProcess)) {
            if (task) {
                task->jobAssigned = TRUE;
            }
        } else {
            DWORD assignError = GetLastError();
            CloseHandle(job);
            job = 0;
            SetRegImportDetail(task, RegImportStageAssignJob, assignError, 1);
        }
    }

    if (ResumeThread(processInfo.hThread) == (DWORD)-1) {
        DWORD resumeError = GetLastError();
        TerminateProcess(processInfo.hProcess, 1);
        WaitForSingleObject(processInfo.hProcess, 5000);
        ClearActiveRegistryProcess();
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        if (job) {
            CloseHandle(job);
        }
        SetRegImportDetail(task, RegImportStageResumeThread, resumeError, 1);
        return RegImportFailed;
    }

    CloseHandle(processInfo.hThread);
    DWORD waitResult = WaitForSingleObject(processInfo.hProcess, XHL_REG_IMPORT_TIMEOUT_MS);

    if (waitResult == WAIT_TIMEOUT) {
        TerminateProcess(processInfo.hProcess, 1);
        WaitForSingleObject(processInfo.hProcess, 5000);
        ClearActiveRegistryProcess();
        CloseHandle(processInfo.hProcess);
        if (job) {
            CloseHandle(job);
        }
        SetRegImportDetail(task, RegImportStageWait, WAIT_TIMEOUT, 1);
        return RegImportTimeout;
    }

    DWORD exitCode = 1;
    if (!GetExitCodeProcess(processInfo.hProcess, &exitCode)) {
        DWORD exitError = GetLastError();
        ClearActiveRegistryProcess();
        CloseHandle(processInfo.hProcess);
        if (job) {
            CloseHandle(job);
        }
        SetRegImportDetail(task, RegImportStageExitCode, exitError, 1);
        return RegImportFailed;
    }
    ClearActiveRegistryProcess();
    CloseHandle(processInfo.hProcess);
    if (job) {
        CloseHandle(job);
    }
    SetRegImportDetail(task, exitCode == 0 ? RegImportStageNone : RegImportStageProcessExit, 0, exitCode);
    return waitResult == WAIT_OBJECT_0 && exitCode == 0 ? RegImportSuccess : RegImportFailed;
}

static DWORD WINAPI RegImportThreadProc(void* parameter) {
    RegImportTask* task = (RegImportTask*)parameter;
    RegImportResult result = task ? ImportRegistryFileString(task->regPath, task->workingDirectory, task) : RegImportFailed;
    if (task && task->window) {
        if (!PostMessageW(task->window, XHL_WM_REG_IMPORT_DONE, (WPARAM)result, (LPARAM)task)) {
            InterlockedExchange(&g_regImportInProgress, 0);
            delete task;
        }
    } else if (task) {
        InterlockedExchange(&g_regImportInProgress, 0);
        delete task;
    }
    return 0;
}

static void ShowRuntimeError(HWND owner, const WCHAR* zhHans, const WCHAR* zhHant, const WCHAR* enUs) {
    AppLanguage language = ReadConfiguredLanguage();
    BOOL chinese = LanguageIsChinese(language);
    ShowAppMessageForLanguage(owner, LocalizedText(language, zhHans, zhHant, enUs), g_displayTitle, TD_ERROR_ICON, MB_ICONERROR, chinese);
}

static const WCHAR* DefaultRegImportingText(BOOL chinese) {
    return LocalizedText(chinese ? ReadConfiguredLanguage() : AppLanguageEnglish, L"正在导入...", L"正在導入...", L"Importing...");
}

static const WCHAR* DefaultRegSuccessMessage(BOOL chinese) {
    return LocalizedText(chinese ? ReadConfiguredLanguage() : AppLanguageEnglish, L"注册表导入成功。", L"註冊表導入成功。", L"Registry import succeeded.");
}

static const WCHAR* DefaultRegFailureMessage(BOOL chinese) {
    return LocalizedText(chinese ? ReadConfiguredLanguage() : AppLanguageEnglish,
        L"注册表导入失败。注册表可能已经部分导入，请重试或手动导入。", L"註冊表導入失敗。註冊表可能已經部分導入，請重試或手動導入。",
        L"Registry import failed. The registry may have been partially imported. Please retry or import it manually.");
}

static const WCHAR* DefaultRegTimeoutMessage(BOOL chinese) {
    return LocalizedText(chinese ? ReadConfiguredLanguage() : AppLanguageEnglish,
        L"注册表导入超过60秒，已停止。注册表可能已经部分导入，请重试或手动导入。", L"註冊表導入超過60秒，已停止。註冊表可能已經部分導入，請重試或手動導入。",
        L"Registry import took more than 60 seconds and was stopped. The registry may have been partially imported. Please retry or import it manually.");
}

static std::wstring DefaultedTextString(const WCHAR* configured, const WCHAR* fallback) {
    if (configured && configured[0]) {
        return configured;
    }
    return fallback ? std::wstring(fallback) : std::wstring();
}

static const WCHAR* RegImportStageName(RegImportStage stage) {
    switch (stage) {
    case RegImportStageWindowsDirectory:
        return L"WindowsDirectory";
    case RegImportStageLocateRegExe:
        return L"LocateRegExe";
    case RegImportStageCreateProcess:
        return L"CreateProcess";
    case RegImportStageAssignJob:
        return L"AssignJob";
    case RegImportStageResumeThread:
        return L"ResumeThread";
    case RegImportStageWait:
        return L"Wait";
    case RegImportStageExitCode:
        return L"GetExitCode";
    case RegImportStageProcessExit:
        return L"ProcessExit";
    default:
        return L"None";
    }
}

static void AppendRegImportDetailMessage(std::wstring& message, const RegImportTask* task) {
    if (!task || task->stage == RegImportStageNone) {
        return;
    }

    AppLanguage language = ReadConfiguredLanguage();
    message += L"\r\n\r\n";
    message += LocalizedText(language, L"诊断信息：", L"診斷信息：", L"Diagnostics:");
    message += L"\r\nStage: ";
    message += RegImportStageName(task->stage);
    message += L"\r\nExitCode: ";
    message += std::to_wstring(task->exitCode);
    if (task->lastError) {
        message += L"\r\nLastError: ";
        message += std::to_wstring(task->lastError);
    }
    if (!task->jobAssigned) {
        message += L"\r\nJobObject: ";
        message += LocalizedText(language, L"未绑定，已继续尝试导入", L"未綁定，已繼續嘗試導入", L"not attached, import continued");
    }
}

static void ShowMode2PathResolveError(HWND owner, PathResolveResult result, BOOL workingDirectory) {
    if (workingDirectory) {
        if (result == PathResolveMissingDirectory) {
            ShowRuntimeError(owner, L"工作目录不存在。", L"工作目錄不存在。", L"Working directory does not exist.");
        } else {
            ShowRuntimeError(owner, L"工作目录无效或无法解析。", L"工作目錄無效或無法解析。", L"Working directory is invalid or cannot be resolved.");
        }
        return;
    }

    if (result == PathResolveWrongExtension) {
        ShowRuntimeError(owner, L"路径后缀不支持。", L"路徑後綴不支持。", L"Path extension is not supported.");
    } else if (result == PathResolveMissingFile) {
        ShowRuntimeError(owner, L"文件不存在。", L"文件不存在。", L"File does not exist.");
    } else if (result == PathResolveWrongPathType) {
        ShowRuntimeError(owner, L"路径类型不匹配。请检查相对路径或绝对路径设置。", L"路徑類型不匹配。請檢查相對路徑或絕對路徑設置。", L"Path type does not match. Check the relative or absolute path setting.");
    } else if (result == PathResolveOutsideLauncher) {
        ShowRuntimeError(owner, L"相对路径不能离开启动器所在目录。", L"相對路徑不能離開啟動器所在目錄。", L"Relative path cannot leave the launcher folder.");
    } else {
        ShowRuntimeError(owner, L"路径无效或无法解析。", L"路徑無效或無法解析。", L"Path is invalid or cannot be resolved.");
    }
}

static BOOL OpenConfiguredTargetString(HWND owner, const std::wstring& targetPath, const std::wstring& arguments, const std::wstring& workingDirectory, BOOL showErrors) {
    std::wstring expandedArguments = arguments;
    if (!ExpandEnvironmentVariablesString(expandedArguments)) {
        if (showErrors) {
            ShowRuntimeError(owner, L"启动参数中的变量无法解析。", L"啟動參數中的變量無法解析。", L"Variables in arguments cannot be resolved.");
        }
        return FALSE;
    }

    SHELLEXECUTEINFOW info;
    ZeroMemory(&info, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_NOASYNC;
    info.hwnd = owner;
    info.lpVerb = L"open";
    info.lpFile = targetPath.c_str();
    info.lpParameters = expandedArguments.empty() ? 0 : expandedArguments.c_str();
    info.lpDirectory = workingDirectory.empty() ? g_ctx.fullBaseDir.c_str() : workingDirectory.c_str();
    info.nShow = SW_SHOWNORMAL;
    if (ShellExecuteExW(&info)) {
        return TRUE;
    }

    INT_PTR errorCode = (INT_PTR)info.hInstApp;
    if (errorCode == SE_ERR_NOASSOC) {
        ZeroMemory(&info, sizeof(info));
        info.cbSize = sizeof(info);
        info.fMask = SEE_MASK_NOASYNC;
        info.hwnd = owner;
        info.lpVerb = L"openas";
        info.lpFile = targetPath.c_str();
        info.lpParameters = expandedArguments.empty() ? 0 : expandedArguments.c_str();
        info.lpDirectory = workingDirectory.empty() ? g_ctx.fullBaseDir.c_str() : workingDirectory.c_str();
        info.nShow = SW_SHOWNORMAL;
        if (ShellExecuteExW(&info)) {
            return TRUE;
        }
    }

    if (showErrors) {
        ShowRuntimeError(owner, L"无法打开目标文件。", L"無法打開目標文件。", L"Cannot open the target file.");
    }
    return FALSE;
}

static void ExecuteMode1Registry() {
    const WCHAR* registrySection = Mode1RegistrySection();

    if (ReadModeIniFlag(ModeSilent, registrySection, L"AddReg", FALSE) && !ReadModeIniFlag(ModeSilent, registrySection, L"RegImported", FALSE)) {
        std::wstring regValue;
        if (!ReadModeIniTextStringChecked(ModeSilent, registrySection, L"RegFileName", L"", regValue)) {
            return;
        }
        std::wstring regPath;
        std::wstring workingDirectory;
        BOOL useRelativePath = ReadModeIniFlag(ModeSilent, registrySection, L"UseRelativePath", TRUE);
        if (ResolveConfiguredFileString(regValue, L".reg", useRelativePath, regPath) == PathResolveOk
            && ResolveWorkingDirectoryString(ModeSilent, registrySection, regPath, L"WorkingDirectoryMode", L"WorkingDirectory", workingDirectory) == PathResolveOk) {
            RegImportResult result = ImportRegistryFileString(regPath, workingDirectory, 0);
            if (result == RegImportSuccess) {
                WriteIniTextUtf8(registrySection, L"RegImported", L"1");
            }
        }
    }
}

static void ExecuteMode1Config() {
    LoadMode1Variables();
    ExecuteMode1Registry();

    for (UINT i = 1; i <= XHL_MODE1_RUN_COUNT; ++i) {
        WCHAR section[32];
        std::wstring targetValue;
        std::wstring targetPath;
        std::wstring arguments;
        std::wstring workingDirectory;

        Mode1RunItemSection(i, section, 32);
        if (!ReadModeIniFlag(ModeSilent, section, L"Run", FALSE)) {
            continue;
        }

        if (!ReadModeIniTextStringChecked(ModeSilent, section, L"FileName", L"", targetValue)
            || !ReadModeIniTextStringChecked(ModeSilent, section, L"Arguments", L"", arguments)) {
            continue;
        }
        BOOL useRelativePath = ReadModeIniFlag(ModeSilent, section, L"UseRelativePath", TRUE);

        if (ResolveConfiguredFileString(targetValue, L"", useRelativePath, targetPath) == PathResolveOk
            && ResolveWorkingDirectoryString(ModeSilent, section, targetPath, L"WorkingDirectoryMode", L"WorkingDirectory", workingDirectory) == PathResolveOk) {
            OpenConfiguredTargetString(0, targetPath, arguments, workingDirectory, FALSE);
        }
    }
}

static void ClearMode2Elements() {
    ZeroMemory(g_mode2Elements, sizeof(g_mode2Elements));
    for (UINT i = 0; i < XHL_MODE2_ELEMENT_COUNT; ++i) {
        g_mode2TextFileDisplayTexts[i].clear();
    }
}

static UINT LoadMode2Elements() {
    ClearMode2Elements();

    UINT count = 0;
    for (UINT i = 1; i <= XHL_MODE2_ELEMENT_COUNT; ++i) {
        WCHAR section[32];
        WCHAR type[32];

        Mode2ElementSection(i, section, 32);
        if (!ReadModeIniFlag(ModeWindow, section, L"Show", FALSE)) {
            continue;
        }

        ReadIniText(section, L"Type", L"", type, 32);
        Mode2ElementKind kind = Mode2KindFromType(type);
        if (kind == Mode2ElementNone) {
            continue;
        }

        Mode2Element* element = &g_mode2Elements[count++];
        element->visible = TRUE;
        element->kind = kind;
        element->index = i;
        ReadIniText(section, L"HeightScale", L"1", type, 32);
        element->heightScale = NormalizeHeightScale(ParseUIntInRange(type, 1, 1, 3));
        element->minimizeOnClick = (kind == Mode2ElementOpenButton || kind == Mode2ElementMessageButton || kind == Mode2ElementRegButton)
            ? ReadModeIniFlag(ModeWindow, section, L"MinimizeOnClick", FALSE)
            : FALSE;
        element->control = 0;
        ReadModeIniText(ModeWindow, section, L"Text", L"", element->text, XHL_MAX_TEXT);
        if (kind == Mode2ElementTextBox) {
            ExpandLineBreakEscapes(element->text, XHL_MAX_TEXT);
        }
        if (kind == Mode2ElementTextFileBox) {
            element->textFileUseRelativePath = ReadModeIniFlag(ModeWindow, section, L"TextFileUseRelativePath", TRUE);
            ReadModeIniText(ModeWindow, section, L"TextFileName", L"", element->textFileName, XHL_MAX_TEXT);
            std::wstring textPath;
            std::wstring fileText;
            if (ResolveConfiguredFileString(element->textFileName, L"", element->textFileUseRelativePath, textPath) == PathResolveOk
                && LoadDisplayTextFileStringLimited(textPath.c_str(), XHL_TEXT_FILE_MAX_BYTES, fileText)) {
                g_mode2TextFileDisplayTexts[count - 1].swap(fileText);
            }
        } else if (kind == Mode2ElementText) {
            CopyText(element->displayText, XHL_MAX_TEXT, element->text);
            TruncateText(element->displayText, XHL_TEXT_DISPLAY_MAX_CHARS);
        } else if (IsMode2ButtonKind(kind)) {
            CopyText(element->displayText, XHL_MAX_TEXT, element->text);
            TruncateText(element->displayText, XHL_BUTTON_DISPLAY_MAX_CHARS);
        } else {
            CopyText(element->displayText, XHL_MAX_TEXT, element->text);
        }

        if (kind == Mode2ElementOpenButton) {
            for (UINT runIndex = 0; runIndex < XHL_MODE2_BUTTON_RUN_COUNT; ++runIndex) {
                WCHAR key[32];
                CopyText(key, 32, L"Run");
                AppendUInt(key, 32, runIndex + 1);
                element->runs[runIndex].run = ReadModeIniFlag(ModeWindow, section, key, FALSE);

                CopyText(key, 32, L"UseRelativePath");
                AppendUInt(key, 32, runIndex + 1);
                element->runs[runIndex].useRelativePath = ReadModeIniFlag(ModeWindow, section, key, TRUE);

                CopyText(key, 32, L"FileName");
                AppendUInt(key, 32, runIndex + 1);
                ReadModeIniText(ModeWindow, section, key, L"", element->runs[runIndex].fileName, XHL_MAX_TEXT);

                CopyText(key, 32, L"Arguments");
                AppendUInt(key, 32, runIndex + 1);
                ReadModeIniText(ModeWindow, section, key, L"", element->runs[runIndex].arguments, XHL_MAX_TEXT);

                CopyText(key, 32, L"WorkingDirectoryMode");
                AppendUInt(key, 32, runIndex + 1);
                ReadIniText(section, key, L"TargetDir", type, 32);
                element->runs[runIndex].workingDirectoryMode = WorkingDirectoryModeFromText(type);

                CopyText(key, 32, L"WorkingDirectory");
                AppendUInt(key, 32, runIndex + 1);
                ReadModeIniText(ModeWindow, section, key, L"", element->runs[runIndex].workingDirectory, XHL_MAX_TEXT);
            }
        } else if (kind == Mode2ElementMessageButton) {
            ReadModeIniText(ModeWindow, section, L"Message", L"", element->message, XHL_MAX_TEXT);
            ExpandLineBreakEscapes(element->message, XHL_MAX_TEXT);
        } else if (kind == Mode2ElementRegButton) {
            element->regUseRelativePath = ReadModeIniFlag(ModeWindow, section, L"UseRelativePath", TRUE);
            ReadModeIniText(ModeWindow, section, L"RegFileName", L"", element->regFileName, XHL_MAX_TEXT);
            ReadIniText(section, L"WorkingDirectoryMode", L"TargetDir", type, 32);
            element->regWorkingDirectoryMode = WorkingDirectoryModeFromText(type);
            ReadModeIniText(ModeWindow, section, L"WorkingDirectory", L"", element->regWorkingDirectory, XHL_MAX_TEXT);
            ReadModeIniText(ModeWindow, section, L"RegImportingText", L"", element->regImportingText, XHL_MAX_TEXT);
            ReadModeIniText(ModeWindow, section, L"RegSuccessMessage", L"", element->regSuccessMessage, XHL_MAX_TEXT);
            ReadModeIniText(ModeWindow, section, L"RegFailureMessage", L"", element->regFailureMessage, XHL_MAX_TEXT);
            ReadModeIniText(ModeWindow, section, L"RegTimeoutMessage", L"", element->regTimeoutMessage, XHL_MAX_TEXT);
            ExpandLineBreakEscapes(element->regSuccessMessage, XHL_MAX_TEXT);
            ExpandLineBreakEscapes(element->regFailureMessage, XHL_MAX_TEXT);
            ExpandLineBreakEscapes(element->regTimeoutMessage, XHL_MAX_TEXT);
        } else if (kind == Mode2ElementTable) {
            element->csvUseRelativePath = ReadModeIniFlag(ModeWindow, section, L"CsvUseRelativePath", TRUE);
            ReadModeIniText(ModeWindow, section, L"CsvFileName", L"", element->csvFileName, XHL_MAX_TEXT);
        } else if (kind == Mode2ElementInput) {
            ReadIniText(section, L"Variable", L"1", type, 32);
            element->inputVariableIndex = ParseUIntInRange(type, 1, 1, XHL_MODE2_VARIABLE_COUNT) - 1;
        }
    }

    return count;
}

static void CreateMode2Controls(HWND window) {
    for (UINT i = 0; i < XHL_MODE2_ELEMENT_COUNT; ++i) {
        Mode2Element* element = &g_mode2Elements[i];
        if (!element->visible) {
            continue;
        }

        if (IsMode2ButtonKind(element->kind)) {
            element->control = CreateWindowExW(
                0,
                L"BUTTON",
                element->displayText,
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                0,
                0,
                0,
                0,
                window,
                (HMENU)(UINT_PTR)(XHL_MODE2_CONTROL_ID_BASE + element->index),
                0,
                0);
        } else if (element->kind == Mode2ElementTextBox || element->kind == Mode2ElementTextFileBox) {
            const WCHAR* editText = element->kind == Mode2ElementTextFileBox ? g_mode2TextFileDisplayTexts[i].c_str() : element->displayText;
            element->control = CreateWindowExW(
                WS_EX_CLIENTEDGE,
                L"EDIT",
                L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                0,
                0,
                0,
                0,
                window,
                (HMENU)(UINT_PTR)(XHL_MODE2_CONTROL_ID_BASE + element->index),
                0,
                0);
            if (element->control) {
                if (element->kind == Mode2ElementTextFileBox) {
                    SendMessageW(element->control, EM_LIMITTEXT, (WPARAM)(XHL_TEXT_FILE_MAX_BYTES * 2), 0);
                }
                SetWindowTextW(element->control, editText);
                SendMessageW(element->control, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(6, 6));
            }
        } else if (element->kind == Mode2ElementText) {
            element->control = CreateWindowExW(
                0,
                L"STATIC",
                element->displayText,
                WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | SS_NOPREFIX,
                0,
                0,
                0,
                0,
                window,
                (HMENU)(UINT_PTR)(XHL_MODE2_CONTROL_ID_BASE + element->index),
                0,
                0);
        } else if (element->kind == Mode2ElementTable) {
            element->control = CreateWindowExW(
                WS_EX_CLIENTEDGE,
                WC_LISTVIEWW,
                L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
                0,
                0,
                0,
                0,
                window,
                (HMENU)(UINT_PTR)(XHL_MODE2_CONTROL_ID_BASE + element->index),
                GetModuleHandleW(0),
                0);
            if (element->control) {
                SendMessageW(element->control, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
                std::wstring csvPath;
                if (ResolveConfiguredFileString(element->csvFileName, L".csv", element->csvUseRelativePath, csvPath) == PathResolveOk) {
                    PopulateTableControlFromCsv(element->control, csvPath);
                }
            }
        } else if (element->kind == Mode2ElementInput) {
            const WCHAR* value = L"";
            if (element->inputVariableIndex < XHL_MODE2_VARIABLE_COUNT) {
                value = g_mode2VariableValues[element->inputVariableIndex];
            }
            element->control = CreateWindowExW(
                WS_EX_CLIENTEDGE,
                L"EDIT",
                value,
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
                0,
                0,
                0,
                0,
                window,
                (HMENU)(UINT_PTR)(XHL_MODE2_CONTROL_ID_BASE + element->index),
                GetModuleHandleW(0),
                0);
            if (element->control) {
                SendMessageW(element->control, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(6, 6));
            }
        }
    }
}

static void SyncMode2InputVariables() {
    for (UINT i = 0; i < XHL_MODE2_ELEMENT_COUNT; ++i) {
        Mode2Element* element = &g_mode2Elements[i];
        if (!element->visible || element->kind != Mode2ElementInput || !element->control) {
            continue;
        }
        if (element->inputVariableIndex >= XHL_MODE2_VARIABLE_COUNT) {
            continue;
        }
        GetWindowTextW(element->control, g_mode2VariableValues[element->inputVariableIndex], XHL_MAX_TEXT);
        g_mode2VariableEnabled[element->inputVariableIndex] = TRUE;
    }
}

static Mode2Element* FindMode2ElementById(UINT controlId) {
    if (controlId < XHL_MODE2_CONTROL_ID_BASE) {
        return 0;
    }

    UINT index = controlId - XHL_MODE2_CONTROL_ID_BASE;
    for (UINT i = 0; i < XHL_MODE2_ELEMENT_COUNT; ++i) {
        Mode2Element* element = &g_mode2Elements[i];
        if (element->visible && IsMode2ButtonKind(element->kind) && element->index == index) {
            return element;
        }
    }
    return 0;
}

static void StartMode2RegistryImport(HWND window, Mode2Element* element, const std::wstring& regPath, const std::wstring& workingDirectory) {
    if (!element || !element->control) {
        return;
    }
    if (InterlockedCompareExchange(&g_regImportInProgress, 1, 0) != 0) {
        ShowRuntimeError(window, L"注册表正在导入，请等待当前操作完成。", L"註冊表正在導入，請等待當前操作完成。", L"Registry import is already running. Please wait for it to finish.");
        return;
    }

    RegImportTask* task = new RegImportTask;
    BOOL chinese = IsChineseLanguage();
    task->window = window;
    task->button = element->control;
    task->regPath = regPath;
    task->workingDirectory = workingDirectory;
    task->originalText = element->displayText;
    task->importingText = DefaultedTextString(element->regImportingText, DefaultRegImportingText(chinese));
    task->successMessage = DefaultedTextString(element->regSuccessMessage, DefaultRegSuccessMessage(chinese));
    task->failureMessage = DefaultedTextString(element->regFailureMessage, DefaultRegFailureMessage(chinese));
    task->timeoutMessage = DefaultedTextString(element->regTimeoutMessage, DefaultRegTimeoutMessage(chinese));
    task->stage = RegImportStageNone;
    task->lastError = 0;
    task->exitCode = 1;
    task->jobAssigned = FALSE;

    EnableWindow(element->control, FALSE);
    SetWindowTextW(element->control, task->importingText.c_str());

    HANDLE thread = CreateThread(0, 0, RegImportThreadProc, task, 0, 0);
    if (!thread) {
        SetWindowTextW(element->control, task->originalText.c_str());
        EnableWindow(element->control, TRUE);
        InterlockedExchange(&g_regImportInProgress, 0);
        delete task;
        ShowRuntimeError(window, L"注册表导入失败。", L"註冊表導入失敗。", L"Registry import failed.");
        return;
    }
    CloseHandle(thread);
}

static void RunMode2ButtonAction(HWND window, UINT controlId) {
    SyncMode2InputVariables();

    Mode2Element* element = FindMode2ElementById(controlId);
    if (!element) {
        return;
    }

    if (element->minimizeOnClick && element->kind != Mode2ElementCloseButton) {
        ShowWindow(window, SW_MINIMIZE);
    }

    if (element->kind == Mode2ElementOpenButton) {
        WCHAR section[32];
        Mode2ElementSection(element->index, section, 32);
        for (UINT i = 0; i < XHL_MODE2_BUTTON_RUN_COUNT; ++i) {
            WCHAR key[32];
            CopyText(key, 32, L"Run");
            AppendUInt(key, 32, i + 1);
            if (!ReadModeIniFlag(ModeWindow, section, key, FALSE)) {
                continue;
            }

            CopyText(key, 32, L"UseRelativePath");
            AppendUInt(key, 32, i + 1);
            BOOL useRelativePath = ReadModeIniFlag(ModeWindow, section, key, TRUE);

            CopyText(key, 32, L"FileName");
            AppendUInt(key, 32, i + 1);
            std::wstring targetValue;
            if (!ReadModeIniTextStringChecked(ModeWindow, section, key, L"", targetValue)) {
                ShowMode2PathResolveError(window, PathResolveInvalid, FALSE);
                continue;
            }
            std::wstring targetPath;
            PathResolveResult pathResult = ResolveConfiguredFileString(targetValue, L"", useRelativePath, targetPath);
            if (pathResult != PathResolveOk) {
                ShowMode2PathResolveError(window, pathResult, FALSE);
                continue;
            }

            CopyText(key, 32, L"WorkingDirectoryMode");
            AppendUInt(key, 32, i + 1);
            std::wstring workingDirectoryMode = ReadIniTextString(section, key, L"TargetDir");
            CopyText(key, 32, L"WorkingDirectory");
            AppendUInt(key, 32, i + 1);
            std::wstring configuredWorkingDirectory;
            if (!ReadModeIniTextStringChecked(ModeWindow, section, key, L"", configuredWorkingDirectory)) {
                ShowMode2PathResolveError(window, PathResolveInvalid, TRUE);
                continue;
            }
            std::wstring workingDirectory;
            PathResolveResult workResult = ResolveWorkingDirectoryFromValuesString(WorkingDirectoryModeFromText(workingDirectoryMode.c_str()), configuredWorkingDirectory, targetPath, workingDirectory);
            if (workResult != PathResolveOk) {
                ShowMode2PathResolveError(window, workResult, TRUE);
                continue;
            }

            CopyText(key, 32, L"Arguments");
            AppendUInt(key, 32, i + 1);
            std::wstring arguments;
            if (!ReadModeIniTextStringChecked(ModeWindow, section, key, L"", arguments)) {
                ShowRuntimeError(window, L"启动参数过长或无法解析。", L"啟動參數過長或無法解析。", L"Arguments are too long or cannot be resolved.");
                continue;
            }
            OpenConfiguredTargetString(window, targetPath, arguments, workingDirectory, TRUE);
        }
        return;
    }

    if (element->kind == Mode2ElementMessageButton) {
        WCHAR section[32];
        Mode2ElementSection(element->index, section, 32);
        WCHAR messageText[XHL_MAX_TEXT];
        ReadModeIniText(ModeWindow, section, L"Message", L"", messageText, XHL_MAX_TEXT);
        ExpandLineBreakEscapes(messageText, XHL_MAX_TEXT);
        ShowAppMessage(window, messageText, g_displayTitle, TD_INFORMATION_ICON, MB_ICONINFORMATION);
        return;
    }

    if (element->kind == Mode2ElementRegButton) {
        WCHAR section[32];
        Mode2ElementSection(element->index, section, 32);
        BOOL useRelativePath = ReadModeIniFlag(ModeWindow, section, L"UseRelativePath", TRUE);
        std::wstring regValue;
        if (!ReadModeIniTextStringChecked(ModeWindow, section, L"RegFileName", L"", regValue)) {
            ShowMode2PathResolveError(window, PathResolveInvalid, FALSE);
            return;
        }
        std::wstring regPath;
        PathResolveResult pathResult = ResolveConfiguredFileString(regValue, L".reg", useRelativePath, regPath);
        if (pathResult != PathResolveOk) {
            ShowMode2PathResolveError(window, pathResult, FALSE);
            return;
        }

        std::wstring workingDirectoryMode;
        std::wstring configuredWorkingDirectory;
        workingDirectoryMode = ReadIniTextString(section, L"WorkingDirectoryMode", L"TargetDir");
        if (!ReadModeIniTextStringChecked(ModeWindow, section, L"WorkingDirectory", L"", configuredWorkingDirectory)) {
            ShowMode2PathResolveError(window, PathResolveInvalid, TRUE);
            return;
        }
        std::wstring workingDirectory;
        PathResolveResult workResult = ResolveWorkingDirectoryFromValuesString(WorkingDirectoryModeFromText(workingDirectoryMode.c_str()), configuredWorkingDirectory, regPath, workingDirectory);
        if (workResult != PathResolveOk) {
            ShowMode2PathResolveError(window, workResult, TRUE);
            return;
        }

        ReadModeIniText(ModeWindow, section, L"RegImportingText", L"", element->regImportingText, XHL_MAX_TEXT);
        ReadModeIniText(ModeWindow, section, L"RegSuccessMessage", L"", element->regSuccessMessage, XHL_MAX_TEXT);
        ReadModeIniText(ModeWindow, section, L"RegFailureMessage", L"", element->regFailureMessage, XHL_MAX_TEXT);
        ReadModeIniText(ModeWindow, section, L"RegTimeoutMessage", L"", element->regTimeoutMessage, XHL_MAX_TEXT);
        ExpandLineBreakEscapes(element->regSuccessMessage, XHL_MAX_TEXT);
        ExpandLineBreakEscapes(element->regFailureMessage, XHL_MAX_TEXT);
        ExpandLineBreakEscapes(element->regTimeoutMessage, XHL_MAX_TEXT);

        StartMode2RegistryImport(window, element, regPath, workingDirectory);
        return;
    }

    if (element->kind == Mode2ElementCloseButton) {
        SendMessageW(window, WM_CLOSE, 0, 0);
        return;
    }
}

static void CalculateMode2WindowSize(DWORD style, DWORD exStyle, UINT dpi, int* width, int* height) {
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = ScaleForDpi(XHL_MODE2_WINDOW_WIDTH, dpi);
    rect.bottom = Mode2ClientHeight(dpi);

    typedef BOOL(WINAPI* AdjustWindowRectExForDpiProc)(LPRECT, DWORD, BOOL, DWORD, UINT);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    AdjustWindowRectExForDpiProc adjustForDpi = user32 ? (AdjustWindowRectExForDpiProc)GetProcAddress(user32, "AdjustWindowRectExForDpi") : 0;
    BOOL adjusted = adjustForDpi ? adjustForDpi(&rect, style, FALSE, exStyle, dpi) : FALSE;
    if (!adjusted) {
        AdjustWindowRectEx(&rect, style, FALSE, exStyle);
    }

    *width = rect.right - rect.left;
    *height = rect.bottom - rect.top;
}

static AppLanguage ReadConfiguredLanguage() {
    WCHAR language[32];
    ReadIniText(L"Launcher", L"Language", L"en-US", language, 32);
    TrimInPlace(language);
    return AppLanguageFromText(language);
}

static BOOL IsChineseLanguage() {
    return LanguageIsChinese(ReadConfiguredLanguage());
}

static const WCHAR* EditorText(BOOL chinese, const WCHAR* zhHans, const WCHAR* zhHant, const WCHAR* enUs) {
    if (!chinese) {
        return enUs;
    }
    return LocalizedText(GraphEditorLanguage(), zhHans, zhHant, enUs);
}

static HWND CreateEditorControl(HWND window, const WCHAR* className, const WCHAR* text, DWORD style, DWORD exStyle, int x, int y, int width, int height, int id, UINT dpi) {
    if (SameTextI(className, L"STATIC")) {
        style |= SS_NOTIFY;
    }

    HWND control = CreateWindowExW(
        exStyle,
        className,
        text,
        WS_CHILD | WS_VISIBLE | style,
        ScaleForDpi(x, dpi),
        ScaleForDpi(y, dpi),
        ScaleForDpi(width, dpi),
        ScaleForDpi(height, dpi),
        window,
        (HMENU)(UINT_PTR)id,
        GetModuleHandleW(0),
        0);
    if (control && g_uiFont) {
        SendMessageW(control, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
    }
    return control;
}

static void SetEditorModeCombo(LauncherMode mode) {
    if (!g_editorModeCombo) {
        return;
    }
    BOOL chinese = LanguageIsChinese(g_editorConfig.language);
    SendMessageW(g_editorModeCombo, CB_RESETCONTENT, 0, 0);
    SendMessageW(g_editorModeCombo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"编辑模式", L"編輯模式", L"Editor mode"));
    SendMessageW(g_editorModeCombo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"静默模式", L"靜默模式", L"Silent mode"));
    SendMessageW(g_editorModeCombo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"窗口模式", L"窗口模式", L"Window mode"));
    int selected = 0;
    if (mode == ModeSilent) selected = 1;
    else if (mode == ModeWindow) selected = 2;
    SendMessageW(g_editorModeCombo, CB_SETCURSEL, selected, 0);
}

static void SetEditorLanguageCombo(AppLanguage language) {
    if (!g_editorLanguageCombo) {
        return;
    }
    SendMessageW(g_editorLanguageCombo, CB_RESETCONTENT, 0, 0);
    SendMessageW(g_editorLanguageCombo, CB_ADDSTRING, 0, (LPARAM)L"English");
    SendMessageW(g_editorLanguageCombo, CB_ADDSTRING, 0, (LPARAM)L"简体中文");
    SendMessageW(g_editorLanguageCombo, CB_ADDSTRING, 0, (LPARAM)L"繁體中文");
    int selected = 0;
    if (language == AppLanguageSimplifiedChinese) selected = 1;
    else if (language == AppLanguageTraditionalChinese) selected = 2;
    SendMessageW(g_editorLanguageCombo, CB_SETCURSEL, selected, 0);
}

static LauncherMode ReadEditorModeCombo() {
    if (!g_editorModeCombo) {
        return g_editorConfig.mode;
    }
    int selected = (int)SendMessageW(g_editorModeCombo, CB_GETCURSEL, 0, 0);
    if (selected == 1) {
        return ModeSilent;
    }
    if (selected == 2) {
        return ModeWindow;
    }
    return ModeDisabled;
}

static BOOL GraphEditorUsesChinese() {
    return LanguageIsChinese(GraphEditorLanguage());
}

static AppLanguage GraphEditorLanguage() {
    if (g_editorLanguageCombo) {
        int selected = (int)SendMessageW(g_editorLanguageCombo, CB_GETCURSEL, 0, 0);
        if (selected == 1) {
            return AppLanguageSimplifiedChinese;
        }
        if (selected == 2) {
            return AppLanguageTraditionalChinese;
        }
        return AppLanguageEnglish;
    }
    return g_editorConfig.language;
}

static HWND GraphPageControl(const WCHAR* className, const WCHAR* text, DWORD style, DWORD exStyle, int x, int y, int width, int height, int id) {
    return CreateEditorControl(g_editorPanel, className, text, style, exStyle, x, y, width, height, id, GetDpi(g_editorPanel));
}

static void ApplyBoldEditorFont(HWND control) {
    if (control && g_uiBoldFont) {
        SendMessageW(control, WM_SETFONT, (WPARAM)g_uiBoldFont, TRUE);
    }
}

static void InitEditorTooltip(HWND window, UINT dpi) {
    if (g_editorTooltip) {
        DestroyWindow(g_editorTooltip);
        g_editorTooltip = 0;
    }
    g_editorTooltipTexts.clear();

    INITCOMMONCONTROLSEX controls;
    ZeroMemory(&controls, sizeof(controls));
    controls.dwSize = sizeof(controls);
    controls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&controls);

    g_editorTooltip = CreateWindowExW(
        WS_EX_TOPMOST,
        TOOLTIPS_CLASSW,
        0,
        WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        window,
        0,
        GetModuleHandleW(0),
        0);
    if (!g_editorTooltip) {
        return;
    }

    SendMessageW(g_editorTooltip, TTM_SETMAXTIPWIDTH, 0, ScaleForDpi(360, dpi));
    SetWindowPos(g_editorTooltip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

static void AddEditorTooltip(HWND control, const WCHAR* text) {
    if (!g_editorTooltip || !control || !text || !text[0]) {
        return;
    }
    g_editorTooltipTexts.push_back(text);

    TOOLINFOW tool;
    ZeroMemory(&tool, sizeof(tool));
    tool.cbSize = sizeof(tool);
    tool.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    tool.hwnd = GetParent(control);
    tool.uId = (UINT_PTR)control;
    tool.lpszText = (LPWSTR)g_editorTooltipTexts.back().c_str();
    SendMessageW(g_editorTooltip, TTM_ADDTOOLW, 0, (LPARAM)&tool);
}

static void RemoveEditorTooltip(HWND control) {
    if (!g_editorTooltip || !control) {
        return;
    }

    TOOLINFOW tool;
    ZeroMemory(&tool, sizeof(tool));
    tool.cbSize = sizeof(tool);
    tool.uFlags = TTF_IDISHWND;
    tool.hwnd = GetParent(control);
    tool.uId = (UINT_PTR)control;
    SendMessageW(g_editorTooltip, TTM_DELTOOLW, 0, (LPARAM)&tool);
}

static void ClearEditorTooltipsFrom(HWND parent) {
    if (!parent) {
        return;
    }

    HWND child = GetWindow(parent, GW_CHILD);
    while (child) {
        HWND next = GetWindow(child, GW_HWNDNEXT);
        RemoveEditorTooltip(child);
        child = next;
    }
}

static void ApplyTopEditorTooltips() {
    BOOL chinese = GraphEditorUsesChinese();
    const WCHAR* titleTip = EditorText(chinese,
        L"窗口标题。编辑模式和窗口模式会使用，最多保留32个字符。", L"窗口標題。編輯模式和窗口模式會使用，最多保留32個字符。",
        L"Window title. Used by editor mode and window mode. Kept to 32 characters.");
    const WCHAR* versionTip = EditorText(chinese,
        L"版本号。会跟在模式0和模式2窗口标题后面显示，最多保留32个字符，可填写任意文字。", L"版本號。會跟在模式0和模式2窗口標題後面顯示，最多保留32個字符，可填寫任意文字。",
        L"Version text. Appended after the editor-mode and window-mode title. Kept to 32 characters and may contain any text.");
    const WCHAR* languageTip = EditorText(chinese,
        L"界面语言。可选 English、简体中文、繁体中文；配置文件中保存为 en-US、zh-Hans 或 zh-Hant。", L"界面語言。可選 English、簡體中文、繁體中文；配置文件中保存為 en-US、zh-Hans 或 zh-Hant。",
        L"UI language. Choose English, Simplified Chinese, or Traditional Chinese; the INI stores en-US, zh-Hans, or zh-Hant.");
    const WCHAR* modeTip = EditorText(chinese,
        L"选择程序运行模式。编辑模式只修改配置；静默模式无窗口执行Mode1；窗口模式显示Mode2界面。", L"選擇程序運行模式。編輯模式只修改配置；靜默模式無窗口執行Mode1；窗口模式顯示Mode2界面。",
        L"Choose the program mode. Editor mode edits settings only; Silent mode runs Mode1 without a window; Window mode shows the Mode2 UI.");

    HWND controls[] = {
        g_editorTitleLabel, g_editorTitleEdit, g_editorVersionLabel, g_editorVersionEdit, g_editorLanguageLabel,
        g_editorLanguageCombo, g_editorAboutButton, g_editorModeLabel, g_editorModeCombo
    };
    for (UINT i = 0; i < sizeof(controls) / sizeof(controls[0]); ++i) {
        RemoveEditorTooltip(controls[i]);
    }

    AddEditorTooltip(g_editorTitleLabel, titleTip);
    AddEditorTooltip(g_editorVersionLabel, versionTip);
    AddEditorTooltip(g_editorLanguageLabel, languageTip);
    AddEditorTooltip(g_editorLanguageCombo, languageTip);
    AddEditorTooltip(g_editorModeLabel, modeTip);
    AddEditorTooltip(g_editorModeCombo, modeTip);
    AddEditorTooltip(g_editorAboutButton, EditorText(chinese, L"查看软件简介。", L"查看軟體簡介。", L"Show basic program information."));
}

static void GraphCollapseLineBreaks(WCHAR* text, UINT capacity) {
    CopyText(g_substitutionBuffer, XHL_MAX_TEXT, text);
    text[0] = L'\0';
    for (UINT i = 0; g_substitutionBuffer[i]; ++i) {
        if (g_substitutionBuffer[i] == L'\r') {
            AppendText(text, capacity, L"\\n");
            if (g_substitutionBuffer[i + 1] == L'\n') {
                ++i;
            }
        } else if (g_substitutionBuffer[i] == L'\n') {
            AppendText(text, capacity, L"\\n");
        } else {
            AppendChar(text, capacity, g_substitutionBuffer[i]);
        }
    }
}

static void GraphReadTopToState(HWND window) {
    HWND titleEdit = window ? GetDlgItem(window, XHL_EDITOR_ID_TITLE) : g_editorTitleEdit;
    HWND versionEdit = window ? GetDlgItem(window, XHL_EDITOR_ID_VERSION) : g_editorVersionEdit;

    if (titleEdit) {
        GetWindowTextW(titleEdit, g_editorConfig.title, XHL_MAX_TEXT);
        TruncateText(g_editorConfig.title, XHL_TITLE_MAX_CHARS);
    }
    if (versionEdit) {
        GetWindowTextW(versionEdit, g_editorConfig.version, XHL_MAX_TEXT);
        TruncateText(g_editorConfig.version, XHL_CONFIG_VERSION_MAX_CHARS);
    }
    g_editorConfig.language = GraphEditorLanguage();
    g_editorConfig.chinese = LanguageIsChinese(g_editorConfig.language);
    g_editorConfig.mode = ReadEditorModeCombo();
}

static void GraphGetText(HWND parent, int id, WCHAR* out, UINT capacity) {
    HWND control = GetDlgItem(parent, id);
    if (control) {
        GetWindowTextW(control, out, capacity);
    }
}

static BOOL GraphGetCheck(HWND parent, int id, BOOL fallback) {
    HWND control = GetDlgItem(parent, id);
    return control ? (SendMessageW(control, BM_GETCHECK, 0, 0) == BST_CHECKED) : fallback;
}

static UINT GraphReadComboIndex(HWND parent, int id, UINT fallback, UINT maxExclusive) {
    HWND control = GetDlgItem(parent, id);
    if (!control) {
        return fallback;
    }
    int selected = (int)SendMessageW(control, CB_GETCURSEL, 0, 0);
    if (selected < 0 || (UINT)selected >= maxExclusive) {
        return fallback;
    }
    return (UINT)selected;
}

static void GraphSetCheck(HWND control, BOOL checked) {
    if (control) {
        SendMessageW(control, BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
    }
}

static BOOL GraphReadPathCombo(HWND parent, int id, BOOL fallback) {
    HWND control = GetDlgItem(parent, id);
    if (!control) {
        return fallback;
    }
    return (int)SendMessageW(control, CB_GETCURSEL, 0, 0) == 0;
}

static WorkingDirectoryMode GraphReadWorkingDirectoryCombo(HWND parent, int id, WorkingDirectoryMode fallback) {
    HWND control = GetDlgItem(parent, id);
    if (!control) {
        return fallback;
    }
    int selected = (int)SendMessageW(control, CB_GETCURSEL, 0, 0);
    if (selected == 1) {
        return WorkingDirectoryLauncherDir;
    }
    if (selected == 2) {
        return WorkingDirectoryCustom;
    }
    return WorkingDirectoryTargetDir;
}

static SplashPosition GraphReadSplashPositionCombo(HWND parent, int id, SplashPosition fallback) {
    HWND control = GetDlgItem(parent, id);
    if (!control) {
        return fallback;
    }
    int selected = (int)SendMessageW(control, CB_GETCURSEL, 0, 0);
    if (selected == 1) {
        return SplashTopLeft;
    }
    if (selected == 2) {
        return SplashTopRight;
    }
    if (selected == 3) {
        return SplashBottomLeft;
    }
    if (selected == 4) {
        return SplashBottomRight;
    }
    return SplashCenter;
}

static UINT GraphReadHeightScaleCombo(HWND parent, int id, UINT fallback) {
    HWND control = GetDlgItem(parent, id);
    if (!control) {
        return fallback;
    }
    int selected = (int)SendMessageW(control, CB_GETCURSEL, 0, 0);
    if (selected == 1) {
        return 2;
    }
    if (selected == 2) {
        return 3;
    }
    return 1;
}

static UINT GraphReadSplashScaleCombo(HWND parent, int id, UINT fallback) {
    HWND control = GetDlgItem(parent, id);
    if (!control) {
        return fallback;
    }
    int selected = (int)SendMessageW(control, CB_GETCURSEL, 0, 0);
    if (selected < 0) {
        return fallback;
    }
    UINT scale = XHL_SPLASH_MIN_SCALE + (UINT)selected * 5;
    if (scale < XHL_SPLASH_MIN_SCALE || scale > XHL_SPLASH_MAX_SCALE) {
        return fallback;
    }
    return scale;
}

static void GraphReadTypeCombo(HWND parent, int id, WCHAR* out, UINT capacity) {
    HWND control = GetDlgItem(parent, id);
    if (!control) {
        return;
    }
    int selected = (int)SendMessageW(control, CB_GETCURSEL, 0, 0);
    if (selected == 0) CopyText(out, capacity, L"Text");
    else if (selected == 1) CopyText(out, capacity, L"TextBox");
    else if (selected == 2) CopyText(out, capacity, L"TextFileBox");
    else if (selected == 3) CopyText(out, capacity, L"Input");
    else if (selected == 4) CopyText(out, capacity, L"OpenButton");
    else if (selected == 5) CopyText(out, capacity, L"MessageButton");
    else if (selected == 6) CopyText(out, capacity, L"RegButton");
    else if (selected == 7) CopyText(out, capacity, L"Table");
    else if (selected == 8) CopyText(out, capacity, L"CloseButton");
    else CopyText(out, capacity, L"TextBox");
}

static int GraphM1VarEnableId(UINT index) { return XHL_EDITOR_ID_M1_VAR_BASE + (int)index * 10 + 1; }
static int GraphM1VarValueId(UINT index) { return XHL_EDITOR_ID_M1_VAR_BASE + (int)index * 10 + 2; }
static int GraphM1RegAddId() { return XHL_EDITOR_ID_M1_REG_BASE + 1; }
static int GraphM1RegPathId() { return XHL_EDITOR_ID_M1_REG_BASE + 2; }
static int GraphM1RegFileId() { return XHL_EDITOR_ID_M1_REG_BASE + 3; }
static int GraphM1RegImportedId() { return XHL_EDITOR_ID_M1_REG_BASE + 4; }
static int GraphM1RegWorkModeId() { return XHL_EDITOR_ID_M1_REG_BASE + 5; }
static int GraphM1RegWorkDirId() { return XHL_EDITOR_ID_M1_REG_BASE + 6; }
static int GraphM1RunRunId(UINT index) { return XHL_EDITOR_ID_M1_RUN_BASE + (int)index * 10 + 1; }
static int GraphM1RunPathId(UINT index) { return XHL_EDITOR_ID_M1_RUN_BASE + (int)index * 10 + 2; }
static int GraphM1RunFileId(UINT index) { return XHL_EDITOR_ID_M1_RUN_BASE + (int)index * 10 + 3; }
static int GraphM1RunArgsId(UINT index) { return XHL_EDITOR_ID_M1_RUN_BASE + (int)index * 10 + 4; }
static int GraphM1RunWorkModeId(UINT index) { return XHL_EDITOR_ID_M1_RUN_BASE + (int)index * 10 + 5; }
static int GraphM1RunWorkDirId(UINT index) { return XHL_EDITOR_ID_M1_RUN_BASE + (int)index * 10 + 6; }
static int GraphM2ElementBase(UINT index) { return XHL_EDITOR_ID_M2_ELEMENT_BASE + (int)index * 100; }
static int GraphM2ElementShowId(UINT index) { return GraphM2ElementBase(index) + 1; }
static int GraphM2ElementTypeId(UINT index) { return GraphM2ElementBase(index) + 3; }
static int GraphM2ElementTextId(UINT index) { return GraphM2ElementBase(index) + 4; }
static int GraphM2ElementMessageId(UINT index) { return GraphM2ElementBase(index) + 5; }
static int GraphM2ElementRegFileId(UINT index) { return GraphM2ElementBase(index) + 6; }
static int GraphM2ElementRegPathId(UINT index) { return GraphM2ElementBase(index) + 7; }
static int GraphM2ElementRegWorkModeId(UINT index) { return GraphM2ElementBase(index) + 8; }
static int GraphM2ElementRegWorkDirId(UINT index) { return GraphM2ElementBase(index) + 9; }
static int GraphM2ElementRegImportingId(UINT index) { return GraphM2ElementBase(index) + 10; }
static int GraphM2ElementRegSuccessId(UINT index) { return GraphM2ElementBase(index) + 11; }
static int GraphM2ElementRegFailureId(UINT index) { return GraphM2ElementBase(index) + 12; }
static int GraphM2ElementRegTimeoutId(UINT index) { return GraphM2ElementBase(index) + 13; }
static int GraphM2ElementCsvFileId(UINT index) { return GraphM2ElementBase(index) + 14; }
static int GraphM2ElementCsvPathId(UINT index) { return GraphM2ElementBase(index) + 15; }
static int GraphM2ElementHeightScaleId(UINT index) { return GraphM2ElementBase(index) + 16; }
static int GraphM2ElementInputVariableId(UINT index) { return GraphM2ElementBase(index) + 17; }
static int GraphM2ElementTextFileId(UINT index) { return GraphM2ElementBase(index) + 18; }
static int GraphM2ElementTextFilePathId(UINT index) { return GraphM2ElementBase(index) + 19; }
static int GraphM2ElementMinimizeId(UINT index) { return GraphM2ElementBase(index) + 20; }
static int GraphM2ElementRunId(UINT element, UINT run) { return GraphM2ElementBase(element) + 30 + (int)run * 10 + 1; }
static int GraphM2ElementRunPathId(UINT element, UINT run) { return GraphM2ElementBase(element) + 30 + (int)run * 10 + 2; }
static int GraphM2ElementRunFileId(UINT element, UINT run) { return GraphM2ElementBase(element) + 30 + (int)run * 10 + 3; }
static int GraphM2ElementRunArgsId(UINT element, UINT run) { return GraphM2ElementBase(element) + 30 + (int)run * 10 + 4; }
static int GraphM2ElementRunWorkModeId(UINT element, UINT run) { return GraphM2ElementBase(element) + 30 + (int)run * 10 + 5; }
static int GraphM2ElementRunWorkDirId(UINT element, UINT run) { return GraphM2ElementBase(element) + 30 + (int)run * 10 + 6; }

static HWND GraphLabelAt(const WCHAR* text, int x, int y, int width, int height) {
    return GraphPageControl(L"STATIC", text, SS_LEFT | SS_CENTERIMAGE | SS_NOPREFIX, 0, x, y, width, height, 0);
}

static HWND GraphLabelTipAt(const WCHAR* text, const WCHAR* tooltip, int x, int y, int width, int height) {
    HWND label = GraphLabelAt(text, x, y, width, height);
    AddEditorTooltip(label, tooltip);
    return label;
}

static HWND GraphGroupAt(const WCHAR* text, int x, int y, int width, int height) {
    HWND group = GraphPageControl(L"BUTTON", text, BS_GROUPBOX, 0, x, y, width, height, 0);
    ApplyBoldEditorFont(group);
    return group;
}

static HWND GraphEditAt(int id, const WCHAR* value, int x, int y, int width) {
    return GraphPageControl(L"EDIT", value, WS_TABSTOP | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, x, y, width, 24, id);
}

static HWND GraphPathEditAt(int id, const WCHAR* value, int x, int y, int width, BOOL allowDropAbsolutePath) {
    (void)allowDropAbsolutePath;
    return GraphEditAt(id, value, x, y, width);
}

static HWND GraphCheckAt(const WCHAR* text, int id, BOOL checked, int x, int y, int width) {
    HWND check = GraphPageControl(L"BUTTON", text, WS_TABSTOP | BS_AUTOCHECKBOX, 0, x, y, width, 24, id);
    GraphSetCheck(check, checked);
    return check;
}

static HWND GraphPathComboAt(int id, BOOL relative, int x, int y, int width) {
    BOOL chinese = GraphEditorUsesChinese();
    HWND combo = GraphPageControl(L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL, 0, x, y, width, 160, id);
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"相对路径", L"相對路徑", L"Relative"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"绝对路径", L"絕對路徑", L"Absolute"));
    SendMessageW(combo, CB_SETCURSEL, relative ? 0 : 1, 0);
    return combo;
}

static HWND GraphWorkingDirectoryComboAt(int id, WorkingDirectoryMode mode, int x, int y, int width) {
    BOOL chinese = GraphEditorUsesChinese();
    HWND combo = GraphPageControl(L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL, 0, x, y, width, 170, id);
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"目标目录", L"目標目錄", L"Target folder"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"启动器目录", L"啟動器目錄", L"Launcher folder"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"自定义目录", L"自定義目錄", L"Custom folder"));
    int selected = 0;
    if (mode == WorkingDirectoryLauncherDir) selected = 1;
    else if (mode == WorkingDirectoryCustom) selected = 2;
    SendMessageW(combo, CB_SETCURSEL, selected, 0);
    return combo;
}

static HWND GraphSplashPositionComboAt(int id, SplashPosition position, int x, int y, int width) {
    BOOL chinese = GraphEditorUsesChinese();
    HWND combo = GraphPageControl(L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL, 0, x, y, width, 170, id);
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"居中", L"居中", L"Center"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"左上角", L"左上角", L"Top left"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"右上角", L"右上角", L"Top right"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"左下角", L"左下角", L"Bottom left"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"右下角", L"右下角", L"Bottom right"));
    int selected = 0;
    if (position == SplashTopLeft) selected = 1;
    else if (position == SplashTopRight) selected = 2;
    else if (position == SplashBottomLeft) selected = 3;
    else if (position == SplashBottomRight) selected = 4;
    SendMessageW(combo, CB_SETCURSEL, selected, 0);
    return combo;
}

static HWND GraphSplashScaleComboAt(int id, const WCHAR* scaleText, int x, int y, int width) {
    HWND combo = GraphPageControl(L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL, 0, x, y, width, 260, id);
    UINT scale = ParseUIntInRange(scaleText, XHL_SPLASH_DEFAULT_SCALE, XHL_SPLASH_MIN_SCALE, XHL_SPLASH_MAX_SCALE);
    UINT selectedScale = ((scale + 2) / 5) * 5;
    if (selectedScale < XHL_SPLASH_MIN_SCALE) {
        selectedScale = XHL_SPLASH_MIN_SCALE;
    }
    if (selectedScale > XHL_SPLASH_MAX_SCALE) {
        selectedScale = XHL_SPLASH_MAX_SCALE;
    }

    int selected = 0;
    int index = 0;
    for (UINT value = XHL_SPLASH_MIN_SCALE; value <= XHL_SPLASH_MAX_SCALE; value += 5) {
        WCHAR item[16];
        item[0] = L'\0';
        AppendUInt(item, 16, value);
        AppendText(item, 16, L"%");
        SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)item);
        if (value == selectedScale) {
            selected = index;
        }
        ++index;
    }
    SendMessageW(combo, CB_SETCURSEL, selected, 0);
    return combo;
}

static HWND GraphHeightScaleComboAt(int id, UINT scale, int x, int y, int width) {
    BOOL chinese = GraphEditorUsesChinese();
    HWND combo = GraphPageControl(L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL, 0, x, y, width, 130, id);
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"一倍", L"一倍", L"1x"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"两倍", L"兩倍", L"2x"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"三倍", L"三倍", L"3x"));
    UINT normalized = NormalizeHeightScale(scale);
    SendMessageW(combo, CB_SETCURSEL, normalized - 1, 0);
    return combo;
}

static void GraphCustomWorkingDirectoryAt(int id, const WCHAR* value, const WCHAR* tooltip, WorkingDirectoryMode mode, int y) {
    if (mode != WorkingDirectoryCustom) {
        return;
    }
    BOOL chinese = GraphEditorUsesChinese();
    GraphLabelTipAt(EditorText(chinese, L"自定义目录", L"自定義目錄", L"Custom dir"), tooltip, 236, y, 84, 24);
    GraphEditAt(id, value, 326, y, XHL_EDITOR_FIELD_RIGHT - 326);
}

static HWND GraphTypeComboAt(int id, const WCHAR* type, int x, int y, int width) {
    BOOL chinese = GraphEditorUsesChinese();
    HWND combo = GraphPageControl(L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL, 0, x, y, width, 210, id);
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"文本", L"文本", L"Text"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"文本框（输入）", L"文本框（輸入）", L"TextBox"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"文本框（读取）", L"文本框（讀取）", L"TextFileBox"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"输入框", L"輸入框", L"Input field"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"打开文件按钮", L"打開文件按鈕", L"Open button"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"弹窗按钮", L"彈窗按鈕", L"Message button"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"注册表按钮", L"註冊表按鈕", L"Registry button"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"表格", L"表格", L"Table"));
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)EditorText(chinese, L"关闭按钮", L"關閉按鈕", L"Close button"));
    Mode2ElementKind kind = Mode2KindFromType(type);
    int selected = 1;
    if (kind == Mode2ElementText) selected = 0;
    else if (kind == Mode2ElementTextBox) selected = 1;
    else if (kind == Mode2ElementTextFileBox) selected = 2;
    else if (kind == Mode2ElementInput) selected = 3;
    else if (kind == Mode2ElementOpenButton) selected = 4;
    else if (kind == Mode2ElementMessageButton) selected = 5;
    else if (kind == Mode2ElementRegButton) selected = 6;
    else if (kind == Mode2ElementTable) selected = 7;
    else if (kind == Mode2ElementCloseButton) selected = 8;
    SendMessageW(combo, CB_SETCURSEL, selected, 0);
    return combo;
}

static HWND GraphSelectorAt(int id, UINT selected, UINT count, const WCHAR* prefix, int x, int y, int width) {
    HWND combo = GraphPageControl(L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL, 0, x, y, width, 220, id);
    for (UINT i = 0; i < count; ++i) {
        WCHAR item[64];
        CopyText(item, 64, prefix);
        AppendUInt(item, 64, i + 1);
        SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)item);
    }
    SendMessageW(combo, CB_SETCURSEL, selected, 0);
    return combo;
}

static HWND GraphMode2VariableComboAt(int id, UINT selected, int x, int y, int width) {
    HWND combo = GraphPageControl(L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL, 0, x, y, width, 180, id);
    for (UINT i = 0; i < XHL_MODE2_VARIABLE_COUNT; ++i) {
        WCHAR item[32];
        CopyText(item, 32, L"%XHLM2V");
        AppendUInt(item, 32, i + 1);
        AppendText(item, 32, L"%");
        SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)item);
    }
    if (selected >= XHL_MODE2_VARIABLE_COUNT) {
        selected = 0;
    }
    SendMessageW(combo, CB_SETCURSEL, selected, 0);
    return combo;
}

static void GraphBuildPage();
static void GraphLayoutWindow(HWND window);

static void GraphRebuildPanel(HWND window) {
    if (!g_editorPanel) {
        GraphBuildPage();
        return;
    }
    if (!window) {
        window = GetParent(g_editorPanel);
    }
    DestroyWindow(g_editorPanel);
    g_editorPanel = 0;
    UINT dpi = GetDpi(window);
    g_editorPanel = CreateEditorControl(window, L"XHLauncherGraphEditorPanel", L"", WS_CLIPCHILDREN, 0, 20, 84, XHL_EDITOR_PAGE_WIDTH, 470, XHL_EDITOR_ID_PANEL, dpi);
    GraphBuildPage();
    GraphLayoutWindow(window);
    RedrawWindow(window, 0, 0, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
}

static void GraphLoadEditorState() {
    ZeroMemory(&g_editorConfig, sizeof(g_editorConfig));
    ReadIniText(L"Launcher", L"Title", L"Xiao Hammer Launcher", g_editorConfig.title, XHL_MAX_TEXT);
    TruncateText(g_editorConfig.title, XHL_TITLE_MAX_CHARS);
    ReadIniText(L"Launcher", L"Version", L"", g_editorConfig.version, XHL_MAX_TEXT);
    TruncateText(g_editorConfig.version, XHL_CONFIG_VERSION_MAX_CHARS);
    g_editorConfig.splashEnable = ReadIniFlag(L"Launcher", L"SplashEnable", FALSE);
    g_editorConfig.splashUseRelativePath = ReadIniFlag(L"Launcher", L"SplashUseRelativePath", TRUE);
    ReadIniText(L"Launcher", L"SplashImage", L"", g_editorConfig.splashImage, XHL_MAX_TEXT);
    ReadIniText(L"Launcher", L"SplashDurationMs", L"1000", g_editorConfig.splashDurationMs, 32);
    ReadIniText(L"Launcher", L"SplashScale", L"100", g_editorConfig.splashScale, 32);
    WCHAR splashPosition[32];
    ReadIniText(L"Launcher", L"SplashPosition", L"Center", splashPosition, 32);
    g_editorConfig.splashPosition = SplashPositionFromText(splashPosition);
    g_editorConfig.language = ReadConfiguredLanguage();
    g_editorConfig.chinese = LanguageIsChinese(g_editorConfig.language);
    g_editorConfig.mode = ReadMode();
    for (UINT i = 0; i < XHL_MODE1_VARIABLE_COUNT; ++i) {
        WCHAR section[32];
        Mode1VariableSection(i + 1, section, 32);
        g_editorConfig.mode1Variables[i].enable = ReadIniFlag(section, L"Enable", FALSE);
        ReadIniText(section, L"Value", L"", g_editorConfig.mode1Variables[i].value, XHL_MAX_TEXT);
    }
    g_editorConfig.mode1Registry.addReg = ReadIniFlag(Mode1RegistrySection(), L"AddReg", FALSE);
    g_editorConfig.mode1Registry.useRelativePath = ReadIniFlag(Mode1RegistrySection(), L"UseRelativePath", TRUE);
    g_editorConfig.mode1Registry.regImported = ReadIniFlag(Mode1RegistrySection(), L"RegImported", FALSE);
    WCHAR workingModeText[32];
    ReadIniText(Mode1RegistrySection(), L"WorkingDirectoryMode", L"TargetDir", workingModeText, 32);
    g_editorConfig.mode1Registry.workingDirectoryMode = WorkingDirectoryModeFromText(workingModeText);
    ReadIniText(Mode1RegistrySection(), L"RegFileName", L"", g_editorConfig.mode1Registry.regFileName, XHL_MAX_TEXT);
    ReadIniText(Mode1RegistrySection(), L"WorkingDirectory", L"", g_editorConfig.mode1Registry.workingDirectory, XHL_MAX_TEXT);
    for (UINT i = 0; i < XHL_MODE1_RUN_COUNT; ++i) {
        WCHAR section[32];
        Mode1RunItemSection(i + 1, section, 32);
        g_editorConfig.mode1Runs[i].run = ReadIniFlag(section, L"Run", FALSE);
        g_editorConfig.mode1Runs[i].useRelativePath = ReadIniFlag(section, L"UseRelativePath", TRUE);
        ReadIniText(section, L"WorkingDirectoryMode", L"TargetDir", workingModeText, 32);
        g_editorConfig.mode1Runs[i].workingDirectoryMode = WorkingDirectoryModeFromText(workingModeText);
        ReadIniText(section, L"FileName", L"", g_editorConfig.mode1Runs[i].fileName, XHL_MAX_TEXT);
        ReadIniText(section, L"Arguments", L"", g_editorConfig.mode1Runs[i].arguments, XHL_MAX_TEXT);
        ReadIniText(section, L"WorkingDirectory", L"", g_editorConfig.mode1Runs[i].workingDirectory, XHL_MAX_TEXT);
    }
    for (UINT i = 0; i < XHL_MODE2_ELEMENT_COUNT; ++i) {
        WCHAR section[32];
        Mode2ElementSection(i + 1, section, 32);
        EditorMode2ElementConfig* element = &g_editorConfig.mode2Elements[i];
        element->show = ReadIniFlag(section, L"Show", FALSE);
        ReadIniText(section, L"Type", L"TextBox", element->type, 32);
        element->minimizeOnClick = ReadIniFlag(section, L"MinimizeOnClick", FALSE);
        WCHAR heightScaleText[32];
        ReadIniText(section, L"HeightScale", L"1", heightScaleText, 32);
        element->heightScale = NormalizeHeightScale(ParseUIntInRange(heightScaleText, 1, 1, 3));
        ReadIniText(section, L"Text", L"", element->text, XHL_MAX_TEXT);
        ReadIniText(section, L"Message", L"", element->message, XHL_MAX_TEXT);
        ReadIniText(section, L"RegFileName", L"", element->regFileName, XHL_MAX_TEXT);
        element->regUseRelativePath = ReadIniFlag(section, L"UseRelativePath", TRUE);
        ReadIniText(section, L"WorkingDirectoryMode", L"TargetDir", workingModeText, 32);
        element->regWorkingDirectoryMode = WorkingDirectoryModeFromText(workingModeText);
        ReadIniText(section, L"WorkingDirectory", L"", element->regWorkingDirectory, XHL_MAX_TEXT);
        ReadIniText(section, L"RegImportingText", L"", element->regImportingText, XHL_MAX_TEXT);
        ReadIniText(section, L"RegSuccessMessage", L"", element->regSuccessMessage, XHL_MAX_TEXT);
        ReadIniText(section, L"RegFailureMessage", L"", element->regFailureMessage, XHL_MAX_TEXT);
        ReadIniText(section, L"RegTimeoutMessage", L"", element->regTimeoutMessage, XHL_MAX_TEXT);
        element->csvUseRelativePath = ReadIniFlag(section, L"CsvUseRelativePath", TRUE);
        ReadIniText(section, L"CsvFileName", L"", element->csvFileName, XHL_MAX_TEXT);
        element->textFileUseRelativePath = ReadIniFlag(section, L"TextFileUseRelativePath", TRUE);
        ReadIniText(section, L"TextFileName", L"", element->textFileName, XHL_MAX_TEXT);
        WCHAR variableText[32];
        ReadIniText(section, L"Variable", L"1", variableText, 32);
        element->inputVariableIndex = ParseUIntInRange(variableText, 1, 1, XHL_MODE2_VARIABLE_COUNT) - 1;
        for (UINT run = 0; run < XHL_MODE2_BUTTON_RUN_COUNT; ++run) {
            WCHAR key[32];
            CopyText(key, 32, L"Run"); AppendUInt(key, 32, run + 1);
            element->runs[run].run = ReadIniFlag(section, key, FALSE);
            CopyText(key, 32, L"UseRelativePath"); AppendUInt(key, 32, run + 1);
            element->runs[run].useRelativePath = ReadIniFlag(section, key, TRUE);
            CopyText(key, 32, L"WorkingDirectoryMode"); AppendUInt(key, 32, run + 1);
            ReadIniText(section, key, L"TargetDir", workingModeText, 32);
            element->runs[run].workingDirectoryMode = WorkingDirectoryModeFromText(workingModeText);
            CopyText(key, 32, L"FileName"); AppendUInt(key, 32, run + 1);
            ReadIniText(section, key, L"", element->runs[run].fileName, XHL_MAX_TEXT);
            CopyText(key, 32, L"Arguments"); AppendUInt(key, 32, run + 1);
            ReadIniText(section, key, L"", element->runs[run].arguments, XHL_MAX_TEXT);
            CopyText(key, 32, L"WorkingDirectory"); AppendUInt(key, 32, run + 1);
            ReadIniText(section, key, L"", element->runs[run].workingDirectory, XHL_MAX_TEXT);
        }
    }
}

static void GraphReadPageToState() {
    if (!g_editorPanel) return;
    if (GetDlgItem(g_editorPanel, XHL_EDITOR_ID_M1_RUN_SELECTOR)) {
        for (UINT i = 0; i < XHL_MODE1_VARIABLE_COUNT; ++i) {
            g_editorConfig.mode1Variables[i].enable = GraphGetCheck(g_editorPanel, GraphM1VarEnableId(i), g_editorConfig.mode1Variables[i].enable);
            GraphGetText(g_editorPanel, GraphM1VarValueId(i), g_editorConfig.mode1Variables[i].value, XHL_MAX_TEXT);
        }
        g_editorConfig.mode1Registry.addReg = GraphGetCheck(g_editorPanel, GraphM1RegAddId(), g_editorConfig.mode1Registry.addReg);
        g_editorConfig.mode1Registry.useRelativePath = GraphReadPathCombo(g_editorPanel, GraphM1RegPathId(), g_editorConfig.mode1Registry.useRelativePath);
        g_editorConfig.mode1Registry.workingDirectoryMode = GraphReadWorkingDirectoryCombo(g_editorPanel, GraphM1RegWorkModeId(), g_editorConfig.mode1Registry.workingDirectoryMode);
        g_editorConfig.mode1Registry.regImported = GraphGetCheck(g_editorPanel, GraphM1RegImportedId(), g_editorConfig.mode1Registry.regImported);
        GraphGetText(g_editorPanel, GraphM1RegFileId(), g_editorConfig.mode1Registry.regFileName, XHL_MAX_TEXT);
        GraphGetText(g_editorPanel, GraphM1RegWorkDirId(), g_editorConfig.mode1Registry.workingDirectory, XHL_MAX_TEXT);
        for (UINT i = 0; i < XHL_MODE1_RUN_COUNT; ++i) {
            if (!GetDlgItem(g_editorPanel, GraphM1RunRunId(i))) {
                continue;
            }
            g_editorMode1RunIndex = i;
            g_editorConfig.mode1Runs[i].run = GraphGetCheck(g_editorPanel, GraphM1RunRunId(i), g_editorConfig.mode1Runs[i].run);
            g_editorConfig.mode1Runs[i].useRelativePath = GraphReadPathCombo(g_editorPanel, GraphM1RunPathId(i), g_editorConfig.mode1Runs[i].useRelativePath);
            g_editorConfig.mode1Runs[i].workingDirectoryMode = GraphReadWorkingDirectoryCombo(g_editorPanel, GraphM1RunWorkModeId(i), g_editorConfig.mode1Runs[i].workingDirectoryMode);
            GraphGetText(g_editorPanel, GraphM1RunFileId(i), g_editorConfig.mode1Runs[i].fileName, XHL_MAX_TEXT);
            GraphGetText(g_editorPanel, GraphM1RunArgsId(i), g_editorConfig.mode1Runs[i].arguments, XHL_MAX_TEXT);
            GraphGetText(g_editorPanel, GraphM1RunWorkDirId(i), g_editorConfig.mode1Runs[i].workingDirectory, XHL_MAX_TEXT);
            break;
        }
    } else if (GetDlgItem(g_editorPanel, XHL_EDITOR_ID_M2_ELEMENT_SELECTOR)) {
        for (UINT i = 0; i < XHL_MODE2_ELEMENT_COUNT; ++i) {
            if (!GetDlgItem(g_editorPanel, GraphM2ElementShowId(i))) {
                continue;
            }
            g_editorMode2ElementIndex = i;
            EditorMode2ElementConfig* element = &g_editorConfig.mode2Elements[i];
            element->show = GraphGetCheck(g_editorPanel, GraphM2ElementShowId(i), element->show);
            GraphReadTypeCombo(g_editorPanel, GraphM2ElementTypeId(i), element->type, 32);
            element->minimizeOnClick = GraphGetCheck(g_editorPanel, GraphM2ElementMinimizeId(i), element->minimizeOnClick);
            element->heightScale = GraphReadHeightScaleCombo(g_editorPanel, GraphM2ElementHeightScaleId(i), element->heightScale);
            GraphGetText(g_editorPanel, GraphM2ElementTextId(i), element->text, XHL_MAX_TEXT);
            GraphGetText(g_editorPanel, GraphM2ElementMessageId(i), element->message, XHL_MAX_TEXT);
            GraphGetText(g_editorPanel, GraphM2ElementRegFileId(i), element->regFileName, XHL_MAX_TEXT);
            element->regUseRelativePath = GraphReadPathCombo(g_editorPanel, GraphM2ElementRegPathId(i), element->regUseRelativePath);
            element->regWorkingDirectoryMode = GraphReadWorkingDirectoryCombo(g_editorPanel, GraphM2ElementRegWorkModeId(i), element->regWorkingDirectoryMode);
            GraphGetText(g_editorPanel, GraphM2ElementRegWorkDirId(i), element->regWorkingDirectory, XHL_MAX_TEXT);
            element->inputVariableIndex = GraphReadComboIndex(g_editorPanel, GraphM2ElementInputVariableId(i), element->inputVariableIndex, XHL_MODE2_VARIABLE_COUNT);
            GraphGetText(g_editorPanel, GraphM2ElementRegImportingId(i), element->regImportingText, XHL_MAX_TEXT);
            GraphGetText(g_editorPanel, GraphM2ElementRegSuccessId(i), element->regSuccessMessage, XHL_MAX_TEXT);
            GraphGetText(g_editorPanel, GraphM2ElementRegFailureId(i), element->regFailureMessage, XHL_MAX_TEXT);
            GraphGetText(g_editorPanel, GraphM2ElementRegTimeoutId(i), element->regTimeoutMessage, XHL_MAX_TEXT);
            element->csvUseRelativePath = GraphReadPathCombo(g_editorPanel, GraphM2ElementCsvPathId(i), element->csvUseRelativePath);
            GraphGetText(g_editorPanel, GraphM2ElementCsvFileId(i), element->csvFileName, XHL_MAX_TEXT);
            element->textFileUseRelativePath = GraphReadPathCombo(g_editorPanel, GraphM2ElementTextFilePathId(i), element->textFileUseRelativePath);
            GraphGetText(g_editorPanel, GraphM2ElementTextFileId(i), element->textFileName, XHL_MAX_TEXT);
            for (UINT run = 0; run < XHL_MODE2_BUTTON_RUN_COUNT; ++run) {
                if (!GetDlgItem(g_editorPanel, GraphM2ElementRunId(i, run))) {
                    continue;
                }
                g_editorMode2RunIndex = run;
                element->runs[run].run = GraphGetCheck(g_editorPanel, GraphM2ElementRunId(i, run), element->runs[run].run);
                element->runs[run].useRelativePath = GraphReadPathCombo(g_editorPanel, GraphM2ElementRunPathId(i, run), element->runs[run].useRelativePath);
                element->runs[run].workingDirectoryMode = GraphReadWorkingDirectoryCombo(g_editorPanel, GraphM2ElementRunWorkModeId(i, run), element->runs[run].workingDirectoryMode);
                GraphGetText(g_editorPanel, GraphM2ElementRunFileId(i, run), element->runs[run].fileName, XHL_MAX_TEXT);
                GraphGetText(g_editorPanel, GraphM2ElementRunArgsId(i, run), element->runs[run].arguments, XHL_MAX_TEXT);
                GraphGetText(g_editorPanel, GraphM2ElementRunWorkDirId(i, run), element->runs[run].workingDirectory, XHL_MAX_TEXT);
                break;
            }
            break;
        }
    } else if (GetDlgItem(g_editorPanel, XHL_EDITOR_ID_MODE0_SPLASH_IMAGE)) {
        g_editorConfig.splashEnable = GraphGetCheck(g_editorPanel, XHL_EDITOR_ID_MODE0_SPLASH_ENABLE, g_editorConfig.splashEnable);
        g_editorConfig.splashUseRelativePath = GraphReadPathCombo(g_editorPanel, XHL_EDITOR_ID_MODE0_SPLASH_PATH, g_editorConfig.splashUseRelativePath);
        GraphGetText(g_editorPanel, XHL_EDITOR_ID_MODE0_SPLASH_IMAGE, g_editorConfig.splashImage, XHL_MAX_TEXT);
        GraphGetText(g_editorPanel, XHL_EDITOR_ID_MODE0_SPLASH_DURATION, g_editorConfig.splashDurationMs, 32);
        UINT splashScale = GraphReadSplashScaleCombo(
            g_editorPanel,
            XHL_EDITOR_ID_MODE0_SPLASH_SCALE,
            ParseUIntInRange(g_editorConfig.splashScale, XHL_SPLASH_DEFAULT_SCALE, XHL_SPLASH_MIN_SCALE, XHL_SPLASH_MAX_SCALE));
        g_editorConfig.splashScale[0] = L'\0';
        AppendUInt(g_editorConfig.splashScale, 32, splashScale);
        g_editorConfig.splashPosition = GraphReadSplashPositionCombo(g_editorPanel, XHL_EDITOR_ID_MODE0_SPLASH_POSITION, g_editorConfig.splashPosition);
    }
}

static void GraphWriteBool(const WCHAR* section, const WCHAR* key, BOOL value, BOOL* ok) {
    if (!*ok) {
        return;
    }
    if (g_graphSaveText) {
        *ok = UpdateIniTextValue(*g_graphSaveText, section, key, value ? L"1" : L"0");
    } else {
        *ok = WriteIniTextUtf8(section, key, value ? L"1" : L"0");
    }
}

static void GraphWriteText(const WCHAR* section, const WCHAR* key, const WCHAR* value, BOOL* ok) {
    if (!*ok) {
        return;
    }
    if (g_graphSaveText) {
        *ok = UpdateIniTextValue(*g_graphSaveText, section, key, value ? value : L"");
    } else {
        *ok = WriteIniTextUtf8(section, key, value ? value : L"");
    }
}

static BOOL GraphSaveStateToIni() {
    if (!LoadIniText()) {
        return FALSE;
    }

    std::wstring updated = g_iniText;
    g_graphSaveText = &updated;

    BOOL ok = TRUE;
    WCHAR modeText[4];
    CopyText(modeText, 4, g_editorConfig.mode == ModeSilent ? L"1" : (g_editorConfig.mode == ModeWindow ? L"2" : L"0"));
    GraphWriteText(L"Launcher", L"Title", g_editorConfig.title, &ok);
    GraphWriteText(L"Launcher", L"Version", g_editorConfig.version, &ok);
    GraphWriteText(L"Launcher", L"Language", AppLanguageToConfigText(g_editorConfig.language), &ok);
    GraphWriteText(L"Launcher", L"Mode", modeText, &ok);
    GraphWriteBool(L"Launcher", L"SplashEnable", g_editorConfig.splashEnable, &ok);
    GraphWriteBool(L"Launcher", L"SplashUseRelativePath", g_editorConfig.splashUseRelativePath, &ok);
    GraphWriteText(L"Launcher", L"SplashImage", g_editorConfig.splashImage, &ok);
    GraphWriteText(L"Launcher", L"SplashDurationMs", g_editorConfig.splashDurationMs, &ok);
    GraphWriteText(L"Launcher", L"SplashScale", g_editorConfig.splashScale, &ok);
    GraphWriteText(L"Launcher", L"SplashPosition", SplashPositionToText(g_editorConfig.splashPosition), &ok);
    for (UINT i = 0; i < XHL_MODE1_VARIABLE_COUNT; ++i) {
        WCHAR section[32]; Mode1VariableSection(i + 1, section, 32);
        GraphWriteBool(section, L"Enable", g_editorConfig.mode1Variables[i].enable, &ok);
        GraphWriteText(section, L"Value", g_editorConfig.mode1Variables[i].value, &ok);
    }
    GraphWriteBool(Mode1RegistrySection(), L"AddReg", g_editorConfig.mode1Registry.addReg, &ok);
    GraphWriteBool(Mode1RegistrySection(), L"UseRelativePath", g_editorConfig.mode1Registry.useRelativePath, &ok);
    GraphWriteText(Mode1RegistrySection(), L"RegFileName", g_editorConfig.mode1Registry.regFileName, &ok);
    GraphWriteText(Mode1RegistrySection(), L"WorkingDirectoryMode", WorkingDirectoryModeToText(g_editorConfig.mode1Registry.workingDirectoryMode), &ok);
    GraphWriteText(Mode1RegistrySection(), L"WorkingDirectory", g_editorConfig.mode1Registry.workingDirectory, &ok);
    GraphWriteBool(Mode1RegistrySection(), L"RegImported", g_editorConfig.mode1Registry.regImported, &ok);
    for (UINT i = 0; i < XHL_MODE1_RUN_COUNT; ++i) {
        WCHAR section[32]; Mode1RunItemSection(i + 1, section, 32);
        GraphWriteBool(section, L"Run", g_editorConfig.mode1Runs[i].run, &ok);
        GraphWriteBool(section, L"UseRelativePath", g_editorConfig.mode1Runs[i].useRelativePath, &ok);
        GraphWriteText(section, L"FileName", g_editorConfig.mode1Runs[i].fileName, &ok);
        GraphWriteText(section, L"Arguments", g_editorConfig.mode1Runs[i].arguments, &ok);
        GraphWriteText(section, L"WorkingDirectoryMode", WorkingDirectoryModeToText(g_editorConfig.mode1Runs[i].workingDirectoryMode), &ok);
        GraphWriteText(section, L"WorkingDirectory", g_editorConfig.mode1Runs[i].workingDirectory, &ok);
    }
    for (UINT i = 0; i < XHL_MODE2_ELEMENT_COUNT; ++i) {
        WCHAR section[32]; Mode2ElementSection(i + 1, section, 32);
        EditorMode2ElementConfig* element = &g_editorConfig.mode2Elements[i];
        GraphWriteBool(section, L"Show", element->show, &ok);
        GraphWriteText(section, L"Type", element->type, &ok);
        GraphWriteBool(section, L"MinimizeOnClick", element->minimizeOnClick, &ok);
        GraphWriteText(section, L"HeightScale", HeightScaleToText(element->heightScale), &ok);
        WCHAR temp[XHL_MAX_TEXT];
        CopyText(temp, XHL_MAX_TEXT, element->text);
        if (Mode2KindFromType(element->type) == Mode2ElementTextBox) GraphCollapseLineBreaks(temp, XHL_MAX_TEXT);
        GraphWriteText(section, L"Text", temp, &ok);
        CopyText(temp, XHL_MAX_TEXT, element->message);
        GraphCollapseLineBreaks(temp, XHL_MAX_TEXT);
        GraphWriteText(section, L"Message", temp, &ok);
        GraphWriteText(section, L"RegFileName", element->regFileName, &ok);
        GraphWriteBool(section, L"UseRelativePath", element->regUseRelativePath, &ok);
        GraphWriteText(section, L"WorkingDirectoryMode", WorkingDirectoryModeToText(element->regWorkingDirectoryMode), &ok);
        GraphWriteText(section, L"WorkingDirectory", element->regWorkingDirectory, &ok);
        GraphWriteText(section, L"RegImportingText", element->regImportingText, &ok);
        CopyText(temp, XHL_MAX_TEXT, element->regSuccessMessage);
        GraphCollapseLineBreaks(temp, XHL_MAX_TEXT);
        GraphWriteText(section, L"RegSuccessMessage", temp, &ok);
        CopyText(temp, XHL_MAX_TEXT, element->regFailureMessage);
        GraphCollapseLineBreaks(temp, XHL_MAX_TEXT);
        GraphWriteText(section, L"RegFailureMessage", temp, &ok);
        CopyText(temp, XHL_MAX_TEXT, element->regTimeoutMessage);
        GraphCollapseLineBreaks(temp, XHL_MAX_TEXT);
        GraphWriteText(section, L"RegTimeoutMessage", temp, &ok);
        GraphWriteText(section, L"CsvFileName", element->csvFileName, &ok);
        GraphWriteBool(section, L"CsvUseRelativePath", element->csvUseRelativePath, &ok);
        GraphWriteText(section, L"TextFileName", element->textFileName, &ok);
        GraphWriteBool(section, L"TextFileUseRelativePath", element->textFileUseRelativePath, &ok);
        temp[0] = L'\0';
        AppendUInt(temp, XHL_MAX_TEXT, element->inputVariableIndex + 1);
        GraphWriteText(section, L"Variable", temp, &ok);
        for (UINT run = 0; run < XHL_MODE2_BUTTON_RUN_COUNT; ++run) {
            WCHAR key[32];
            CopyText(key, 32, L"Run"); AppendUInt(key, 32, run + 1);
            GraphWriteBool(section, key, element->runs[run].run, &ok);
            CopyText(key, 32, L"UseRelativePath"); AppendUInt(key, 32, run + 1);
            GraphWriteBool(section, key, element->runs[run].useRelativePath, &ok);
            CopyText(key, 32, L"FileName"); AppendUInt(key, 32, run + 1);
            GraphWriteText(section, key, element->runs[run].fileName, &ok);
            CopyText(key, 32, L"Arguments"); AppendUInt(key, 32, run + 1);
            GraphWriteText(section, key, element->runs[run].arguments, &ok);
            CopyText(key, 32, L"WorkingDirectoryMode"); AppendUInt(key, 32, run + 1);
            GraphWriteText(section, key, WorkingDirectoryModeToText(element->runs[run].workingDirectoryMode), &ok);
            CopyText(key, 32, L"WorkingDirectory"); AppendUInt(key, 32, run + 1);
            GraphWriteText(section, key, element->runs[run].workingDirectory, &ok);
        }
    }

    g_graphSaveText = 0;
    if (!ok) {
        return FALSE;
    }
    return WriteUtf8TextFileAtomic(g_ctx.configPath.c_str(), updated);
}

static void GraphDestroyPageControls() {
    ClearEditorTooltipsFrom(g_editorPanel);
    HWND child = g_editorPanel ? GetWindow(g_editorPanel, GW_CHILD) : 0;
    while (child) {
        HWND next = GetWindow(child, GW_HWNDNEXT);
        DestroyWindow(child);
        child = next;
    }
}

static void GraphAddMode0Page() {
    BOOL chinese = GraphEditorUsesChinese();
    const WCHAR* splashEnableTip = EditorText(chinese,
        L"启动图开关。勾选后才会尝试显示启动图；如果路径为空或图片无效，仍会静默跳过。", L"啟動圖開關。勾選後才會嘗試顯示啟動圖；如果路徑為空或圖片無效，仍會靜默跳過。",
        L"Splash switch. When enabled, the program tries to show the splash image; if the path is empty or invalid, it is still skipped silently.");
    const WCHAR* splashImageTip = EditorText(chinese,
        L"启动图路径。留空表示不显示。只支持PNG，支持透明通道。", L"啟動圖路徑。留空表示不顯示。只支持PNG，支持透明通道。",
        L"Splash image path. Empty means disabled. PNG only. Alpha transparency is supported.");
    const WCHAR* splashPathModeTip = EditorText(chinese,
        L"启动图路径类型。相对路径以exe所在目录为基准；绝对路径可使用Windows环境变量和%XHLROOT%。", L"啟動圖路徑類型。相對路徑以exe所在目錄為基準；絕對路徑可使用Windows環境變量和%XHLROOT%。",
        L"Splash path mode. Relative paths use the exe folder. Absolute paths may use Windows environment variables and %XHLROOT%.");
    const WCHAR* splashDurationTip = EditorText(chinese,
        L"启动图显示时间，单位毫秒。建议填写1000。程序会在时间到后自动关闭启动图。", L"啟動圖顯示時間，單位毫秒。建議填寫1000。程序會在時間到後自動關閉啟動圖。",
        L"Splash display time in milliseconds. 1000 is recommended. The splash closes automatically when time is up.");
    const WCHAR* splashScaleTip = EditorText(chinese,
        L"启动图缩放比例。编辑器中从10%到200%按5%步进选择；配置文件仍保存不带百分号的数字。最终渲染尺寸会限制在3840x2160以内。", L"啟動圖縮放比例。編輯器中從10%到200%按5%步進選擇；配置文件仍保存不帶百分號的數字。最終渲染尺寸會限制在3840x2160以內。",
        L"Splash scale. In the editor, choose from 10% to 200% in 5% steps. The INI still stores the number without %. Final rendered size is limited to 3840x2160.");
    const WCHAR* splashPositionTip = EditorText(chinese,
        L"启动图显示位置。可选居中、左上角、右上角、左下角或右下角。", L"啟動圖顯示位置。可選居中、左上角、右上角、左下角或右下角。",
        L"Splash position. Choose center, top left, top right, bottom left, or bottom right.");

    int y = 28;
    GraphGroupAt(EditorText(chinese, L"启动图", L"啟動圖", L"Splash image"), 0, 0, XHL_EDITOR_PAGE_WIDTH, 130);
    GraphLabelTipAt(EditorText(chinese, L"启动图路径", L"啟動圖路徑", L"Splash path"), splashImageTip, 18, y, 76, 24);
    GraphPathEditAt(XHL_EDITOR_ID_MODE0_SPLASH_IMAGE, g_editorConfig.splashImage, 96, y, 456, !g_editorConfig.splashUseRelativePath);
    HWND splashEnable = GraphCheckAt(EditorText(chinese, L"启用", L"啟用", L"Enable"), XHL_EDITOR_ID_MODE0_SPLASH_ENABLE, g_editorConfig.splashEnable, 570, y, 70);
    AddEditorTooltip(splashEnable, splashEnableTip);
    y += 34;
    GraphLabelTipAt(EditorText(chinese, L"路径类型", L"路徑類型", L"Path mode"), splashPathModeTip, 18, y, 76, 24);
    GraphPathComboAt(XHL_EDITOR_ID_MODE0_SPLASH_PATH, g_editorConfig.splashUseRelativePath, 96, y, XHL_EDITOR_COMBO_WIDTH);
    GraphLabelTipAt(EditorText(chinese, L"显示时间", L"顯示時間", L"Duration"), splashDurationTip, 236, y, 76, 24);
    GraphEditAt(XHL_EDITOR_ID_MODE0_SPLASH_DURATION, g_editorConfig.splashDurationMs, 314, y, 82);
    GraphLabelTipAt(EditorText(chinese, L"缩放比例", L"縮放比例", L"Scale"), splashScaleTip, 426, y, 76, 24);
    GraphSplashScaleComboAt(XHL_EDITOR_ID_MODE0_SPLASH_SCALE, g_editorConfig.splashScale, 508, y, 82);
    y += 34;
    GraphLabelTipAt(EditorText(chinese, L"显示位置", L"顯示位置", L"Position"), splashPositionTip, 18, y, 76, 24);
    GraphSplashPositionComboAt(XHL_EDITOR_ID_MODE0_SPLASH_POSITION, g_editorConfig.splashPosition, 96, y, 156);

    const int notesTop = 144;
    const int notesHeight = 300;
    GraphGroupAt(EditorText(chinese, L"说明", L"說明", L"Notes"), 0, notesTop, XHL_EDITOR_PAGE_WIDTH, notesHeight);
    y = notesTop + 28;
    GraphPageControl(L"STATIC", EditorText(chinese,
        L"编辑模式仅用于修改当前配置文件，不会打开文件或导入注册表。切换上方模式后，下方会显示对应配置项。", L"編輯模式僅用於修改當前配置文件，不會打開文件或導入註冊表。切換上方模式後，下方會顯示對應配置項。",
        L"Editor mode is only for editing the current configuration file. It does not open files or import registry files. Choose a mode above to show its settings."),
        SS_LEFT | SS_NOPREFIX, 0, 18, y, XHL_EDITOR_PAGE_WIDTH - 36, 42, XHL_EDITOR_ID_MODE0_INFO);

    AppLanguage language = GraphEditorLanguage();
    static BOOL readmeEnLoaded = FALSE;
    static BOOL readmeZhHansLoaded = FALSE;
    static BOOL readmeZhHantLoaded = FALSE;
    static std::wstring readmeEnText;
    static std::wstring readmeZhHansText;
    static std::wstring readmeZhHantText;
    std::wstring* readmeText = &readmeEnText;
    BOOL* readmeLoaded = &readmeEnLoaded;
    UINT readmeResourceId = XHL_README_EN_RESOURCE_ID;
    if (language == AppLanguageSimplifiedChinese) {
        readmeText = &readmeZhHansText;
        readmeLoaded = &readmeZhHansLoaded;
        readmeResourceId = XHL_README_ZH_HANS_RESOURCE_ID;
    } else if (language == AppLanguageTraditionalChinese) {
        readmeText = &readmeZhHantText;
        readmeLoaded = &readmeZhHantLoaded;
        readmeResourceId = XHL_README_ZH_HANT_RESOURCE_ID;
    }
    if (!*readmeLoaded) {
        if (!LoadTextResourceString(readmeResourceId, *readmeText) || readmeText->empty()) {
            *readmeText = EditorText(chinese,
                L"无法加载内置说明文档。", L"無法加載內置說明文檔。",
                L"Failed to load the embedded user guide.");
        }
        *readmeLoaded = TRUE;
    }

    y += 54;
    HWND readme = GraphPageControl(
        L"EDIT",
        L"",
        WS_TABSTOP | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        WS_EX_CLIENTEDGE,
        18,
        y,
        XHL_EDITOR_PAGE_WIDTH - 36,
        notesTop + notesHeight - y - 20,
        XHL_EDITOR_ID_MODE0_README);
    if (readme) {
        SendMessageW(readme, EM_SETLIMITTEXT, (WPARAM)(readmeText->length() + 1), 0);
        SendMessageW(readme, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(6, 6));
        SetWindowTextW(readme, readmeText->c_str());
    }
}

static void GraphAddMode1Page() {
    BOOL chinese = GraphEditorUsesChinese();
    const WCHAR* variableTip = EditorText(chinese,
        L"变量开关和替换内容。启用后，程序运行时会将同名变量替换为右侧内容，不会改写配置文件原文。替换后的运行时文本最多32767个UTF-16字符，超过会视为无效。",
        L"變量開關和替換內容。啟用後，程序運行時會將同名變量替換為右側內容，不會改寫配置文件原文。替換後的運行時文本最多32767個UTF-16字符，超過會視為無效。",
        L"Variable switch and replacement value. When enabled, the matching token is replaced at run time without rewriting the INI file. Runtime text after replacement is limited to 32767 UTF-16 characters; longer values are treated as invalid.");
    const WCHAR* regPathModeTip = EditorText(chinese,
        L"注册表路径类型。相对路径以exe所在目录为基准，只替换模式1变量；若替换后变成绝对路径也会跳过。绝对路径可使用Windows环境变量和%XHLROOT%。", L"註冊表路徑類型。相對路徑以exe所在目錄為基準，只替換模式1變量；若替換後變成絕對路徑也會跳過。絕對路徑可使用Windows環境變量和%XHLROOT%。",
        L"Registry path mode. Relative paths are based on the executable folder and only Mode1 variables are replaced; if the result becomes absolute, it is skipped. Absolute paths may use Windows environment variables and %XHLROOT%.");
    const WCHAR* regEnableTip = EditorText(chinese,
        L"勾选后，静默模式会尝试导入下方填写的 .reg 注册表文件。", L"勾選後，靜默模式會嘗試導入下方填寫的 .reg 註冊表文件。",
        L"When enabled, silent mode attempts to import the .reg file below.");
    const WCHAR* regImportedTip = EditorText(chinese,
        L"标记当前注册表文件已导入。勾选后静默模式会跳过注册表导入；成功导入后程序会自动勾选。", L"標記當前註冊表文件已導入。勾選後靜默模式會跳過註冊表導入；成功導入後程序會自動勾選。",
        L"Marks the registry file as already imported. When checked, silent mode skips registry import; the program checks it automatically after a successful import.");
    const WCHAR* regFileTip = EditorText(chinese,
        L"注册表路径。相对路径只替换模式1变量，不能使用%XHLROOT%或系统环境变量；替换后若是绝对路径会跳过。最终路径必须以.reg结尾。", L"註冊表路徑。相對路徑只替換模式1變量，不能使用%XHLROOT%或系統環境變量；替換後若是絕對路徑會跳過。最終路徑必須以.reg結尾。",
        L"Registry path. Relative paths only replace Mode1 variables and cannot use %XHLROOT% or system environment variables; if the result is absolute, it is skipped. The final path must end in .reg.");
    const WCHAR* runSelectorTip = EditorText(chinese,
        L"运行项选择。静默模式最多支持5个运行项，选择后可编辑该项的开关、路径和启动参数。", L"運行項選擇。靜默模式最多支持5個運行項，選擇後可編輯該項的開關、路徑和啟動參數。",
        L"Run item selector. Silent mode supports up to 5 run items; choose one to edit its switch, path, and arguments.");
    const WCHAR* runEnableTip = EditorText(chinese,
        L"勾选后，这个运行项会在静默模式中执行。", L"勾選後，這個運行項會在靜默模式中執行。",
        L"When enabled, this run item runs in silent mode.");
    const WCHAR* runPathModeTip = EditorText(chinese,
        L"文件路径类型。相对路径以exe所在目录为基准，只替换模式1变量；若替换后变成绝对路径也会跳过。绝对路径可使用Windows环境变量和%XHLROOT%。", L"文件路徑類型。相對路徑以exe所在目錄為基準，只替換模式1變量；若替換後變成絕對路徑也會跳過。絕對路徑可使用Windows環境變量和%XHLROOT%。",
        L"File path mode. Relative paths are based on the executable folder and only Mode1 variables are replaced; if the result becomes absolute, it is skipped. Absolute paths may use Windows environment variables and %XHLROOT%.");
    const WCHAR* runFileTip = EditorText(chinese,
        L"文件路径。相对路径只替换模式1变量，不能使用%XHLROOT%或系统环境变量；替换后若是绝对路径会跳过。无效路径会静默跳过。", L"文件路徑。相對路徑只替換模式1變量，不能使用%XHLROOT%或系統環境變量；替換後若是絕對路徑會跳過。無效路徑會靜默跳過。",
        L"File path. Relative paths only replace Mode1 variables and cannot use %XHLROOT% or system environment variables; if the result is absolute, it is skipped. Invalid paths are skipped silently.");
    const WCHAR* runArgsTip = EditorText(chinese,
        L"填写启动参数。主要用于打开程序；支持Windows环境变量和%XHLROOT%；打开普通文件时通常留空。", L"填寫啟動參數。主要用於打開程序；支持Windows環境變量和%XHLROOT%；打開普通文件時通常留空。",
        L"Optional launch arguments. Mainly used for programs; supports Windows environment variables and %XHLROOT%; usually empty for regular files.");
    const WCHAR* workingDirModeTip = EditorText(chinese,
        L"工作目录模式。目标目录表示目标文件所在目录；启动器目录表示exe所在目录；自定义目录表示使用右侧填写的目录。", L"工作目錄模式。目標目錄表示目標文件所在目錄；啟動器目錄表示exe所在目錄；自定義目錄表示使用右側填寫的目錄。",
        L"Working directory mode. Target folder uses the target file folder; Launcher folder uses the executable folder; Custom folder uses the folder on the right.");
    const WCHAR* workingDirTip = EditorText(chinese,
        L"自定义工作目录。仅选择自定义目录时使用；支持模式1变量、Windows环境变量和%XHLROOT%；最终必须是存在的文件夹。", L"自定義工作目錄。僅選擇自定義目錄時使用；支持模式1變量、Windows環境變量和%XHLROOT%；最終必須是存在的文件夾。",
        L"Custom working directory. Used only with Custom folder; Mode1 variables, Windows environment variables, and %XHLROOT% are supported. The final path must be an existing folder.");
    GraphGroupAt(EditorText(chinese, L"变量", L"變量", L"Variables"), 0, 0, XHL_EDITOR_PAGE_WIDTH, 126);
    for (UINT i = 0; i < XHL_MODE1_VARIABLE_COUNT; ++i) {
        int column = i < 3 ? 0 : 1;
        int row = i < 3 ? (int)i : (int)i - 3;
        int x = column ? 336 : 18;
        int y = 28 + row * 32;
        WCHAR label[96]; CopyText(label, 96, L"%XHLM1V"); AppendUInt(label, 96, i + 1); AppendText(label, 96, L"%");
        HWND check = GraphCheckAt(label, GraphM1VarEnableId(i), g_editorConfig.mode1Variables[i].enable, x, y, 100);
        AddEditorTooltip(check, variableTip);
        GraphEditAt(GraphM1VarValueId(i), g_editorConfig.mode1Variables[i].value, x + 104, y, XHL_EDITOR_FIELD_WIDTH);
    }
    int leftY = 166;
    GraphGroupAt(EditorText(chinese, L"注册表", L"註冊表", L"Registry"), 0, 138, XHL_EDITOR_PAGE_WIDTH, 130);
    GraphLabelTipAt(EditorText(chinese, L"路径类型", L"路徑類型", L"Path mode"), regPathModeTip, 18, leftY, 76, 24);
    GraphPathComboAt(GraphM1RegPathId(), g_editorConfig.mode1Registry.useRelativePath, 96, leftY, XHL_EDITOR_COMBO_WIDTH);
    HWND regEnable = GraphCheckAt(EditorText(chinese, L"启用", L"啟用", L"Enable"), GraphM1RegAddId(), g_editorConfig.mode1Registry.addReg, 236, leftY, 70);
    AddEditorTooltip(regEnable, regEnableTip);
    HWND regImported = GraphCheckAt(EditorText(chinese, L"标记为已导入", L"標記為已導入", L"Mark imported"), GraphM1RegImportedId(), g_editorConfig.mode1Registry.regImported, 326, leftY, 126);
    AddEditorTooltip(regImported, regImportedTip);
    leftY += 34;
    GraphLabelTipAt(EditorText(chinese, L"注册表路径", L"註冊表路徑", L"Reg path"), regFileTip, 18, leftY, 76, 24);
    GraphPathEditAt(GraphM1RegFileId(), g_editorConfig.mode1Registry.regFileName, 96, leftY, XHL_EDITOR_FIELD_RIGHT - 96, !g_editorConfig.mode1Registry.useRelativePath);
    leftY += 34;
    GraphLabelTipAt(EditorText(chinese, L"工作目录", L"工作目錄", L"Work dir"), workingDirModeTip, 18, leftY, 76, 24);
    GraphWorkingDirectoryComboAt(GraphM1RegWorkModeId(), g_editorConfig.mode1Registry.workingDirectoryMode, 96, leftY, XHL_EDITOR_COMBO_WIDTH);
    GraphCustomWorkingDirectoryAt(GraphM1RegWorkDirId(), g_editorConfig.mode1Registry.workingDirectory, workingDirTip, g_editorConfig.mode1Registry.workingDirectoryMode, leftY);

    int rightY = 308;
    GraphGroupAt(EditorText(chinese, L"打开文件", L"打開文件", L"Open file"), 0, 280, XHL_EDITOR_PAGE_WIDTH, 164);
    GraphLabelTipAt(EditorText(chinese, L"运行项", L"運行項", L"Run item"), runSelectorTip, 18, rightY, 76, 24);
    GraphSelectorAt(XHL_EDITOR_ID_M1_RUN_SELECTOR, g_editorMode1RunIndex, XHL_MODE1_RUN_COUNT, EditorText(chinese, L"运行项 ", L"運行項 ", L"Run "), 96, rightY, XHL_EDITOR_COMBO_WIDTH);
    UINT i = g_editorMode1RunIndex;
    HWND runEnable = GraphCheckAt(EditorText(chinese, L"启用", L"啟用", L"Enable"), GraphM1RunRunId(i), g_editorConfig.mode1Runs[i].run, 236, rightY, 70);
    AddEditorTooltip(runEnable, runEnableTip);
    GraphLabelTipAt(EditorText(chinese, L"路径类型", L"路徑類型", L"Path mode"), runPathModeTip, 326, rightY, 80, 24);
    GraphPathComboAt(GraphM1RunPathId(i), g_editorConfig.mode1Runs[i].useRelativePath, 414, rightY, XHL_EDITOR_COMBO_WIDTH);
    rightY += 34;
    GraphLabelTipAt(EditorText(chinese, L"文件路径", L"文件路徑", L"File path"), runFileTip, 18, rightY, 76, 24);
    GraphPathEditAt(GraphM1RunFileId(i), g_editorConfig.mode1Runs[i].fileName, 96, rightY, XHL_EDITOR_FIELD_RIGHT - 96, !g_editorConfig.mode1Runs[i].useRelativePath);
    rightY += 34;
    GraphLabelTipAt(EditorText(chinese, L"启动参数", L"啟動參數", L"Arguments"), runArgsTip, 18, rightY, 76, 24);
    GraphEditAt(GraphM1RunArgsId(i), g_editorConfig.mode1Runs[i].arguments, 96, rightY, XHL_EDITOR_FIELD_RIGHT - 96);
    rightY += 34;
    GraphLabelTipAt(EditorText(chinese, L"工作目录", L"工作目錄", L"Work dir"), workingDirModeTip, 18, rightY, 76, 24);
    GraphWorkingDirectoryComboAt(GraphM1RunWorkModeId(i), g_editorConfig.mode1Runs[i].workingDirectoryMode, 96, rightY, XHL_EDITOR_COMBO_WIDTH);
    GraphCustomWorkingDirectoryAt(GraphM1RunWorkDirId(i), g_editorConfig.mode1Runs[i].workingDirectory, workingDirTip, g_editorConfig.mode1Runs[i].workingDirectoryMode, rightY);
}

static void GraphAddMode2Page() {
    BOOL chinese = GraphEditorUsesChinese();
    const WCHAR* elementTip = EditorText(chinese,
        L"元素选择。窗口模式最多支持10个元素，显示顺序固定为Element1到Element10。", L"元素選擇。窗口模式最多支持10個元素，顯示順序固定為Element1到Element10。",
        L"Element selector. Window mode supports up to 10 elements, displayed in fixed Element1 to Element10 order.");
    const WCHAR* elementEnableTip = EditorText(chinese,
        L"勾选后该元素会显示；未勾选时窗口模式会跳过这个元素。", L"勾選後該元素會顯示；未勾選時窗口模式會跳過這個元素。",
        L"When enabled, this element is shown. Otherwise window mode skips it.");
    const WCHAR* elementTypeTip = EditorText(chinese,
        L"元素类型。可选文本、文本框（输入）、文本框（读取）、输入框、打开文件按钮、弹窗按钮、注册表按钮、表格或关闭按钮；不同类型只使用与自身相关的配置项。", L"元素類型。可選文本、文本框（輸入）、文本框（讀取）、輸入框、打開文件按鈕、彈窗按鈕、註冊表按鈕、表格或關閉按鈕；不同類型只使用與自身相關的配置項。",
        L"Element type. Choose Text, TextBox, TextFileBox, Input field, Open button, Message button, Registry button, Table, or Close button; each type only uses its relevant settings.");
    const WCHAR* elementTextTip = EditorText(chinese,
        L"显示文字。文本框（输入）显示为可滚动文本框；文本显示为单行文本，最多512个字符；按钮显示为按钮文字，最多512个字符。输入框、文本框（读取）和表格不使用此项。",
        L"顯示文字。文本框（輸入）顯示為可滾動文本框；文本顯示為單行文本，最多512個字符；按鈕顯示為按鈕文字，最多512個字符。輸入框、文本框（讀取）和表格不使用此項。",
        L"Display text. TextBox is scrollable, Text is single-line up to 512 characters, and buttons use it as button text up to 512 characters. Input, TextFileBox, and Table do not use it.");
    const WCHAR* minimizeOnClickTip = EditorText(chinese,
        L"点击后最小化。仅打开文件、弹窗和注册表按钮使用；勾选后，点击按钮时会同时把窗口最小化。",
        L"點擊後最小化。僅打開文件、彈窗和註冊表按鈕使用；勾選後，點擊按鈕時會同時把窗口最小化。",
        L"Minimize after click. Used only by Open file, Message, and Registry buttons. When enabled, clicking the button also minimizes the window.");
    const WCHAR* messageTip = EditorText(chinese,
        L"弹窗内容。仅弹窗使用；配置中保持一行，用\\n表示实际换行；支持模式2变量。", L"彈窗內容。僅彈窗使用；配置中保持一行，用\\n表示實際換行；支持模式2變量。",
        L"Message content. Used only by Message; keep it on one INI line and use \\n for line breaks. Mode2 variables are supported.");
    const WCHAR* regPathModeTip = EditorText(chinese,
        L"注册表路径类型。仅注册表使用。相对路径以exe所在目录为基准，只替换模式2变量；若替换后变成绝对路径也会跳过。绝对路径可使用Windows环境变量和%XHLROOT%。", L"註冊表路徑類型。僅註冊表使用。相對路徑以exe所在目錄為基準，只替換模式2變量；若替換後變成絕對路徑也會跳過。絕對路徑可使用Windows環境變量和%XHLROOT%。",
        L"Registry path mode. Used only by Registry. Relative paths are based on the executable folder and only Mode2 variables are replaced; if the result becomes absolute, it is skipped. Absolute paths may use Windows environment variables and %XHLROOT%.");
    const WCHAR* regFileTip = EditorText(chinese,
        L"注册表路径。仅注册表使用；相对路径只替换模式2变量，不能使用%XHLROOT%或系统环境变量；替换后若是绝对路径会跳过。", L"註冊表路徑。僅註冊表使用；相對路徑只替換模式2變量，不能使用%XHLROOT%或系統環境變量；替換後若是絕對路徑會跳過。",
        L"Registry path. Used only by Registry. Relative paths only replace Mode2 variables and cannot use %XHLROOT% or system environment variables; if the result is absolute, it is skipped.");
    const WCHAR* regImportingTip = EditorText(chinese,
        L"导入中文字。仅注册表使用；点击后导入完成前显示在按钮上。留空时使用默认文字。", L"導入中文字。僅註冊表使用；點擊後導入完成前顯示在按鈕上。留空時使用預設文字。",
        L"Importing text. Used only by Registry. Shown on the button while import is running. Empty means default text.");
    const WCHAR* regSuccessTip = EditorText(chinese,
        L"成功提示。仅注册表使用；导入成功后显示。留空时使用默认成功提示。", L"成功提示。僅註冊表使用；導入成功後顯示。留空時使用預設成功提示。",
        L"Success message. Used only by Registry. Shown after a successful import. Empty means default message.");
    const WCHAR* regFailureTip = EditorText(chinese,
        L"失败提示。仅注册表使用；导入失败后显示。建议提醒可能已部分导入，需要重试或手动导入。留空时使用默认提示。", L"失敗提示。僅註冊表使用；導入失敗後顯示。建議提醒可能已部分導入，需要重試或手動導入。留空時使用預設提示。",
        L"Failure message. Used only by Registry. Shown after a failed import. It should mention possible partial import and retry/manual import. Empty means default message.");
    const WCHAR* regTimeoutTip = EditorText(chinese,
        L"超时提示。仅注册表使用；导入超过60秒并被停止后显示。建议提醒可能已部分导入，需要重试或手动导入。留空时使用默认提示。", L"超時提示。僅註冊表使用；導入超過60秒並被停止後顯示。建議提醒可能已部分導入，需要重試或手動導入。留空時使用預設提示。",
        L"Timeout message. Used only by Registry. Shown when import exceeds 60 seconds and is stopped. It should mention possible partial import and retry/manual import. Empty means default message.");
    const WCHAR* runSelectorTip = EditorText(chinese,
        L"运行项选择。每个打开文件元素最多支持5个运行项，选择后可编辑该项的开关、路径和启动参数。", L"運行項選擇。每個打開文件元素最多支持5個運行項，選擇後可編輯該項的開關、路徑和啟動參數。",
        L"Run item selector. Each Open file element supports up to 5 run items; choose one to edit its switch, path, and arguments.");
    const WCHAR* runEnableTip = EditorText(chinese,
        L"勾选后，点击打开文件元素时会执行该运行项。", L"勾選後，點擊打開文件元素時會執行該運行項。",
        L"When enabled, this run item runs when the Open file element is clicked.");
    const WCHAR* runPathModeTip = EditorText(chinese,
        L"文件路径类型。仅打开文件使用。相对路径以exe所在目录为基准，只替换模式2变量；若替换后变成绝对路径也会跳过。绝对路径可使用Windows环境变量和%XHLROOT%。", L"文件路徑類型。僅打開文件使用。相對路徑以exe所在目錄為基準，只替換模式2變量；若替換後變成絕對路徑也會跳過。絕對路徑可使用Windows環境變量和%XHLROOT%。",
        L"File path mode. Used only by Open file. Relative paths are based on the executable folder and only Mode2 variables are replaced; if the result becomes absolute, it is skipped. Absolute paths may use Windows environment variables and %XHLROOT%.");
    const WCHAR* runFileTip = EditorText(chinese,
        L"文件路径。仅打开文件使用；相对路径只替换模式2变量，不能使用%XHLROOT%或系统环境变量；替换后若是绝对路径会跳过。", L"文件路徑。僅打開文件使用；相對路徑只替換模式2變量，不能使用%XHLROOT%或系統環境變量；替換後若是絕對路徑會跳過。",
        L"File path. Used only by Open file. Relative paths only replace Mode2 variables and cannot use %XHLROOT% or system environment variables; if the result is absolute, it is skipped.");
    const WCHAR* runArgsTip = EditorText(chinese,
        L"填写启动参数。主要用于打开程序；支持Windows环境变量和%XHLROOT%；打开普通文件时通常留空。", L"填寫啟動參數。主要用於打開程序；支持Windows環境變量和%XHLROOT%；打開普通文件時通常留空。",
        L"Optional launch arguments. Mainly used for programs; supports Windows environment variables and %XHLROOT%; usually empty for regular files.");
    const WCHAR* workingDirModeTip = EditorText(chinese,
        L"工作目录模式。目标目录表示目标文件所在目录；启动器目录表示exe所在目录；自定义目录表示使用右侧填写的目录。", L"工作目錄模式。目標目錄表示目標文件所在目錄；啟動器目錄表示exe所在目錄；自定義目錄表示使用右側填寫的目錄。",
        L"Working directory mode. Target folder uses the target file folder; Launcher folder uses the executable folder; Custom folder uses the folder on the right.");
    const WCHAR* workingDirTip = EditorText(chinese,
        L"自定义工作目录。仅选择自定义目录时使用；支持模式2变量、Windows环境变量和%XHLROOT%；最终必须是存在的文件夹。", L"自定義工作目錄。僅選擇自定義目錄時使用；支持模式2變量、Windows環境變量和%XHLROOT%；最終必須是存在的文件夾。",
        L"Custom working directory. Used only with Custom folder; Mode2 variables, Windows environment variables, and %XHLROOT% are supported. The final path must be an existing folder.");
    const WCHAR* tablePathModeTip = EditorText(chinese,
        L"表格路径类型。仅表格使用。相对路径以exe所在目录为基准，只替换模式2变量；若替换后变成绝对路径也会跳过。绝对路径可使用Windows环境变量和%XHLROOT%。", L"表格路徑類型。僅表格使用。相對路徑以exe所在目錄為基準，只替換模式2變量；若替換後變成絕對路徑也會跳過。絕對路徑可使用Windows環境變量和%XHLROOT%。",
        L"Table path mode. Used only by table elements. Relative paths are based on the executable folder and only Mode2 variables are replaced; if the result becomes absolute, it is skipped. Absolute paths may use Windows environment variables and %XHLROOT%.");
    const WCHAR* tableFileTip = EditorText(chinese,
        L"表格路径。仅表格使用；只支持.csv文件，最终路径必须以.csv结尾。第一行作为表头，后续行作为内容；最多显示前20列和1000行。", L"表格路徑。僅表格使用；只支持.csv文件，最終路徑必須以.csv結尾。第一行作為表頭，後續行作為內容；最多顯示前20列和1000行。",
        L"Table path. Used only by table elements. Only .csv files are supported; the final path must end in .csv. The first row is the header and later rows are content; up to 20 columns and 1000 rows are shown.");
    const WCHAR* heightScaleTip = EditorText(chinese,
        L"高度倍率。仅两种文本框和表格使用；一倍、两倍、三倍分别约等于2个、4个、6个按钮高度。若选择三倍并显示较多文本框或表格，窗口可能超出屏幕。", L"高度倍率。僅兩種文本框和表格使用；一倍、兩倍、三倍分別約等於2個、4個、6個按鈕高度。若選擇三倍並顯示較多文本框或表格，窗口可能超出屏幕。",
        L"Height scale. Used only by both TextBox types and Table. 1x, 2x, and 3x are about 2, 4, and 6 button heights. With 3x and many TextBox or Table elements, the window may exceed the screen.");
    const WCHAR* textFilePathModeTip = EditorText(chinese,
        L"文本路径类型。仅文本框（读取）使用。相对路径以exe所在目录为基准，只替换模式2变量；若替换后变成绝对路径也会跳过。绝对路径可使用Windows环境变量和%XHLROOT%。", L"文本路徑類型。僅文本框（讀取）使用。相對路徑以exe所在目錄為基準，只替換模式2變量；若替換後變成絕對路徑也會跳過。絕對路徑可使用Windows環境變量和%XHLROOT%。",
        L"Text file path mode. Used only by TextFileBox. Relative paths are based on the executable folder and only Mode2 variables are replaced; if the result becomes absolute, it is skipped. Absolute paths may use Windows environment variables and %XHLROOT%.");
    const WCHAR* textFileTip = EditorText(chinese,
        L"文本路径。仅文本框（读取）使用；读取本地文本文件并显示为只读滚动文本。优先按UTF-8读取；如果不是有效UTF-8，会按系统默认编码转换后显示。支持 CRLF 和 LF 换行。文件最大约4MB。",
        L"文本路徑。僅文本框（讀取）使用；讀取本地文本文件並顯示為只讀滾動文本。優先按UTF-8讀取；如果不是有效UTF-8，會按系統預設編碼轉換後顯示。支持 CRLF 和 LF 換行。文件最大約4MB。",
        L"Text file path. Used only by TextFileBox; reads a local text file and shows it as read-only scrolling text. UTF-8 is used first; if it is not valid UTF-8, the system default code page is used. CRLF and LF line breaks are supported. File size is up to about 4 MB.");
    const WCHAR* inputVariableTip = EditorText(chinese,
        L"输入框变量。仅输入框使用；用户在窗口中输入的内容会作为所选模式2变量的临时值，供之后点击按钮时替换使用；不会写回配置文件。多个输入框写同一变量时，元素编号靠后的输入框会覆盖前面的值。替换后的运行时文本最多32767个UTF-16字符，超过会视为无效。",
        L"輸入框變量。僅輸入框使用；用戶在窗口中輸入的內容會作為所選模式2變量的臨時值，供之後點擊按鈕時替換使用；不會寫回配置文件。多個輸入框寫同一變量時，元素編號靠後的輸入框會覆蓋前面的值。替換後的運行時文本最多32767個UTF-16字符，超過會視為無效。",
        L"Input variable. Used only by Input; text entered in the window becomes the selected Mode2 variable value for later button actions. It is not written back to the INI file. If multiple Input elements write the same variable, the later element number overwrites earlier ones. Runtime text after replacement is limited to 32767 UTF-16 characters; longer values are treated as invalid.");

    UINT i = g_editorMode2ElementIndex;
    EditorMode2ElementConfig* element = &g_editorConfig.mode2Elements[i];
    Mode2ElementKind kind = Mode2KindFromType(element->type);
    BOOL compactElement = (kind == Mode2ElementTable || kind == Mode2ElementTextFileBox || kind == Mode2ElementInput);
    int elementGroupHeight = compactElement ? 62 : 96;
    int detailTop = compactElement ? 74 : 108;
    int y = 28;
    GraphGroupAt(EditorText(chinese, L"界面元素", L"界面元素", L"Window elements"), 0, 0, XHL_EDITOR_PAGE_WIDTH, elementGroupHeight);
    GraphLabelTipAt(EditorText(chinese, L"元素", L"元素", L"Element"), elementTip, 18, y, 76, 24);
    GraphSelectorAt(XHL_EDITOR_ID_M2_ELEMENT_SELECTOR, g_editorMode2ElementIndex, XHL_MODE2_ELEMENT_COUNT, EditorText(chinese, L"元素 ", L"元素 ", L"Element "), 96, y, XHL_EDITOR_COMBO_WIDTH);
    HWND elementEnable = GraphCheckAt(EditorText(chinese, L"启用", L"啟用", L"Enable"), GraphM2ElementShowId(i), element->show, 236, y, 70);
    AddEditorTooltip(elementEnable, elementEnableTip);
    GraphLabelTipAt(EditorText(chinese, L"类型", L"類型", L"Type"), elementTypeTip, 326, y, 64, 24);
    GraphTypeComboAt(GraphM2ElementTypeId(i), element->type, 396, y, XHL_EDITOR_COMBO_WIDTH);
    if (kind == Mode2ElementOpenButton || kind == Mode2ElementMessageButton || kind == Mode2ElementRegButton) {
        HWND minimizeCheck = GraphCheckAt(EditorText(chinese, L"点击后最小化", L"點擊後最小化", L"Minimize"), GraphM2ElementMinimizeId(i), element->minimizeOnClick, 532, y, 104);
        AddEditorTooltip(minimizeCheck, minimizeOnClickTip);
    }

    y += 34;
    if (kind != Mode2ElementTable && kind != Mode2ElementTextFileBox && kind != Mode2ElementInput) {
        int textWidth = kind == Mode2ElementTextBox ? 300 : XHL_EDITOR_FIELD_RIGHT - 96;
        GraphLabelTipAt(EditorText(chinese, L"显示文字", L"顯示文字", L"Text"), elementTextTip, 18, y, 76, 24);
        GraphEditAt(GraphM2ElementTextId(i), element->text, 96, y, textWidth);
        if (kind == Mode2ElementTextBox) {
            GraphLabelTipAt(EditorText(chinese, L"高度倍率", L"高度倍率", L"Height"), heightScaleTip, 414, y, 76, 24);
            GraphHeightScaleComboAt(GraphM2ElementHeightScaleId(i), element->heightScale, 496, y, 124);
        }
    }

    y = detailTop + 28;
    if (kind == Mode2ElementMessageButton) {
        GraphGroupAt(EditorText(chinese, L"弹窗", L"彈窗", L"Message"), 0, detailTop, XHL_EDITOR_PAGE_WIDTH, 62);
        GraphLabelTipAt(EditorText(chinese, L"弹窗内容", L"彈窗內容", L"Message"), messageTip, 18, y, 76, 24);
        GraphEditAt(GraphM2ElementMessageId(i), element->message, 96, y, XHL_EDITOR_FIELD_RIGHT - 96);
    } else if (kind == Mode2ElementRegButton) {
        GraphGroupAt(EditorText(chinese, L"注册表", L"註冊表", L"Registry"), 0, detailTop, XHL_EDITOR_PAGE_WIDTH, 266);
        GraphLabelTipAt(EditorText(chinese, L"路径类型", L"路徑類型", L"Path mode"), regPathModeTip, 18, y, 76, 24);
        GraphPathComboAt(GraphM2ElementRegPathId(i), element->regUseRelativePath, 96, y, XHL_EDITOR_COMBO_WIDTH);
        y += 34;
        GraphLabelTipAt(EditorText(chinese, L"注册表路径", L"註冊表路徑", L"Reg path"), regFileTip, 18, y, 76, 24);
        GraphPathEditAt(GraphM2ElementRegFileId(i), element->regFileName, 96, y, XHL_EDITOR_FIELD_RIGHT - 96, !element->regUseRelativePath);
        y += 34;
        GraphLabelTipAt(EditorText(chinese, L"工作目录", L"工作目錄", L"Work dir"), workingDirModeTip, 18, y, 76, 24);
        GraphWorkingDirectoryComboAt(GraphM2ElementRegWorkModeId(i), element->regWorkingDirectoryMode, 96, y, XHL_EDITOR_COMBO_WIDTH);
        GraphCustomWorkingDirectoryAt(GraphM2ElementRegWorkDirId(i), element->regWorkingDirectory, workingDirTip, element->regWorkingDirectoryMode, y);
        y += 34;
        GraphLabelTipAt(EditorText(chinese, L"导入中", L"導入中", L"Importing"), regImportingTip, 18, y, 76, 24);
        GraphEditAt(GraphM2ElementRegImportingId(i), element->regImportingText, 96, y, XHL_EDITOR_FIELD_RIGHT - 96);
        y += 34;
        GraphLabelTipAt(EditorText(chinese, L"成功提示", L"成功提示", L"Success"), regSuccessTip, 18, y, 76, 24);
        GraphEditAt(GraphM2ElementRegSuccessId(i), element->regSuccessMessage, 96, y, XHL_EDITOR_FIELD_RIGHT - 96);
        y += 34;
        GraphLabelTipAt(EditorText(chinese, L"失败提示", L"失敗提示", L"Failure"), regFailureTip, 18, y, 76, 24);
        GraphEditAt(GraphM2ElementRegFailureId(i), element->regFailureMessage, 96, y, XHL_EDITOR_FIELD_RIGHT - 96);
        y += 34;
        GraphLabelTipAt(EditorText(chinese, L"超时提示", L"超時提示", L"Timeout"), regTimeoutTip, 18, y, 76, 24);
        GraphEditAt(GraphM2ElementRegTimeoutId(i), element->regTimeoutMessage, 96, y, XHL_EDITOR_FIELD_RIGHT - 96);
    } else if (kind == Mode2ElementOpenButton) {
        GraphGroupAt(EditorText(chinese, L"打开文件", L"打開文件", L"Open file"), 0, detailTop, XHL_EDITOR_PAGE_WIDTH, 164);
        GraphLabelTipAt(EditorText(chinese, L"运行项", L"運行項", L"Run item"), runSelectorTip, 18, y, 76, 24);
        GraphSelectorAt(XHL_EDITOR_ID_M2_RUN_SELECTOR, g_editorMode2RunIndex, XHL_MODE2_BUTTON_RUN_COUNT, EditorText(chinese, L"运行项 ", L"運行項 ", L"Run "), 96, y, XHL_EDITOR_COMBO_WIDTH);
        y += 34;
        UINT run = g_editorMode2RunIndex;
        HWND runEnable = GraphCheckAt(EditorText(chinese, L"启用", L"啟用", L"Enable"), GraphM2ElementRunId(i, run), element->runs[run].run, 236, y - 34, 70);
        AddEditorTooltip(runEnable, runEnableTip);
        GraphLabelTipAt(EditorText(chinese, L"路径类型", L"路徑類型", L"Path mode"), runPathModeTip, 326, y - 34, 80, 24);
        GraphPathComboAt(GraphM2ElementRunPathId(i, run), element->runs[run].useRelativePath, 414, y - 34, XHL_EDITOR_COMBO_WIDTH);
        GraphLabelTipAt(EditorText(chinese, L"文件路径", L"文件路徑", L"File path"), runFileTip, 18, y, 76, 24);
        GraphPathEditAt(GraphM2ElementRunFileId(i, run), element->runs[run].fileName, 96, y, XHL_EDITOR_FIELD_RIGHT - 96, !element->runs[run].useRelativePath);
        y += 34;
        GraphLabelTipAt(EditorText(chinese, L"启动参数", L"啟動參數", L"Arguments"), runArgsTip, 18, y, 76, 24);
        GraphEditAt(GraphM2ElementRunArgsId(i, run), element->runs[run].arguments, 96, y, XHL_EDITOR_FIELD_RIGHT - 96);
        y += 34;
        GraphLabelTipAt(EditorText(chinese, L"工作目录", L"工作目錄", L"Work dir"), workingDirModeTip, 18, y, 76, 24);
        GraphWorkingDirectoryComboAt(GraphM2ElementRunWorkModeId(i, run), element->runs[run].workingDirectoryMode, 96, y, XHL_EDITOR_COMBO_WIDTH);
        GraphCustomWorkingDirectoryAt(GraphM2ElementRunWorkDirId(i, run), element->runs[run].workingDirectory, workingDirTip, element->runs[run].workingDirectoryMode, y);
    } else if (kind == Mode2ElementTable) {
        GraphGroupAt(EditorText(chinese, L"表格", L"表格", L"Table"), 0, detailTop, XHL_EDITOR_PAGE_WIDTH, 96);
        GraphLabelTipAt(EditorText(chinese, L"路径类型", L"路徑類型", L"Path mode"), tablePathModeTip, 18, y, 76, 24);
        GraphPathComboAt(GraphM2ElementCsvPathId(i), element->csvUseRelativePath, 96, y, XHL_EDITOR_COMBO_WIDTH);
        GraphLabelTipAt(EditorText(chinese, L"高度倍率", L"高度倍率", L"Height"), heightScaleTip, 326, y, 64, 24);
        GraphHeightScaleComboAt(GraphM2ElementHeightScaleId(i), element->heightScale, 396, y, XHL_EDITOR_COMBO_WIDTH);
        y += 34;
        GraphLabelTipAt(EditorText(chinese, L"表格路径", L"表格路徑", L"Table path"), tableFileTip, 18, y, 76, 24);
        GraphPathEditAt(GraphM2ElementCsvFileId(i), element->csvFileName, 96, y, XHL_EDITOR_FIELD_RIGHT - 96, !element->csvUseRelativePath);
    } else if (kind == Mode2ElementTextFileBox) {
        GraphGroupAt(EditorText(chinese, L"文本框（读取）", L"文本框（讀取）", L"TextFileBox"), 0, detailTop, XHL_EDITOR_PAGE_WIDTH, 96);
        GraphLabelTipAt(EditorText(chinese, L"路径类型", L"路徑類型", L"Path mode"), textFilePathModeTip, 18, y, 76, 24);
        GraphPathComboAt(GraphM2ElementTextFilePathId(i), element->textFileUseRelativePath, 96, y, XHL_EDITOR_COMBO_WIDTH);
        GraphLabelTipAt(EditorText(chinese, L"高度倍率", L"高度倍率", L"Height"), heightScaleTip, 326, y, 64, 24);
        GraphHeightScaleComboAt(GraphM2ElementHeightScaleId(i), element->heightScale, 396, y, XHL_EDITOR_COMBO_WIDTH);
        y += 34;
        GraphLabelTipAt(EditorText(chinese, L"文本路径", L"文本路徑", L"Text path"), textFileTip, 18, y, 76, 24);
        GraphPathEditAt(GraphM2ElementTextFileId(i), element->textFileName, 96, y, XHL_EDITOR_FIELD_RIGHT - 96, !element->textFileUseRelativePath);
    } else if (kind == Mode2ElementInput) {
        GraphGroupAt(EditorText(chinese, L"输入框", L"輸入框", L"Input"), 0, detailTop, XHL_EDITOR_PAGE_WIDTH, 62);
        GraphLabelTipAt(EditorText(chinese, L"变量", L"變量", L"Variable"), inputVariableTip, 18, y, 76, 24);
        GraphMode2VariableComboAt(GraphM2ElementInputVariableId(i), element->inputVariableIndex, 96, y, XHL_EDITOR_COMBO_WIDTH);
    }
}

static void GraphBuildPage() {
    GraphDestroyPageControls();
    g_editorPageMode = g_editorConfig.mode;
    if (g_editorPageMode == ModeSilent) GraphAddMode1Page();
    else if (g_editorPageMode == ModeWindow) GraphAddMode2Page();
    else GraphAddMode0Page();
}

static void GraphApplyEditorWindowTitle(HWND window) {
    std::wstring title = g_editorConfig.title;
    std::wstring version = g_editorConfig.version;
    TruncateString(title, XHL_TITLE_MAX_CHARS);
    TruncateString(version, XHL_CONFIG_VERSION_MAX_CHARS);
    ComposeDisplayTitle(title.c_str(), version.c_str(), g_displayTitle, XHL_MAX_TEXT);
    SetWindowTextW(window, g_displayTitle);
}

static void GraphRefreshLanguage(HWND window) {
    g_editorConfig.language = GraphEditorLanguage();
    BOOL chinese = LanguageIsChinese(g_editorConfig.language);
    g_editorConfig.chinese = chinese;
    GraphApplyEditorWindowTitle(window);
    SetWindowTextW(g_editorTitleLabel, EditorText(chinese, L"标题", L"標題", L"Title"));
    SetWindowTextW(g_editorVersionLabel, EditorText(chinese, L"版本号", L"版本號", L"Version"));
    SetWindowTextW(g_editorLanguageLabel, EditorText(chinese, L"语言", L"語言", L"Language"));
    SetWindowTextW(g_editorModeLabel, EditorText(chinese, L"模式", L"模式", L"Mode"));
    SetEditorModeCombo(g_editorConfig.mode);
    SetEditorLanguageCombo(g_editorConfig.language);
    SetWindowTextW(g_editorSaveButton, EditorText(chinese, L"保存并关闭", L"保存並關閉", L"Save and Close"));
    SetWindowTextW(g_editorCancelButton, EditorText(chinese, L"取消并退出", L"取消並退出", L"Cancel and Exit"));
    SetWindowTextW(g_editorAboutButton, EditorText(chinese, L"关于", L"關於", L"About"));
    ApplyTopEditorTooltips();
}

static void GraphSaveAndClose(HWND window) {
    GraphReadPageToState();
    GraphReadTopToState(window);
    if (!GraphSaveStateToIni()) {
        ShowSaveFailedMessage(window);
        return;
    }
    ExitApplication(window, 0);
}

static void GraphCreateControls(HWND window) {
    UINT dpi = GetDpi(window);
    BOOL chinese = LanguageIsChinese(g_editorConfig.language);
    UpdateUIFont(dpi);
    InitEditorTooltip(window, dpi);
    g_editorTitleLabel = CreateEditorControl(window, L"STATIC", EditorText(chinese, L"标题", L"標題", L"Title"), SS_LEFT | SS_CENTERIMAGE | SS_NOPREFIX, 0, 20, 16, 46, 22, XHL_EDITOR_ID_TITLE_LABEL, dpi);
    ApplyBoldEditorFont(g_editorTitleLabel);
    g_editorTitleEdit = CreateEditorControl(window, L"EDIT", g_editorConfig.title, WS_TABSTOP | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, 70, 14, 338, 24, XHL_EDITOR_ID_TITLE, dpi);
    g_editorVersionLabel = CreateEditorControl(window, L"STATIC", EditorText(chinese, L"版本号", L"版本號", L"Version"), SS_LEFT | SS_CENTERIMAGE | SS_NOPREFIX, 0, 424, 16, 64, 22, XHL_EDITOR_ID_VERSION_LABEL, dpi);
    ApplyBoldEditorFont(g_editorVersionLabel);
    g_editorVersionEdit = CreateEditorControl(window, L"EDIT", g_editorConfig.version, WS_TABSTOP | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, 490, 14, 190, 24, XHL_EDITOR_ID_VERSION, dpi);
    g_editorModeLabel = CreateEditorControl(window, L"STATIC", EditorText(chinese, L"模式", L"模式", L"Mode"), SS_LEFT | SS_CENTERIMAGE | SS_NOPREFIX, 0, 20, 50, 46, 22, XHL_EDITOR_ID_MODE_LABEL, dpi);
    ApplyBoldEditorFont(g_editorModeLabel);
    g_editorModeCombo = CreateEditorControl(window, L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL, 0, 70, 47, 230, 160, XHL_EDITOR_ID_MODE_COMBO, dpi);
    SetEditorModeCombo(g_editorConfig.mode);
    g_editorLanguageLabel = CreateEditorControl(window, L"STATIC", EditorText(chinese, L"语言", L"語言", L"Language"), SS_LEFT | SS_CENTERIMAGE | SS_NOPREFIX, 0, 404, 50, 64, 22, XHL_EDITOR_ID_LANGUAGE_LABEL, dpi);
    ApplyBoldEditorFont(g_editorLanguageLabel);
    g_editorLanguageCombo = CreateEditorControl(window, L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL, 0, 470, 47, 154, 120, XHL_EDITOR_ID_LANGUAGE_COMBO, dpi);
    g_editorAboutButton = CreateEditorControl(window, L"BUTTON", EditorText(chinese, L"关于", L"關於", L"About"), WS_TABSTOP | BS_PUSHBUTTON, 0, 628, 48, 52, 24, XHL_EDITOR_ID_ABOUT, dpi);
    SetEditorLanguageCombo(g_editorConfig.language);
    g_editorPanel = CreateEditorControl(window, L"XHLauncherGraphEditorPanel", L"", WS_CLIPCHILDREN, 0, 20, 84, XHL_EDITOR_PAGE_WIDTH, 470, XHL_EDITOR_ID_PANEL, dpi);
    g_editorSaveButton = CreateEditorControl(window, L"BUTTON", EditorText(chinese, L"保存并关闭", L"保存並關閉", L"Save and Close"), WS_TABSTOP | BS_DEFPUSHBUTTON, 0, 430, 594, 140, 32, XHL_EDITOR_ID_SAVE, dpi);
    g_editorCancelButton = CreateEditorControl(window, L"BUTTON", EditorText(chinese, L"取消并退出", L"取消並退出", L"Cancel and Exit"), WS_TABSTOP | BS_PUSHBUTTON, 0, 560, 594, 120, 32, XHL_EDITOR_ID_CANCEL, dpi);
    ApplyTopEditorTooltips();
    GraphBuildPage();
}

static void GraphMoveControl(HWND control, int x, int y, int width, int height, UINT dpi) {
    if (control) {
        MoveWindow(control, ScaleForDpi(x, dpi), ScaleForDpi(y, dpi), ScaleForDpi(width, dpi), ScaleForDpi(height, dpi), TRUE);
    }
}

static void ApplyTopEditorFonts() {
    ApplyBoldEditorFont(g_editorTitleLabel);
    ApplyBoldEditorFont(g_editorVersionLabel);
    ApplyBoldEditorFont(g_editorModeLabel);
    ApplyBoldEditorFont(g_editorLanguageLabel);

    HWND controls[] = {
        g_editorTitleEdit, g_editorVersionEdit,
        g_editorModeCombo, g_editorLanguageCombo, g_editorAboutButton,
        g_editorSaveButton, g_editorCancelButton
    };
    for (UINT i = 0; i < sizeof(controls) / sizeof(controls[0]); ++i) {
        if (controls[i] && g_uiFont) {
            SendMessageW(controls[i], WM_SETFONT, (WPARAM)g_uiFont, TRUE);
        }
    }
}

static void GraphLayoutWindow(HWND window) {
    if (!window) return;
    UINT dpi = GetDpi(window);
    RECT client;
    GetClientRect(window, &client);
    int margin = ScaleForDpi(20, dpi);
    int panelTop = ScaleForDpi(84, dpi);
    int buttonTop = client.bottom - ScaleForDpi(46, dpi);
    int panelHeight = buttonTop - panelTop - ScaleForDpi(12, dpi);
    if (panelHeight < ScaleForDpi(300, dpi)) panelHeight = ScaleForDpi(300, dpi);

    GraphMoveControl(g_editorTitleLabel, 20, 16, 46, 22, dpi);
    GraphMoveControl(g_editorTitleEdit, 70, 14, 338, 24, dpi);
    GraphMoveControl(g_editorVersionLabel, 424, 16, 64, 22, dpi);
    GraphMoveControl(g_editorVersionEdit, 490, 14, 190, 24, dpi);
    GraphMoveControl(g_editorModeLabel, 20, 50, 46, 22, dpi);
    GraphMoveControl(g_editorModeCombo, 70, 47, 230, 160, dpi);
    GraphMoveControl(g_editorLanguageLabel, 404, 50, 64, 22, dpi);
    GraphMoveControl(g_editorLanguageCombo, 470, 47, 154, 120, dpi);
    GraphMoveControl(g_editorAboutButton, 628, 48, 52, 24, dpi);

    if (g_editorPanel) {
        MoveWindow(g_editorPanel, margin, panelTop, client.right - margin * 2, panelHeight, TRUE);
    }
    int saveWidth = ScaleForDpi(150, dpi);
    int cancelWidth = ScaleForDpi(130, dpi);
    int buttonGap = ScaleForDpi(12, dpi);
    int buttonLeft = (client.right - saveWidth - cancelWidth - buttonGap) / 2;
    MoveWindow(g_editorSaveButton, buttonLeft, buttonTop, saveWidth, ScaleForDpi(32, dpi), TRUE);
    MoveWindow(g_editorCancelButton, buttonLeft + saveWidth + buttonGap, buttonTop, cancelWidth, ScaleForDpi(32, dpi), TRUE);
}

static LRESULT CALLBACK GraphPanelProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_ERASEBKGND: {
        RECT rect; GetClientRect(window, &rect);
        FillRect((HDC)wParam, &rect, GetSysColorBrush(COLOR_WINDOW));
        return 1;
    }
    case WM_MOUSEWHEEL:
        return 0;
    case WM_COMMAND:
        SendMessageW(GetParent(window), WM_COMMAND, wParam, lParam);
        return 0;
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLOREDIT: {
        HDC dc = (HDC)wParam;
        SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(dc, GetSysColor(COLOR_WINDOW));
        SetBkMode(dc, OPAQUE);
        return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
    }
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

static LRESULT CALLBACK GraphEditorProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        GraphCreateControls(window);
        GraphLayoutWindow(window);
        return 0;
    case WM_SIZE:
        GraphLayoutWindow(window);
        return 0;
    case WM_DPICHANGED: {
        GraphReadPageToState();
        GraphReadTopToState(window);
        UINT dpi = HIWORD(wParam);
        RECT* suggested = (RECT*)lParam;
        if (suggested) {
            SetWindowPos(
                window,
                0,
                suggested->left,
                suggested->top,
                suggested->right - suggested->left,
                suggested->bottom - suggested->top,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        UpdateUIFont(dpi);
        ApplyTopEditorFonts();
        InitEditorTooltip(window, dpi);
        GraphRebuildPanel(window);
        GraphLayoutWindow(window);
        InvalidateRect(window, 0, TRUE);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == XHL_EDITOR_ID_ABOUT && HIWORD(wParam) == BN_CLICKED) {
            ShowAboutMessage(window);
            return 0;
        }
        if (LOWORD(wParam) == XHL_EDITOR_ID_SAVE && HIWORD(wParam) == BN_CLICKED) {
            GraphSaveAndClose(window);
            return 0;
        }
        if (LOWORD(wParam) == XHL_EDITOR_ID_CANCEL && HIWORD(wParam) == BN_CLICKED) {
            ExitApplication(window, 0);
        }
        if (LOWORD(wParam) == XHL_EDITOR_ID_LANGUAGE_COMBO && HIWORD(wParam) == CBN_SELCHANGE) {
            GraphReadPageToState();
            GraphReadTopToState(window);
            GraphRefreshLanguage(window);
            GraphRebuildPanel(window);
            InvalidateRect(window, 0, TRUE);
            return 0;
        }
        if (LOWORD(wParam) == XHL_EDITOR_ID_MODE_COMBO && HIWORD(wParam) == CBN_SELCHANGE) {
            GraphReadPageToState();
            GraphReadTopToState(window);
            GraphRebuildPanel(window);
            return 0;
        }
        if (LOWORD(wParam) == XHL_EDITOR_ID_M1_RUN_SELECTOR && HIWORD(wParam) == CBN_SELCHANGE) {
            GraphReadPageToState();
            g_editorMode1RunIndex = GraphReadComboIndex(g_editorPanel, XHL_EDITOR_ID_M1_RUN_SELECTOR, g_editorMode1RunIndex, XHL_MODE1_RUN_COUNT);
            GraphRebuildPanel(window);
            return 0;
        }
        if (LOWORD(wParam) == XHL_EDITOR_ID_M2_ELEMENT_SELECTOR && HIWORD(wParam) == CBN_SELCHANGE) {
            GraphReadPageToState();
            g_editorMode2ElementIndex = GraphReadComboIndex(g_editorPanel, XHL_EDITOR_ID_M2_ELEMENT_SELECTOR, g_editorMode2ElementIndex, XHL_MODE2_ELEMENT_COUNT);
            GraphRebuildPanel(window);
            return 0;
        }
        if (LOWORD(wParam) == XHL_EDITOR_ID_M2_RUN_SELECTOR && HIWORD(wParam) == CBN_SELCHANGE) {
            GraphReadPageToState();
            g_editorMode2RunIndex = GraphReadComboIndex(g_editorPanel, XHL_EDITOR_ID_M2_RUN_SELECTOR, g_editorMode2RunIndex, XHL_MODE2_BUTTON_RUN_COUNT);
            GraphRebuildPanel(window);
            return 0;
        }
        if (HIWORD(wParam) == CBN_SELCHANGE) {
            if (LOWORD(wParam) == XHL_EDITOR_ID_MODE0_SPLASH_PATH || LOWORD(wParam) == GraphM1RegPathId()) {
                GraphReadPageToState();
                GraphRebuildPanel(window);
                return 0;
            }
            for (UINT i = 0; i < XHL_MODE1_RUN_COUNT; ++i) {
                if (LOWORD(wParam) == GraphM1RunPathId(i)) {
                    GraphReadPageToState();
                    GraphRebuildPanel(window);
                    return 0;
                }
            }
            if (LOWORD(wParam) == GraphM1RegWorkModeId()) {
                GraphReadPageToState();
                GraphRebuildPanel(window);
                return 0;
            }
            for (UINT i = 0; i < XHL_MODE1_RUN_COUNT; ++i) {
                if (LOWORD(wParam) == GraphM1RunWorkModeId(i)) {
                    GraphReadPageToState();
                    GraphRebuildPanel(window);
                    return 0;
                }
            }
            for (UINT i = 0; i < XHL_MODE2_ELEMENT_COUNT; ++i) {
                if (LOWORD(wParam) == GraphM2ElementTypeId(i)) {
                    GraphReadPageToState();
                    GraphRebuildPanel(window);
                    return 0;
                }
                if (LOWORD(wParam) == GraphM2ElementRegPathId(i)
                    || LOWORD(wParam) == GraphM2ElementCsvPathId(i)
                    || LOWORD(wParam) == GraphM2ElementTextFilePathId(i)) {
                    GraphReadPageToState();
                    GraphRebuildPanel(window);
                    return 0;
                }
                if (LOWORD(wParam) == GraphM2ElementRegWorkModeId(i)) {
                    GraphReadPageToState();
                    GraphRebuildPanel(window);
                    return 0;
                }
                for (UINT run = 0; run < XHL_MODE2_BUTTON_RUN_COUNT; ++run) {
                    if (LOWORD(wParam) == GraphM2ElementRunPathId(i, run)) {
                        GraphReadPageToState();
                        GraphRebuildPanel(window);
                        return 0;
                    }
                    if (LOWORD(wParam) == GraphM2ElementRunWorkModeId(i, run)) {
                        GraphReadPageToState();
                        GraphRebuildPanel(window);
                        return 0;
                    }
                }
            }
        }
        break;
    case WM_CLOSE:
        ExitApplication(window, 0);
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLOREDIT: {
        HDC dc = (HDC)wParam;
        SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(dc, GetSysColor(COLOR_WINDOW));
        SetBkMode(dc, OPAQUE);
        return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
    }
    case WM_DESTROY:
        if (g_editorTooltip) {
            DestroyWindow(g_editorTooltip);
            g_editorTooltip = 0;
        }
        if (g_uiFont) {
            DeleteObject(g_uiFont);
            g_uiFont = 0;
        }
        if (g_uiBoldFont) {
            DeleteObject(g_uiBoldFont);
            g_uiBoldFont = 0;
        }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

static int RunGraphicalEditorWindow() {
    GraphLoadEditorState();
    ComposeDisplayTitle(g_editorConfig.title, g_editorConfig.version, g_displayTitle, XHL_MAX_TEXT);

    WNDCLASSEXW panelClass;
    ZeroMemory(&panelClass, sizeof(panelClass));
    panelClass.cbSize = sizeof(panelClass);
    panelClass.lpfnWndProc = GraphPanelProc;
    panelClass.hInstance = GetModuleHandleW(0);
    panelClass.hCursor = LoadCursorW(0, IDC_ARROW);
    panelClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    panelClass.lpszClassName = L"XHLauncherGraphEditorPanel";
    RegisterClassExW(&panelClass);

    WNDCLASSEXW windowClass;
    ZeroMemory(&windowClass, sizeof(windowClass));
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = GraphEditorProc;
    windowClass.hInstance = GetModuleHandleW(0);
    windowClass.hIcon = (HICON)LoadImageW(windowClass.hInstance, MAKEINTRESOURCEW(XHL_APP_ICON_ID), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR | LR_SHARED);
    windowClass.hIconSm = (HICON)LoadImageW(windowClass.hInstance, MAKEINTRESOURCEW(XHL_APP_ICON_ID), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED);
    windowClass.hCursor = LoadCursorW(0, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.lpszClassName = L"XHLauncherGraphEditorWindow";
    if (!RegisterClassExW(&windowClass)) {
        return 1;
    }

    UINT dpi = GetDpi(0);
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = ScaleForDpi(XHL_EDITOR_WINDOW_WIDTH, dpi);
    rect.bottom = ScaleForDpi(XHL_EDITOR_WINDOW_HEIGHT, dpi);
    AdjustWindowRectEx(&rect, style, FALSE, 0);

    HWND window = CreateWindowExW(
        0,
        L"XHLauncherGraphEditorWindow",
        g_displayTitle,
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        0,
        0,
        GetModuleHandleW(0),
        0);
    if (!window) {
        return 1;
    }

    CenterWindowOnMonitor(window);
    ShowWindow(window, SW_SHOWNORMAL);
    UpdateWindow(window);

    MSG message;
    while (GetMessageW(&message, 0, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return (int)message.wParam;
}

static LRESULT CALLBACK Mode2WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        CreateMode2Controls(window);
        LayoutWindow(window, GetDpi(window));
        return 0;
    case XHL_WM_REG_IMPORT_DONE: {
        RegImportTask* task = (RegImportTask*)lParam;
        if (task) {
            RegImportResult result = (RegImportResult)wParam;
            InterlockedExchange(&g_regImportInProgress, 0);
            if (task->button && IsWindow(task->button)) {
                SetWindowTextW(task->button, task->originalText.c_str());
                EnableWindow(task->button, TRUE);
            }
            if (result == RegImportSuccess) {
                ShowAppMessage(window, task->successMessage.c_str(), g_displayTitle, TD_INFORMATION_ICON, MB_ICONINFORMATION);
            } else if (result == RegImportTimeout) {
                std::wstring detailMessage = task->timeoutMessage;
                AppendRegImportDetailMessage(detailMessage, task);
                ShowAppMessage(window, detailMessage.c_str(), g_displayTitle, TD_WARNING_ICON, MB_ICONWARNING);
            } else {
                std::wstring detailMessage = task->failureMessage;
                AppendRegImportDetailMessage(detailMessage, task);
                ShowAppMessage(window, detailMessage.c_str(), g_displayTitle, TD_WARNING_ICON, MB_ICONWARNING);
            }
            delete task;
        }
        return 0;
    }
    case WM_COMMAND:
        if (HIWORD(wParam) == EN_CHANGE) {
            SyncMode2InputVariables();
            return 0;
        }
        if (HIWORD(wParam) == BN_CLICKED) {
            RunMode2ButtonAction(window, LOWORD(wParam));
            return 0;
        }
        break;
    case WM_CLOSE:
        if (InterlockedCompareExchange(&g_regImportInProgress, 0, 0) != 0) {
            AppLanguage language = ReadConfiguredLanguage();
            int choice = ShowAppYesNo(window,
                LocalizedText(language,
                    L"注册表正在导入。\r\n\r\n选择“是”继续等待。\r\n选择“否”关闭窗口并停止导入。", L"註冊表正在導入。\r\n\r\n選擇“是”繼續等待。\r\n選擇“否”關閉窗口並停止導入。",
                    L"Registry import is running.\r\n\r\nChoose Yes to keep waiting.\r\nChoose No to close the window and stop the import."),
                g_displayTitle,
                TD_WARNING_ICON,
                MB_ICONWARNING);
            if (choice == IDYES) {
                return 0;
            }
            CancelActiveRegistryImport();
        }
        ExitApplication(window, 0);
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC: {
        HDC dc = (HDC)wParam;
        SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(dc, GetSysColor(COLOR_WINDOW));
        return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
    }
    case WM_DESTROY:
        if (g_editorTooltip) {
            DestroyWindow(g_editorTooltip);
            g_editorTooltip = 0;
        }
        if (g_uiFont) {
            DeleteObject(g_uiFont);
            g_uiFont = 0;
        }
        if (g_uiBoldFont) {
            DeleteObject(g_uiBoldFont);
            g_uiBoldFont = 0;
        }
        PostQuitMessage(0);
        return 0;
    case WM_DPICHANGED: {
        UINT dpi = HIWORD(wParam);
        RECT* suggestedRect = (RECT*)lParam;
        DWORD style = (DWORD)GetWindowLongPtrW(window, GWL_STYLE);
        DWORD exStyle = (DWORD)GetWindowLongPtrW(window, GWL_EXSTYLE);
        int windowWidth = 0;
        int windowHeight = 0;
        CalculateMode2WindowSize(style, exStyle, dpi, &windowWidth, &windowHeight);
        if (suggestedRect) {
            SetWindowPos(window, 0, suggestedRect->left, suggestedRect->top, windowWidth, windowHeight, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        LayoutWindow(window, dpi);
        return 0;
    }
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

static int RunWindow() {
    WNDCLASSEXW windowClass;
    ZeroMemory(&windowClass, sizeof(windowClass));
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = Mode2WindowProc;
    windowClass.hInstance = GetModuleHandleW(0);
    windowClass.hIcon = (HICON)LoadImageW(windowClass.hInstance, MAKEINTRESOURCEW(XHL_APP_ICON_ID), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR | LR_SHARED);
    windowClass.hIconSm = (HICON)LoadImageW(windowClass.hInstance, MAKEINTRESOURCEW(XHL_APP_ICON_ID), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED);
    windowClass.hCursor = LoadCursorW(0, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.lpszClassName = L"XHLauncherWindow";

    if (!RegisterClassExW(&windowClass)) {
        return 1;
    }

    UINT initialDpi = GetDpi(0);
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    DWORD exStyle = 0;
    int windowWidth = 0;
    int windowHeight = 0;
    LoadMode2Elements();
    CalculateMode2WindowSize(style, exStyle, initialDpi, &windowWidth, &windowHeight);

    HWND window = CreateWindowExW(
        exStyle,
        L"XHLauncherWindow",
        g_displayTitle,
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowWidth,
        windowHeight,
        0,
        0,
        GetModuleHandleW(0),
        0);

    if (!window) {
        return 1;
    }

    CenterWindowOnMonitor(window);
    ShowWindow(window, SW_SHOWNORMAL);
    UpdateWindow(window);

    MSG message;
    while (GetMessageW(&message, 0, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return (int)message.wParam;
}

static int AppMain() {
    SetDefaultDisplayTitle();
    if (!InitContext()) {
        ShowAppMessage(0, L"Failed to get the executable path. The program cannot locate the matching INI file.", g_displayTitle, TD_ERROR_ICON, MB_ICONERROR);
        return 1;
    }

    if (!IsFile(g_ctx.configPath.c_str())) {
        ReleaseConfigIfMissing();
        if (!IsFile(g_ctx.configPath.c_str())) {
            ShowConfigReadFailedMessage();
            return 1;
        }
    }

    if (!LoadIniText()) {
        ShowConfigReadFailedMessage();
        return 1;
    }

    LauncherMode mode = ModeDisabled;
    if (!ValidateLauncherNumericConfig(&mode)) {
        ShowConfigReadFailedMessage();
        return 1;
    }

    if (!ValidateModeNumericConfig(mode)) {
        ShowConfigReadFailedMessage();
        return 1;
    }

    if (mode == ModeSilent) {
        LoadMode1Variables();
    } else if (mode == ModeWindow) {
        LoadMode2Variables();
    }
    ReadDisplayTitle(mode);
    ShowSplashIfConfigured(mode);

    if (mode == ModeDisabled) {
        return RunGraphicalEditorWindow();
    }
    if (mode == ModeSilent) {
        ExecuteMode1Config();
        return 0;
    }

    return RunWindow();
}
int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE previousInstance, _In_ PWSTR commandLine, _In_ int showCommand) {
    (void)instance;
    (void)previousInstance;
    (void)commandLine;
    (void)showCommand;
    InitCommonControlsForApp();
    if (!InitIniBuffers()) {
        return 1;
    }

    int result = AppMain();
    FreeIniBuffers();
    return result;
}



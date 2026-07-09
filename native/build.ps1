param(
    [ValidateSet('x86', 'x64')]
    [string] $Arch = 'x86'
)

$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent $root
$src = Join-Path $root 'src'
$obj = Join-Path $root 'obj'
$publish = Join-Path $root 'publish'

New-Item -ItemType Directory -Force -Path $obj, $publish | Out-Null

function Find-VsDevCmd {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (Test-Path -LiteralPath $vswhere) {
        $installPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if ($LASTEXITCODE -eq 0 -and $installPath) {
            $candidate = Join-Path $installPath 'Common7\Tools\VsDevCmd.bat'
            if (Test-Path -LiteralPath $candidate) {
                return $candidate
            }
        }
    }

    $candidates = @(
        'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat',
        'C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat',
        'C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat',
        'C:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
    )

    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate) {
            return $candidate
        }
    }

    throw 'VsDevCmd.bat was not found. Install Visual Studio 2022 with the C++ build tools.'
}

$vsDevCmd = Find-VsDevCmd

$cpp = Join-Path $src 'XHLauncher.cpp'
$rc = Join-Path $src 'XHLauncher.rc'
$manifest = Join-Path $src 'XHLauncher.manifest'
$res = Join-Path $obj 'XHLauncher.res'
$exe = Join-Path $publish 'XHLauncher.exe'

$rcText = Get-Content -LiteralPath $rc -Raw
$manifestText = Get-Content -LiteralPath $manifest -Raw
if ($rcText -notmatch 'FILEVERSION\s+(\d+),(\d+),(\d+),(\d+)') {
    throw "Could not read FILEVERSION from $rc"
}
$rcVersion = '{0}.{1}.{2}.{3}' -f $Matches[1], $Matches[2], $Matches[3], $Matches[4]
if ($manifestText -notmatch '<assemblyIdentity\s+version="([^"]+)"') {
    throw "Could not read assemblyIdentity version from $manifest"
}
if ($Matches[1] -ne $rcVersion) {
    throw "Manifest version $($Matches[1]) does not match resource version $rcVersion"
}

$cmdPath = Join-Path $obj 'build-native.cmd'
$cmdLines = @(
    '@echo off',
    ('call "{0}" -arch={1} -host_arch=x64' -f $vsDevCmd, $Arch),
    'if errorlevel 1 exit /b %errorlevel%',
    ('rc /nologo /i "{0}" /fo "{1}" "{2}"' -f $projectRoot, $res, $rc),
    'if errorlevel 1 exit /b %errorlevel%',
    ('cl /nologo /utf-8 /W4 /O2 /MT /EHsc /guard:cf /DUNICODE /D_UNICODE "{0}" "{1}" /Fe:"{2}" /link /SUBSYSTEM:WINDOWS /guard:cf /OPT:REF /OPT:ICF /INCREMENTAL:NO kernel32.lib user32.lib gdi32.lib shell32.lib comctl32.lib ole32.lib windowscodecs.lib msimg32.lib uuid.lib' -f $cpp, $res, $exe),
    'exit /b %errorlevel%'
)
Set-Content -LiteralPath $cmdPath -Value $cmdLines -Encoding ASCII

cmd /d /c "`"$cmdPath`""
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

$now = Get-Date
$item = Get-Item -LiteralPath $exe
$item.CreationTime = $now
$item.LastWriteTime = $now
$item.LastAccessTime = $now

Write-Host $exe

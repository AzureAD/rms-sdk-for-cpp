## THIS IS A WIP

## REPO_ROOT\scripts\build_windows.ps1
$REPO_ROOT = Resolve-Path -Path "${Script:PSScriptRoot}\.."
Write-Host ("REPO_ROOT: {0}" -f $REPO_ROOT) -ForegroundColor Yellow

$QtVersion = '5.4.2'

$QT_DIR = 'c:\Qt\Qt{0}\{1}\msvc2013_64' -f $QtVersion, [System.Version]::Parse($QtVersion).ToString(2)
$QT_BIN = '{0}\bin' -f $QT_DIR
$QMAKE  = '{0}\qmake.exe' -f $QT_BIN

Write-Host "=== Adding QT DLLs to Path"
if (($env:Path -split ';') -notcontains $QT_BIN) {
    [System.Environment]::SetEnvironmentVariable('Path', ('{0};{1}' -f $env:Path, $QT_BIN), [System.EnvironmentVariableTarget]::Machine)
}

$QT_TOOLS_BIN = 'C:\Qt\Qt{0}\Tools\QtCreator\bin' -f $QtVersion
$JOM    = '{0}\jom.exe' -f $QT_TOOLS_BIN

$VSVersion = '12.0'
$VC_DIR = 'C:\Program Files (x86)\Microsoft Visual Studio {0}\VC' -f $VSVersion
$VC_BIN_DIR = '{0}\bin' -f $VC_DIR
$VC_INCLUDE_DIR = '{0}\include' -f $VC_DIR
$env:Path += ';{0}' -f $VC_BIN_DIR

$env:INCLUDE = '{0};{1}' -f $VC_INCLUDE_DIR,'C:\Program Files (x86)\Windows Kits\8.1\Include\shared;C:\Program Files (x86)\Windows Kits\8.1\Include\um'
$env:LIB     = "${REPO_ROOT}\third_party\lib\eay;C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x64;C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib;${QT_DIR}\lib"


#region InstallChoco
$ChocoInstallVariableName = "ChocolateyInstall"
$ChocoPath = [Environment]::GetEnvironmentVariable($ChocoInstallVariableName, [System.EnvironmentVariableTarget]::User)
$ChocoExePath = 'C:\ProgramData\Chocolatey\bin'
if ($ChocoPath) {
  $ChocoExePath = Join-Path $ChocoPath 'bin'
}

if (!(Test-Path $ChocoExePath)) {
    Write-Verbose -Message '=== Installing Chocolatey'
    Invoke-Expression (Invoke-WebRequest -Uri 'https://chocolatey.org/install.ps1' | % Content)
} else {
    Write-Verbose -Message ('=== Choco available at {0}' -f $ChocoExePath)
}
#endregion

function Test-ChocoPackageInstall {
    param($PackageName)

    $output = choco list $PackageName --local-only
    if ($output -notmatch '0 packages installed.') {
        return $true
    }

    return $false
}

if (-not (Test-ChocoPackageInstall openssl.light)) {
    Write-Verbose -Message '=== Installing OpenSSL'
    choco install openssl.light --force -y
}

cd "${REPO_ROOT}\sdk"
& $QMAKE
& $JOM

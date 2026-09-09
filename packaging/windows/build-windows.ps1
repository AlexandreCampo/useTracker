<#
.SYNOPSIS
    Build useTracker on Windows and package it as a .zip (and an NSIS installer
    when makensis is available).

.DESCRIPTION
    Dependencies come from vcpkg. If VCPKG_ROOT is not set, vcpkg is cloned into
    build/vcpkg and bootstrapped. The first build compiles OpenCV from source and
    takes a long time (roughly an hour); later builds reuse the vcpkg binary cache.

    The resulting .zip contains useTracker.exe with every DLL beside it, so it
    runs on a machine with nothing else installed.

.EXAMPLE
    .\packaging\windows\build-windows.ps1
    .\packaging\windows\build-windows.ps1 -BuildType RelWithDebInfo
#>
[CmdletBinding()]
param(
    [ValidateSet('Release', 'RelWithDebInfo', 'Debug', 'MinSizeRel')]
    [string]$BuildType = 'Release',

    [string]$BuildDir = 'build-windows',
    [string]$DistDir  = 'dist',
    [string]$Triplet  = 'x64-windows'
)

$ErrorActionPreference = 'Stop'

$root = Resolve-Path (Join-Path $PSScriptRoot '..\..')
Set-Location $root

function Require-Command($name, $hint) {
    if (-not (Get-Command $name -ErrorAction SilentlyContinue)) {
        throw "$name was not found in PATH. $hint"
    }
}

Require-Command cmake 'Install it from https://cmake.org/download/ or with: winget install Kitware.CMake'
Require-Command git   'Install it with: winget install Git.Git'

# --- vcpkg ------------------------------------------------------------------
$vcpkgRoot = $env:VCPKG_ROOT
if (-not $vcpkgRoot -or -not (Test-Path (Join-Path $vcpkgRoot 'vcpkg.exe'))) {
    $vcpkgRoot = Join-Path $root "$BuildDir\vcpkg"
    if (-not (Test-Path $vcpkgRoot)) {
        Write-Host '>> cloning vcpkg'
        New-Item -ItemType Directory -Force -Path (Split-Path $vcpkgRoot) | Out-Null
        git clone --depth 1 https://github.com/microsoft/vcpkg.git $vcpkgRoot
    }
    if (-not (Test-Path (Join-Path $vcpkgRoot 'vcpkg.exe'))) {
        Write-Host '>> bootstrapping vcpkg'
        & (Join-Path $vcpkgRoot 'bootstrap-vcpkg.bat') -disableMetrics
    }
}
Write-Host ">> vcpkg: $vcpkgRoot"

$toolchain = Join-Path $vcpkgRoot 'scripts\buildsystems\vcpkg.cmake'

# --- configure and build ----------------------------------------------------
Write-Host ">> configuring ($BuildType, $Triplet)"
cmake -S . -B $BuildDir `
    -DCMAKE_TOOLCHAIN_FILE="$toolchain" `
    -DVCPKG_TARGET_TRIPLET=$Triplet `
    -DX_VCPKG_APPLOCAL_DEPS_INSTALL=ON `
    -DCMAKE_BUILD_TYPE=$BuildType
if ($LASTEXITCODE -ne 0) { throw 'cmake configure failed' }

Write-Host '>> building'
cmake --build $BuildDir --config $BuildType --parallel
if ($LASTEXITCODE -ne 0) { throw 'build failed' }

# --- package ----------------------------------------------------------------
Write-Host '>> packaging'
New-Item -ItemType Directory -Force -Path $DistDir | Out-Null
Push-Location $BuildDir
try {
    cpack -C $BuildType
    if ($LASTEXITCODE -ne 0) { throw 'cpack failed' }
} finally {
    Pop-Location
}

Get-ChildItem -Path $BuildDir -File |
    Where-Object { $_.Extension -in '.zip', '.exe' -and $_.Name -like 'useTracker-*' } |
    ForEach-Object { Move-Item -Force $_.FullName $DistDir }

Write-Host ''
Write-Host '>> artifacts:'
Get-ChildItem $DistDir | Select-Object -ExpandProperty Name

param(
    [ValidateSet("debug", "release")]
    [string]$Preset = "debug"
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $root "build\$Preset"

if (Test-Path $buildDir) {
    Write-Host "Borrando $buildDir..."
    Remove-Item -Recurse -Force $buildDir
}

cmake --preset $Preset
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

cmake --build --preset $Preset
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$exe = Join-Path $buildDir "hsecs_sandbox.exe"
if (-not (Test-Path $exe)) {
    Write-Error "No se encontro el ejecutable: $exe"
    exit 1
}

Write-Host "`n--- Ejecutando $exe ---`n"
& $exe
exit $LASTEXITCODE

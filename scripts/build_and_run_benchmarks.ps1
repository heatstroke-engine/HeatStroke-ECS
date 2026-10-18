param(
    # Number of times each benchmark is repeated (mean, median and stddev are reported)
    [int]$Repetitions = 10,
    # Regex to run only some benchmarks, e.g. "CREATE"
    [string]$Filter = "."
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot

# Benchmarks are always measured in release: debug has asserts, log and no optimizations.
# The build folder is not deleted, so Google Benchmark is not downloaded again every time.
$preset = "release"
$buildDir = Join-Path $root "build\$preset"

cmake --preset $preset
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

cmake --build --preset $preset
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$exe = Join-Path $buildDir "hsecs_benchmarks.exe"
if (-not (Test-Path $exe)) {
    Write-Error "No se encontro el ejecutable: $exe"
    exit 1
}

# One json per execution, named like the logs: <dd-mm-YYYY-HHh-MMm-SSs>_benchmark.json
$resultsDir = Join-Path $root "benchmarks\results"
New-Item -ItemType Directory -Force -Path $resultsDir | Out-Null

$date = Get-Date -Format "dd-MM-yyyy-HH'h'-mm'm'-ss's'"
$outFile = Join-Path $resultsDir "${date}_benchmark.json"

Write-Host "`n--- Ejecutando $exe ---"
Write-Host "--- Resultados en $outFile ---`n"

# Google Benchmark writes its header (date, CPU...) to stderr; it is not an error
$ErrorActionPreference = "Continue"

& $exe `
    --benchmark_out="$outFile" `
    --benchmark_out_format=json `
    --benchmark_repetitions=$Repetitions `
    --benchmark_report_aggregates_only=true `
    --benchmark_filter="$Filter"
exit $LASTEXITCODE

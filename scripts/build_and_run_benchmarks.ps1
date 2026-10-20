param(
    # Which ECS to benchmark: hsecs (ours) or entt
    [ValidateSet("hsecs", "entt")]
    [string]$Ecs = "hsecs",
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
$target = "${Ecs}_benchmarks"

cmake --preset $preset
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# Only the chosen target, so an error in the other one does not stop this one
cmake --build --preset $preset --target $target
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$exe = Join-Path $buildDir "$target.exe"
if (-not (Test-Path $exe)) {
    Write-Error "No se encontro el ejecutable: $exe"
    exit 1
}

# One json per execution, named like the logs: <dd-mm-YYYY-HHh-MMm-SSs>_<ecs>_benchmark.json
$resultsDir = Join-Path $root "benchmarks\results"
New-Item -ItemType Directory -Force -Path $resultsDir | Out-Null

$date = Get-Date -Format "dd-MM-yyyy-HH'h'-mm'm'-ss's'"
$outFile = Join-Path $resultsDir "${date}_${Ecs}_benchmark.json"

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

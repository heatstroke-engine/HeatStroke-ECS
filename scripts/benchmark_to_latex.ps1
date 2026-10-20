param(
    # Google Benchmark json (run with --benchmark_repetitions and aggregates)
    [Parameter(Mandatory = $true)]
    [string]$Json,
    # Output .tex. By default, next to the json with the same name
    [string]$Out = ""
)

$ErrorActionPreference = "Stop"
$inv = [System.Globalization.CultureInfo]::InvariantCulture

if ($Out -eq "") { $Out = [System.IO.Path]::ChangeExtension($Json, ".tex") }

$data = Get-Content $Json -Raw -Encoding UTF8 | ConvertFrom-Json

# Description of each test, shown above its table
$descriptions = @{
    "BM_CREATEENTITES"         = "Crear N entidades vacías en un manager vacío. La construcción del manager no se mide."
    "BM_CREATE1CMP"            = "Añadir 1 componente (\texttt{CPhysics}) a N entidades vacías."
    "BM_CREATE3CMPS"           = "Añadir 3 componentes (\texttt{CPhysics}, \texttt{CRender}, \texttt{CTransform}) a N entidades vacías con \texttt{addComponents}."
    "BM_CREATE5CMPS"           = "Añadir 5 componentes (\texttt{CPhysics}, \texttt{CRender}, \texttt{CTransform}, \texttt{CGeneric<0>}, \texttt{CGeneric<1>}) a N entidades vacías."
    "BM_ERASEENTITYW1CMP"      = "Destruir N entidades con 1 componente. Sin sistemas registrados."
    "BM_ERASEENTITYW3CMPS"     = "Destruir N entidades con 3 componentes. Sin sistemas registrados."
    "BM_ERASEENTITYW5CMPS"     = "Destruir N entidades con 5 componentes. Sin sistemas registrados."
    "BM_ERASECMP"              = "Quitar 1 componente (\texttt{CPhysics}) a N entidades. Sin sistemas registrados."
    "BM_ERASECMPS"             = "Quitar 2 componentes (\texttt{CPhysics}, \texttt{CTransform}) a N entidades con \texttt{removeComponents}. Sin sistemas registrados."
    "BM_DESTROYENTITIES"       = "Destruir N entidades vacías. Sin sistemas registrados."
    "BM_GETCMP"                = "Acceder y escribir \texttt{CPhysics} de N entidades con \texttt{getComponent}, en orden de creación."
    "BM_ITERATESINGLECMP"      = "\texttt{RenderSystem} (1 componente) sobre N entidades que cumplen su firma."
    "BM_ITERATETWOCMPS"        = "\texttt{PhysicsSystem} (2 componentes) sobre N entidades que cumplen su firma."
    "BM_ITERATETWOCMPSHALF"    = "\texttt{PhysicsSystem} sobre N entidades de las que solo la mitad cumple su firma. El throughput cuenta las N/2 entidades recorridas."
    "BM_ADDREMOVECMP"          = "Añadir y quitar \texttt{CHealth} a N entidades con sistemas registrados, sin que cambie ninguna caché. Cada entidad cuenta como 2 operaciones."
    "BM_ADDREMOVECMPCACHED"    = "Añadir y quitar \texttt{CHealth} a N entidades: cada una entra y sale de la caché de \texttt{HealthSystem}. Cada entidad cuenta como 2 operaciones."
    "BM_REGISTERSYSTEM"        = "Registrar \texttt{PhysicsSystem} con N entidades ya creadas que cumplen su firma (crear y rellenar la caché)."
    "BM_DESTROYENTITIESCACHED" = "Destruir N entidades que están en las cachés de dos sistemas (\texttt{PhysicsSystem} y \texttt{HealthSystem})."
    "BM_GAMESCENARIO"          = "Un frame con \texttt{PhysicsSystem}, \texttt{HealthSystem} y \texttt{RenderSystem} sobre un mundo heterogéneo: 50\,\% unidades, 30\,\% decorado y 20\,\% proyectiles."
}

function Escape-Latex([string]$s) { return $s.Replace('\', '\textbackslash{}').Replace('_', '\_').Replace('%', '\%').Replace('&', '\&').Replace('#', '\#') }

# Time in ns -> the most readable unit
function Format-Time([double]$ns) {
    if ($ns -ge 1e9) { return ($ns / 1e9).ToString("0.000", $inv) + " s" }
    if ($ns -ge 1e6) { return ($ns / 1e6).ToString("0.000", $inv) + " ms" }
    if ($ns -ge 1e3) { return ($ns / 1e3).ToString("0.000", $inv) + " \textmu s" }
    return $ns.ToString("0.0", $inv) + " ns"
}

# Items per second -> k/s, M/s, G/s
function Format-Rate([double]$r) {
    if ($r -ge 1e9) { return ($r / 1e9).ToString("0.00", $inv) + " G/s" }
    if ($r -ge 1e6) { return ($r / 1e6).ToString("0.00", $inv) + " M/s" }
    if ($r -ge 1e3) { return ($r / 1e3).ToString("0.00", $inv) + " k/s" }
    return $r.ToString("0.00", $inv) + " /s"
}

# Group the aggregates (mean, median, stddev, cv) of each run
$runs = @{}
foreach ($b in $data.benchmarks) {
    if ($b.run_type -ne "aggregate") { continue }
    if (-not $runs.ContainsKey($b.run_name)) {
        $parts = $b.run_name.Split('/')
        $runs[$b.run_name] = [ordered]@{
            family   = [int]$b.family_index
            test     = $parts[0]
            entities = [long]$parts[1]
            reps     = $b.repetitions
        }
    }
    $runs[$b.run_name][$b.aggregate_name] = $b
}

$ctx = $data.context
$cpuGHz = ([double]$ctx.mhz_per_cpu / 1000).ToString("0.00", $inv)
$cacheLines = ($ctx.caches | ForEach-Object {
    $size = if ($_.size -ge 1MB) { "{0} MiB" -f ($_.size / 1MB) } else { "{0} KiB" -f ($_.size / 1KB) }
    "L$($_.level) $($_.type): $size"
}) -join ", "
$reps = ($runs.Values | Select-Object -First 1).reps

$sb = New-Object System.Text.StringBuilder
[void]$sb.AppendLine('\documentclass[11pt,a4paper]{article}')
[void]$sb.AppendLine('\usepackage[utf8]{inputenc}')
[void]$sb.AppendLine('\usepackage[T1]{fontenc}')
[void]$sb.AppendLine('\usepackage[spanish,es-nodecimaldot]{babel}')
[void]$sb.AppendLine('\usepackage[margin=2.2cm]{geometry}')
[void]$sb.AppendLine('\usepackage{booktabs}')
[void]$sb.AppendLine('\usepackage{textcomp}')
[void]$sb.AppendLine('\usepackage{float}')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('\title{HeatStroke ECS dummy\\Resultados de los benchmarks}')
[void]$sb.AppendLine('\date{' + ([datetime]$ctx.date).ToString("dd/MM/yyyy HH:mm", $inv) + '}')
[void]$sb.AppendLine('\author{}')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('\begin{document}')
[void]$sb.AppendLine('\maketitle')
[void]$sb.AppendLine('\sloppy % long \texttt{} names can break lines')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('\section*{Entorno}')
[void]$sb.AppendLine('\begin{itemize}')
[void]$sb.AppendLine("  \item CPU: $($ctx.num_cpus) hilos l\'ogicos a $cpuGHz~GHz.")
[void]$sb.AppendLine("  \item Cach\'es: $cacheLines.")
[void]$sb.AppendLine("  \item Google Benchmark $($ctx.library_version), compilado en $($ctx.library_build_type).")
[void]$sb.AppendLine("  \item Cada test se repite $reps veces. Se muestran la media, la mediana, la desviaci\'on t\'ipica y el coeficiente de variaci\'on (CV) del tiempo real, y el throughput medio.")
[void]$sb.AppendLine('\end{itemize}')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('\section*{Resultados}')

$families = $runs.Values | Group-Object { $_.family } | Sort-Object { [int]$_.Name }
foreach ($family in $families) {
    $rows = $family.Group | Sort-Object { $_.entities }
    $test = $rows[0].test
    [void]$sb.AppendLine('')
    [void]$sb.AppendLine('\subsection*{\texttt{' + (Escape-Latex $test) + '}}')
    if ($descriptions.ContainsKey($test)) { [void]$sb.AppendLine($descriptions[$test]) }
    [void]$sb.AppendLine('\begin{table}[H]')
    [void]$sb.AppendLine('\centering')
    [void]$sb.AppendLine('\begin{tabular}{rrrrrr}')
    [void]$sb.AppendLine('\toprule')
    [void]$sb.AppendLine('Entidades & Media & Mediana & Desv. t\''ipica & CV & Throughput \\')
    [void]$sb.AppendLine('\midrule')
    foreach ($r in $rows) {
        $entities = $r.entities.ToString("N0", $inv).Replace(",", "\,")
        $cv = ([double]$r.cv.real_time * 100).ToString("0.00", $inv) + "\,\%"
        [void]$sb.AppendLine("$entities & $(Format-Time $r.mean.real_time) & $(Format-Time $r.median.real_time) & $(Format-Time $r.stddev.real_time) & $cv & $(Format-Rate $r.mean.items_per_second) \\")
    }
    [void]$sb.AppendLine('\bottomrule')
    [void]$sb.AppendLine('\end{tabular}')
    [void]$sb.AppendLine('\end{table}')
}

[void]$sb.AppendLine('')
[void]$sb.AppendLine('\end{document}')

[System.IO.File]::WriteAllText((Resolve-Path -LiteralPath (Split-Path -Parent $Out)).Path + "\" + (Split-Path -Leaf $Out), $sb.ToString(), (New-Object System.Text.UTF8Encoding($false)))
Write-Host "LaTeX generado en $Out"

param(
    # Google Benchmark json of HeatStroke ECS (run with --benchmark_repetitions and aggregates)
    [Parameter(Mandatory = $true)]
    [string]$HsJson,
    # Google Benchmark json of EnTT, same tests and sizes
    [Parameter(Mandatory = $true)]
    [string]$EnttJson,
    # Output .tex. By default, benchmarks/results/<date of the EnTT json>_comparison.tex
    [string]$Out = ""
)

$ErrorActionPreference = "Stop"
$inv = [System.Globalization.CultureInfo]::InvariantCulture

if ($Out -eq "") {
    $name = (Split-Path -Leaf $EnttJson) -replace '_entt_benchmark\.json$', '_comparison.tex'
    $Out = Join-Path (Split-Path -Parent $EnttJson) $name
}

# Description of each test. In EnTT a registered system is an owning group, a tag an empty component
$descriptions = @{
    "BM_CREATEENTITES"         = "Crear N entidades vacías en un mundo vacío. La construcción del mundo no se mide."
    "BM_CREATE1CMP"            = "Añadir 1 componente (\texttt{CPhysics}) a N entidades vacías."
    "BM_CREATE3CMPS"           = "Añadir 3 componentes (\texttt{CPhysics}, \texttt{CRender}, \texttt{CTransform}) a N entidades vacías."
    "BM_CREATE5CMPS"           = "Añadir 5 componentes (\texttt{CPhysics}, \texttt{CRender}, \texttt{CTransform}, \texttt{CGeneric<0>}, \texttt{CGeneric<1>}) a N entidades vacías."
    "BM_ERASEENTITYW1CMP"      = "Destruir N entidades con 1 componente. Sin sistemas registrados."
    "BM_ERASEENTITYW3CMPS"     = "Destruir N entidades con 3 componentes. Sin sistemas registrados."
    "BM_ERASEENTITYW5CMPS"     = "Destruir N entidades con 5 componentes. Sin sistemas registrados."
    "BM_ERASECMP"              = "Quitar 1 componente (\texttt{CPhysics}) a N entidades. Sin sistemas registrados."
    "BM_ERASECMPS"             = "Quitar 2 componentes (\texttt{CPhysics}, \texttt{CTransform}) a N entidades. Sin sistemas registrados."
    "BM_DESTROYENTITIES"       = "Destruir N entidades vacías. Sin sistemas registrados."
    "BM_GETCMP"                = "Acceder y escribir \texttt{CPhysics} de N entidades por su identificador, en orden de creación."
    "BM_ITERATESINGLECMP"      = "\texttt{RenderSystem} (1 componente) sobre N entidades que cumplen su firma."
    "BM_ITERATETWOCMPS"        = "\texttt{PhysicsSystem} (2 componentes) sobre N entidades que cumplen su firma."
    "BM_ITERATETWOCMPSHALF"    = "\texttt{PhysicsSystem} sobre N entidades de las que solo la mitad cumple su firma."
    "BM_ADDREMOVECMP"          = "Añadir y quitar \texttt{CHealth} a N entidades con sistemas registrados, sin que cambie ninguna caché ni group."
    "BM_ADDREMOVECMPCACHED"    = "Añadir y quitar \texttt{CHealth} a N entidades: cada una entra y sale de la caché (group en EnTT) de \texttt{HealthSystem}."
    "BM_REGISTERSYSTEM"        = "Registrar \texttt{PhysicsSystem} con N entidades ya creadas que cumplen su firma. En EnTT, crear su group."
    "BM_DESTROYENTITIESCACHED" = "Destruir N entidades que están en las cachés (groups en EnTT) de \texttt{PhysicsSystem} y \texttt{HealthSystem}."
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

function Format-Cv([double]$cv) { return ($cv * 100).ToString("0.0", $inv) + "\,\%" }

# Ratio t_HS / t_EnTT -> "x2.35" (greater than 1: HeatStroke is slower)
function Format-Ratio([double]$r) {
    if ($r -ge 100) { return '$\times$' + $r.ToString("0", $inv) }
    if ($r -ge 10) { return '$\times$' + $r.ToString("0.0", $inv) }
    return '$\times$' + $r.ToString("0.00", $inv)
}

function Bold-If([string]$s, [bool]$cond) { if ($cond) { return '\textbf{' + $s + '}' } return $s }

function Num([double]$x) { return $x.ToString("G6", $inv) }

$unitToNs = @{ "ns" = 1.0; "us" = 1e3; "ms" = 1e6; "s" = 1e9 }

# json -> @{ "test/entities" = run } with the aggregates in ns
function Load-Runs([string]$path) {
    $data = Get-Content $path -Raw -Encoding UTF8 | ConvertFrom-Json
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
        $k = $unitToNs[$b.time_unit]
        switch ($b.aggregate_name) {
            "mean"   { $runs[$b.run_name].mean = [double]$b.real_time * $k; $runs[$b.run_name].ips = [double]$b.items_per_second }
            "median" { $runs[$b.run_name].median = [double]$b.real_time * $k }
            "stddev" { $runs[$b.run_name].stddev = [double]$b.real_time * $k }
            "cv"     { $runs[$b.run_name].cv = [double]$b.real_time }
        }
    }
    return @{ context = $data.context; runs = $runs }
}

$hs = Load-Runs $HsJson
$en = Load-Runs $EnttJson

# Tests in the order of the HeatStroke executable. Only those that exist in both
$tests = $hs.runs.Values | Sort-Object { $_.family } | ForEach-Object { $_.test } | Select-Object -Unique |
    Where-Object { $t = $_; @($en.runs.Values | Where-Object { $_.test -eq $t }).Count -gt 0 }

function Get-Rows($runs, [string]$test) { return @($runs.Values | Where-Object { $_.test -eq $test } | Sort-Object { $_.entities }) }
function Get-Run($runs, [string]$test, [long]$n) { return $runs["$test/$n"] }

# Largest size measured by both ECS for each test, used in the summary
$summary = foreach ($t in $tests) {
    $common = @(Get-Rows $hs.runs $t | Where-Object { $en.runs.ContainsKey("$t/$($_.entities)") })
    if ($common.Count -eq 0) { continue }
    $n = $common[-1].entities
    $h = Get-Run $hs.runs $t $n
    $e = Get-Run $en.runs $t $n
    $v = Get-Run $en.runs "${t}_VIEW" $n
    [pscustomobject]@{ test = $t; n = $n; hs = $h.mean; entt = $e.mean; view = $(if ($v) { $v.mean } else { $null }); ratio = $h.mean / $e.mean }
}
$hsWins = @($summary | Where-Object { $_.ratio -lt 1 }).Count
$enWins = @($summary | Where-Object { $_.ratio -ge 1 }).Count

function Format-Entities([long]$n) { return $n.ToString("N0", $inv).Replace(",", "\,") }
function Short-Name([string]$t) { return $t -replace '^BM_', '' }

$sb = New-Object System.Text.StringBuilder
function L([string]$s = "") { [void]$sb.AppendLine($s) }

L '\documentclass[11pt,a4paper]{article}'
L '\usepackage[utf8]{inputenc}'
L '\usepackage[T1]{fontenc}'
L '\usepackage[spanish,es-nodecimaldot]{babel}'
L '\usepackage[margin=2.2cm]{geometry}'
L '\usepackage{booktabs}'
L '\usepackage{textcomp}'
L '\usepackage{float}'
L '\usepackage{pgfplots}'
L '\usepgfplotslibrary{groupplots}'
L '\pgfplotsset{compat=1.18}'
L ''
L '% Same style for each ECS in every chart'
L '\definecolor{hscolor}{RGB}{214,96,26}'
L '\definecolor{enttcolor}{RGB}{38,110,180}'
L '\pgfplotsset{'
L '  hs/.style={color=hscolor, mark=*, thick, mark size=1.8pt},'
L '  entt/.style={color=enttcolor, mark=square*, thick, mark size=1.6pt},'
L '  enttview/.style={color=enttcolor, mark=triangle*, thick, dashed, mark size=2pt},'
L '}'
L ''
L '\title{HeatStroke ECS dummy frente a EnTT\\Comparativa de los benchmarks}'
L ('\date{' + (Get-Date).ToString("dd/MM/yyyy", $inv) + '}')
L '\author{}'
L ''
L '\begin{document}'
L '\maketitle'
L '\sloppy % long \texttt{} names can break lines'
L ''

# --- Environment
$ctx = $hs.context
$cpuGHz = ([double]$ctx.mhz_per_cpu / 1000).ToString("0.00", $inv)
$cacheLines = ($ctx.caches | ForEach-Object {
    $size = if ($_.size -ge 1MB) { "{0} MiB" -f ($_.size / 1MB) } else { "{0} KiB" -f ($_.size / 1KB) }
    "L$($_.level) $($_.type): $size"
}) -join ", "
$hsReps = ($hs.runs.Values | Select-Object -First 1).reps
$enReps = ($en.runs.Values | Select-Object -First 1).reps
$hsDate = ([datetime]$hs.context.date).ToString("dd/MM/yyyy HH:mm", $inv)
$enDate = ([datetime]$en.context.date).ToString("dd/MM/yyyy HH:mm", $inv)

L '\section*{Entorno}'
L '\begin{itemize}'
L "  \item CPU: $($ctx.num_cpus) hilos lógicos a $cpuGHz~GHz."
L "  \item Cachés: $cacheLines."
L "  \item Google Benchmark $($ctx.library_version), compilado en $($ctx.library_build_type). Ambos ejecutables se compilan en release con el mismo compilador, las mismas opciones y el runtime de C++ enlazado de forma estática."
L "  \item HeatStroke ECS: ejecución del $hsDate, $hsReps repeticiones por test."
L "  \item EnTT v4.0.0: ejecución del $enDate, $enReps repeticiones por test."
L '  \item Todos los tiempos son la media del tiempo real de las repeticiones. El CV es el coeficiente de variación (desviación típica / media).'
L '\end{itemize}'
L ''

# --- Methodology
L '\section*{Cómo se comparan}'
L 'Los dos ejecutables tienen los mismos tests, con los mismos nombres, componentes, tamaños y preparación del mundo. Las equivalencias son:'
L '\begin{itemize}'
L '  \item Un \emph{tag} de HeatStroke es un componente vacío en EnTT.'
L '  \item Un sistema registrado en HeatStroke guarda una caché con las entidades que cumplen su firma y la actualiza en cada cambio estructural. En EnTT lo equivalente es un \emph{owning group}: registrar un sistema es crear su group, y su \texttt{update} recorre ese group.'
L '  \item Además, en EnTT se miden los tests de iteración también con \emph{views} (sufijo \texttt{\_VIEW}), que no guardan nada y son la forma habitual de iterar en EnTT. No tienen equivalente en HeatStroke.'
L '\end{itemize}'
L 'La columna \textbf{HS/EnTT} es el tiempo de HeatStroke dividido entre el de EnTT: mayor que 1 significa que HeatStroke es más lento ese número de veces, y menor que 1 que es más rápido. En cada fila aparece en negrita el tiempo del más rápido.'
L ''

# --- Summary
L '\section*{Resumen}'
L "Comparación con el mayor tamaño medido por ambos. HeatStroke es más rápido en $hsWins de $($summary.Count) tests y EnTT en $enWins."
L '\begin{table}[H]'
L '\centering'
L '\small'
L '\begin{tabular}{lrrrr}'
L '\toprule'
L 'Test & Entidades & HeatStroke & EnTT & HS/EnTT \\'
L '\midrule'
foreach ($s in $summary) {
    $hsT = Bold-If (Format-Time $s.hs) ($s.hs -le $s.entt)
    $enT = Bold-If (Format-Time $s.entt) ($s.entt -lt $s.hs)
    L "\texttt{$(Escape-Latex (Short-Name $s.test))} & $(Format-Entities $s.n) & $hsT & $enT & $(Format-Ratio $s.ratio) \\"
}
L '\bottomrule'
L '\end{tabular}'
L '\end{table}'
L ''

# --- Charts
L '\section*{Gráficos}'
L '\subsection*{Relación de tiempos}'
L 'Cada barra es HS/EnTT con el mayor tamaño común, en escala logarítmica. Las barras a la derecha de la línea son tests en los que HeatStroke es más lento; las de la izquierda, tests en los que es más rápido.'
L '\begin{figure}[H]'
L '\centering'
L '\begin{tikzpicture}'
L '\begin{axis}['
L '  xbar, xmode=log, log origin=0, log basis x=10,'
L "  width=0.85\textwidth, height=$(0.62 * $summary.Count + 1.5)cm,"
L '  bar width=7pt, y dir=reverse,'
L "  ytick={0,...,$($summary.Count - 1)},"
L ('  yticklabels={' + (($summary | ForEach-Object { '\texttt{' + (Escape-Latex (Short-Name $_.test)) + '}' }) -join ',') + '},')
L '  yticklabel style={font=\scriptsize},'
L '  xlabel={HS/EnTT (escala logarítmica)},'
L '  xmajorgrids, grid style={gray!25},'
L '  enlarge y limits=0.03,'
L '  nodes near coords={\pgfmathprintnumber[fixed,precision=2]{\pgfplotspointmeta}},'
L '  point meta=rawx, nodes near coords style={font=\tiny},'
L ']'
L '\addplot[fill=hscolor!70, draw=hscolor] coordinates {'
$i = 0
foreach ($s in $summary) { L "  ($(Num $s.ratio),$i)"; $i++ }
L '};'
L ('\draw[black, thick] (axis cs:1,-1) -- (axis cs:1,' + $summary.Count + ');')
L '\end{axis}'
L '\end{tikzpicture}'
L '\end{figure}'
L ''

L '\subsection*{Escalado por test}'
L 'Tiempo medio por entidad (tiempo total / N) frente al número de entidades, ambos en escala logarítmica. Una línea horizontal significa coste lineal; una línea que sube, coste peor que lineal.'
L ''
L '\begin{center}'
L '\begin{tikzpicture}'
L '\begin{axis}[hide axis, xmin=0, xmax=1, ymin=0, ymax=1, width=2cm, height=2cm,'
L '  legend columns=3, legend style={draw=none, font=\small, column sep=8pt}]'
L '\addlegendimage{hs}\addlegendentry{HeatStroke}'
L '\addlegendimage{entt}\addlegendentry{EnTT (group)}'
L '\addlegendimage{enttview}\addlegendentry{EnTT (view)}'
L '\end{axis}'
L '\end{tikzpicture}'
L '\end{center}'

function Add-Series([string]$style, $rows) {
    if ($rows.Count -eq 0) { return }
    L "\addplot[$style] coordinates {"
    foreach ($r in $rows) { L "  ($($r.entities),$(Num ($r.mean / $r.entities)))" }
    L '};'
}

# Two charts per row
for ($i = 0; $i -lt $tests.Count; $i += 2) {
    $pair = @($tests[$i..([Math]::Min($i + 1, $tests.Count - 1))])
    L '\begin{figure}[H]'
    L '\centering'
    L '\begin{tikzpicture}'
    L '\begin{groupplot}['
    L "  group style={group size=$($pair.Count) by 1, horizontal sep=1.6cm},"
    L '  width=0.47\textwidth, height=5.2cm,'
    L '  xmode=log, ymode=log, log basis x=10,'
    L '  xlabel={Entidades}, ylabel={ns / entidad},'
    L '  title style={font=\small}, tick label style={font=\scriptsize}, label style={font=\scriptsize},'
    L '  grid=major, grid style={gray!25},'
    L ']'
    foreach ($t in $pair) {
        L "\nextgroupplot[title={\texttt{$(Escape-Latex (Short-Name $t))}}]"
        Add-Series 'hs' (Get-Rows $hs.runs $t)
        Add-Series 'entt' (Get-Rows $en.runs $t)
        Add-Series 'enttview' (Get-Rows $en.runs "${t}_VIEW")
    }
    L '\end{groupplot}'
    L '\end{tikzpicture}'
    L '\end{figure}'
}
L ''

# --- Detailed tables
L '\section*{Resultados detallados}'
foreach ($t in $tests) {
    $hasView = @(Get-Rows $en.runs "${t}_VIEW").Count -gt 0
    L ''
    L ('\subsection*{\texttt{' + (Escape-Latex $t) + '}}')
    if ($descriptions.ContainsKey($t)) { L $descriptions[$t] }
    L '\begin{table}[H]'
    L '\centering'
    L '\small'
    if ($hasView) {
        L '\begin{tabular}{rrrrrrrrr}'
        L '\toprule'
        L ' & \multicolumn{2}{c}{HeatStroke} & \multicolumn{3}{c}{EnTT (group)} & \multicolumn{3}{c}{EnTT (view)} \\'
        L '\cmidrule(lr){2-3}\cmidrule(lr){4-6}\cmidrule(lr){7-9}'
        L 'Entidades & Media & CV & Media & CV & HS/EnTT & Media & CV & HS/EnTT \\'
    } else {
        L '\begin{tabular}{rrrrrr}'
        L '\toprule'
        L ' & \multicolumn{2}{c}{HeatStroke} & \multicolumn{3}{c}{EnTT} \\'
        L '\cmidrule(lr){2-3}\cmidrule(lr){4-6}'
        L 'Entidades & Media & CV & Media & CV & HS/EnTT \\'
    }
    L '\midrule'
    # All sizes measured by any of them; "---" where one has no result
    $sizes = @((Get-Rows $hs.runs $t) + (Get-Rows $en.runs $t) | ForEach-Object { $_.entities } | Sort-Object -Unique)
    foreach ($n in $sizes) {
        $h = Get-Run $hs.runs $t $n
        $e = Get-Run $en.runs $t $n
        $v = Get-Run $en.runs "${t}_VIEW" $n
        $times = @($h, $e, $v) | Where-Object { $_ } | ForEach-Object { $_.mean }
        $best = ($times | Measure-Object -Minimum).Minimum

        $cells = @(Format-Entities $n)
        $cols = if ($hasView) { @($h, $e, $v) } else { @($h, $e) }
        for ($c = 0; $c -lt $cols.Count; $c++) {
            $r = $cols[$c]
            if ($r) {
                $cells += Bold-If (Format-Time $r.mean) ($r.mean -eq $best -and $times.Count -gt 1)
                $cells += Format-Cv $r.cv
                # EnTT columns also show HS/EnTT
                if ($c -gt 0) { $cells += $(if ($h) { Format-Ratio ($h.mean / $r.mean) } else { '---' }) }
            } else {
                $cells += $(if ($c -eq 0) { @('---', '---') } else { @('---', '---', '---') })
            }
        }
        L (($cells -join ' & ') + ' \\')
    }
    L '\bottomrule'
    L '\end{tabular}'
    L '\end{table}'
}

L ''
L '\end{document}'

[System.IO.File]::WriteAllText((Resolve-Path -LiteralPath (Split-Path -Parent $Out)).Path + "\" + (Split-Path -Leaf $Out), $sb.ToString(), (New-Object System.Text.UTF8Encoding($false)))
Write-Host "LaTeX generado en $Out"

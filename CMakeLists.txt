# =========================================================================
# 1) CABECERA: versión mínima de CMake y datos del proyecto.
#    "project()" también fija internamente CMAKE_PROJECT_NAME, etc.
# =========================================================================
cmake_minimum_required(VERSION 3.20)

project(HeatStrokeECS
    VERSION 0.1.0
    LANGUAGES CXX   # este proyecto solo usa C++ (no C)
)

# =========================================================================
# 2) ESTÁNDAR DE C++ Y FLAGS DEL COMPILADOR
#    Aquí es donde tocas si quieres cambiar la versión de C++ o los
#    warnings/optimizaciones. Todo lo que pongas en CXX_FLAGS_COMUNES
#    se aplicará a TODOS los targets definidos más abajo.
# =========================================================================
set(CMAKE_CXX_STANDARD 23)             # <- versión de C++ (17, 20, 23...)
set(CMAKE_CXX_STANDARD_REQUIRED ON)    # falla si el compilador no lo soporta
set(CMAKE_CXX_EXTENSIONS OFF)          # usa -std=c++23 puro, no -std=gnu++23

set(CXX_FLAGS_COMUNES "")
if(MSVC)
    list(APPEND CXX_FLAGS_COMUNES /W4)
else()
    list(APPEND CXX_FLAGS_COMUNES -Wall -Wextra -Wpedantic)
endif()

# Ejemplo de cómo añadir tus propias flags a mano:
# list(APPEND CXX_FLAGS_COMUNES -fno-exceptions -march=native)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # genera compile_commands.json (autocompletado en editores)

# =========================================================================
# 3) LA LIBRERÍA DEL ECS
#    add_library(nombre TIPO archivos.cpp...) crea el target.
#    Para añadir un .cpp nuevo, solo lo listas aquí abajo.
# =========================================================================
add_library(heatstroke_ecs STATIC
    code/source/ecs.cpp
)

# Carpeta donde este target (y quien lo use) busca sus #include
target_include_directories(heatstroke_ecs PUBLIC code/include)

# Le aplicamos las flags que definimos en el punto 2
target_compile_options(heatstroke_ecs PRIVATE ${CXX_FLAGS_COMUNES})

if(MINGW)
    # Enlaza el runtime de C++ de forma estática: el .exe resultante no
    # depende de libstdc++-6.dll / libgcc_s_*.dll al ejecutarse.
    target_link_options(heatstroke_ecs PUBLIC -static-libgcc -static-libstdc++ -static)
endif()

# =========================================================================
# 4) EJECUTABLE DE PRUEBA (sandbox)
#    add_executable funciona igual que add_library pero produce un .exe.
#    target_link_libraries lo conecta con heatstroke_ecs: automáticamente
#    hereda su include dir y sus flags de link, porque los declaramos PUBLIC.
# =========================================================================
option(HSECS_BUILD_SANDBOX "Compilar el ejecutable de pruebas sandbox" ON)

if(HSECS_BUILD_SANDBOX)
    add_executable(hsecs_sandbox sandbox/main.cpp)
    target_link_libraries(hsecs_sandbox PRIVATE heatstroke_ecs)
    target_compile_options(hsecs_sandbox PRIVATE ${CXX_FLAGS_COMUNES})
endif()

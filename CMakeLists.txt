# =========================================================================
# 1) CABECERA: versión mínima de CMake y datos del proyecto.
#    "project()" también fija internamente CMAKE_PROJECT_NAME, etc.
# =========================================================================
cmake_minimum_required(VERSION 3.20)

project(HeatStrokeECS
    VERSION 1.0.0
    LANGUAGES CXX   # este proyecto solo usa C++ (no C)
)

include(FetchContent) # To integrate benchmarks

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
# 3) MÓDULOS DE LA LIBRERÍA
#    Un target por módulo. Cada uno solo ve los #include de los módulos
#    que enlaza, así CMake impone las dependencias: ecs -> mp -> utils.
#    Para añadir un .cpp nuevo, lo listas en el add_library de su módulo.
# =========================================================================

# --- utils: log y asserts. Es el módulo base, no depende de ningún otro.
add_library(hs_utils STATIC
    code/utils/source/warning.cpp
    code/utils/source/log.cpp
)
target_include_directories(hs_utils PUBLIC code/utils/include)
target_compile_options(hs_utils PRIVATE ${CXX_FLAGS_COMUNES})

# Modo debug: activa asserts, warnings y log solo en Debug (preset debug).
# PUBLIC porque las macros se expanden en las cabeceras de ecs, que se compilan
# en los .cpp de quien las incluya (sandbox, benchmarks...).
target_compile_definitions(hs_utils PUBLIC $<$<CONFIG:Debug>:HS_DEBUG_MODE>)

if(MINGW)
    # Enlaza el runtime de C++ de forma estática: el .exe resultante no
    # depende de libstdc++-6.dll / libgcc_s_*.dll al ejecutarse.
    # Al ser PUBLIC, lo hereda todo lo que enlace (directa o indirectamente) hs_utils.
    target_link_options(hs_utils PUBLIC -static-libgcc -static-libstdc++ -static)
endif()

# --- mp: metaprogramación. Solo cabeceras (INTERFACE: no compila nada).
add_library(hs_mp INTERFACE)
target_include_directories(hs_mp INTERFACE code/mp/include)
target_link_libraries(hs_mp INTERFACE hs_utils)

# --- ecs: el ECS en sí. Solo cabeceras.
add_library(hs_ecs INTERFACE)
target_include_directories(hs_ecs INTERFACE code/ecs/include)
target_link_libraries(hs_ecs INTERFACE hs_mp hs_utils)

# =========================================================================
# 4) EJECUTABLE DE PRUEBA (sandbox)
#    add_executable funciona igual que add_library pero produce un .exe.
#    target_link_libraries lo conecta con hs_ecs: automáticamente hereda
#    los include dirs y las flags de link de hs_ecs, hs_mp y hs_utils.
# =========================================================================
option(HSECS_BUILD_SANDBOX "Compilar el ejecutable de pruebas sandbox" ON)

option(HSECS_BUILD_BENCHMARKS "Compilar el ejecutable de Google benchmarks" ON)

option(HSECS_BUILD_ENTT_BENCHMARKS "Compilar el ejecutable de Google benchmarks con EnTT" ON)

if(HSECS_BUILD_SANDBOX)
    add_executable(hsecs_sandbox sandbox/main.cpp)
    target_link_libraries(hsecs_sandbox PRIVATE hs_ecs)
    target_compile_options(hsecs_sandbox PRIVATE ${CXX_FLAGS_COMUNES})
endif()

if(HSECS_BUILD_BENCHMARKS OR HSECS_BUILD_ENTT_BENCHMARKS)
    FetchContent_Declare(googlebenchmark GIT_REPOSITORY "https://github.com/google/benchmark.git" GIT_TAG v1.10.0 GIT_SHALLOW true)
    # Disable default Google Benchmark ouwn tests
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googlebenchmark)
endif()

if(HSECS_BUILD_BENCHMARKS)
    add_executable(hsecs_benchmarks benchmarks/benchmarks.cpp
        benchmarks/benchmarks.hpp)
    target_link_libraries(hsecs_benchmarks PRIVATE hs_ecs)
    target_link_libraries(hsecs_benchmarks PRIVATE benchmark::benchmark)
    target_compile_options(hsecs_benchmarks PRIVATE ${CXX_FLAGS_COMUNES})
endif()

if(HSECS_BUILD_ENTT_BENCHMARKS)
    # SYSTEM: EnTT headers are treated as system headers, so our -Wall -Wextra -Wpedantic don't warn about them
    FetchContent_Declare(entt GIT_REPOSITORY "https://github.com/skypjack/entt.git" GIT_TAG v4.0.0 GIT_SHALLOW true SYSTEM)
    FetchContent_MakeAvailable(entt)
    add_executable(entt_benchmarks benchmarks/enttbenchmarks.cpp
        benchmarks/enttbenchmarks.hpp)
    target_link_libraries(entt_benchmarks PRIVATE benchmark::benchmark)
    target_link_libraries(entt_benchmarks PRIVATE EnTT::EnTT)
    target_compile_options(entt_benchmarks PRIVATE ${CXX_FLAGS_COMUNES})

    if(MINGW)
        # Same static runtime as hsecs_benchmarks (which inherits it from hs_utils), so both are linked the same way
        target_link_options(entt_benchmarks PRIVATE -static-libgcc -static-libstdc++ -static)
    endif()
endif()

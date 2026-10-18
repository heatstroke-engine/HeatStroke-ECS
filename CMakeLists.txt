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

if(HSECS_BUILD_SANDBOX)
    add_executable(hsecs_sandbox sandbox/main.cpp)
    target_link_libraries(hsecs_sandbox PRIVATE hs_ecs)
    target_compile_options(hsecs_sandbox PRIVATE ${CXX_FLAGS_COMUNES})
endif()

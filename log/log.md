# DevLog — HeatStroke ECS

Registro de progreso diario del desarrollo del ECS de arquetipos con job system multihilo, dentro del TFG de HeatStroke Engine.




---

## 2026-09-14

**Fase:** Fase1 - ECS basico

**Tiempo invertido:** 3h

### Qué he hecho hoy
- Implementar el Slotmap para el dummy ECS
-

### Decisiones de diseño
- ECS con array(tamaño estatico) se podria haber usado vector, pero como lo que se quiere es hacerlo lo mas simple posible.

### Problemas / bloqueos
- Ninguno

### Aprendizajes
- Repaso sobre que era un Slotmap y su funcionamiento interno

### Próximos pasos
- Metaprogramacion para tener los cmps traits y poder actualizar las mascaras de nuestras entidades.

---

## 2026-09-09

**Fase:** Fase1 - ECS basico

**Tiempo invertido:** 1h

### Qué he hecho hoy
- Creada la rama sw/ecs/basic/1.0 Donde se desarrolla un ECS basico con slotmaps.
- Diseñar la carcasa de EntityManager.

### Decisiones de diseño
_(elecciones tomadas y por qué, alternativas descartadas)_
- Entity sera un struct dentro de EntityManager. Las entidades viven para el EntityManager
- Usar template para pasar la lista de componentes existentes a la clase.

### Problemas / bloqueos
- Necesito desarrollar el almacenamiento para los componentes (Slotmaps).
- Necesito metaprogramacion para las CMPs traits que mas adelante usaremos para gestionar nuestras entidades.

### Aprendizajes
_(algo nuevo entendido sobre DOD, arquetipos, concurrencia, etc.)_
- NA (Ya hice esto en la carrera) 

### Próximos pasos
- Ver los videos de Francisco Gallego Duran (Profesor Retroman), sobre [Slotmaps](https://www.youtube.com/watch?v=ZfWF9tqQOh0) y [metaprogramcion](https://www.youtube.com/watch?v=4NKbmCfZ9OI).
- Una vez vistos los videos, proceder a implementarlo en este ECS basico.

---
## 2026-09-07

**Fase:** Fase0 - Puesta en marcha

**Tiempo invertido:** 1h
### Qué he hecho hoy
- He visualizado la conferencia de  Vittorio Romeo en la CppCon 2025 sobre [Practical Data-Oriented Design in C++](https://www.youtube.com/watch?v=SzjJfKHygaQ)

### Decisiones de diseño
- NA

### Problemas / bloqueos
- NA

### Aprendizajes
- He aprendido sobre el diseño orientado a datos:
    - Los datos estan siempre expuestos.
    - Se centra en la actualizacion del estado de los mismos datos.
    - El bucle es el que decide como se actualizan dichos datos.
    - Estudiar el hardware donde piensas trabajar para poder exprimirlo al maximo.
    - Suprimir herencia/single heap allocations.

### Próximos pasos
- Implementar un ECS sencillo empleando el DOD (Data oriented desing).

## 2026-09-05

**Fase:** Fase0 - Puesta en marcha

**Tiempo invertido:** 2h

### Qué he hecho hoy
- Instalado herramientas necesarias (Msys, Camke, mingw... )
- Configurado CMajeLists.txt para compilar de manera sencilla el proyecto
- Creado ecs.hpp/ecs.cpp y main.cpp como prueba
- Todo compila y se ejecuta sin problema.

### Decisiones de diseño
- Usar Cmake debido a su facilidad y ser ampliamente usado para librerias
- Estructura del proyecto carpeta code/source + code/include
- Se usara la version de C++ 23

### Problemas / bloqueos
- Instalar todo lo necesario para poder compilar C++ y usar cmake.
- Decidir la estructura de la libreria

### Aprendizajes
- Como funciona Cmake, añadir librerias, archivos de cabecera, flags...

### Próximos pasos
- Investigar sobre el DOD (data oriented desing)
- Primer prototipo de ECS sencillo

---
## 2026-09-05

**Fase:** Fase 0 — Puesta en marcha.

**Tiempo invertido:** 3h

### Qué he hecho hoy
- Creada la organización `heatstroke-engine` en GitHub.
- Repositorio principal `HeatStroke` creado.
- Definida la licencia (MIT + cláusula de atribución).
- Repo del ECS creado como `heatstroke-ecs`.

### Decisiones de diseño
- Cada subsistema del motor tendrá su propio repo bajo la organización, prefijado con `heatstroke-` para mantener consistencia.

### Problemas / bloqueos
- Ninguno relevante hoy, trabajo organizativo más que técnico.

### Aprendizajes
- Diferencia entre licencias OSI puras (MIT/Apache) y licencias con cláusula de atribución tipo BSD-4-Clause.

### Próximos pasos
- Preparar el entorno para poder compilar C++ empleando CMAKE.

---

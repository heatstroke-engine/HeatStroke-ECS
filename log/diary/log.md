# DevLog — HeatStroke ECS

Registro de progreso diario del desarrollo del ECS de arquetipos con job system multihilo, dentro del TFG de HeatStroke Engine.

----> Obsidian
---

## 2026-10-17

**Fase:** Fase1 - ECS basico

**Tiempo invertido:** 2h

### Qué he hecho hoy
- Creado un sistema de log (`log.hpp`/`log.cpp`): `Log::printTrace` imprime el mensaje junto al fichero, la línea, la columna y la función de la llamada con `std::source_location`, y `Log::printNoTrace` imprime solo el mensaje.
- `Log::initialize()` redirige `std::clog` a un fichero por ejecución, `log/execution/<dd-mm-aaaa-HHh-MMm-SSs>_log.txt`. Crea la carpeta si no existe y, si no puede abrir el fichero, avisa por `std::cerr` y deja `clog` en la terminal. Al cerrar el programa, el destructor restaura el buffer original de `clog`.
- `runTimeAssertion`/`debugAssertion` aceptan un mensaje opcional y muestran la expresión que ha fallado. `warning()` también acepta un mensaje opcional. Ambos usan `std::source_location` como parámetro por defecto, así que la traza apunta a la línea donde se llama a la macro y no a `warning.cpp`.
- Corregidas llamadas a los asserts: faltaba una coma en `slotmap.hpp` y un paréntesis mal cerrado en `ecs.hpp` hacía que el mensaje se descartara sin error.
- Añadido `#include <cstdint>` en `slotmap.hpp`: usaba `std::uint32_t` y solo compilaba porque le llegaba indirectamente desde otra cabecera.
- `log/execution` añadido al `.gitignore`.

### Decisiones de diseño
- El log escribe en `std::clog` y no en `std::cout`, para no mezclarse con la salida normal del programa y poder redirigirlo de forma independiente.
- `Log` es un singleton creado con un `static` local dentro de `initialize()`. Como se construye después que el `ofstream` global, se destruye antes que él y restaura `clog` antes de que se cierre el fichero.
- Un fichero de log por ejecución, con fecha y hora en el nombre, en lugar de sobrescribir siempre el mismo.

### Problemas / bloqueos
- Ninguno

### Aprendizajes
- Diferencia entre `std::cout`, `std::clog` y `std::cerr`: `cout` va a stdout y `clog`/`cerr` a stderr. `cout` y `clog` tienen buffer y `cerr` no. Dos streams con buffers distintos pueden mostrar sus mensajes desordenados.
- Se puede redirigir un stream cambiando su buffer con `rdbuf()`. El `ofstream` tiene que vivir más que la redirección, o el stream acaba usando un buffer destruido.
- Un argumento por defecto `std::source_location::current()` se evalúa en el sitio de la llamada, y eso es lo que permite que la traza apunte a la línea que llama al assert.
- Una variable `static` definida en una cabecera crea una copia en cada `.cpp` que la incluye. Los detalles internos van en un namespace anónimo dentro del `.cpp`.
- Una función definida en una cabecera tiene que ser `inline` para no violar la ODR (One Definition Rule).
- `ofstream` no crea directorios: si la carpeta no existe, `open` falla sin avisar.

### Próximos pasos
- Realizar el precacheado para los sistemas: que el `foreach` recorra un array precacheado con las entidades que cumplen la firma del sistema.

---

## 2026-10-16

**Fase:** Fase1 - ECS basico

**Tiempo invertido:** 2h

### Qué he hecho hoy
- Añadidos `addTag`/`removeTag` y `addTags`/`removeTags` en `EntityManager`, con su versión privada en `Entity` que actualiza la máscara de tags (`|=` para añadir, `&= ~mask` para quitar).
- `Entity` declara `friend class EntityManager` y deja privados sus `addComponent`/`removeComponent`/`addTag`/`removeTag`: solo el manager puede modificar las máscaras, así no se desincronizan del slotmap.
- Añadido `getComponent<CMP>(e)`, que devuelve una referencia al componente de la entidad, con una sobrecarga `const` del manager que devuelve `const CMP&`. Ambas reciben `const Entity&`.
- Añadidas las sobrecargas `const` de `ComponentStorage::getStorage<CMP>()` y `SlotMap::operator[]` para soportar el `getComponent` const.
- Corregido `addTags`/`removeTags`: el fold `(debugAssertion(...), ...)` no compilaba en release, porque la macro expandía a `static_cast<void>(0)` sin el pack. Ahora delegan en `addTag<TAGs>(e)`/`removeTag<TAGs>(e)`, que ya llevan el assert.

### Decisiones de diseño
- `getComponent` recibe la entidad como `const` en ambas sobrecargas: la entidad solo guarda la máscara y la clave, y el componente vive en el storage del manager. La constness del componente devuelto la decide el manager, no la entidad.
- Las versiones con pack (`addTags`/`removeTags`) delegan en las individuales para no duplicar el assert.

### Problemas / bloqueos
- Ninguno

### Aprendizajes
- No meter un parameter pack dentro de una macro que puede desaparecer en release: si la expansión deja de mencionar el pack, la fold expression no compila.
- Para devolver `const T&` desde un método `const`, toda la cadena de acceso (storage, slotmap) necesita su sobrecarga `const`.

### Próximos pasos
- Realizar el precacheado para los sistemas: que el `foreach` recorra un array precacheado con las entidades que cumplen la firma del sistema.

---

## 2026-10-14

**Fase:** Fase1 - ECS basico

**Tiempo invertido:** 4h

### Qué he hecho hoy
- Arreglada la definición fuera de clase de `addComponent` (faltaba separar la cabecera `template<>` de la clase y la del método, por eso la declaración era incompatible).
- Añadidos `addComponents` y `deleteComponent`/`deleteComponents`, que expanden un parameter pack de tipos con fold expressions (`(f<CMPs>(...), ...)`) y usan `std::forward` para preservar la categoría de valor de cada componente.
- Implementado `type_at<i, Ts...>` (y `type_at_t`) en `typetraits.hpp`, con `static_assert` de rango, y expuesto como `Typelist::at<i>` y `type_traits::at<i>`.
- `removeEntity` ahora elimina todos los componentes de la entidad: `removeEntityimp` recorre `std::make_index_sequence<cmpinfo::list_size>` y, para cada índice, llama a `removeIfPresent<cmpinfo::at<I>>(e)` mediante una fold expression (sin lambda).
- Corregido `removeComponent`: la clave se leía sin inicializar; ahora se obtiene con `getComponentKey<CMP>()`. `Entity::removeComponent` limpia el bit con `&= ~mask` en lugar de invertirlo con `^=`.
- `addComponent` pasa a usar forwarding reference con `std::remove_cvref_t<CMP>` para el tipo real del componente, de modo que acepta lvalues y rvalues (copia o mueve en el `push_back` del slotmap). `addComponents` deduce los tipos sin `<CMPs>` explícito.
- Documentado `typetraits.hpp` con comentarios estilo Doxygen.

### Decisiones de diseño
- Definir los métodos plantilla de `EntityManager` fuera de la clase con doble cabecera `template<typename CMPS, typename TAGS, std::size_t Capacity> template<typename CMP>`, manteniendo el patrón ya usado para `createEntity`/`addEntity`.
- Sustituir la lambda de `removeEntityimp` por un método plantilla `removeIfPresent<CMP>`, que deja el fold en una sola expresión y es más legible.
- `addComponent` no se llama con el tipo explícito (`addComponent<T>(e, x)`): con el tipo fijado a mano, `CMP&&` deja de ser forwarding reference y no admite lvalues.

### Problemas / bloqueos
- Ninguno

### Aprendizajes
- Diferencia entre `std::move` y `std::forward` en forwarding references: `forward` reenvía la categoría de valor original (lvalue/rvalue) según el tipo deducido del pack, `move` la fuerza siempre a rvalue.
- Un índice runtime no puede usarse para indexar un parameter pack de tipos: el tipo con el que se instancia una plantilla debe resolverse en compile-time.
- Con `std::index_sequence<I...>` no hay un array de índices en runtime, sino un pack de constantes compile-time; cada `I` sirve para instanciar `at<I>`.
- `T&&` solo es forwarding reference cuando `T` se deduce; con el tipo explícito es una referencia a rvalue y no enlaza con lvalues.

### Próximos pasos
- Crear `addTag`/`removeTag`.
- Crear `getComponent<CMP>(e)` que devuelve una referencia al componente de dicha entidad.

---

## 2026-09-23

**Fase:** Fase1 - ECS basico

**Tiempo invertido:** 6h

### Qué he hecho hoy
- ComponentStorage realizado, el tipo del almacenamiento se deduce en tiempo de compilacion, metafunciones para devolver el storage de cada componente
- En el entitymanager he diseñado el addComponent, que añade un componente nuevo al slotmap asi como en la entidad (Actualizando la mascara y añadiendo la Key)

### Decisiones de diseño
- Añadir un componente ya existente salta un warning y devuelve el componente actual de la entidad.

### Problemas / bloqueos
- Ninguno

### Aprendizajes
- Flujo de manejo de errores 

### Próximos pasos
- Borrar componente, asi como borrar entidad(importante borrar todos los cmps de la entidad).
- El forech() precacheado por la firma de los sistemas.

---

## 2026-09-18

**Fase:** Fase1 - ECS basico

**Tiempo invertido:** 6h

### Qué he hecho hoy
- Metaprogramar para deducir directamente el tipo del Storage: std::tuple<Slotmap<Cmps, Capacity> ....>,
- Type_traits para componentes y tags, que permiten asignar una id a cada una.

### Decisiones de diseño
- Tupla como almacenador de Slotmaps.

### Problemas / bloqueos
- Ninguno

### Aprendizajes
- Repaso sobre templates y metaprogramacion

### Próximos pasos
- Crear el componentStorage y permiter asociar los identificadors de cada cmps, tag a las entidades del manager.

---

## 2026-09-14

**Fase:** Fase1 - ECS basico

**Tiempo invertido:** 3h

### Qué he hecho hoy
- Implementar el Slotmap para el dummy ECS

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

# How To Develop

Этот документ объясняет, как раскладывать C++ файлы в проекте, как писать `CMakeLists.txt` и как собирать весь проект или отдельную цель.

## Общая идея

Проект собирается через CMake. Важная единица сборки в CMake - это target.

Target бывает, например:

```text
library/example  -> библиотека game_example
bin/game_service -> бинарь game_service
game_example_ut  -> тестовый бинарь для library/example
```

Мы не "собираем папку" напрямую. Мы собираем target, который описан в `CMakeLists.txt`.

## Текущая структура проекта

На старте держим минимальный скелет:

```text
bin/
  gateway_service/
  game_service/
  storage_service/

library/
  common/
  example/
  game_core/
  json/

docs/
```

`bin/*` - точки входа сервисов. Обычно там лежит `main.cpp` и очень мало логики.

`library/*` - переиспользуемые библиотеки. Основной production-код должен жить здесь.

Тесты библиотеки лежат рядом с этой библиотекой:

```text
library/<name>/tests/ut/
```

Так проще читать проект: код библиотеки и проверки на него находятся в одной области.

## План папок проекта

Дальше проект будет расти примерно так:

```text
bin/
  gateway_service/
  game_service/
  storage_service/

library/
  common/
  example/
  json/
  game_core/
  async/
  net/
  http/
  websocket/
  rpc/
  game_runtime/
  db/

games/
  tic_tac_toe/
  connect_four/

db/
  migrations/

docker/

web/

docs/
```

Это именно план, а не то, что нужно создать сразу. Новые папки добавляем тогда, когда появляется реальная задача под эту часть.

## Как устроена библиотека

Основной пример - `library/example`:

```text
library/example/
  CMakeLists.txt
  include/
    game/
      example/
        heart.h
  src/
    heart.cpp
  tests/
    ut/
      CMakeLists.txt
      heart_ut.cpp
```

`include/` - публичные заголовки библиотеки. Их можно подключать из других библиотек, бинарей и тестов.

`src/` - реализация библиотеки. Обычно `.cpp` файлы и приватные детали.

`tests/ut/` - unit-тесты этой библиотеки. Позже внутри `tests/` могут появиться и другие виды тестов, например `integration/`.

Если файл лежит здесь:

```text
library/example/include/game/example/heart.h
```

то подключается он так:

```cpp
#include <game/example/heart.h>
```

Путь `library/example/include` добавляет CMake. Поэтому эта часть пути как бы отбрасывается при `#include`, а в коде остается только `game/example/heart.h`.

То есть CMake говорит компилятору: "ищи заголовки внутри `library/example/include`". После этого строка:

```cpp
#include <game/example/heart.h>
```

ведет к файлу:

```text
library/example/include/game/example/heart.h
```

Внутри `include` путь специально начинается с `game/example/`, а не просто с `heart.h`. Так меньше шансов получить конфликт имен. В большом проекте легко могут появиться разные файлы `types.h`, `parser.h`, `config.h`, `heart.h`. Поэтому лучше писать явно:

```cpp
#include <game/example/heart.h>
#include <game/json/json_value.h>
```

а не так:

```cpp
#include <heart.h>
#include <json_value.h>
```

## Почему не класть все рядом

Так тоже можно:

```text
heart.h
heart.cpp
```

Но для библиотеки лучше разделять публичный API и реализацию:

```text
include/  -> то, чем разрешено пользоваться снаружи
src/      -> внутренности библиотеки
```

Это помогает не превращать внутренние файлы в случайный API.

## Минимальный словарь CMake

`add_subdirectory(path)` подключает другую папку с её `CMakeLists.txt`. Обычно корневой `CMakeLists.txt` просто перечисляет крупные части проекта:

```cmake
add_subdirectory(library/common)
add_subdirectory(library/example)
add_subdirectory(bin/game_service)
```

`add_library(name ...)` создает библиотеку. В скобках перечисляются `.cpp` файлы, из которых она собирается:

```cmake
add_library(game_example
    src/heart.cpp
)
```

`add_executable(name ...)` создает исполняемый файл. Так описываются сервисы и тестовые бинарники:

```cmake
add_executable(game_service
    main.cpp
)
```

`target_include_directories(name ...)` говорит компилятору, где искать заголовки для target-а:

```cmake
target_include_directories(game_example
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

`target_link_libraries(name ...)` подключает зависимости. Если бинарь использует библиотеку, он должен с ней слинковаться:

```cmake
target_link_libraries(game_service
    PRIVATE
        game_common
)
```

`add_test(NAME ... COMMAND ...)` регистрирует тест для `ctest`:

```cmake
add_test(NAME game_example_ut COMMAND game_example_ut)
```

`PRIVATE`, `PUBLIC`, `INTERFACE` описывают, кому видна настройка или зависимость:

```text
PRIVATE   нужна только текущему target-у
PUBLIC    нужна текущему target-у и тем, кто от него зависит
INTERFACE нужна только тем, кто зависит от target-а
```

На старте чаще всего достаточно двух правил:

```text
target_include_directories(library PUBLIC include)
target_link_libraries(binary PRIVATE library)
```

## Пример CMakeLists для библиотеки

`library/example/CMakeLists.txt`:

```cmake
add_library(game_example
    src/heart.cpp
)

target_include_directories(game_example
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests/ut)
```

`add_library(game_example ...)` создает библиотеку `game_example`.

`PUBLIC include` означает: все, кто линкуются с `game_example`, смогут писать:

```cpp
#include <game/example/heart.h>
```

`add_subdirectory(tests/ut)` подключает unit-тесты библиотеки.

## Пример CMakeLists для бинаря

`bin/game_service/CMakeLists.txt`:

```cmake
add_executable(game_service
    main.cpp
)

target_link_libraries(game_service
    PRIVATE
        game_common
)
```

`add_executable(game_service ...)` создает исполняемый файл `game_service`.

`target_link_libraries(... game_common)` подключает библиотеку `game_common`.

## Корневой CMakeLists.txt

В корневом `CMakeLists.txt` подключаются подпапки:

```cmake
add_subdirectory(library/common)
add_subdirectory(library/example)
add_subdirectory(library/game_core)
add_subdirectory(library/json)

add_subdirectory(bin/gateway_service)
add_subdirectory(bin/game_service)
add_subdirectory(bin/storage_service)
```

Если добавляешь новую библиотеку `library/net`, нужно добавить:

```cmake
add_subdirectory(library/net)
```

## Где писать тесты

Тесты пишем внутри библиотеки, которую они проверяют.

Для библиотеки `library/example` unit-тесты лежат здесь:

```text
library/example/tests/ut/
  CMakeLists.txt
  heart_ut.cpp
```

Пример тестового `CMakeLists.txt`:

```cmake
add_executable(game_example_ut
    heart_ut.cpp
)

target_link_libraries(game_example_ut
    PRIVATE
        game_example
)

add_test(NAME game_example_ut COMMAND game_example_ut)
```

Так тест собирается как отдельный бинарь и линкуется с production-библиотекой `game_example`.

## Как собрать проект

Первый раз настроить build-директорию:

```bash
cmake -S . -B build
```

Если установлен Ninja, можно так:

```bash
cmake -S . -B build -G Ninja
```

Собрать всё:

```bash
cmake --build build
```

## Как собрать конкретную цель

Собрать только учебную библиотеку `example`:

```bash
cmake --build build --target game_example
```

Собрать только тесты учебной библиотеки:

```bash
cmake --build build --target game_example_ut
```

Собрать только сервис `game_service`:

```bash
cmake --build build --target game_service
```

Собрать только библиотеку `json`, когда она будет реализовываться:

```bash
cmake --build build --target game_json
```

## Как запустить тесты

После сборки:

```bash
ctest --test-dir build
```

Запустить только тесты примера:

```bash
ctest --test-dir build -R example
```

Запустить только тесты `json`, когда они появятся:

```bash
ctest --test-dir build -R json
```

## Полный учебный пример: library/example

В проекте есть маленькая полностью реализованная библиотека `library/example`. Она не относится к продуктовой логике, а нужна как образец раскладки файлов, CMake и тестов.

Публичный header:

```cpp
#pragma once

#include <string>

namespace game::example {

std::string heart();
std::string heart_message(const std::string& name);

} // namespace game::example
```

Реализация лежит отдельно в `src/heart.cpp`. Другой код подключает только header:

```cpp
#include <game/example/heart.h>
```

Тест лежит в `library/example/tests/ut` и линкуется с production-библиотекой.

Собрать и запустить только тест примера:

```bash
cmake --build build --target game_example_ut
ctest --test-dir build -R example
```

Когда ученик будет делать `library/json`, он должен повторять эту же схему: публичные `.h` в `include/game/json`, реализация в `src`, unit-тесты в `tests/ut`, отдельные targets в CMake.

## Как добавлять новую библиотеку

Например, хотим добавить `library/net`.

Создаем структуру:

```text
library/net/
  CMakeLists.txt
  include/game/net/socket.h
  src/socket.cpp
  tests/ut/CMakeLists.txt
  tests/ut/socket_ut.cpp
```

Пишем `library/net/CMakeLists.txt`:

```cmake
add_library(game_net
    src/socket.cpp
)

target_include_directories(game_net
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests/ut)
```

Добавляем в корневой `CMakeLists.txt`:

```cmake
add_subdirectory(library/net)
```

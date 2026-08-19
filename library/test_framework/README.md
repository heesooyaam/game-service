# Test Framework

`test_framework` — маленькая библиотека для unit-тестов проекта. Она заменяет
`assert` и локальные функции `check`, не добавляя внешних зависимостей.

Framework умеет:

- автоматически регистрировать и запускать test cases;
- показывать имя прошедшего или упавшего test case;
- показывать файл, строку и выражение при ошибке;
- проверять обычные условия, равенство, неравенство и исключения;
- продолжать запуск после падения отдельного test case;
- возвращать ненулевой exit code, если хотя бы один test case упал.

## Структура

```text
library/test_framework/
  CMakeLists.txt
  README.md
  include/library/test_framework/test.h
  src/test.cpp
```

В header лежат публичные типы, макросы и шаблон `check_throws`. В `src/test.cpp`
лежат registry, runner и форматирование ошибок.

Шаблон `check_throws` должен быть виден в месте использования, поэтому его
реализация остается в header. Остальной код компилируется в библиотеку
`test_framework`.

## Быстрый старт

```cpp
#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <library/common/version.h>

TEST_CASE(version) {
    CHECK_EQ(game::common::version(), "0.1.0");
}
```

В CMake тест подключается так:

```cmake
add_executable(common_ut
    version_ut.cpp
)

target_link_libraries(common_ut
    PRIVATE
        common
        test_framework
)

add_test(NAME common_ut COMMAND common_ut)
```

В одном тестовом executable должен быть ровно один `.cpp` с `NTEST_MAIN`.
Остальные test files, если они есть, просто подключают `test.h`.

## Как регистрируется TEST_CASE

Запись:

```cpp
TEST_CASE(version) {
    CHECK_EQ(game::common::version(), "0.1.0");
}
```

макросом превращается примерно в такой код:

```cpp
static void ntest_case_7();

static const NTesting::TTestRegistration ntest_registration_7{
    "version",
    &ntest_case_7
};

static void ntest_case_7() {
    CHECK_EQ(game::common::version(), "0.1.0");
}
```

Число в имени берется из `__LINE__`, чтобы функции и registration objects имели
уникальные имена.

До входа в `main` конструктор `TTestRegistration` добавляет в registry:

```cpp
TTestCase{
    name,
    function
}
```

`function` — обычный указатель на тестовую функцию.

## Где хранится registry

`test_cases()` возвращает ссылку на один function-local static vector:

```cpp
std::vector<TTestCase>& test_cases() {
    static std::vector<TTestCase> cases;
    return cases;
}
```

Function-local static создается при первом обращении. Это позволяет registration
objects безопасно получить registry во время статической инициализации.

## Откуда берется main

Если перед include определен `NTEST_MAIN`, header добавляет:

```cpp
int main() {
    return NTesting::run_all_tests();
}
```

`run_all_tests()` проходит по registry и вызывает каждую функцию. Успешный test
case печатается как `[ OK ]`. Исключение помечает case как `[FAIL]`, но runner
переходит к следующему тесту.

В конце runner печатает сводку и возвращает:

- `EXIT_SUCCESS`, если все tests прошли;
- `EXIT_FAILURE`, если registry пуст или хотя бы один test упал.

CTest и CI используют этот exit code, чтобы определить результат теста.

## Как работает CHECK

```cpp
CHECK(value > 0);
```

Макрос передает в функцию два значения:

```cpp
NTesting::check(
    static_cast<bool>(value > 0),
    "value > 0"
);
```

Первая часть — результат проверки. Вторая получается через `#expression` и
нужна для сообщения об ошибке.

У `check` есть аргумент по умолчанию:

```cpp
std::source_location::current()
```

Он запоминает файл и строку вызова. Если условие ложно, `check` вызывает `fail`,
а `fail` бросает `std::runtime_error` с готовым сообщением. Runner ловит это
исключение на границе test case.

## Остальные проверки

```cpp
CHECK_EQ(actual, expected);
CHECK_NE(actual, unexpected);
STATIC_CHECK(sizeof(int) >= 4);
CHECK_THROWS_AS(parse(input), TParseError);
```

`CHECK_EQ` и `CHECK_NE` — короткие формы обычного `CHECK`.

`STATIC_CHECK` использует `static_assert`, поэтому выполняется во время
компиляции.

`CHECK_THROWS_AS` заворачивает выражение в lambda и передает её шаблону
`check_throws<TException>`. Проверка проходит только при исключении указанного
типа. Отсутствие исключения или исключение другого типа считается ошибкой.

## Пример вывода

```text
[ OK ] version
[FAIL] parser_rejects_bad_input
       json_parser_ut.cpp:42: parse(input) did not throw TParseError

2 test case(s): 1 passed, 1 failed
```

## Ограничения

Framework намеренно маленький. В нем нет fixtures, parameterized tests,
matchers, mocks и параллельного запуска. Если проекту действительно понадобятся
такие возможности, можно отдельно обсудить переход на готовую библиотеку.

Пока задача framework — дать учебному проекту понятные проверки и полезные
ошибки без сложной настройки.

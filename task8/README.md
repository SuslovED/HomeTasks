# АТД Вещественная матрица

**Автор:** Суслов Егор, группа 212

Реализация абстрактного типа данных «матрица» на C++ с полным математическим интерфейсом. Внутреннее устройство (двумерный динамический массив) скрыто; пользователь работает только через публичные методы и операторы.

## Используемые библиотеки и функции

| Библиотека | Функции / элементы |
|------------|-------------------|
| `<iostream>` | `std::ostream`, `std::istream` (для `operator<<`) |
| `<stdexcept>` | `std::exception` (базовый класс для `matrix::error`) |
| `<cstring>` | `std::memcpy`, `std::strlen` |
| `<cctype>` | `std::isdigit` |
| `<cmath>` | `std::abs` (сравнение с точностью) |
| `<cstdio>` | `std::snprintf` (формирование сообщения исключения) |
| `<cstdlib>` | `std::strtod` (парсинг чисел из строки) |

## Краткое описание методов класса `matrix`

### Управление памятью (приватные)
- `allocate(rows, cols)` – выделяет память под матрицу, инициализирует нулями.
- `deallocate()` – освобождает память, обнуляет указатель.
- `copyFrom(other)` – глубокое копирование из другой матрицы.

### Конструкторы
- `matrix()` – пустая матрица (0×0).
- `matrix(int n, int m)` – нулевая матрица n×m.
- `matrix(double d)` – матрица 1×1.
- `matrix(double* arr, int m)` – матрица-строка из массива.
- `matrix(int n, double* arr)` – матрица-столбец из массива.
- `matrix(const char* str)` – из строкового представления `"{{...},{...}}"`.
- `matrix(const matrix& other)` – копирование.
- `~matrix()` – деструктор.

### Статические методы
- `static matrix identity(int n)` – единичная матрица.
- `static matrix diagonal(double* vals, int n)` – диагональная матрица.

### Доступ к элементам
- `int rows() const` – число строк.
- `int columns() const` – число столбцов.
- `void set(int i, int j, double val)` – присвоить значение элементу.
- `double get(int i, int j) const` – получить значение элемента.

### Операторы
- `matrix operator[](int i) const` – i-я строка как новая матрица.
- `matrix operator+(const matrix&) const`, `matrix& operator+=(const matrix&)`.
- `matrix operator-(const matrix&) const`, `matrix& operator-=(const matrix&)`.
- `matrix operator*(const matrix&) const`, `matrix& operator*=(const matrix&)`.
- `matrix operator*(double) const`, `friend operator*(double, const matrix&)`, `matrix& operator*=(double)`.
- `matrix operator-() const` – унарный минус.
- `matrix operator|(const matrix&) const` – вертикальная конкатенация (вторую справа).
- `matrix operator/(const matrix&) const` – горизонтальная конкатенация (вторую под первой).
- `bool operator==(const matrix&) const`, `bool operator!=(const matrix&) const` – сравнение с точностью `matrix::EPS = 1e-9`.
- `friend std::ostream& operator<<(std::ostream&, const matrix&)` – вывод матрицы построчно.

### Исключения
- Вложенный класс `matrix::error` – содержит имя файла и номер строки ошибки, формирует сообщение через `snprintf`.

## Сборка и запуск

```bash
make            # сборка и запуск тестов
make clean      # очистка объектных файлов и исполняемого файла
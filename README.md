# eprint - Easy Print for C++20

A header-only library for simplified printing of complex objects in C++20.

## Features

- **Policy-based design**: Customize output target (cout, wcout, file, custom)
- **Wide character support**: Works with `char`, `wchar_t`, and other character types
- **Automatic type reflection**: Print aggregate types with member names
- **No iostream dependency**: Can be used in embedded systems, WinUI3, etc.

## Requirements

- C++20 compiler (GCC 10+, Clang 10+, MSVC 19.29+)

## Quick Start

```cpp
#include "eprint.hpp"

int main() {
    // Use global printer 'jo' (outputs to std::cout)
    jo.println("Hello, World!");
    jo.println(std::vector{1, 2, 3});
    jo.println(std::map<std::string, int>{{"a", 1}, {"b", 2}});
    
    // Debug macro
    int x = 42;
    ju_dbg(x);  // Output: x: 42
}
```

## API Reference

### Global Printer

```cpp
// Global printer instance (char + std::cout)
inline ju::Printer<cout_policy> jo;
```

### Printer Class

```cpp
template <PrintPolicy Policy>
class ju::Printer {
    // Print objects
    void print(Args&&... args);
    void println(Args&&... args);
    
    // Print type name
    template <typename T> void print();    // prints type name of T
    template <typename T> void println();  // prints type name of T with newline
    
    // Convert to string without output
    template <typename Obj>
    string_type to_string(Obj&& obj);
    
    // Access underlying policy
    Policy& policy();
};
```

### Factory Functions

```cpp
// Create printer with custom policy
template <PrintPolicy Policy>
auto ju::make_printer(Policy&& policy);

// Create printer for any ostream
template <typename CharT>
auto ju::make_ostream_printer(std::basic_ostream<CharT>& os);
```

### Debug Macros

```cpp
ju_dbg(expr)                 // Print "expr: value" using global jo
ju_dbg_with(printer, expr)   // Print "expr: value" using specified printer
```

## Supported Types

### Standard Library Types

| Type | Example Output |
|------|----------------|
| `std::pair<T1, T2>` | `(1, "hello")` |
| `std::tuple<Args...>` | `(1, 2.5, "text")` |
| `std::optional<T>` | `42` or `None` |
| `std::vector<T>` | `[1, 2, 3]` |
| `std::map<K, V>` | `{ "a": 1, "b": 2 }` |
| `std::set<T>` | `[1, 2, 3]` |
| `std::shared_ptr<T>` | `{ address: 0x..., count: 2 }` |
| `std::unique_ptr<T>` | `{ address: 0x... }` |
| `std::complex<T>` | `(3, 4)` |
| `std::filesystem::path` | `path/to/file` |
| `std::chrono::duration` | `100ms`, `5s`, etc. |

### Custom Types

```cpp
// 1. Types with to_string() method
struct Point {
    int x, y;
    std::string to_string() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

// 2. Types convertible to std::string
class Color {
    operator std::string() const { return "RGB(...)"; }
};

// 3. Aggregate types (automatic reflection)
struct Person {
    std::string name;
    int age;
};
// Output: Person { name: "Alice", age: 25 }

// 4. Any range type
std::list<int> lst{1, 2, 3};  // Output: [1, 2, 3]
```

## Custom Policy

Create custom output policies for different environments:

```cpp
// Policy concept requirements
template <typename P>
concept PrintPolicy = requires {
    typename P::char_type;  // Must define character type
} && requires(P& p, std::basic_string_view<typename P::char_type> sv) {
    { p.write(sv) } -> std::same_as<void>;  // Must have write()
};

// Optional: flush() for FlushablePolicy
```

### Built-in Policies

```cpp
// Output to std::cout (char)
struct cout_policy {
    using char_type = char;
    void write(std::string_view sv) { std::cout << sv; }
    void flush() { std::cout.flush(); }
};

// Output to std::wcout (wchar_t)
struct wcout_policy {
    using char_type = wchar_t;
    void write(std::wstring_view sv) { std::wcout << sv; }
    void flush() { std::wcout.flush(); }
};

// Output to any ostream
template <typename CharT>
struct ostream_policy {
    using char_type = CharT;
    std::basic_ostream<CharT>& os_;
    void write(std::basic_string_view<CharT> sv) { os_ << sv; }
    void flush() { os_.flush(); }
};
```

### Custom Policy Examples

```cpp
// WinUI3 / Windows Debug Output
struct debug_output_policy {
    using char_type = wchar_t;
    void write(std::wstring_view sv) {
        OutputDebugStringW(std::wstring(sv).c_str());
    }
};
auto dbg = ju::make_printer(debug_output_policy{});
dbg.println(L"Error: ", error_code);

// Embedded system with custom UART
struct uart_policy {
    using char_type = char;
    void write(std::string_view sv) {
        uart_send(sv.data(), sv.size());
    }
};
auto uart = ju::make_printer(uart_policy{});
uart.println("Sensor: ", value);
```

## Usage Examples

### Basic Usage

```cpp
#include "eprint.hpp"

int main() {
    // Simple values
    jo.println("Value: ", 42);
    jo.println("Pi: ", 3.14159);
    
    // Containers
    jo.println(std::vector{1, 2, 3, 4, 5});
    jo.println(std::map<std::string, int>{{"one", 1}, {"two", 2}});
    
    // Nested structures
    jo.println(std::tuple{
        std::pair{1, "one"},
        std::vector{2, 3, 4},
        std::optional<double>{2.718}
    });
}
```

### Wide Character Output

```cpp
auto wout = ju::make_printer(wcout_policy{});
wout.println(L"Wide string: ", std::vector{1, 2, 3});
```

### Output to File

```cpp
std::ofstream file("log.txt");
auto flog = ju::make_ostream_printer(file);
flog.println("Log entry: ", data);
```

### Output to String

```cpp
std::string s = jo.to_string(std::vector{1, 2, 3});
// s == "[1, 2, 3]"
```

### Type Name Printing

```cpp
jo.print("Type: ");
jo.println<std::vector<int>>();
// Output: Type: std::vector<int>
```

### Aggregate Type Reflection

```cpp
struct Employee {
    std::string name;
    int age;
    std::vector<std::string> skills;
};

Employee emp{"Alice", 30, {"C++", "Python"}};
jo.println(emp);
// Output: Employee { name: "Alice", age: 30, skills: ["C++", "Python"] }
```

## Output Format

| Type | Format |
|------|--------|
| Strings (in containers) | `"quoted"` |
| Strings (top-level) | `unquoted` |
| Pairs | `(first, second)` |
| Tuples | `(a, b, c, ...)` |
| Ranges | `[a, b, c, ...]` |
| Maps | `{ key: value, ... }` |
| Optional (empty) | `None` |
| Aggregate | `TypeName { field: value, ... }` |

## License

MIT License

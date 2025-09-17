# Only Header Eazy Print

# Function
```c++
template <typename ...Args>
void ju::println(Args&&... args);
template <typename ...Args>
void ju::print(Args&&... args);
``` 

# Support Type

## STD Type [Enable std::format]
- pair<T1, T2>        √
- tuple<...Args>      √
- optional\<T\>       √
- filesystem::path    √
- shared_ptr          √
- unique_ptr          √
- complex             √
- chrono              √
    - milliseconds
    - seconds
    - minutes
    - hours
- std range           √
    - vector
    - array
    - deque
    - ...
- std map √

## Custom Type
- class with operator <<(os&, ...)
- class with function to_string()
- class convertible to std::string
- **custom range**
- **aggregate type**

# Usage
**see main.cpp**


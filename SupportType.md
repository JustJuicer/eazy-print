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
- std range
  - vector
  - array
  - deque
  - ...

## Custom Type
- class with operator <<(os&, ...)
- class with function to_string()
- custom range
- aggregate type


## Config
- color
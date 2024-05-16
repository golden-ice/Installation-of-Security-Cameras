## Compilation Instructions

### Using default compiler
```bash

$ mkdir build 
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Debug ../
$ make
```

### Using Clang compiler

```bash
$ mkdir build 
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ../
$ make
```

### With AddressSanitizer

AddressSanitizer is a run-time checker that monitors your program for potential
memory errors. It is very useful for thoroughly testing your C/C++ code at the
early stages of development. It is recommended that you always compile with
AddressSantizer enabled and fix any of the errors that it uncovers.


```bash
$ mkdir build 
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DWITH_ASAN=ON ../
$ make
```


# Module Development Kit

This repository contains the Module Development Kit to produce compatible modules with
GameProgressive's Master Server.

### Cloning submodules
```
git submodule init
cd libuv-cmake
git submodule init
cd ../mysql-connector-cpp
git submodule init
cd ..
git submodule update --recursive --remote
```

### Special thanks
* [MySQL](https://www.mysql.com/) for their database server and C++ connector used in RetroSpy
* [libUV](http://libuv.org/) for their awesome asyncrous I/O library that is used on the Server

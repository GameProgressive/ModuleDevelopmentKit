# Module Development Kit

Module Development Kit is an easy to use kit to produce compatible modules with
GameProgressive's Master Server.

**WARNING:** You are currently using an experimental/unfinished branch!

### Features
- Asynchronous database connection and querying (MySQL/MariaDB)
- Asynchronous server socket connection and handling
- Reimplemented database result data
- No third party modules are included in modules (They are only required inside this project)
- Logging API
- Extended C standard string support
- stdint.h definition for Visual Studio 2008 or lesser

### Cloning submodules
```
git submodule update --recursive --remote
```

### Future development branch roadmap
[ ] Replace MySQL/MariaDB C API with Amy
[ ] Replace old libUV code with Boost::Asio/Asio

<img src="docs/cpp-logo.png" width="80" align="right" />

<img src="docs/nodejs-logo.svg" width="200" align="right" />


# Embedding Node.js

> Example of embedding Node.js in a C++ application.

At the time of writing, there are almost no examples of embedding Node.js in a C++ application. This repository aims to provide a simple example of how to do this.

> Node.js : C++ embedder API
>
> https://nodejs.org/api/embedding.html

## Instructions

The following are included as git submodules:
- `node`: https://github.com/nodejs/node
- `v8`: https://github.com/v8/v8
- `libuv`: https://github.com/libuv/libuv

### Building Node.js

Follow the instructions from `BUILDING.md` in the `node` repository
to build Node.js for your platform:

- https://github.com/nodejs/node/blob/main/BUILDING.md

> Note: build Node.js in both Debug and Release mode.
>
> For example, on Windows simply run:
>
> ```bash
> cd modules/node/
> ./vcbuild.bat
> ./vcbuild.bat debug
> ```

Once it's done, you should be good to go!

### Build this project

#### xmake

```bash
xmake     # to build
xmake run # to run
```

#### CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
./example-of-embedding-nodejs.exe
```

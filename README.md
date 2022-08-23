# SDL Template
I feel like I keep remaking the same projects over and over again. This is a git repo to simply allow me or anyone who finds this
to be able to clone this repo and get started with SDL2 and C. any other projects. Feel free to use this.

# Build Instructions
For Linux you must install `cmake` and `libsdl2-dev`

run the commands

```
cmake -B build -S src
```

On windows you must edit in `/build/CMakeCache.txt` the SDL2_DIR variable to the path of this folder.

afterwards run the following commands on either windows or linux:
```
cmake -B build -S src
cd build
cmake --build .
```

On windows, be sure the SDL2.dll is in the same directory as the executable before executing. This file can be found in the lib directory
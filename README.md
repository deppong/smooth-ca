# SDL Template
I feel like I keep remaking the same projects over and over again. This is a git repo to simply allow me or anyone who finds this
to be able to clone this repo and get started with SDL2 and C. any other projects. Feel free to use this.

# Build Instructions
Recently I decided to start using [vcpkg](https://github.com/microsoft/vcpkg).
This project is meant to be used with VSCode and the CMake Tools Extension as well; However, could be used without an IDE of any kind.

For Windows users you should be able to run this as long as you have a C++ Visual Studio Community Compiler installed, CMake, and VSCode (or
text editor of choice).

If vcpkg is empty run `git submodule update --init`
On VSCode as long as you have CMake Tools installed, just hitting F5 should build the project. You may need to select the proper compiler
and by opening the command palette `CTRL+SHIFT+P` and typing `CMake: Quick Start` should prompt this. If just hitting F5 doesn't seem to work.

Running this without any IDE may require a bit of fiddling with the CMakeLists.txt file and running seperate commands. I haven't tried this yet but
from the vcpkg README it seems to say these commands will do the trick:
```bash
cmake -B [build directory] -S . "-DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake"
cmake --build [build directory]
```
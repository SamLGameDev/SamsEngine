# CMake Project Template
This is a basic 'hello world' cmake project.

## Building this project
This project uses the cmake build system, this is a cross-platform build system with wide support outside of the Microsoft ecosystem. Many of the FOSS projects, such as OBS Studio and SDL are developed with this build system. You can view these on their github pages.

### Windows
On windows, the path of least resistance is to install visual studio community edition, along with the C++ tools (and cmake support). You can then open the cmake project from the file menu (I don't know of a way to open an existing cmake project from the launcher - sorry). 

There is a step by step guide I wrote for security. You can find it in the [COMP280 Resources](http://fal.fosslab.uk/comp280/unreal/sec_pt1/). Ignore the bit about the adventure game in the guide.

### Linux/CLI
If you have a cmake CLI, toolchain and compiler setup. You can use modern cmake commands to build this project:

```bash
cmake -B build/ -S .
cmake --build build
```

Another option is to use CLion, you can get free Educational licences while you are a student. If you've used pycharm, it's pycharm for C++.

## Dealing with dependencies
CMake has reasonably good dependency resolution support if setup correctly, and it takes a lot of the headache out of dealing with dependency management.

## Linux
For most linux distrubtions, the common way to install the cmake library, `packagename` would be:

In an ideal world, you would use the package manager provided by your distrubtion to manage your dependeicies:

On Fedora: `sudo dnf install pkgconfig(packagename)`
On Debian/Ubuntu: `sudo apt install libpackagename-dev`

You can integrate 3rd party package managers (eg, conan), but please don't. It makes life a pain for maintaining distrubtion packages (not that it's likely you'll want to package your project).

## Windows
On windows, you should use vcpkg, you can configure this as a standard user (ie, no admin rights). This project contains the code needed to bootstrap this into the project. Follow [my guide](http://fal.fosslab.uk/comp290/workshops/00_vcpkg/) on how to do this.

On windows: `vcpkg install packagename:x64-windows`

## Mac
I don't have mac instuctions. If someone wants to figure them out send me a pull request.

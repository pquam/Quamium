# Quamium
This project is based on the Web Browser Engineering book by Pavel Panchekha & Chris Harrelson
It can be found online here: https://browser.engineering/

While the book is written in python, I was interested in learning a new language. I messed around with golang long enough to find out it wouldn't be suited twords making a browser, and seeing as my end goal is to use what I learn to contribute to the  Ladybird browser project, I decided to switch to C++.

You can find the Ladybird Browser project here:
https://github.com/LadybirdBrowser/ladybird

AI was used for modifying application layouts in Qt and doing code reviews. I wrote all the application code my self.


## Requirements
The project is built with CMake and a Qt-based toolchain. You will need:

- CMake 3.16 or newer
- A C++17 compiler (GCC, Clang, or MSVC)
- Qt 6 (Widgets and LinguistTools components); ensure the `gcc_64`, `msvc2019_64`, etc. kit you installed includes `Qt6Widgets`
- Boost (system component)
- OpenSSL (SSL and Crypto libraries)

On Ubuntu or Debian you can install the common prerequisites with:

```bash
sudo apt update
sudo apt install build-essential cmake ninja-build qt6-base-dev \
                 libboost-system-dev libssl-dev
```

You may install Qt through the Qt Online Installer, your package manager, or Qt Creator. Point CMake at the Qt installation by setting `CMAKE_PREFIX_PATH`, `Qt6_DIR`, or the `-DQT_HOST_PATH` argument as shown below.

## Configure & Build
Create an out-of-tree build directory and run CMake. The `CMAKE_PREFIX_PATH` argument should reference the kit directory that contains the Qt libraries (for example, `/home/<user>/Qt/6.10.0/gcc_64`):

```bash
cmake -S . -B build -G Ninja \
      -DCMAKE_PREFIX_PATH=/path/to/Qt/6.10.0/gcc_64
cmake --build build
```

You may omit `-G Ninja` to use your default generator (Makefiles, Visual Studio, etc.). Repeat the `cmake --build` command whenever you want to rebuild.

## Running
The executable is produced as `build/Quamium` (or `Quamium.exe` on Windows). Run it directly:

```bash
./build/Quamium
```

To install into a prefix (e.g., `/usr/local`), run `cmake --install build`.

## Localization Artifacts
Translations are defined in `Quamium_en_US.ts`. If you edit UI strings, refresh the translation catalog with:

```bash
cmake --build build --target lupdate
cmake --build build --target lrelease
```

## Additional Notes
- If CMake cannot find Qt, verify the path passed to `CMAKE_PREFIX_PATH` points at the correct kit directory.
- Boost and OpenSSL are linked dynamically by default. Ensure the development headers and libraries are available on your system.
- Qt Creator and CLion both detect the CMake project automatically; simply open the repository folder and configure a kit/profile pointed at your Qt installation.
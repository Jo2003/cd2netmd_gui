# Build Linux Dependencies #
Tried on Ubuntu 18.04. The created package should work until 20.04 for sure.

We assume that our source dir is cloned to `${HOME}/src/cd2netmd_gui`.
We need the whole bunch of autotools and some more. Simply look at the 
console output for missing tools and install them.

## json-c
```bash
cd ${HOME}/src
git clone https://github.com/json-c/json-c
mkdir -p build_json_c && cd build_json_c
cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=${HOME}/src/cd2netmd_gui/linux_deps/ -DBUILD_SHARED_LIBS=false ../json-c/
make 
make install
```

## libcdio
```bash
cd ${HOME}/src
wget https://git.savannah.gnu.org/cgit/libcdio.git/snapshot/libcdio-release-2.1.0.tar.gz
tar -xvzpf libcdio-release-2.1.0.tar.gz
cd libcdio-release-2.1.0
./autogen.sh --prefix=${HOME}/src/cd2netmd_gui/linux_deps --disable-shared
make
make install
```

## libcdio-paranoia
```bash
cd ${HOME}/src
git clone https://github.com/rocky/libcdio-paranoia.git
cd libcdio-paranoia
./autogen.sh --prefix=${HOME}/src/cd2netmd_gui/linux_deps --disable-shared PKG_CONFIG_PATH=${HOME}/src/cd2netmd_gui/linux_deps/lib/pkgconfig
make
make install
```

# Linux Build
## OS
Build tested on **Ubuntu 22.04. x86_64**

## Needed Packages

```bash 
sudo apt install build-essential git gawk cmake pkg-config
sudo apt install qtbase5-dev qt5-qmake
sudo apt install libusb-1.0-0-dev
sudo apt install libgcrypt-dev
```
## create src folder and checkout sources

```bash 
cd ~ && mkdir -p src && cd src
git clone https://github.com/Jo2003/cd2netmd_gui.git
```
## patch pkg config files

```bash
cd ~/src && TMP=/tmp/08-15.pc ; PCF=$(find cd2netmd_gui/prebuilt/linux -name "*.pc"); D=$(pwd) ; for i in $PCF ; do sed "s|/home/joergn/src|${D}|g" $i > ${TMP} && cp -v ${TMP} $i ; done
```

## export pkg-config folder

```bash
cd ~ && export PKG_CONFIG_PATH=$(pwd)/src/cd2netmd_gui/prebuilt/linux/lib/pkgconfig
```

## create build folder

```bash
cd ~/src && mkdir -p build_nw && cd build_nw`
```

## configure build and compile

```bash
cmake ../cd2netmd_gui
make
```

## create deb package

```bash
cd ~/src/cd2netmd_gui && rm -rf packages/*
./create_deb.sh ../build_nw/netmd_wizard
```

## install the package

```bash
sudo apt install ./$(ls packages/netmd_wizard*.deb)
```


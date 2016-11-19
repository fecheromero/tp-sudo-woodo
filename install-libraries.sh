#!/bin/bash

echo "INSTALANDO CURSES"
sudo apt-get install libncurses5-dev

mkdir libraries
cd libraries
echo "DESCARGANDO COMMONS"
git clone https://github.com/sisoputnfrba/so-commons-library.git
echo "DESCARGANDO NIVEL-GUI"
git clone https://github.com/sisoputnfrba/so-nivel-gui-library.git
echo "DESCARGANDO PKMN-UTILS"
git clone https://github.com/sisoputnfrba/so-pkmn-utils.git

cd so-commons-library
echo "INSTALANDO COMMONS"
sudo make install
cd ..

cd so-pkmn-utils
cd src
echo "INSTALANDO PKMN-UTILS"
make all
sudo make install
cd ../..

cd so-nivel-gui-library
echo "INSTALANDO NIVEL-GUI"
make && make install

cd ../../Support
echo "INSTALANDO SUPPORT"
sudo make install
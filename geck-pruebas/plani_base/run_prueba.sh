#!/usr/bin/env bash


PROJECT_PATH=~/Documents/tp-2022-2c-Compilant/GECK
PRUEBAS_PATH=~/Documents/tp-2022-2c-Compilant/geck-pruebas
PRUEBAS_BASE_PATH=~/Documents/tp-2022-2c-Compilant/geck-pruebas/plani_base
CONFIG_PATH=$PRUEBAS_BASE_PATH/consola.config

echo Yendo a la carpeta del proyecto. NOTA: Tiene que estar dentro de la carpeta de Documents.
echo $PROJECT_PATH

echo "Ejecutando GECK - Prueba plani base"
cd $PRUEBAS_PATH && sh ./run_all_by_config.sh plani_base

sleep 2

cd $PROJECT_PATH

echo "Ejecutando Consola 1 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "./Debug/consola $CONFIG_PATH $PRUEBAS_BASE_PATH/PLANI_BASE_1"

echo "Ejecutando Consola 2 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "./Debug/consola $CONFIG_PATH $PRUEBAS_BASE_PATH/PLANI_BASE_2"

echo "Ejecutando Consola 3 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "./Debug/consola $CONFIG_PATH $PRUEBAS_BASE_PATH/PLANI_BASE_3"

echo "Ejecutando Consola 4 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "./Debug/consola $CONFIG_PATH $PRUEBAS_BASE_PATH/PLANI_BASE_4"

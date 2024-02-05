#/bin/bash
APP_FOLDER="/usr/share/fireplace"

echo "Updating resources"
./resource-installer.sh $APP_FOLDER liberation_serif_font ttf
./resource-installer.sh $APP_FOLDER images png
./resource-installer.sh $APP_FOLDER icons png

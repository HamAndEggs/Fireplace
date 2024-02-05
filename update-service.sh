#/bin/bash
APP_NAME="fireplace"
APP_FOLDER="/usr/share/$APP_NAME"

echo "Updating repo"
git pull

./update-resources.sh

echo "Building application"
#appbuild
./makeit.sh DRM Release

echo "Updaing service"
sudo systemctl stop $APP_NAME
sudo cp ./build/Release/$APP_NAME /usr/bin/$APP_NAME
sudo systemctl start $APP_NAME
sudo systemctl status $APP_NAME


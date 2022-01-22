#!/bin/bash

path="/var/www/html/test"
#scene to build from, must be a named .json file in resources/scenes/
scene=$1
#any 2nd value creates a menu item
menu=$2

if [ -z "$scene" ]
then
      echo "Must provide a scene to build"
      exit 1
fi


echo "Downloading textures..."
echo
#downloads any URL textures provided in the scene json
python3 $path/src/download_textures.py $scene
echo
echo "Minifiying $scene.json..."
echo
#minify config json file
python3 -c 'import json, sys;json.dump(json.load(sys.stdin), sys.stdout)' < $path/resources/scenes/$scene.json > $path/resources/scenes/build.json.min
echo

#create build directory
# if [ -d $path/build/$scene ] 
# then
# fi
echo "Creating $scene build directory..."
mkdir $path/build/$scene/
echo
echo "Running make..."
#build raylib
make PROJECT_NAME=$scene PLATFORM=PLATFORM_WEB -B 
echo
echo "Removing downloaded textures..."
#remove downloaded files
xargs -a $path/resources/delete.txt -d'\n' rm
rm $path/resources/delete.txt
echo
echo "Removing minified $scene.json..."
#remove minifed file
rm $path/resources/scenes/build.json.min
echo
if [ -n "$menu" ]
then
      echo "Adding menu item..."
      python3 $path/src/game_subjects.py $scene
fi


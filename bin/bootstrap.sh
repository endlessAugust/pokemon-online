#!/bin/bash

function zip_files(){
    cd $1
    name=`basename $1 .txt`
    zip -r $name.zip .
    mv $name.zip ../
    cd -
    rm -Rf $1
}

[ "$1" == "--nodmg" ] && NODMG=true
[ "$1" == "--dmgonly" ] && DMGONLY=true

if [ -z "$DMGONLY" ]
then
set -e trap

VERSION=$(grep VERSION ../src/Shared/config.h | sed -n "s/.*\"\(.*\)\".*/\1/p")
ARCHIVE=Pokemon-Online-${VERSION}.zip
DMGARCHIVE=Pokemon-Online-${VERSION}.dmg

/usr/libexec/PlistBuddy -c "Add :NSHighResolutionCapable bool true" Pokemon-Online.app/Contents/Info.plist

cp -r ./clientplugins Pokemon-Online.app/Contents/Resources
cp -r ./*.dylib Pokemon-Online.app/Contents/Frameworks/
macdeployqt.app/Contents/MacOS/macdeployqt Pokemon-Online.app
mac-deploy-scripts/bundle_mac_app.sh
mac-deploy-scripts/fix_plugin_linking.py
fi

cd Pokemon-Online.app/Contents/Resources/db/pokes
for g in 1G 2G 3G 4G 5G 6G
do
    zip_files $g/sprites
done
zip_files icons
zip_files cries
cd ..
zip_files items/items
zip_files items/berries
cd ../../../..

if [ -z "$NODMG" ]
then
    mac-deploy-scripts/create-DMG.sh $DMGARCHIVE
fi

#!/bin/bash

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

if [ -z "$NODMG" ]
then
    mac-deploy-scripts/create-DMG.sh $DMGARCHIVE
fi

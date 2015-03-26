#!/bin/bash

rm -rf Pokemon-Online.app
mv Pokemon-Online_debug.app Pokemon-Online.app
mv Pokemon-Online.app/Contents/MacOS/Pokemon-Online_debug Pokemon-Online.app/Contents/MacOS/Pokemon-Online

VERSION=$(grep VERSION ../src/Shared/config.h | sed -n "s/.*\"\(.*\)\".*/\1/p")
ARCHIVE=Pokemon-Online-${VERSION}.zip

/usr/libexec/PlistBuddy -c "Add :NSHighResolutionCapable bool true" Pokemon-Online.app/Contents/Info.plist

cp -r ./clientplugins Pokemon-Online.app/Contents/Resources
cp -r ./*.dylib Pokemon-Online.app/Contents/Frameworks/
macdeployqt.app/Contents/MacOS/macdeployqt Pokemon-Online.app
mac-deploy-scripts/bundle_mac_app.sh
mac-deploy-scripts/fix_plugin_linking.py

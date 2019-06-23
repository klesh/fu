#!/usr/bin/env bash

if ! which convert > /dev/null; then
  echo "Please install imagemagick first"
  exit 1
fi

DIR=$(dirname "$0")
TMP=$DIR/tmp
SIZES=( 16 24 32 48 64 128 )
echo "Using tmp dir: $DIR"

rm -rf $TMP
mkdir -p $TMP

echo "Creating 180x180 icon for AboutDialog"
convert $DIR/icon.png -resize 180x180 $DIR/icon-180.png

echo "Generating uploading icon for System Tray"
convert $DIR/icon-uploading.png -resize 32x32 $DIR/icon-uploading-32.png

echo "Generating icon for Windows"
convert $DIR/icon.png -define icon:auto-resize=16,24,32,48,64,128 -compress zip $TMP/fu.ico
mv $TMP/fu.ico $DIR/../platforms/win32
echo fu.ico generated successfully

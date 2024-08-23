#!/bin/bash

OTA=${OTA=0}

set +v

echo "Building..."

cd ./www || (echo "No directory ./www" && exit 1)
npm run build || (echo "Failed" && exit 2)
cd ..

echo "Compress..."
gzip -9 ./data/*

echo "Uploading..."

if (("$OTA" == 1)); then
  echo "*** OTA mode selected ***"
  pio run -t uploadfs -e ota "$@"
else
  echo "*** WIRE mode selected ***"
  pio run -t uploadfs "$@"
fi
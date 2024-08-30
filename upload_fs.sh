#!/bin/bash

OTA=${OTA:0}
PLATFORM=${PLATFORM=esp8266}

set +v

echo "Building..."

cd ./www || (echo "No directory ./www" && exit 1)
npm run build || (echo "Failed" && exit 2)
cd ..

echo "Compress..."
gzip -9 -r ./data/*

echo "Uploading..."
echo "*** Platform: ${PLATFORM} ***"

if (("$OTA" == 1)); then
  echo "*** OTA mode selected ***"
  pio run -t uploadfs -e ${PLATFORM}-ota "$@"
else
  echo "*** WIRE mode selected ***"
  pio run -t uploadfs -e ${PLATFORM}-release "$@"
fi
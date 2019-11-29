#!/bin/bash

# Script produces all binaries for onvifsdk

# BuildScript name
bs="buildOSDK"

SCRIPT=`realpath $0`
# BuildScript working directory
SC_DIR=`dirname $SCRIPT`

source ${SC_DIR}/helperFunc.sh

# go to bin dir
mkdir $SC_DIR/bin
cd $SC_DIR/bin

# dir for OnvifSDK
if [ ! -f "${SC_DIR}/bin/OnvifSDK-b/libOnvifSDK.a" ]; then
	mkdir OnvifSDK-b
	cd OnvifSDK-b
	cmake ../../OnvifSDK -DSIGRLOG_OUTPUT=1 -DDEVICE_TYPE=NVT -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} || checkErr "OnvifSDK config failed"
	make -j8 VERBOSE=1 || checkErr "OnvifSDK make failed"
	cd ..
fi
# dir for Client
mkdir client
cd client
cmake ../../examples/OnvifClient -DSDK="$SC_DIR/bin/OnvifSDK-b" -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} || checkErr "OnvifClient config failed"
make -j8 VERBOSE=1 || checkErr "OnvifClient make failed"
cd ..
# dir for Server
mkdir server
cd server
cmake ../../examples/OnvifServer -DSDK="$SC_DIR/bin/OnvifSDK-b" -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} || checkErr "OnvifServer config failed"
make -j8 VERBOSE=1 || checkErr "OnvifServer make failed"
cd ../..

succMessage "We are all good"

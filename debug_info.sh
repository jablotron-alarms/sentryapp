#!/bin/bash

BIN_NAME=$1
OBJCOPY=${2:-objcopy}

$OBJCOPY --only-keep-debug $BIN_NAME $BIN_NAME.debug
$OBJCOPY --strip-debug --strip-unneeded $BIN_NAME
$OBJCOPY --add-gnu-debuglink=$BIN_NAME.debug $BIN_NAME

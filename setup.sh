#!/bin/bash

fun()
{
    if [ -f "settings.mk" ]; then
        return 0
    else
        echo "Installing ExtLib"
        DIR=$(pwd)"/ExtLib"
        echo "PATH_EXTLIB = ${DIR}" > settings.mk
        git clone --recurse-submodules https://github.com/rankaisija64/ExtLib.git ${DIR}
        chmod u+x {DIR}/tools/dfc.py
        chmod u+x {DIR}/tools/pkg.py
    fi
}

fun
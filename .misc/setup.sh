#!/bin/bash

# This script updates a fresh Ubuntu installation with all the dependent
# components necessary to use the IoT Client SDK for C.

scriptdir=$(cd "$(dirname "$0")" && pwd)
deps="curl build-essential libcurl4-openssl-dev git cmake"

push_dir () { pushd $1 > /dev/null; }
pop_dir () { popd $1 > /dev/null; }

deps_install ()
{
    sudo apt-get update
    sudo apt-get install -y $deps
}

deps_install

pop_dir

#!/bin/bash

#Original author: Kartal Kaan Bozdoğan (mstniy)
#Source repository: https://github.com/mstniy/safepm

cd "$(dirname "$0")"
../build/tests/overflow_persistence_helper.exe 1
# Start a new process to really make sure the persistent shadow memory is persistent
../build/tests/overflow_persistence_helper.exe 2

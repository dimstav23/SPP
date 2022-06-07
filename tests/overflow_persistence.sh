#!/bin/bash

#Original author: Kartal Kaan Bozdoğan (mstniy)
#Source repository: https://github.com/mstniy/safepm

cd "$(dirname "$0")"
../build/tests/overflow_persistence_helper 1
# Start a new process to really make sure that overflows are identified upon openning
../build/tests/overflow_persistence_helper 2

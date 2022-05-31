#!/bin/bash
if [ -f pmembench_map_bug.cfg ]
then
  sed -i 's|/dev/shm/testfile.map|/mnt/pmembench/benchfile.map|' pmembench_map_bug.cfg
fi
#!/bin/bash

set -e

echo "1. pmembench map memcheck report:"
tail -n 1 results/map_memcheck
echo ""

echo "2. pmembench tx memcheck report:"
tail -n 1 results/tx_memcheck
echo ""

echo "3. pmembench map pmemcheck report:"
tail -n 1 results/map_pmemcheck
echo ""

echo "4. pmembench tx pmemcheck report:"
tail -n 1 results/tx_pmemcheck

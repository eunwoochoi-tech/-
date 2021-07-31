#!/bin/bash

dd if=/dev/zero of=./testfile oflag=direct bs=1M count=1K

echo ""
echo ""

echo "read speed disk -> page cache -> process"
free
time cat testfile > /dev/null

echo ""
echo ""

echo "read speed page cache -> process"
free
time cat testfile > /dev/null

rm testfile

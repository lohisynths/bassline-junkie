#!/bin/bash
git submodule init
git submodule update

cd stk
git checkout x86

cd ../
git checkout master

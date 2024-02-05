#!/bin/bash

make clean
python3 replace_minus.py || python replace_minus.py
make run
sh dot_img.sh

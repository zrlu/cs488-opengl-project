#!/bin/bash

find . -name "*.bmp" -exec sh -c 'convert "$0" "${0%.*}.png"' {} \;

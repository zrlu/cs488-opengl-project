#!/bin/bash

find . -name "*.jpg" -exec sh -c 'convert "$0" "${0%.*}.png"' {} \;

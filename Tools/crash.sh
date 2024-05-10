#!/bin/bash

py logcat.py | grep -B "$1" -E "for vold"
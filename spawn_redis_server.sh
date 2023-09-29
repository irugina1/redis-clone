#!/bin/sh
#
# DON'T EDIT THIS!
#
# CodeCrafters uses this file to test your code. Don't make any changes here!
#
# DON'T EDIT THIS!
set -e
tmpFile=$(mktemp)
gcc app/*.c -o $tmpFile
# do i need gcc app/*.c -o $tmpFile -lpthread ?
exec $tmpFile

#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

#
# Order doesn't matter yet since each patchfile does not
# change any source file that any other patchfile changed.
#

cat ${DIR}/*.diff | patch -p0

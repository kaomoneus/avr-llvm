#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

patch -p0 -i ${DIR}/Targets.diff
patch -p0 -i ${DIR}/interrupt.diff
patch -p0 -i ${DIR}/flash.diff


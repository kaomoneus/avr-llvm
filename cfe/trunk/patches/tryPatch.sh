#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

patch --dry-run -p0 -i ${DIR}/Targets.diff
patch --dry-run -p0 -i ${DIR}/interrupt.diff
patch --dry-run -p0 -i ${DIR}/flash.diff


#!/bin/bash

SCRIPTS_PATH="`dirname \"$0\"`"                      # relative
SCRIPTS_PATH="`( cd \"$SCRIPTS_PATH\" && pwd )`"     # absolutized and normalized
ROOT_PATH="`( cd \"$SCRIPTS_PATH/../..\" && pwd )`"  # absolutized and normalized

find $ROOT_PATH -name \*.c -o -name \*.h | grep -v "/ext" | xargs uncrustify -c $SCRIPTS_PATH/uncrustify.cfg --replace --no-backup

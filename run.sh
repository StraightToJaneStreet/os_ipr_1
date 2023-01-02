#!/bin/bash

TARGET_CATALOG=$1
CURRENT_DIR=$(pwd)
FILES_COUNTER=0

function inspect {
  local CURRENT_DIR=$1
  local ITEM="$CURRENT_DIR/$2"

  if [ -d "$ITEM" ] ; then
    local SUBDIRECTORY=$ITEM

    CHILDS=$(ls $ITEM)

    for CHILD in $CHILDS ; do
      inspect $SUBDIRECTORY $CHILD
    done
  else
    ls -l $ITEM
    FILES_COUNTER=$(($FILES_COUNTER+1))
    #echo "PATH: $ITEM; SIZE: $(wc -c $ITEM | awk '{print $1}')"
    #ls -l $ITEM
    #echo ""
  fi

}

inspect $CURRENT_DIR $TARGET_CATALOG
echo "Files count: $FILES_COUNTER"

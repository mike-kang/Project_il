#!/bin/bash

if [ ! -p /tmp/cmd -o ! -p /tmp/response ]; then
  echo "fail! Debug Service doesn't work!"
  exit
fi

repeat=false

if [ "$1" = -r ]; then
  while :
  do
    echo memory > /tmp/cmd
    date
    cat /tmp/response
    sleep $2
  done
else
  echo memory > /tmp/cmd
  cat /tmp/response
fi


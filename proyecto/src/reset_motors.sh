#!/bin/bash -ue
# Script_name     : reset_motors.sh
# Author          : DSSE
# Description     : Reset each connected motor
# Requisites      :
# Version         : v1.0

for m in /sys/class/tacho-motor/motor*
  do
    MOTORPATH="$m"
    if [ -e $MOTORPATH ]; then
      address=`cat $MOTORPATH/address`
      printf "${MOTORPATH##*/} in $address\n"

      # report status before reset
      motorstate=`cat $MOTORPATH/state`
      printf "  Old state: $motorstate\n"

      # reset motor
      echo reset > "$MOTORPATH/command"

      # report status after reset
      motorstate=`cat $MOTORPATH/state`
      printf "  New state: $motorstate\n"
    fi
  done

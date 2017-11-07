#!/bin/bash
# This script flashes the baseline.ndz OS image in all platform nodes
# NOTE:
# Usage: ./initialize.sh <username>

# Log into platform's main grid console
ssh $1@console.grid.orbit-lab.org << 'END_CONSOLE_SSH'

    NODEX=11
    NODEY=10

    # Turn the exp nodes off (hard)
    echo "Turning the nodes off, please wait..."
    omf tell -a offh -t all

    # Sleep for 60s for safety purposes
    echo "Finished turning all the nodes off, sleeping for 60s seconds..."
    sleep 60

    # Flash baseline-sdr.ndz to desired node
    # NOTE: we set the flashing timeout to 1000s, as suggested by Ivan Seskar
    echo "Flashing OS image in all nodes, please wait..."
    omf load -i baseline-sdr.ndz -t node$NODEX-$NODEY -o 300

    # Sleep for 60s for safety purposes (default was 120s)
    echo "Finished loading image, sleeping for 60s seconds..."
    sleep 60

    # Turn the exp nodes on
    echo "Turning the exp nodes on, please wait..."
    omf tell -a on -t node$NODEX-$NODEY

    # Farewell
    echo "Initialization process finished."
    echo "Use node"$NODEX"-"$NODEY"."
    echo "Please allow some time for nodes to boot up and become sshable before moving to the next step."
    echo

    # Logout from console
    exit

END_CONSOLE_SSH

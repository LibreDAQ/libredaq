#!/bin/sh
set -e # Any commands which fail will cause the script to exit
set -x # Verbose script

# find out srcs path:
SCRIPTS_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
BRANCH_NAME="master"

# Regenerate doxygen docs:
cd "${SCRIPTS_DIR}/../libldaq/"
doxygen
cd "${SCRIPTS_DIR}/../"
git checkout gh-pages

set +e 

rm -fr "${SCRIPTS_DIR}/../${BRANCH_NAME}"
mv "${SCRIPTS_DIR}/../libldaq/html" "${SCRIPTS_DIR}/../${BRANCH_NAME}"
git add "${SCRIPTS_DIR}/../${BRANCH_NAME}"
git commit -a
git checkout ${BRANCH_NAME}


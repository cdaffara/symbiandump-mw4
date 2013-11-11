#!/bin/bash -x

PATH="/usr/bin:${PATH}"

SRCROOT="`pwd`/../../../JavaScriptCore"
SRCROOT=`realpath "$SRCROOT"`
# Do a little dance to get the path into 8.3 form to make it safe for gnu make
# http://bugzilla.opendarwin.org/show_bug.cgi?id=8173
SRCROOT=`cygpath -m -s "$SRCROOT"`
SRCROOT=`cygpath -u "$SRCROOT"`
export SRCROOT
#group is one level deeper for s60 so BUILT_PRODUCTS_DIR cannot be used for both "mkdir -p "../${BUILT_PRODUCTS_DIR}"" and for "chartables.c : $(BUILT_PRODUCTS_DIR)/dftables" in JavaScriptCore/DerivedSources.make
export JavaScriptCore="${SRCROOT}"
export WebKitS60GROUP="../../WebKit/s60/group"
export BUILT_PRODUCTS_DIR="../../DerivedSources/JavaScriptCore"
export CREATE_HASH_TABLE="$JavaScriptCore/kjs/create_hash_table"
export PCRE_DIR="$JavaScriptCore/pcre"

mkdir -p "../${BUILT_PRODUCTS_DIR}"
cd "../${BUILT_PRODUCTS_DIR}"

make -f "$WebKitS60GROUP/dftables.make" || exit 1
make -f "$JavaScriptCore/DerivedSources.make" || exit 1

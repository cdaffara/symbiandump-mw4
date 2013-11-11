#!/usr/bin/bash

# Convert the DOS WebKitOutputDir variable to a unix path.
#WebKitOutputDirUnix=`cygpath -a -u "$2"`
#WebKitOutputConfigDirUnix="${WebKitOutputDirUnix}/$1"

PATH="/usr/bin:${PATH}"

SRCROOT="`pwd`/../../../WebCore"
SRCROOT=`realpath "$SRCROOT"`
# Do a little dance to get the path into 8.3 form to make it safe for gnu make
# http://bugzilla.opendarwin.org/show_bug.cgi?id=8173
SRCROOT=`cygpath -m -s "$SRCROOT"`
SRCROOT=`cygpath -u "$SRCROOT"`
export SRCROOT

SOURCE_ROOT="`pwd`/../../../WebCore"
SOURCE_ROOT=`realpath "$SOURCE_ROOT"`
SOURCE_ROOT=`cygpath -m -s "$SOURCE_ROOT"`
SOURCE_ROOT=`cygpath -u "$SOURCE_ROOT"`
export SOURCE_ROOT


# FIXME: Eventually win32 might wish to generate to the Debug/Release output directories.
export BUILT_PRODUCTS_DIR="../../.."
export CREATE_HASH_TABLE="$SRCROOT/../JavaScriptCore/kjs/create_hash_table"

mkdir -p "${BUILT_PRODUCTS_DIR}/DerivedSources/WebCore"
cd "${BUILT_PRODUCTS_DIR}/DerivedSources/WebCore"

export WebCore="${SRCROOT}"
export ENCODINGS_FILE="${WebCore}/platform/symbian/symbian-encodings.txt";
export ENCODINGS_PREFIX="\" \""
# FIXME: Should make XPath Support configurable someday on Win32.
#export FEATURE_DEFINES="XPATH_SUPPORT"
export FEATURE_DEFINES=""
make -f "$WebCore/DerivedSources.make" || exit 1

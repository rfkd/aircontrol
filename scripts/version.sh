#!/bin/sh

# Determine full version string
HASH=`git rev-parse HEAD`
VERSION=`git describe --exact-match HEAD 2>/dev/null`
if [ -z "$VERSION" ]; then
    VERSION="0.0.0"
fi
FULL_VERSION="$VERSION+$HASH"

# Generate the version header
cat >include/Version.h <<EOM
// This file will be generated during the build process. Do not edit, any 
// changes will be lost. See 'scripts/version.sh'.

#pragma once

#include <string>

/// Version string.
const std::string VERSION = "$FULL_VERSION";
EOM

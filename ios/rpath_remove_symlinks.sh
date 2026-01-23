#!/usr/bin/env bash

if [[ -z "$CODESIGNING_FOLDER_PATH" ]]; then
	exit 0
fi

frameworksPath="$CODESIGNING_FOLDER_PATH/Frameworks"
if [[ ! -d "$frameworksPath" ]]; then
	exit 0
fi

cd "$frameworksPath"
if [[ ! -f "libNullkiller.dylib" ]]; then
	exit 0
fi

tbbFilename=$(otool -L libNullkiller.dylib 2>/dev/null | egrep --only-matching 'libtbb\S+')
if [[ -n "$tbbFilename" && -L "$tbbFilename" ]]; then
	mv -f "$(readlink "$tbbFilename")" "$tbbFilename"
fi

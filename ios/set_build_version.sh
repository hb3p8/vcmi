#!/usr/bin/env bash

today=$(date '+%Y-%m-%d')
commitShort=$(git rev-parse --short HEAD)
bundleVersion="$today-$commitShort"

/usr/libexec/PlistBuddy "$1/Info.plist" -c "Set :CFBundleVersion $bundleVersion"

settingsPlist="$1/Settings.bundle/Root.plist"
if [[ -f "$settingsPlist" ]]; then
	/usr/libexec/PlistBuddy "$settingsPlist" -c "Set :PreferenceSpecifiers:0:DefaultValue $MARKETING_VERSION" || true
	/usr/libexec/PlistBuddy "$settingsPlist" -c "Set :PreferenceSpecifiers:1:DefaultValue $bundleVersion" || true
fi

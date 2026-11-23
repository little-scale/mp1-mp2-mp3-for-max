#!/bin/sh
# Build Layer I and II Max externals. Optional second argument selects an output folder.
set -eu
cd "$(dirname "$0")"
support="${1:?Supply the path to max-sdk-base}/c74support"
destination="${2:-../..}"
for layer in 1 2; do
    for role in encode decode pack unpack frames clock bands; do
        name="mp${layer}${role}~"
        bundle="$destination/$name.mxo"
        mkdir -p "$bundle/Contents/MacOS"
        version=1.0; build=1
        if [ "$layer" = 1 ] && [ "$role" = decode ]; then version=2.0; build=2; fi
        cat > "$bundle/Contents/Info.plist" <<PLIST
<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0"><dict>
<key>CFBundleExecutable</key><string>$name</string>
<key>CFBundleIdentifier</key><string>audio.subband.mp${layer}${role}</string>
<key>CFBundleName</key><string>$name</string><key>CFBundlePackageType</key><string>iLaX</string>
<key>CFBundleVersion</key><string>$build</string><key>CFBundleShortVersionString</key><string>$version</string>
</dict></plist>
PLIST
        case "$role" in
            encode|decode|pack) set -- "max_$role.c" subband.c filter.c convert.c worker.c
                if [ "$role" = decode ] && [ "$layer" = 1 ]; then set -- "$@" layer1.c; fi ;;
            unpack) set -- max_unpack.c subband.c filter.c convert.c reader.c ;;
            bands) if [ "$layer" = 1 ]; then set -- max_bands.c layer1.c
                   else set -- max_bands2.c subband.c filter.c; fi ;;
            *) set -- "max_$role.c" subband.c ;;
        esac
        xcrun clang -std=c11 -O3 -DMPEG_AUDIO_LAYERS_12 -DLAYER="$layer" -Wall -Wextra -Wno-unused-parameter -Wno-cast-function-type-mismatch \
          -arch arm64 -arch x86_64 -mmacosx-version-min=11.0 -I"$support/max-includes" -I"$support/msp-includes" \
          -F"$support/msp-includes" -framework MaxAudioAPI -bundle -Wl,-undefined,dynamic_lookup -fvisibility=hidden \
          "$@" -o "$bundle/Contents/MacOS/$name.new"
        mv "$bundle/Contents/MacOS/$name.new" "$bundle/Contents/MacOS/$name"
        codesign --force --sign - "$bundle"
    done
done

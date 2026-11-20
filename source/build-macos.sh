#!/bin/sh
# sh build-macos.sh /absolute/path/to/max-sdk-base [patched-universal-libmp3lame.0.dylib]
set -eu
cd "$(dirname "$0")"
support="${1:?Specify max-sdk-base}/c74support"
lame_lib="${2:-}"
if [ -z "$lame_lib" ] || ! nm -gU "$lame_lib" 2>/dev/null | grep -q ' _lame_set_frame_bitrate$'; then
    scratch=$(mktemp -d)
    trap 'rm -rf "$scratch"' EXIT
    lame_lib="$scratch/libmp3lame.0.dylib"
    sh build-lame-macos.sh "$lame_lib"
fi
for name in mp3encode mp3decode mp3pack mp3frames mp3short mp3clock mp3unpack; do
    bundle="../$name~.mxo"
    mkdir -p "$bundle/Contents/MacOS"
    plist="$bundle/Contents/Info.plist"
    build=1; version=1.0
    if [ "$name" = mp3decode ]; then build=8; version=1.7; fi
    if [ "$name" = mp3pack ]; then build=5; version=1.4; fi
    if [ "$name" = mp3encode ]; then build=5; version=1.4; fi
    if [ "$name" = mp3clock ] || [ "$name" = mp3frames ] || [ "$name" = mp3unpack ]; then build=2; version=1.1; fi
    cat > "$plist" <<PLIST
<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0"><dict>
<key>CFBundleExecutable</key><string>$name~</string>
<key>CFBundleIdentifier</key><string>audio.subband.$name</string>
<key>CFBundleName</key><string>$name~</string><key>CFBundlePackageType</key><string>iLaX</string>
<key>CFBundleVersion</key><string>$build</string><key>CFBundleShortVersionString</key><string>$version</string>
</dict></plist>
PLIST
    set -- "$name.c"
    if [ "$name" = mp3decode ] || [ "$name" = mp3encode ]; then set -- "$@" layer3.c; fi
    if [ "$name" = mp3unpack ]; then set -- mp3unpack.c field_file.c spectral_rate.c layer3.c; fi
    if [ "$name" = mp3pack ] || [ "$name" = mp3decode ]; then set -- "$name.c" pack.c pack_writer.c spectral_rate.c layer3.c; fi
    if [ "$name" = mp3encode ]; then
        mkdir -p "$bundle/Contents/Frameworks"
        library="$bundle/Contents/Frameworks/libmp3lame.0.dylib"
        cp "$lame_lib" "$library.new"
        chmod u+w "$library.new"
        install_name_tool -id @loader_path/../Frameworks/libmp3lame.0.dylib "$library.new"
        codesign --force --sign - "$library.new"
        mv "$library.new" "$library"
        set -- "$@" encoder.c "$bundle/Contents/Frameworks/libmp3lame.0.dylib"
    fi
    xcrun clang -std=c11 -O3 -Wall -Wextra -Wno-unused-parameter -Wno-cast-function-type-mismatch \
        -arch arm64 -arch x86_64 -mmacosx-version-min=11.0 \
        -I"$support/max-includes" -I"$support/msp-includes" -F"$support/msp-includes" \
        -framework MaxAudioAPI -bundle -Wl,-undefined,dynamic_lookup -fvisibility=hidden \
        "$@" -o "$bundle/Contents/MacOS/$name~.new"
    mv "$bundle/Contents/MacOS/$name~.new" "$bundle/Contents/MacOS/$name~"
    codesign --force --sign - "$bundle"
done

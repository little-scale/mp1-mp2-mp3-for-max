#!/bin/sh
# Exercise the full legal recording matrix using a C compiler and FFmpeg.
set -eu
cd "$(dirname "$0")/.."
scratch=$(mktemp -d)
trap 'rm -rf "$scratch"' EXIT
cc -std=c11 -O2 -I. tests/check-band-hold.c -lm -o "$scratch/band-hold"
"$scratch/band-hold"
cc -std=c11 -O2 -I. tests/pack-limits.c pack.c layer3.c -lm -o "$scratch/test"
for hz in 8000 11025 12000 16000 22050 24000 32000 44100 48000; do
    if [ "$hz" -ge 32000 ]; then
        bitrates="32 40 48 56 64 80 96 112 128 160 192 224 256 320"
    elif [ "$hz" -ge 16000 ]; then
        bitrates="8 16 24 32 40 48 56 64 80 96 112 128 144 160"
    else
        bitrates="8 16 24 32 40 48 56 64"
    fi
    for ch in 1 2; do
        for kb in $bitrates; do
            "$scratch/test" "$hz" "$ch" "$kb" "$scratch/check.mp3"
            ffmpeg -nostdin -v error -xerror -err_detect explode -f mp3 -c:a mp3float \
                -i "$scratch/check.mp3" -f null -
        done
    done
    echo "Passed $hz Hz"
done

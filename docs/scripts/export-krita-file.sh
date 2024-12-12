#!/bin/sh
# Export .kra file into PNGs, optimizing size if possible.

set -eu

# Assumes Krita is running under flatpak
krita_cmd='flatpak -u run org.kde.krita'

IFS=' '

# Check if pngquant and oxipng commands are available to optimize png size
for cmd in pngquant oxipng; do
    if command -v "$cmd" >/dev/null; then
        eval "has_$cmd=1"
        echo ":: $cmd found, will be used to optimize png size"
    else
        eval "has_$cmd=0"
        echo ":: $cmd not found"
    fi
done

kra="$1"
png="${kra%.*}.png"

# Because $krita_cmd has spaces
IFS=' '

$krita_cmd --export "$kra" --export-filename "$png"

# shellcheck disable=SC2154
if [ "$has_pngquant" -eq 1 ]; then
    echo
    echo "::: Optimizing with pngquant:"
    pngquant --speed 1 --strip --skip-if-larger -v --ext .png --force "$png"
fi

# shellcheck disable=SC2154
if [ "$has_oxipng" -eq 1 ]; then
    echo
    echo "::: Optimizing with oxipng:"
    oxipng --strip=all --opt=max -- "$png"
fi

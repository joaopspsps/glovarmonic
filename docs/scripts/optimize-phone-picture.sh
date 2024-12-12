#!/bin/sh

set -eu

src="$1"
dst="${1%.*}-optimized.jpg"

magick "$src" -resize 25% "$dst"
jpegoptim --strip-all "$dst"

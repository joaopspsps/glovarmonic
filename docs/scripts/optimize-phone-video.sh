#!/bin/sh

set -eu

src="$1"
dst="${1%.*}-optimized.mp4"

ffmpeg -hide_banner -i "$src" -vf scale='iw/2:ih/2' -map_metadata -1 "$dst"

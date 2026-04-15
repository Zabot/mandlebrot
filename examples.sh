#!/bin/sh

# parameters used from the zoom at https://en.wikipedia.org/wiki/File:Mandel_zoom_00_mandelbrot_set.jpg
real=-0.743643887037151
imag=0.131825904205330
common="-r $real -i $imag -w 1920 -h 1080 --steps 5000"

temp=$(mktemp -d)
./mandlebrot $common -o $temp/1.ppm --zoom           1 -g 002b36 -g 073642 -g 586e75
./mandlebrot $common -o $temp/2.ppm --zoom        2000 -g 000000 -g 300231 -g 00AAFF -g 640700 -g CB6B20 -g FFFFED
./mandlebrot $common -o $temp/3.ppm --zoom 59979000000 -g 000000 -g 313002 -g AAFF00 -g 006407 -g 20CB6B -g EDFFFF
./mandlebrot $common -o $temp/4.ppm --zoom       50000 -g 000000 -g FFFFFF --dither

mkdir -p /images
ls $temp | xargs -I {} basename {} .ppm | xargs -I {} magick $temp/{}.ppm images/{}.png

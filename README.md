Generates escape time fractals to use as wallpapers. Using OpenCL is supported

# Usage

```
./mandlebrot -r -0.743643887037151 -i 0.131825904205330 -w 1920 -h 1080 --steps 5000 --zoom 1 -g 002b36 -g 073642 -g 586e75
```
![Mandlebrot](images/1.png?raw=true "Mandlebrot")

```
./mandlebrot -r -0.743643887037151 -i 0.131825904205330 -w 1920 -h 1080 --steps 5000 --zoom 20000 -g 000000 -g 300231 -g 00AAFF -g 640700 -g CB6B20 -g FFFFED
```
![Mandlebrot](images/2.png?raw=true "Mandlebrot")

```
./mandlebrot -r -0.743643887037151 -i 0.131825904205330 -w 1920 -h 1080 --steps 5000 --zoom 59979000000 -g 000000 -g 313002 -g AAFF00 -g 006407 -g 20CB6B -g EDFFFF
```
![Mandlebrot](images/3.png?raw=true "Mandlebrot")

```
./mandlebrot -r -0.743643887037151 -i 0.131825904205330 -w 1920 -h 1080 --steps 5000 --zoom 50000 -g 000000 -g FFFFFF --dither
```
![Mandlebrot](images/4.png?raw=true "Mandlebrot")

# Compiliation
```
nix develop
autoreconf -i
./configure [--disable-opencl] [--disable-pthread]
make
```

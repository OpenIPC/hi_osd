# hi_osd
Used to create additional VPSS regions and overlay bitmap content over them

Just a PoC/Demo! SDL-1.2 is statically linked, thus increasing binary size drastically

Tested on XiongMai-based hardware

# Building

Only Hi3516CV300 and Hi3516EV200 families are supported. 

Requires arm-himix100-linux toolchain.

```mkdir -p build && cd build && rm -rf *```

```cmake -DCROSS_COMPILE=arm-himix100-linux- ..```

```make clean && make```


# Usage
## Requirements:
Place any TTF font named `font.ttf` in the executable dir

## Command line arguments
           [-n channel]   (0 - main, 1 - extra)  
           [-h handle]    (use to add multiple regions; 0,1,8,9,16,17 - are reserved!)  
           [-c command]   (text2osd,logo2osd,loop2osd) 
           [-t value]     (text,path,etc) 
           [-x X]         (X-coordinate) 
           [-y Y]         (Y-coordinate) 
           [-s N]         (font size, optional) 
         
# Examples
`./hi_osd -n 0 -h 5 -c text2osd -t TEST -x 1320 -y 980 -s 46`

`./hi_osd -n 0 -h 5 -c logo2osd -t ./logo.bmp -x 980 -y 720 -s 46`

`./hi_osd -n 0 -h 5 -c loop2osd -x 1320 -y 980 -s 46`

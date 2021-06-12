# hi_osd
Used to create additional VPSS regions and overlay bitmap content over them
# Building

Only Hi3516CV300 and Hi3516EV200 families are supported. 

Requires arm-himix100-linux toolchain.

```mkdir -p build && cd build && rm -rf *```

```cmake ..```

```make clean && make```


# Usage
         [-n channel]   (0 - main, 1 - extra)  
         [-h handle]    (use to add multiple regions; 0,1,8,9,16,17 - are reserved!)  
         [-c command]   (text2osd,logo2osd,loop2osd) 
         [-t value]     (text,path,etc) 
         [-x X]         (X-coordinate) 
         [-y Y]         (Y-coordinate) 
         [-s N]         (font size, optional) 
         
# Examples
`./hi_osd -n 0 -h 5 -c text2osd -t TEST -x 980 -y 1020 -s 46`

`./hi_osd -n 0 -h 5 -c logo2osd -t ./logo.bmp -x 980 -y 1020 -s 46`

`./hi_osd -n 0 -h 5 -c loop2osd -x 980 -y 1020 -s 46`

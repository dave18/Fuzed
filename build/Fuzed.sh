#!/bin/sh

#sudo -n /usr/pandora/scripts/op_lcdrate.sh 50

export SDL_VIDEODRIVER=omapdss
export SDL_OMAP_LAYER_SIZE=640x480
export SDL_OMAP_VSYNC=1
#LD_PRELOAD=./libSDL-1.2.so.0.11.3 ./Fuzed
./Fuzed

#sudo -n /usr/pandora/scripts/op_lcdrate.sh 60

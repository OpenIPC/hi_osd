#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "sample_comm.h"
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "loadbmp.h"
static char tmp[64];
static TTF_Font *font;
static SDL_Surface *text, *temp;
static SDL_Rect bounds;
static BITMAP_S stBitmap;
extern int FontSize;

BITMAP_S SDL_OSDtoBMP(HI_S32 HandleNum, char *FontFile, int FontSize, char *tmp)
{
    int s32Ret;
    static unsigned char Count=0;
    if (TTF_Init() < 0 )
    {
            fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
            SDL_Quit();
    }
    font = TTF_OpenFont(FontFile, FontSize);
    if ( font == NULL )
    {
            fprintf(stderr, "Couldn't load %d pt font from %s: %s\n",
            "ptsize", 18, SDL_GetError());
    }
    SDL_Color fgcolor= { 0xFF, 0xFF, 0xFF, 0x00 }, bgcolor= {0x00, 0xff, 0xff, 0xff};
    //TTF_SetFontOutline(font, 1);
    TTF_SetFontHinting(font, TTF_HINTING_NORMAL);
    //TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    //text = TTF_RenderUTF8_Solid(font, tmp, fgcolor);
		text = TTF_RenderUTF8_Shaded(font, tmp, fgcolor,bgcolor);
    SDL_PixelFormat *fmt;
    fmt = (SDL_PixelFormat*)malloc(sizeof(SDL_PixelFormat));
    memset(fmt,0,sizeof(SDL_PixelFormat));
    fmt->BitsPerPixel = 16;
    fmt->BytesPerPixel = 2;
    fmt->colorkey = 0xffffffff;
    fmt->alpha = 0xff;
    SDL_Surface *temp = SDL_ConvertSurface(text,fmt,0);
    SDL_SaveBMP(temp, "sys_time.bmp");
    stBitmap.u32Width = temp->pitch/2;
    stBitmap.u32Height = temp->h;
    stBitmap.pData= temp->pixels;
    stBitmap.enPixelFormat= PIXEL_FORMAT_ARGB_1555;
    s32Ret = HI_MPI_RGN_SetBitMap(HandleNum,&stBitmap);
    if(s32Ret != HI_SUCCESS) {
        printf("HI_MPI_RGN_SetBitMap failed with %#x!\n", s32Ret);
    }
    SDL_FreeSurface(text);
    SDL_FreeSurface(temp);
    TTF_CloseFont(font);
    TTF_Quit();
    return stBitmap;
}

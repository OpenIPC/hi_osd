//#define __16CV300__
//#define __16EV200__

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
#include <sys/prctl.h>

#ifdef __16CV300__
#include "cv300/hi_common.h"
#include "cv300/mpi_region.h"
#include "cv300/hi_math.h"
#include "cv300/loadbmp.h"
#else
#include "hi_common.h"
#include "mpi_region.h"
#include "hi_math.h"
#include "loadbmp.h"
#endif

#define FontFile "./font.ttf"
int FontSize = 32;

BITMAP_S SDL_OSDtoBMP();

#ifdef __16CV300__

#define PIXEL_FORMAT_4444 PIXEL_FORMAT_RGB_4444
#define PIXEL_FORMAT_1555 PIXEL_FORMAT_RGB_1555
#define PIXEL_FORMAT_2BPP PIXEL_FORMAT_RGB_2BPP
#define PIXEL_FORMAT_8888 PIXEL_FORMAT_RGB_8888
#else
#define PIXEL_FORMAT_4444 PIXEL_FORMAT_ARGB_4444
#define PIXEL_FORMAT_1555 PIXEL_FORMAT_ARGB_1555
#define PIXEL_FORMAT_2BPP PIXEL_FORMAT_ARGB_2BPP
#define PIXEL_FORMAT_8888 PIXEL_FORMAT_ARGB_8888
#endif

typedef enum hi_WORK_MODE{
    VI_OSDEX_TASK = 0,
    VI_COVEREX_TASK,
    VPSS_OSDEX_TASK,
    VPSS_COVEREX_TASK,
    VPSS_COVER_TASK,
    VPSS_MOSAIC_TASK,
    VO_OSDEX_TASK,
    VO_COVEREX_TASK,
    VENC_OSD_TASK,
}WORK_MODE_E;
WORK_MODE_E Work_Mode;

HI_U32 g_u32DisBufLen = 3;
HI_CHAR* Path_BMP = HI_NULL;

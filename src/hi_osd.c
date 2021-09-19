#include "hi_osd.h"

HI_S32 SAMPLE_REGION_SetBitMap(RGN_HANDLE Handle, BITMAP_S *pstBitmap)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_RGN_SetBitMap(Handle,pstBitmap);
    if(HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_RGN_SetBitMap failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}
HI_S32 REGION_MST_LoadBmp(const char *filename, BITMAP_S *pstBitmap, HI_BOOL bFil, HI_U32 u16FilColor,PIXEL_FORMAT_E enPixelFormat)
{
    OSD_SURFACE_S Surface;
    OSD_BITMAPFILEHEADER bmpFileHeader;
    OSD_BITMAPINFO bmpInfo;
    HI_S32 s32BytesPerPix = 2;
    HI_U8* pu8Data;
    HI_S32 R_Value;
    HI_S32 G_Value;
    HI_S32 B_Value;
    HI_S32 Gr_Value;
    HI_U8  Value_tmp;
    HI_U8  Value;
    HI_S32 s32Width;

    if(GetBmpInfo(filename,&bmpFileHeader,&bmpInfo) < 0)
    {
        printf("GetBmpInfo err!\n");
        return HI_FAILURE;
    }
     if(enPixelFormat == PIXEL_FORMAT_4444)
    {
        Surface.enColorFmt =OSD_COLOR_FMT_RGB4444;
    }
    else if(enPixelFormat == PIXEL_FORMAT_1555 || enPixelFormat == PIXEL_FORMAT_2BPP)
    {
        Surface.enColorFmt =OSD_COLOR_FMT_RGB1555;
    }
    else if(enPixelFormat == PIXEL_FORMAT_8888)
    {
        Surface.enColorFmt =OSD_COLOR_FMT_RGB8888;
        s32BytesPerPix = 4;
    }
    else
    {
        printf("enPixelFormat err %d \n",enPixelFormat);
        return HI_FAILURE;
    }

    pstBitmap->pData = malloc(s32BytesPerPix * (bmpInfo.bmiHeader.biWidth) * (bmpInfo.bmiHeader.biHeight));

    if(NULL == pstBitmap->pData)
    {
        printf("malloc osd memroy err!\n");
        return HI_FAILURE;
    }
    CreateSurfaceByBitMap(filename,&Surface,(HI_U8*)(pstBitmap->pData));

    pstBitmap->u32Width = Surface.u16Width;
    pstBitmap->u32Height = Surface.u16Height;
    pstBitmap->enPixelFormat = enPixelFormat;

    int i,j, k;
    HI_U8*  pu8Temp;

    #ifndef __16CV300__
    if (PIXEL_FORMAT_2BPP == enPixelFormat)
    {
        s32Width = DIV_UP (bmpInfo.bmiHeader.biWidth,4);
        pu8Data = malloc((s32Width)* (bmpInfo.bmiHeader.biHeight));
        if (NULL == pu8Data)
        {
            printf("malloc osd memroy err!\n");
            return HI_FAILURE;
        }
    }

    #endif
    if (PIXEL_FORMAT_2BPP != enPixelFormat)
    {
        HI_U16 *pu16Temp;
        pu16Temp = (HI_U16*)pstBitmap->pData;
        if (bFil)
        {
            for (i = 0; i < pstBitmap->u32Height; i++)
            {
                for (j = 0; j < pstBitmap->u32Width; j++)
                {
                    if (u16FilColor == *pu16Temp)
                    {
                        *pu16Temp &= 0x7FFF;
                    }

                    pu16Temp++;
                }
            }

        }
    }
    else
    {
        HI_U16 *pu16Temp;

        pu16Temp = (HI_U16*)pstBitmap->pData;
        pu8Temp = (HI_U8*)pu8Data;

        for (i = 0; i < pstBitmap->u32Height; i++)
        {
            for (j = 0; j < pstBitmap->u32Width/4; j++)
            {
                Value = 0;

                for (k = j; k < j + 4; k++)
                {
                    B_Value = *pu16Temp & 0x001F;
                    G_Value = *pu16Temp >> 5 & 0x001F;
                    R_Value = *pu16Temp >> 10 & 0x001F;
                    pu16Temp++;

                    Gr_Value = (R_Value * 299 + G_Value * 587 + B_Value * 144 + 500) / 1000;
                    if (Gr_Value > 16)
                    {
                        Value_tmp = 0x01;
                    }
                    else
                    {
                        Value_tmp = 0x00;
                    }
                    Value = (Value << 2) + Value_tmp;
                }
                *pu8Temp = Value;
                pu8Temp++;
            }
        }
        free(pstBitmap->pData);
        pstBitmap->pData = pu8Data;
    }


    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_REGION_SetBitMap(RGN_HANDLE Handle,PIXEL_FORMAT_E enPixelFmt)
{
   HI_S32 s32Ret;
   BITMAP_S stBitmap;
   REGION_MST_LoadBmp(Path_BMP, &stBitmap, HI_FALSE, 0,enPixelFmt);
   s32Ret = SAMPLE_REGION_SetBitMap(Handle, &stBitmap);
   if (s32Ret != HI_SUCCESS)
   {
        printf("SAMPLE_REGION_SetBitMap failed!Handle:%d\n",Handle);
   }
   free(stBitmap.pData);
   return s32Ret;
}

HI_S32 CREATE_REGION(HI_S32 ChannelNum, HI_S32 HandleNum, int x,int y,int width,int height)
{
    HI_S32 s32Ret;
    RGN_TYPE_E enType;
    MPP_CHN_S stChn;

    RGN_ATTR_S stRegionCurrent;
    RGN_ATTR_S stRegion;

    RGN_CHN_ATTR_S stChnAttr;
    RGN_CHN_ATTR_S stChnAttrCurrent;

    PIXEL_FORMAT_E enPixelFmt;
    HI_U32 au32BgColor[3] = {0xffffffff,0x0000000,1};

    enType = OVERLAY_RGN;
    stChn.enModId = HI_ID_VENC;
    stChn.s32DevId = 0;
    stChn.s32ChnId = ChannelNum;
    g_u32DisBufLen = 0;
    Work_Mode = VENC_OSD_TASK;

    stRegion.enType = OVERLAY_RGN;
    #ifdef __16CV300__
    stRegion.unAttr.stOverlay.stSize.u32Height = 48;
    stRegion.unAttr.stOverlay.stSize.u32Width  = 512;
    #else
    stRegion.unAttr.stOverlay.stSize.u32Height = 48;
    stRegion.unAttr.stOverlay.stSize.u32Width  = 512;
    #endif
    stRegion.unAttr.stOverlay.u32CanvasNum = 2;
    stRegion.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_1555;
    stRegion.unAttr.stOverlay.u32BgColor = au32BgColor[1];
    // HI_MPI_RGN_DetachFromChn(HandleNum,&stChn);
    // HI_MPI_RGN_Destroy(HandleNum);
    s32Ret = HI_MPI_RGN_GetAttr(HandleNum,&stRegionCurrent);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[%s:%d]HI_MPI_RGN_GetAttr failed with %#x , creating region %d...\n", __func__, __LINE__, s32Ret, HandleNum);
        s32Ret = HI_MPI_RGN_Create(HandleNum, &stRegion);
        if(HI_SUCCESS != s32Ret)
        {
            printf("HI_MPI_RGN_Create failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }
    else
    {
        if ( stRegionCurrent.unAttr.stOverlay.stSize.u32Height != stRegion.unAttr.stOverlay.stSize.u32Height
          || stRegionCurrent.unAttr.stOverlay.stSize.u32Width != stRegion.unAttr.stOverlay.stSize.u32Width){
            printf("[%s:%d] Region parameters are different, recreating ... \n", __func__, __LINE__);
            HI_MPI_RGN_DetachFromChn(HandleNum,&stChn);
            HI_MPI_RGN_Destroy(HandleNum);
            s32Ret = HI_MPI_RGN_Create(HandleNum, &stRegion);
            if(HI_SUCCESS != s32Ret)
            {
                printf("HI_MPI_RGN_Create failed with %#x!\n", s32Ret);
                return HI_FAILURE;
            }
          }
    }

    s32Ret = HI_MPI_RGN_GetDisplayAttr(HandleNum,&stChn,&stChnAttrCurrent);
    if(HI_SUCCESS != s32Ret)
    {
      printf("[%s:%d]HI_MPI_RGN_GetDisplayAttr failed with %#x %d, attaching...\n", __func__, __LINE__, s32Ret, HandleNum);
    }
    else
    {
      if (stChnAttrCurrent.unChnAttr.stOverlayChn.stPoint.s32X != x || stChnAttrCurrent.unChnAttr.stOverlayChn.stPoint.s32Y != y)
      {
          printf("[%s:%d] Position has changed, detaching handle %d from channel %d...\n", __func__, __LINE__, s32Ret, HandleNum, &stChn.s32ChnId);
          HI_MPI_RGN_DetachFromChn(HandleNum,&stChn);
      }
    }

    stChnAttr.bShow = HI_TRUE;
    stChnAttr.enType = OVERLAY_RGN;
    stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 0;
    stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 128;
    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = HI_FALSE;
    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = HI_FALSE;
    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 0;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = width;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = height;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.enChgMod = LESSTHAN_LUM_THRESH;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = HI_FALSE;
    #ifndef __16CV300__
    stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[0] = 0x3e0;
    stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[1] = 0x7FFF;
    stChnAttr.unChnAttr.stOverlayChn.enAttachDest = ATTACH_JPEG_MAIN;
    #endif
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = x;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = y;
    stChnAttr.unChnAttr.stOverlayChn.u32Layer = 7;

    HI_MPI_RGN_AttachToChn(HandleNum,&stChn,&stChnAttr);

    return s32Ret;
}

HI_S32 TEXT_TO_OSD(HI_S32 HandleNum, const char *text){
  SDL_OSDtoBMP(HandleNum,FontFile,FontSize,text);
}

HI_S32 LOGO_TO_OSD(HI_S32 HandleNum,char *path)
{

    HI_S32             s32Ret;
    Path_BMP = path;

    PIXEL_FORMAT_E enPixelFmt;
    s32Ret = SAMPLE_COMM_REGION_SetBitMap(HandleNum,PIXEL_FORMAT_1555);

    return s32Ret;
}

HI_VOID LOOP_TO_OSD(HI_S32 HandleNum){
  char text[64];
  while(1) {

  #ifdef __16CV300__
  FILE *fd = popen("devmem 0x120300A4 16 | awk '{temp=(((($1)-125)/806)*165)-40} {printf \"CPU:%.1f °C / \", temp}'; cat /proc/umap/rc | grep InsBr -A1 | awk 'FNR==2 {print $2 \" Kbps \"}'", "r");
  #else
  FILE *fd = popen("devmem 0x120280BC 16 | awk '{temp=(((($1)-117)/798)*165)-40} {printf \"CPU:%.1f °C / \", temp}'; cat /proc/umap/rc | grep InsBr -A1 | awk 'FNR==2 {print $2 \" Kbps \"}'", "r");
  #endif

  if(fd == NULL) {
    fprintf(stderr, "Could not open pipe.\n");
    return;
  }

  fgets(text, sizeof(text), fd);
  SDL_OSDtoBMP(HandleNum,FontFile,FontSize,text);

  fclose(fd);
  sleep(2);
  }
}

int main(int argc, char* argv[])
{
    char *cmd = NULL;
    char *value = NULL;
    HI_S32 x,y;
    HI_S32 n,h;
    int index;
    int c;

    if (argc < 3)
    {
        printf("Usage: %s\n", argv[0]);
        printf("\t [-n channel]\t(0 - main, 1 - extra)  \n");
        printf("\t [-h handle]\t(use to add multiple regions; 0,1,8,9,16,17 - are reserved!)  \n");
        printf("\t [-c command]\t(text2osd,logo2osd,loop2osd) \n");
        printf("\t [-t value]\t(text,path,etc) \n");
        printf("\t [-x X]\t\t(X-coordinate) \n");
        printf("\t [-y Y]\t\t(Y-coordinate) \n");
        printf("\t [-s N]\t\t(font size, optional)  \n");
        exit (1);
    }

    opterr = 0;

    while ((c = getopt (argc, argv, "c:t:x:y:w:n:h:f:s:")) != -1)
    switch (c)
      {
      case 'c':
        cmd = optarg;
        break;
      case 't':
        value = optarg;
        break;
      case 'x':
        x = atoi(optarg);
        break;
      case 'y':
        y = atoi(optarg);
        break;
      case 'n':
        n = atoi(optarg);
        break;
      case 'h':
        h = atoi(optarg);
        break;
      // case 'f':
      //   FontName = optarg;
      //   break;
      case 's':
        FontSize = atoi(optarg);
        break;
      default:
        abort ();
      }

      printf ("cmd = %s, value= %s, x = %i, y = %d, channel = %i , handle = %i\n",
              cmd, value, x, y, n ,h);

      CREATE_REGION(n,h,x,y,160,160);

      if (!strcmp(cmd, "text2osd"))
      {
        TEXT_TO_OSD(h,value);
      }
      else if (!strcmp(cmd, "logo2osd"))
      {
        LOGO_TO_OSD(h,value);
      }
      else if (!strcmp(cmd, "loop2osd"))
      {
        LOOP_TO_OSD(h);
      }

      return 0;

}

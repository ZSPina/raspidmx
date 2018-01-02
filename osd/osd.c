//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2013 Andrew Duncan
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//-------------------------------------------------------------------------

#define _GNU_SOURCE

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "key.h"
#include "font.h"
#include "imageGraphics.h"
#include "imageLayer.h"

#include "bcm_host.h"

//-------------------------------------------------------------------------

#define NDEBUG

//-------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    uint32_t displayNumber = 0;

    //-------------------------------------------------------------------

    int opt;

    while ((opt = getopt(argc, argv, "d:")) != -1)
    {
        switch (opt)
        {
        case 'd':

            displayNumber = atoi(optarg);
            break;

        default:

            fprintf(stderr, "Usage: %s [-d <number>]\n", basename(argv[0]));
            fprintf(stderr, "    -d - Raspberry Pi display number\n");
            exit(EXIT_FAILURE);
            break;
        }
    }

    //-------------------------------------------------------------------

    VC_IMAGE_TYPE_T type = VC_IMAGE_RGBA32;

    //---------------------------------------------------------------------

    bcm_host_init();

    //---------------------------------------------------------------------

    DISPMANX_DISPLAY_HANDLE_T display
        = vc_dispmanx_display_open(displayNumber);
    assert(display != 0);

    //---------------------------------------------------------------------

    DISPMANX_MODEINFO_T info;
    int result = vc_dispmanx_display_get_info(display, &info);
    assert(result == 0);

    //---------------------------------------------------------------------

    IMAGE_LAYER_T infoLayer;
    initImageLayer(&infoLayer,
                   info.width,
                   info.height,
                   VC_IMAGE_RGBA16);
    createResourceImageLayer(&infoLayer, 2);
    
    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);
    
    addElementImageLayerOffset(&infoLayer,
                               10,
                               10,
                               display,
                               update);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char buffer[128];
    snprintf(buffer, sizeof(buffer),"now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    static RGBA8_T backgroundColour = { 255, 255, 255, 255 };
    static RGBA8_T textColour = { 0, 0, 0, 255 };
    IMAGE_T *image = &(infoLayer.image);
    clearImageRGB(image, &backgroundColour);
    int x = 0, y = 0;
    drawStringRGB(x, y, buffer, &textColour, image);
    //updateInfo();
    changeSourceAndUpdateImageLayer(&infoLayer);
    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    usleep(1000000);
    //---------------------------------------------------------------------

    result = vc_dispmanx_display_close(display);
    assert(result == 0);

    //---------------------------------------------------------------------

    keyboardReset();

    //---------------------------------------------------------------------

    return 0;
}


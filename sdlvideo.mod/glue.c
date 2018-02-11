/*
 Copyright (c) 2014-2018 Bruce A Henderson

 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/
#include "SDL_video.h"

#include "brl.mod/blitz.mod/blitz.h"

BBArray * bmx_sdl_video_GetVideoDrivers() {

	int n = SDL_GetNumVideoDrivers();
	
	BBArray *p = bbArrayNew1D( "$",n );
	
	BBString **s=(BBString**)BBARRAYDATA( p,p->dims );
	for( int i=0;i<n;++i ){
		s[i] = bbStringFromUTF8String( SDL_GetVideoDriver(i) );
		BBRETAIN( s[i] );
	}
	
	return p;
}

int bmx_sdl_video_VideoInit(BBString * driver) {
	if (driver == &bbEmptyString) {
		return SDL_VideoInit(NULL);
	} else {
		char * d = bbStringToUTF8String(driver);
		int res = SDL_VideoInit(d);
		bbMemFree(d);
		return res;
	}
}

int bmx_sdl_video_GetDisplayBounds(int index, int * w, int * h) {
	SDL_Rect r;
	int res = SDL_GetDisplayBounds(index, &r);
	*w = r.w;
	*h = r.h;
	return res;
}

int bmx_sdl_video_GetDisplayUsableBounds(int index, int * w, int * h) {
	SDL_Rect r;
	int res = SDL_GetDisplayUsableBounds(index, &r);
	*w = r.w;
	*h = r.h;
	return res;
}

SDL_DisplayMode * bmx_sdl_video_GetDisplayMode(int index, int modeIndex) {
	SDL_DisplayMode * mode = (SDL_DisplayMode*)calloc(1, sizeof(SDL_DisplayMode));
	if (!SDL_GetDisplayMode(index, modeIndex, mode)) {
		return mode;
	} else {
		free(mode);
		return NULL;
	}
}

SDL_DisplayMode * bmx_sdl_video_GetDesktopDisplayMode(int index) {
	SDL_DisplayMode * mode = (SDL_DisplayMode*)calloc(1, sizeof(SDL_DisplayMode));
	if (!SDL_GetDesktopDisplayMode(index, mode)) {
		return mode;
	} else {
		free(mode);
		return NULL;
	}
}

SDL_DisplayMode * bmx_sdl_video_GetCurrentDisplayMode(int index) {
	SDL_DisplayMode * mode = (SDL_DisplayMode*)calloc(1, sizeof(SDL_DisplayMode));
	if (!SDL_GetCurrentDisplayMode(index, mode)) {
		return mode;
	} else {
		free(mode);
		return NULL;
	}
}

// --------------------------------------------------------

SDL_DisplayMode * bmx_sdl_video_DisplayMode_new(Uint32 format, int width, int height, int refreshRate) {
	SDL_DisplayMode * mode = (SDL_DisplayMode*)calloc(1, sizeof(SDL_DisplayMode));
	mode->format = format;
	mode->w = width;
	mode->h = height;
	mode->refresh_rate = refreshRate;
}

void bmx_sdl_video_DisplayMode_free(SDL_DisplayMode * mode) {
	free(mode);
}

Uint32 bmx_sdl_video_DisplayMode_format(SDL_DisplayMode * mode) {
	return mode->format;
}

int bmx_sdl_video_DisplayMode_width(SDL_DisplayMode * mode) {
	return mode->w;
}

int bmx_sdl_video_DisplayMode_height(SDL_DisplayMode * mode) {
	return mode->h;
}

int bmx_sdl_video_DisplayMode_refreshRate(SDL_DisplayMode * mode) {
	return mode->refresh_rate;
}

void * bmx_sdl_video_DisplayMode_driverData(SDL_DisplayMode * mode) {
	return mode->driverdata;
}

SDL_DisplayMode * bmx_sdl_video_GetClosestDisplayMode(SDL_DisplayMode * mode, int index) {
	SDL_DisplayMode * closest = (SDL_DisplayMode*)calloc(1, sizeof(SDL_DisplayMode));
	if (SDL_GetClosestDisplayMode(index, mode, closest) != NULL) {
		return closest;
	} else {
		free(closest);
		return NULL;
	}
}

// --------------------------------------------------------

SDL_Window * bmx_sdl_video_CreateWindow(BBString * title, int x, int y, int w, int h, Uint32 flags) {
	char * t = bbStringToUTF8String(title);
	SDL_Window * window = SDL_CreateWindow(t, x, y, w, h, flags);
	bbMemFree(t);
	return window;
}

SDL_DisplayMode * bmx_sdl_video_GetWindowDisplayMode(SDL_Window * window) {
	SDL_DisplayMode * mode = (SDL_DisplayMode*)calloc(1, sizeof(SDL_DisplayMode));
	int res = SDL_GetWindowDisplayMode(window, mode);
	return mode;
}

void bmx_sdl_video_SetWindowTitle(SDL_Window * window, BBString * title) {
	char * t = bbStringToUTF8String(title);
	SDL_SetWindowTitle(window, t);
	bbMemFree(t);
}

// --------------------------------------------------------
/*
// For re-generating pixel formats consts, for sdl.sdl common.bmx
void bmx_sdl_video_generatePixelFormats() {
	printf("Const SDL_PIXELFORMAT_UNKNOWN:Int = $%X\n", SDL_PIXELFORMAT_UNKNOWN);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_INDEX1LSB:Int = $%X\n", SDL_PIXELFORMAT_INDEX1LSB);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_INDEX1MSB:Int = $%X\n", SDL_PIXELFORMAT_INDEX1MSB);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_INDEX4LSB:Int = $%X\n", SDL_PIXELFORMAT_INDEX4LSB);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_INDEX4MSB:Int = $%X\n", SDL_PIXELFORMAT_INDEX4MSB);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_INDEX8:Int = $%X\n", SDL_PIXELFORMAT_INDEX8);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_RGB332:Int = $%X\n", SDL_PIXELFORMAT_RGB332);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_RGB444:Int = $%X\n", SDL_PIXELFORMAT_RGB444);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_RGB555:Int = $%X\n", SDL_PIXELFORMAT_RGB555);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_BGR555:Int = $%X\n", SDL_PIXELFORMAT_BGR555);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_ARGB4444:Int = $%X\n", SDL_PIXELFORMAT_ARGB4444);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_RGBA4444:Int = $%X\n", SDL_PIXELFORMAT_RGBA4444);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_ABGR4444:Int = $%X\n", SDL_PIXELFORMAT_ABGR4444);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_BGRA4444:Int = $%X\n", SDL_PIXELFORMAT_BGRA4444);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_ARGB1555:Int = $%X\n", SDL_PIXELFORMAT_ARGB1555);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_RGBA5551:Int = $%X\n", SDL_PIXELFORMAT_RGBA5551);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_ABGR1555:Int = $%X\n", SDL_PIXELFORMAT_ABGR1555);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_BGRA5551:Int = $%X\n", SDL_PIXELFORMAT_BGRA5551);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_RGB565:Int = $%X\n", SDL_PIXELFORMAT_RGB565);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_BGR565:Int = $%X\n", SDL_PIXELFORMAT_BGR565);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_RGB24:Int = $%X\n", SDL_PIXELFORMAT_RGB24);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_BGR24:Int = $%X\n", SDL_PIXELFORMAT_BGR24);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_RGB888:Int = $%X\n", SDL_PIXELFORMAT_RGB888);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_RGBX8888:Int = $%X\n", SDL_PIXELFORMAT_RGBX8888);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_BGR888:Int = $%X\n", SDL_PIXELFORMAT_BGR888);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_BGRX8888:Int = $%X\n", SDL_PIXELFORMAT_BGRX8888);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_ARGB8888:Int = $%X\n", SDL_PIXELFORMAT_ARGB8888);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_RGBA8888:Int = $%X\n", SDL_PIXELFORMAT_RGBA8888);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_ABGR8888:Int = $%X\n", SDL_PIXELFORMAT_ABGR8888);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_BGRA8888:Int = $%X\n", SDL_PIXELFORMAT_BGRA8888);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_ARGB2101010:Int = $%X\n", SDL_PIXELFORMAT_ARGB2101010);fflush(stdout);
// endian specific (see end of generated list in common.bmx)
//	printf("Const SDL_PIXELFORMAT_RGBA32:Int = $%X\n", SDL_PIXELFORMAT_RGBA32);fflush(stdout);
//	printf("Const SDL_PIXELFORMAT_ARGB32:Int = $%X\n", SDL_PIXELFORMAT_ARGB32);fflush(stdout);
//	printf("Const SDL_PIXELFORMAT_BGRA32:Int = $%X\n", SDL_PIXELFORMAT_BGRA32);fflush(stdout);
//	printf("Const SDL_PIXELFORMAT_ABGR32:Int = $%X\n", SDL_PIXELFORMAT_ABGR32);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_YV12:Int = $%X\n", SDL_PIXELFORMAT_YV12);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_IYUV:Int = $%X\n", SDL_PIXELFORMAT_IYUV);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_YUY2:Int = $%X\n", SDL_PIXELFORMAT_YUY2);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_UYVY:Int = $%X\n", SDL_PIXELFORMAT_UYVY);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_YVYU:Int = $%X\n", SDL_PIXELFORMAT_YVYU);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_NV12:Int = $%X\n", SDL_PIXELFORMAT_NV12);fflush(stdout);
	printf("Const SDL_PIXELFORMAT_NV21:Int = $%X\n", SDL_PIXELFORMAT_NV21);fflush(stdout);
}
*/

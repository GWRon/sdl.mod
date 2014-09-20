/*
 Copyright (c) 2014 Bruce A Henderson

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

#include "SDL_rwops.h"

#include <brl.mod/blitz.mod/blitz.h>

int sdl_sdl__sdl_rwops_seek(BBObject *, int, int);
int sdl_sdl__sdl_rwops_read(BBObject *, void *, int);
int sdl_sdl__sdl_rwops_write(BBObject *, void *, int);
int sdl_sdl__sdl_rwops_close(BBObject *);

void bmx_SDL_FreeRW_stream(SDL_RWops * ops);
BBString * bmx_SDL_GetError();


BBString * bmx_SDL_GetError() {
	return bbStringFromUTF8String(SDL_GetError());
}


Sint64 bmx_SDL_RWops_seek(struct SDL_RWops * context, Sint64 offset, int whence) {
	return sdl_sdl__sdl_rwops_seek(context->hidden.unknown.data1, (int)offset, whence);
}

size_t bmx_SDL_RWops_read(struct SDL_RWops * context, void *ptr, size_t size, size_t maxnum) {
	return sdl_sdl__sdl_rwops_read(context->hidden.unknown.data1, ptr, (int)(size * maxnum)) / size;
}

size_t bmx_SDL_RWops_write(struct SDL_RWops * context, const void *ptr, size_t size, size_t num) {
  return sdl_sdl__sdl_rwops_write(context->hidden.unknown.data1, ptr, (int)(size * num)) / size;
}

int bmx_SDL_RWops_close(struct SDL_RWops *context) {
	sdl_sdl__sdl_rwops_close(context->hidden.unknown.data1);
	bmx_SDL_FreeRW_stream(context);
	return 0;
}

SDL_RWops * bmx_SDL_AllocRW_stream(BBObject * stream) {
	SDL_RWops * ops = SDL_AllocRW();
	if (ops == NULL) {
		return NULL;
	}

	ops->seek = bmx_SDL_RWops_seek;
	ops->read = bmx_SDL_RWops_read;
	ops->write = bmx_SDL_RWops_write;
	ops->close= bmx_SDL_RWops_close;
	ops->type = 0;
	ops->hidden.unknown.data1 = stream;
	
	BBRETAIN(stream);
	
	return ops;
}

void bmx_SDL_FreeRW_stream(SDL_RWops * ops) {
	if (ops) {
		BBRELEASE(ops->hidden.unknown.data1);
		
		SDL_FreeRW(ops);
	}
}

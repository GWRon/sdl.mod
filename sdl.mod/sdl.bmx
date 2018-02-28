' Copyright (c) 2014-2018 Bruce A Henderson
'
' This software is provided 'as-is', without any express or implied
' warranty. In no event will the authors be held liable for any damages
' arising from the use of this software.
'
' Permission is granted to anyone to use this software for any purpose,
' including commercial applications, and to alter it and redistribute it
' freely, subject to the following restrictions:
'
'    1. The origin of this software must not be misrepresented; you must not
'    claim that you wrote the original software. If you use this software
'    in a product, an acknowledgment in the product documentation would be
'    appreciated but is not required.
'
'    2. Altered source versions must be plainly marked as such, and must not be
'    misrepresented as being the original software.
'
'    3. This notice may not be removed or altered from any source
'    distribution.
'
SuperStrict

Rem
bbdoc: SDL Core
End Rem
Module SDL.SDL

ModuleInfo "Version: 1.00"
ModuleInfo "License: zlib/libpng"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

?win32x86
Import "include/win32x86/*.h"

?win32x64
Import "include/win32x64/*.h"

?osx
ModuleInfo "CC_OPTS: -mmmx -msse -msse2 -DTARGET_API_MAC_CARBON -DTARGET_API_MAC_OSX"

Import "include/macos/*.h"

Import "-framework AudioUnit"
Import "-framework CoreAudio"
Import "-framework IOKit"
Import "-framework CoreVideo"
Import "-framework ForceFeedback"

?linuxx86
ModuleInfo "CC_OPTS: -mmmx -m3dnow -msse -msse2 -DHAVE_LINUX_VERSION_H"
ModuleInfo "CC_OPTS: -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include"

Import "include/linuxx86/*.h"
?linuxx64
ModuleInfo "CC_OPTS: -mmmx -m3dnow -msse -msse2 -DHAVE_LINUX_VERSION_H"
ModuleInfo "CC_OPTS: -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include"

Import "include/linuxx64/*.h"
?raspberrypi
ModuleInfo "LD_OPTS: -L%PWD%/lib/raspberrypi"

Import "include/raspberrypi/*.h"
?android
ModuleInfo "CC_OPTS: -DGL_GLEXT_PROTOTYPES"

Import "include/android/*.h"
?emscripten
ModuleInfo "CC_OPTS: -DUSING_GENERATED_CONFIG_H"

Import "include/emscripten/*.h"
?ios
ModuleInfo "CC_OPTS: -fobjc-arc"

Import "include/ios/*.h"
?win32
'
' Note : If you have XINPUT errors during the build, try uncommenting the following CC_OPTS.
'        Some versions of MinGW have it, some don't...
'
'ModuleInfo "CC_OPTS: -DHAVE_XINPUT_GAMEPAD_EX -DHAVE_XINPUT_STATE_EX"

Import "-limm32"
Import "-lole32"
Import "-loleaut32"
Import "-lshell32"
Import "-lversion"

?raspberrypi
Import "-lSDL2"
?linux
Import "-ldl"
?

Import "SDL/include/*.h"

Import "common.bmx"

Import "glue.c"


Type TSDLStream Extends TStream

	Field filePtr:Byte Ptr

	Method Pos:Long()
		Return bmx_SDL_RWtell(filePtr)
	End Method

	Method Size:Long()
		Return bmx_SDL_RWsize(filePtr)
	End Method

	Method Seek:Long( pos:Long, whence:Int = SEEK_SET_ )
		Return bmx_SDL_RWseek(filePtr, pos, whence)
	End Method

	Method Read:Long( buf:Byte Ptr,count:Long )
		Return bmx_SDL_RWread(filePtr, buf, 1, count)
	End Method

	Method Write:Long( buf:Byte Ptr,count:Long )
		Return bmx_SDL_RWwrite(filePtr, buf, 1, count)
	End Method

	Method Close()
		If filePtr Then
			bmx_SDL_RWclose(filePtr)
			filePtr = Null
		End If
	End Method

	Method Delete()
		Close()
	End Method

	Function Create:TSDLStream( file:String, readable:Int, writeable:Int )
		Local stream:TSDLStream=New TSDLStream
		Local Mode:String

		If readable And writeable
			Mode="r+b"
		Else If writeable
			Mode="wb"
		Else
			Mode="rb"
		EndIf

		Local f:Byte Ptr = file.ToUTF8String()		
		stream.filePtr = SDL_RWFromFile(f, Mode)
		MemFree(f)
		
		If Not stream.filePtr Then
			Return Null
		End If
		
		Return stream
	End Function

End Type

Function CreateSDLStream:TSDLStream( file:String, readable:Int, writeable:Int )
	Return TSDLStream.Create( file, readable, writeable )
End Function

Type TSDLStreamFactory Extends TStreamFactory

	Method CreateStream:TStream( url:Object, proto$, path$, readable:Int, writeable:Int )
		If proto="sdl" Then
			Return TSDLStream.Create( path, readable, writeable )
		End If
	End Method
	
End Type

New TSDLStreamFactory

Function _sdl_rwops_seek:Int(stream:TStream, pos:Long, whence:Int)
	Return stream.seek(pos, whence)
End Function

Function _sdl_rwops_read:Long(stream:TStream, buf:Byte Ptr, count:Long)
	Return stream.read(buf, count)
End Function

Function _sdl_rwops_write:Long(stream:TStream, buf:Byte Ptr, count:Long)
	Return stream.write(buf, count)
End Function

Function _sdl_rwops_close(stream:TStream)
	stream.close()
End Function

Rem
bbdoc: Get the directory where the application was run from.
about: This is where the application data directory is.
This is not necessarily a fast call, though, so you should call this once near startup and save the string if you need it.<br/>
Mac OS X and iOS Specific Functionality: If the application is in a ".app" bundle, this function returns the Resource directory
(e.g. MyApp.app/Contents/Resources/). This behaviour can be overridden by adding a property to the Info.plist file. Adding a string key with
the name SDL_FILESYSTEM_BASE_DIR_TYPE with a supported value will change the behaviour.
End Rem
Function GetBasePath:String()
	Return bmx_SDL_GetBasePath()
End Function

Rem
bbdoc: Returns the preferences dir.
about: This is meant to be where the application can write personal files (Preferences and save games, etc.) that are specific to the application.
This directory is unique per user and per application. The path will be Null if there is a problem (creating directory failed, etc.)<br/>
The return path will be guaranteed to end with a path separator ('\' on Windows, '/' on most other platforms).
You should assume the path returned by this function is the only safe place to write files (and that GetBasePath(), while it might be writable, or even
the parent of the returned path, aren't where you should be writing things).<br/>
Both the org and app strings may become part of a directory name, so please follow these rules:<br/>
* Try to use the same org string (including case-sensitivity) for all your applications that use this function.<br/>
* Always use a unique app string for each one, and make sure it never changes for an app once you've decided on it.<br/>
* Only use letters, numbers, and spaces. Avoid punctuation like "Game Name 2: Bad Guy's Revenge!" ... "Game Name 2" is sufficient.
End Rem
Function GetPrefPath:String(org:String, app:String)
	Return bmx_SDL_GetPrefPath(org, app)
End Function
?android
Rem
bbdoc: Gets the path used for external storage for this application.
returns: The path used for external storage for this application on success or NULL on failure; call SDL_GetError() for more information.
about: This path is unique to your application, but is public and can be written to by other applications.
Your external storage path is typically: /storage/sdcard0/Android/data/your.app.package/files.
End Rem
Function AndroidGetExternalStoragePath:String()
	Return String.FromUTF8String(SDL_AndroidGetExternalStoragePath())
End Function

Rem
bbdoc: Gets the current state of external storage.
about: The current state of external storage, a bitmask of these values: SDL_ANDROID_EXTERNAL_STORAGE_READ, SDL_ANDROID_EXTERNAL_STORAGE_WRITE.
If external storage is currently unavailable, this will return 0.
End Rem
Function AndroidGetExternalStorageState:Int()
	Return SDL_AndroidGetExternalStorageState()
End Function

Rem
bbdoc: Gets the path used for internal storage for this application.
returns: The path used for internal storage or NULL on failure; call SDL_GetError() for more information.
about: This path is unique to your application and cannot be written to by other applications.
Your internal storage path is typically: /data/data/your.app.package/files.
End Rem
Function AndroidGetInternalStoragePath:String()
	Return String.FromUTF8String(SDL_AndroidGetInternalStoragePath())
End Function
?
Rem
bbdoc: Return a flag indicating whether the clipboard exists and contains a text string that is non-empty.
End Rem
Function HasClipboardText:Int()
	Return SDL_HasClipboardText()
End Function

Rem
bbdoc: Returns the clipboard text.
End Rem
Function GetClipboardText:String()
	Return bmx_SDL_GetClipboardText()
End Function

Rem
bbdoc: Puts text into the clipboard.
returns: 0 on success or a negative error code on failure.
End Rem
Function SetClipboardText:Int(Text:String)
	Return SDL_SetClipboardText(Text.ToUTF8String())
End Function

Rem
bbdoc: Logs a message with SDL_LOG_CATEGORY_APPLICATION and SDL_LOG_PRIORITY_INFO.
End Rem
Function LogAppInfo(Text:String)
	Local s:Byte Ptr = Text.ToUTF8String()
	SDL_Log(s)
	MemFree s
End Function

Rem
bbdoc: Logs a message with SDL_LOG_PRIORITY_DEBUG.
End Rem
Function LogDebug(category:Int, Text:String)
	Local s:Byte Ptr = Text.ToUTF8String()
	SDL_LogDebug(category, s)
	MemFree s
End Function

Rem
bbdoc: Logs a message with SDL_LOG_PRIORITY_ERROR.
End Rem
Function LogError(category:Int, Text:String)
	Local s:Byte Ptr = Text.ToUTF8String()
	SDL_LogError(category, s)
	MemFree s
End Function

Rem
bbdoc: Logs a message with SDL_LOG_PRIORITY_CRITICAL.
End Rem
Function LogCritical(category:Int, Text:String)
	Local s:Byte Ptr = Text.ToUTF8String()
	SDL_LogCritical(category, s)
	MemFree s
End Function

Rem
bbdoc: Logs a message with SDL_LOG_PRIORITY_INFO.
End Rem
Function LogInfo(category:Int, Text:String)
	Local s:Byte Ptr = Text.ToUTF8String()
	SDL_LogInfo(category, s)
	MemFree s
End Function

Rem
bbdoc: Logs a message with SDL_LOG_PRIORITY_VERBOSE.
End Rem
Function LogVerbose(category:Int, Text:String)
	Local s:Byte Ptr = Text.ToUTF8String()
	SDL_LogVerbose(category, s)
	MemFree s
End Function

Rem
bbdoc: Logs a message with SDL_LOG_PRIORITY_WARN.
End Rem
Function LogWarn(category:Int, Text:String)
	Local s:Byte Ptr = Text.ToUTF8String()
	SDL_LogWarn(category, s)
	MemFree s
End Function

Rem
bbdoc: Sets the priority of all log categories.
about: If you are debugging SDL, you might want to call this with SDL_LOG_PRIORITY_WARN.
End Rem
Function LogSetAllPriority(priority:Int)
	SDL_LogSetAllPriority(priority)
End Function

Rem
bbdoc: Gets the current power supply details.
returns: One of #SDL_POWERSTATE_UNKNOWN, #SDL_POWERSTATE_ON_BATTERY, #SDL_POWERSTATE_NO_BATTERY, #SDL_POWERSTATE_CHARGING, or #SDL_POWERSTATE_CHARGED.
about: You should never take a battery status as absolute truth. Batteries (especially failing batteries) are delicate hardware,
and the values reported here are best estimates based on what that hardware reports. It's not uncommon for older batteries to lose
stored power much faster than it reports, or completely drain when reporting it has 20 percent left, etc.
Battery status can change at any time; if you are concerned with power state, you should call this function frequently,
and perhaps ignore changes until they seem to be stable for a few seconds.
End Rem
Function GetPowerInfo:Int(seconds:Int Var, percent:Int Var)
	Return SDL_GetPowerInfo(Varptr seconds, Varptr percent)
End Function

Rem
bbdoc: Gets the human readable name of a pixel format
End Rem
Function SDLGetPixelFormatName:String(format:UInt)
	Return String.FromUTF8String(SDL_GetPixelFormatName(format))
End Function

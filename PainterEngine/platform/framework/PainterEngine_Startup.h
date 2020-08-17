#ifndef PAINTERENGINE_STARTUP_H
#define PAINTERENGINE_STARTUP_H

//////////////////////////////////////////////////////////////////////////
//Configures
#ifdef _MSC_VER
#define PX_APPLICATION_WINDOWS
#include "platform/windows/windows_file.h"
#include "platform/windows/windows_udp.h"
#else
#define PX_APPLICATION_ANDROID
#include "platform/android/android_file.h"
#include "platform/android/android_udp.h"
#endif


#define PX_APPLICATION_NAME "oscilloscope"
#define PX_APPLICATION_SURFACE_SIZE 800

#define PX_APPLICATION_MEMORYPOOL_UI_SIZE 1024*1024*32
#define PX_APPLICATION_MEMORYPOOL_RESOURCES_SIZE 1024*1024*32
#define PX_APPLICATION_MEMORYPOOL_GAME_SIZE 1024*1024*8
#define PX_APPLICATION_MEMORYPOOL_SPACE_SIZE 1024*1024*8
//////////////////////////////////////////////////////////////////////////

#include "../../../../../PainterEngine/architecture/PainterEngine_Runtime.h"


#ifdef PAINTERENGINE_FILE_H

px_bool PX_LoadTextureFromFile(px_memorypool *mp,px_texture *tex,const px_char path[]);
px_bool PX_LoadShapeFromFile(px_memorypool *mp,px_shape *shape,const px_char path[]);
px_bool PX_LoadSoundFromFile(px_memorypool *mp,px_shape *shape,const px_char path[]);
px_bool PX_LoadAnimationLibraryFromFile(px_memorypool *mp,PX_Animationlibrary *lib,const px_char path[]);
px_bool PX_LoadScriptFromFile(px_memory *code,const px_char path[]);
px_bool PX_LoadScriptInstanceFromFile(px_memorypool *mp,PX_ScriptVM_Instance *ins,const px_char path[]);

px_bool PX_LoadTextureToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadShapeToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadAnimationToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadScriptToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadSoundToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadFontModuleFromFile(PX_FontModule *fm,const px_char Path[]);
px_bool PX_LoadJsonFromFile(PX_Json *json,const px_char *path);

#endif


#endif

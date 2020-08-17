#include "PainterEngine_Application.h"

PX_Application App;
static px_byte PX_ApplicationRuntime[PX_APPLICATION_MEMORYPOOL_UI_SIZE+PX_APPLICATION_MEMORYPOOL_RESOURCES_SIZE+PX_APPLICATION_MEMORYPOOL_GAME_SIZE+PX_APPLICATION_MEMORYPOOL_SPACE_SIZE];


px_bool PX_ApplicationInitialize(PX_Application *App,px_int screen_width,px_int screen_height)
{
	px_int surface_width=0,surface_height=0;
	px_int window_width=0,window_height=0;
	px_double wdh;
    PX_IO_Data ioData;
	if (screen_width<screen_height)
	{
		px_int tmp=screen_height;
		screen_height=screen_width;
		screen_width=tmp;
	}
	wdh=screen_width*1.0/screen_height;
	surface_height=(px_int)(PX_sqrtd(PX_APPLICATION_SURFACE_SIZE*PX_APPLICATION_SURFACE_SIZE/wdh));
	surface_width=(px_int)(surface_height*wdh);

	#ifdef PX_APPLICATION_WINDOWS
	window_width=screen_width/2;
	window_height=screen_height/2;
	#endif
	
	#ifdef PX_APPLICATION_ANDROID
	window_width=screen_width;
	window_height=screen_height;
	#endif
    ioData=PX_LoadAssetsFileToIOData("text.txt");


	if(!PX_RuntimeInitialize(&App->runtime,surface_width,surface_height,window_width,window_height,PX_ApplicationRuntime,sizeof(PX_ApplicationRuntime),PX_APPLICATION_MEMORYPOOL_UI_SIZE,PX_APPLICATION_MEMORYPOOL_RESOURCES_SIZE,PX_APPLICATION_MEMORYPOOL_GAME_SIZE))
		return PX_FALSE;
	return PX_TRUE;
}

px_void PX_ApplicationUpdate(PX_Application *App,px_dword elpased)
{
	
}

px_void PX_ApplicationRender(PX_Application *App,px_dword elpased)
{
	px_surface *pRenderSurface=&App->runtime.RenderSurface;
	PX_RuntimeRenderClear(&App->runtime,PX_COLOR(255,255,255,255));
	
}

px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e)
{
	
}


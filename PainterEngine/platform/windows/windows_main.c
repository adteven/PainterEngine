
//Platform supports
#include "PainterEngine_Application.h"

#ifdef PX_APPLICATION_WINDOWS
#include "../../../PainterEngine/platform/windows/windows_display.h"
//mouse informations
POINT main_zoomPoint;
px_int main_ZoomRegion;
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI DEMO_RenderThreadFunc(LPVOID p)
{   
	DWORD time,elpased;
	PX_Object_Event e;
	WM_MESSAGE msg;
	static POINT LastDownPoint;
	px_float cursorx=-1,cursory=-1,cursorz=-1;
	px_float cursorx_scale,cursory_scale;

	LastDownPoint.x=-1;
	LastDownPoint.y=-1;

	/*	px_char *keyBoardString;*/
	//////////////////////////////////////////////////////////////////////////
	//CreateWindow

	time=timeGetTime();

	while(1)
	{

		PX_SystemReadDeviceState();

		elpased=timeGetTime()-time;
		time=timeGetTime();

		cursorx_scale=App.runtime.surface_width*1.0f/App.runtime.window_width;
		cursory_scale=App.runtime.surface_height*1.0f/App.runtime.window_height;

		while (PX_GetWinMessage(&msg))
		{
			px_char text[2]={0};
			e.Event=PX_OBJECT_EVENT_ANY;
			switch(msg.uMsg)
			{
			case WM_SIZE:
				{
					e.Event=PX_OBJECT_EVENT_WINDOWRESIZE;
					PX_Object_Event_SetWidth(&e,(px_float)((msg.lparam)&0xffff));
					PX_Object_Event_SetHeight(&e,(px_float)((msg.lparam>>16)&0xffff));
					PX_ApplicationPostEvent(&App,e);
					PX_WindowResize(App.runtime.surface_width,App.runtime.surface_height,((msg.lparam)&0xffff),((msg.lparam>>16)&0xffff));
					continue;
					
				}
				break;
			case WM_MOUSEMOVE:
				{
					if (msg.wparam&MK_LBUTTON)
					{
						e.Event=PX_OBJECT_EVENT_CURSORDRAG;
					}
					else
					{
						e.Event=PX_OBJECT_EVENT_CURSORMOVE;
					}

					cursorx=(px_float)((msg.lparam)&0xffff);
					cursory=(px_float)((msg.lparam>>16)&0xffff);

					PX_Object_Event_SetCursorX(&e,cursorx*cursorx_scale);
					PX_Object_Event_SetCursorY(&e,cursory*cursory_scale);
				}
				break;
			case WM_LBUTTONDOWN:
				{
					e.Event=PX_OBJECT_EVENT_CURSORDOWN;
					cursorx=(px_float)((msg.lparam)&0xffff);
					cursory=(px_float)((msg.lparam>>16)&0xffff);
					PX_Object_Event_SetCursorX(&e,cursorx*cursorx_scale);
					PX_Object_Event_SetCursorY(&e,cursory*cursory_scale);

					LastDownPoint.x=((msg.lparam)&0xffff);
					LastDownPoint.y=((msg.lparam>>16)&0xffff);
				}
				break;
			case WM_LBUTTONUP:
				{
					if (((msg.lparam)&0xffff)==LastDownPoint.x&&((msg.lparam>>16)&0xffff)==LastDownPoint.y)
					{
						e.Event=PX_OBJECT_EVENT_CURSORCLICK;
						cursorx=(px_float)((msg.lparam)&0xffff);
						cursory=(px_float)((msg.lparam>>16)&0xffff);
						PX_Object_Event_SetCursorX(&e,cursorx*cursorx_scale);
						PX_Object_Event_SetCursorY(&e,cursory*cursory_scale);

						LastDownPoint.x=-1;
						LastDownPoint.y=-1;
						PX_ApplicationPostEvent(&App,e);
					}
					e.Event=PX_OBJECT_EVENT_CURSORUP;

					cursorx=(px_float)((msg.lparam)&0xffff);
					cursory=(px_float)((msg.lparam>>16)&0xffff);
					PX_Object_Event_SetCursorX(&e,cursorx*cursorx_scale);
					PX_Object_Event_SetCursorY(&e,cursory*cursory_scale);
				}
				break;
			case WM_RBUTTONDOWN:
				{
					e.Event=PX_OBJECT_EVENT_CURSORRDOWN;
					cursorx=(px_float)((msg.lparam)&0xffff);
					cursory=(px_float)((msg.lparam>>16)&0xffff);
					PX_Object_Event_SetCursorX(&e,cursorx*cursorx_scale);
					PX_Object_Event_SetCursorY(&e,cursory*cursory_scale);
				}
				break;
			case WM_RBUTTONUP:
				{
					e.Event=PX_OBJECT_EVENT_CURSORRUP;
					cursorx=(px_float)((msg.lparam)&0xffff);
					cursory=(px_float)((msg.lparam>>16)&0xffff);
					PX_Object_Event_SetCursorX(&e,cursorx*cursorx_scale);
					PX_Object_Event_SetCursorY(&e,cursory*cursory_scale);
				}
				break;

			case  WM_KEYDOWN:
				{
					e.Event=PX_OBJECT_EVENT_KEYDOWN;
					PX_Object_Event_SetKeyDown(&e,msg.wparam);
				}
				break;
			case WM_MOUSEWHEEL:
				{
					RECT rect,crect;
					GetClientRect(PX_GetWindowHwnd(),&crect);
					GetWindowRect(PX_GetWindowHwnd(),&rect);
					e.Event=PX_OBJECT_EVENT_CURSORWHEEL;
					cursorx=(px_float)((msg.lparam)&0xffff)-rect.left;
					cursory=(px_float)((msg.lparam>>16)&0xffff)-rect.top-((rect.bottom-rect.top)-crect.bottom)+2;
					cursorz=(px_float)((msg.wparam>>16)&0xffff);
					PX_Object_Event_SetCursorX(&e,cursorx*cursorx_scale);
					PX_Object_Event_SetCursorY(&e,cursory*cursory_scale);
					PX_Object_Event_SetCursorZ(&e,cursorx*cursorz);

				}
				break;
			case WM_CHAR:
				{
					text[0]=(px_char)msg.wparam;
					e.Event=PX_OBJECT_EVENT_STRING;
					PX_Object_Event_SetStringPtr(&e,text);
				}
				break;
			case WM_GESTURE:
				{
					GESTUREINFO gi;  
					BOOL bHandled;
					BOOL bResult;
					ZeroMemory(&gi, sizeof(GESTUREINFO));

					gi.cbSize = sizeof(GESTUREINFO);

					bResult  = GetGestureInfo((HGESTUREINFO)msg.lparam, &gi);
					bHandled = FALSE;

					if (bResult){
						// now interpret the gesture
						switch (gi.dwID){
						case GID_ZOOM:
							// Code for zooming goes here    

							if (25>(main_zoomPoint.x-gi.ptsLocation.x)*(main_zoomPoint.x-gi.ptsLocation.x)+(main_zoomPoint.y-gi.ptsLocation.y)*(main_zoomPoint.y-gi.ptsLocation.y))
							{
								e.Event=PX_OBJECT_EVENT_SCALE;
								PX_Object_Event_SetScaleCursorX(&e,(px_float)gi.ptsLocation.x);
								PX_Object_Event_SetScaleCursorY(&e,(px_float)gi.ptsLocation.y);
								PX_Object_Event_SetScaleCursorZ(&e,(px_float)(gi.ullArguments-main_ZoomRegion));
							}

							main_zoomPoint.x=gi.ptsLocation.x;
							main_zoomPoint.y=gi.ptsLocation.y;
							main_ZoomRegion=(px_int)gi.ullArguments;

							bHandled = TRUE;
							break;
						case GID_PAN:
							// Code for panning goes here
							bHandled = TRUE;
							break;
						case GID_ROTATE:
							// Code for rotation goes here
							bHandled = TRUE;
							break;
						case GID_TWOFINGERTAP:
							// Code for two-finger tap goes here
							bHandled = TRUE;
							break;
						case GID_PRESSANDTAP:
							// Code for roll over goes here
							bHandled = TRUE;
							break;
						default:
							// A gesture was not recognized
							break;
						}
						CloseGestureInfoHandle((HGESTUREINFO)msg.lparam);
					}else{
						DWORD dwErr = GetLastError();
						if (dwErr > 0){
							//MessageBoxW(hWnd, L"Error!", L"Could not retrieve a GESTUREINFO structure.", MB_OK);
						}
					}
				}
				break;
			default:
				continue;
			}

			if(e.Event!=PX_OBJECT_EVENT_ANY)
			{
				PX_ApplicationPostEvent(&App,e);
			}

		}
		PX_ApplicationUpdate(&App,elpased);
		PX_ApplicationRender(&App,elpased);
		PX_SystemRender(App.runtime.RenderSurface.surfaceBuffer,App.runtime.surface_width,App.runtime.surface_height);
		Sleep(0);
	}
	return 0;
}
//int main()
int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
	HANDLE hThread;
	DWORD  threadId;

	PX_srand(314159);
	if(!PX_ApplicationInitialize(&App,PX_GetScreenWidth(),PX_GetScreenHeight()))return 0;
	if(!PX_CreateWindow(App.runtime.surface_width,App.runtime.surface_height,App.runtime.window_width,App.runtime.window_height,PX_APPLICATION_NAME)) return 0;
	//////////////////////////////////////////////////////////////////////////
	//CreateThread

	hThread = CreateThread(NULL, 0, DEMO_RenderThreadFunc, 0, 0, &threadId);

	while(PX_SystemLoop()){};

	return 0;
}

#endif

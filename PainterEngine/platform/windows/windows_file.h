
#ifndef PAINTERENGINE_FILE_H
#define PAINTERENGINE_FILE_H

#define PAINTERENGIN_FILE_SUPPORT
//////////////////////////////////////////////////////////////////////////
#include "../../../../../PainterEngine/architecture/PainterEngine_Runtime.h"
#include "stdio.h"
#include "stdlib.h"

typedef struct
{
	px_byte *buffer;
	px_uint size;
}PX_IO_Data;


PX_IO_Data PX_LoadFileToIOData(const char path[]);
int PX_SaveDataToFile(void *buffer,int size,const char path[]);
px_void PX_FreeIOData(PX_IO_Data *io);
#endif


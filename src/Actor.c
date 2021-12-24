#include "Actor.h"

void Actor_GetActorInit() {
	MemFile actorOvl = MemFile_Initialize();
	u32* ptr;
	
	if (MemFile_LoadFile(&actorOvl, "x")) {
		printf_error("Could not load file...");
	}
	
	ptr = actorOvl.data;
	
	while (1) {
		u32 match = 0;
		ActorInit* initData = (ActorInit*)ptr;
		
		if ((ReadBE(initData->init) & 0xFF000000) == 0x80000000) {
			if ((ReadBE(initData->update) & 0xFF000000) == 0x80000000) {
				if ((ReadBE(initData->destroy) & 0xFF000000) == 0x80000000 || initData->destroy == 0) {
					if ((ReadBE(initData->draw) & 0xFF000000) == 0x80000000 || initData->draw == 0) {
						match++;
					}
				}
			}
		}
		
		if (ReadBE(initData->id) <= ACTOR_ID_MAX && ReadBE(initData->objectId) <= OBJECT_ID_MAX)
			match++;
		
		if (initData->category <= ACTORCAT_CHEST)
			match++;
		
		if (!(ReadBE(initData->instanceSize) & 0xFF000000))
			match++;
		
		if (match == 4) {
			break;
		}
		
		ptr++;
	}
	
	ActorInit* initData = (ActorInit*)ptr;
	
	OsPrintfEx("ActorID:    %08X", ReadBE(initData->id));
	OsPrintf("Category:   %08X", ReadBE(initData->category));
	OsPrintf("ObjectID:   %08X", ReadBE(initData->objectId));
	OsPrintf("InitFunc:   %08X", ReadBE(initData->init));
	OsPrintf("DestFunc:   %08X", ReadBE(initData->destroy));
	OsPrintf("UpdateFunc: %08X", ReadBE(initData->update));
	OsPrintf("DrawFunc:   %08X", ReadBE(initData->draw));
	
	MemFile_Free(&actorOvl);
}
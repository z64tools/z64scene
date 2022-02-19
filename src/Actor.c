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
	
	printf_debugExt("ActorID:    %08X", ReadBE(initData->id));
	printf_debug("Category:   %08X", ReadBE(initData->category));
	printf_debug("ObjectID:   %08X", ReadBE(initData->objectId));
	printf_debug("InitFunc:   %08X", ReadBE(initData->init));
	printf_debug("DestFunc:   %08X", ReadBE(initData->destroy));
	printf_debug("UpdateFunc: %08X", ReadBE(initData->update));
	printf_debug("DrawFunc:   %08X", ReadBE(initData->draw));
	
	MemFile_Free(&actorOvl);
}
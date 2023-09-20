#ifndef EXPORT_H
#define EXPORT_H

#include <ext_type.h>

/*
 * S C E N E   T O D O   L I S T
 * | Cmd   | Name                | Optional  | Implemented
 * |───────|─────────────────────|───────────|───────────
 * | 0x00  | SpawnList           |           | Yes
 * | 0x03  | CollisionHeader     |           | Yes
 * | 0x04  | RoomList            |           | Yes
 * | 0x06  | EntranceList        |           | Yes
 * | 0x07  | SpecialFiles        |           | Yes
 * | 0x0D  | PathList            | Yes       | Yes
 * | 0x0E  | TransitionActorList | Yes       | Yes
 * | 0x0F  | LightSettingsList   |           | Yes
 * | 0x11  | SkyboxSettings      |           | Yes
 * | 0x13  | ExitList            | Yes       | Yes
 * | 0x14  | End                 |           | Yes
 * | 0x15  | SoundSettings       |           | Yes
 * | 0x17  | CutsceneData        | Yes       |
 * | 0x18  | AlternateHeaderList | Yes       | Yes
 * | 0x19  | MiscSettings        | Yes       | Yes
 * | 0x1A  | RenderInit          |           |
 */

/*
 * R O O M   T O D O   L I S T
 * | Cmd   | Name                | Optional  | Implemented
 * |───────|─────────────────────|───────────|───────────
 * | 0x01  | ActorList           | Yes       | Yes
 * | 0x05  | WindSettings        |           | Yes
 * | 0x08  | RoomBehaviour       |           | Yes
 * | 0x09  | Undefined9          | Yes       | Yes
 * | 0x0A  | MeshHeader          |           | Yes
 * | 0x0B  | ObjectList          | Yes       | Yes
 * | 0x0C  | LightList           | Yes       | Yes
 * | 0x10  | TimeSettings        |           | Yes
 * | 0x12  | SkyboxDisables      |           | Yes
 * | 0x14  | End                 |           | Yes
 * | 0x16  | EchoSettings        |           | Yes
 * | 0x18  | AlternateHeaderList | Yes       | Yes
 */

struct Scene;
typedef struct Export Export;

Export* Export_New(struct Scene* scene, const char* file);
const char* Export_Scene(Export* this);
const char* Export_Room(Export* this, int roomID);
void Export_Free(Export* this);

#endif

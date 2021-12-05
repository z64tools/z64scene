# z64scene

HLE Zelda 64 scene editor in modern OpenGL

- **GUI**
  - Small box at the bottom-left corner of 3D Viewport that provide information about currently selected actor
    - Index, Param (and the settings)
  - Customizable GUI, Blender is good reference for this
    - Dividable subscreens that can be assigned to certain mode
- **GUI Subscreens**
  - 3D Viewport
    - Option to show camera position based on cutscene timeline?
  - Layers
    - Rooms, Actors
  - Flag Viewer
    - List all flags, customize names
    - Bottom part of this subscreen could show in which rooms currently selected flag is used.
  - Objects
    - Set "keep" objects. For example OoT dungeons use one object for all(?) rooms, as it is used for various actors through the dungeon.
  - Timeline
    - For cutscenes
- **Actor Rendering**
  - Get the segment values from `EnActor_Init`?
  - Additional control with game specific JSON file if automation is not possible.
- **scene.JSON**
  - Room Names
  - Flag Names (Switch, Collectible, etc...)
  
- **OoT.JSON**
  - Actor Variable Settings
    - Provide combo-boxes, checkboxes, etc.
    - Manual DisplayList if `EnActor_Init` or `EnActor_Draw` wont provide required stuff
- **Cutscene Actors**
  - Manually gather all animation segments for some actors to provide better preview?
  - Copy functions used for these actors to move in cutscenes to "emulate" the results as accurately as possible
- **Automation**
  - Object ID's automatic sorting
    - If neighbour rooms have same objects, set them accordingly to prevent reloading those
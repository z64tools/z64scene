#include "Properties.h"

///////////////////////////////////////////////////////////////////////////////

void MenuRoomEnv_Init(Editor* editor, void* __this, Split* split) {
	MenuRoomEnv* this = __this;
	
	Element_Button_SetProperties(&this->buttonGlTime, true, false);
	Element_Button_SetProperties(&this->buttonSkybox, true, false);
	Element_Button_SetProperties(&this->buttonSunMoon, true, false);
	
	this->textboxHour.size = 2;
	this->textboxMin.size = 2;
	this->buttonGlTime.align = NVG_ALIGN_CENTER;
	this->textboxMin.align = NVG_ALIGN_LEFT;
	this->buttonSkybox.align = NVG_ALIGN_CENTER;
	this->buttonSunMoon.align = NVG_ALIGN_CENTER;
	
	Element_Slider_SetParams(&this->sliderTimeSpeed, 0, 0xFE, "int");
	Element_Slider_SetParams(&this->sliderEcho, 0, 0xFF, "int");
	
	Element_Combo_SetArli(&this->comboBeha1, DatabaseScene_GetList(SCENE_BEHAVIOR_1));
	Element_Combo_SetArli(&this->comboBeha2, DatabaseScene_GetList(SCENE_BEHAVIOR_2));
	
	Element_SetNameLerp(&this->sliderTimeSpeed, 0.25f);
	Element_SetNameLerp(&this->sliderEcho, 0.25f);
	Element_SetNameLerp(&this->comboBeha1, 0.25f);
	Element_SetNameLerp(&this->comboBeha2, 0.25f);
	
}

void MenuRoomEnv_Update(Editor* editor, void* __this, Split* split) {
	MenuRoomEnv* this = __this;
	Scene* mainScene = &editor->scene;
	RoomHeader* room = Scene_GetRoomHeader(mainScene, mainScene->curRoom);
	
	Properties_BasicHeader(split);
	
	Element_SetName(&this->buttonGlTime, DatabaseScene_GetName(SCENE_TIME_GLOBAL));
	Element_SetName(&this->buttonSkybox, DatabaseScene_GetName(SCENE_ENABLE_SKYBOX));
	Element_SetName(&this->buttonSunMoon, DatabaseScene_GetName(SCENE_ENABLE_SUNMOON));
	Element_SetName(&this->sliderTimeSpeed, DatabaseScene_GetName(SCENE_TIME_SPEED));
	Element_SetName(&this->sliderEcho, DatabaseScene_GetName(SCENE_ECHO));
	Element_SetName(&this->comboBeha1, DatabaseScene_GetName(SCENE_BEHAVIOR_1));
	Element_SetName(&this->comboBeha2, DatabaseScene_GetName(SCENE_BEHAVIOR_2));
	
	if (Element_Box(BOX_START, &this->panelTime, DatabaseScene_GetName(SCENE_TITLE_ROOM_TIME))) {
		Element_Row(&this->textboxHour, 0.25f, &this->textboxMin, 0.25f, &this->buttonGlTime, 0.5f);
		Element_Row(&this->sliderTimeSpeed, 1.0f);
		
		Element_Condition(&this->buttonGlTime,  room != NULL);
		
		if (Element_Button(&this->buttonGlTime))
			room->timeGlobal = this->buttonGlTime.state;
		else if (room)
			Element_Button_SetProperties(&this->buttonGlTime, true, room->timeGlobal);
		
		Element_Condition(&this->textboxHour,     room != NULL && !room->timeGlobal);
		Element_Condition(&this->textboxMin,      room != NULL && !room->timeGlobal);
		Element_Condition(&this->sliderTimeSpeed, room != NULL);
		
		if (Element_Textbox(&this->textboxHour))
			room->timeHour = clamp(sint(this->textboxHour.txt), 0, 24);
		else if (room)
			Element_Textbox_SetText(&this->textboxHour, x_fmt("%d", room->timeHour));
		
		if (Element_Textbox(&this->textboxMin))
			room->timeMinute = clamp(sint(this->textboxMin.txt), 0, 59);
		else if (room)
			Element_Textbox_SetText(&this->textboxMin, x_fmt("%d", room->timeMinute));
		
		if (Element_Slider(&this->sliderTimeSpeed))
			room->timeSpeed = Element_Slider_GetValue(&this->sliderTimeSpeed);
		else if (room)
			Element_Slider_SetValue(&this->sliderTimeSpeed, room->timeSpeed);
		
	}
	Element_Box(BOX_END, &this->panelTime);
	
	if (Element_Box(BOX_START, &this->panelEnvironment, DatabaseScene_GetName(SCENE_TITLE_ROOM_ENV))) {
		Element_Row(&this->buttonSkybox, 0.5f, &this->buttonSunMoon, 0.5f);
		Element_Row(&this->comboBeha1, 1.0f);
		Element_Row(&this->comboBeha2, 1.0f);
		Element_Row(&this->sliderEcho, 1.0f);
		
		Element_Condition(&this->comboBeha1,    room != NULL);
		Element_Condition(&this->comboBeha2,    room != NULL);
		Element_Condition(&this->buttonSkybox,  room != NULL);
		Element_Condition(&this->buttonSunMoon, room != NULL);
		Element_Condition(&this->sliderEcho,    room != NULL);
		
		if (Element_Button(&this->buttonSkybox))
			room->skyBox.disableSky = !this->buttonSkybox.state;
		else if (room)
			Element_Button_SetProperties(&this->buttonSkybox, true, !room->skyBox.disableSky);
		
		if (Element_Button(&this->buttonSunMoon))
			room->skyBox.disableSunMoon = !this->buttonSunMoon.state;
		else if (room)
			Element_Button_SetProperties(&this->buttonSunMoon, true, !room->skyBox.disableSunMoon);
		
		if (Element_Slider(&this->sliderEcho))
			room->echo = Element_Slider_GetValue(&this->sliderEcho);
		else if (room)
			Element_Slider_SetValue(&this->sliderEcho, room->echo);
		
		if (Element_Combo(&this->comboBeha1) > -1)
			room->behaviour.val1 = DatabaseScene_GetKey(SCENE_BEHAVIOR_1);
		else if (room)
			DatabaseScene_SetKey(SCENE_BEHAVIOR_1, room->behaviour.val1);
		
		if (Element_Combo(&this->comboBeha2) > -1)
			room->behaviour.val2 = DatabaseScene_GetKey(SCENE_BEHAVIOR_2);
		else if (room)
			DatabaseScene_SetKey(SCENE_BEHAVIOR_1, room->behaviour.val1);
	}
	Element_Box(BOX_END, &this->panelEnvironment);
	
}

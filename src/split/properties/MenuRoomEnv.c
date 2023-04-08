#include "Properties.h"

///////////////////////////////////////////////////////////////////////////////

void MenuRoomEnv_Init(Editor* editor, void* __this, Split* split) {
    MenuRoomEnv* this = __this;
    
    Element_Name(&this->sliderTimeSpeed, "Speed");
    Element_Name(&this->sliderEcho, "Echo");
    Element_Name(&this->comboBeha1, "Env 1");
    Element_Name(&this->comboBeha2, "Env 2");
    Element_Button_SetProperties(&this->buttonGlTime, "Global Time", true, false);
    Element_Button_SetProperties(&this->buttonSkybox, "Skybox", true, false);
    Element_Button_SetProperties(&this->buttonSunMoon, "Sun & Moon", true, false);
    
    this->textboxHour.size = 2;
    this->textboxMin.size = 2;
    this->buttonGlTime.align = NVG_ALIGN_CENTER;
    this->textboxMin.align = NVG_ALIGN_LEFT;
    this->buttonSkybox.align = NVG_ALIGN_CENTER;
    this->buttonSunMoon.align = NVG_ALIGN_CENTER;
    
    Element_Slider_SetParams(&this->sliderTimeSpeed, 0, 0xFE, "int");
    Element_Slider_SetParams(&this->sliderEcho, 0, 0xFF, "int");
    
    Element_Combo_SetArli(&this->comboBeha1, &this->behaviour1);
    Element_Combo_SetArli(&this->comboBeha2, &this->behaviour2);
    
    Element_SetNameLerp(&this->sliderTimeSpeed, 0.25f);
    Element_SetNameLerp(&this->sliderEcho, 0.25f);
    Element_SetNameLerp(&this->comboBeha1, 0.25f);
    Element_SetNameLerp(&this->comboBeha2, 0.25f);
}

void MenuRoomEnv_Update(Editor* editor, void* __this, Split* split) {
    MenuRoomEnv* this = __this;
    RoomHeader* room = Scene_GetRoomHeader(&editor->scene, editor->scene.curRoom);
    
    Properties_BasicHeader(split);
    
    if (Element_Box(BOX_START, &this->panelTime, "Time")) {
        Element_Row(&this->textboxHour, 0.25f, &this->textboxMin, 0.25f, &this->buttonGlTime, 0.5f);
        Element_Row(&this->sliderTimeSpeed, 1.0f);
        
        Element_Condition(&this->buttonGlTime,  room != NULL);
        
        if (room) {
            Element_Button_SetProperties(&this->buttonGlTime, NULL, true, room->timeGlobal);
            Element_Textbox_SetText(&this->textboxHour, x_fmt("%d", room->timeHour));
            Element_Textbox_SetText(&this->textboxMin, x_fmt("%d", room->timeMinute));
            Element_Slider_SetValue(&this->sliderTimeSpeed, room->timeSpeed);
        }
        
        if (Element_Button(&this->buttonGlTime))
            room->timeGlobal = this->buttonGlTime.state;
        
        Element_Condition(&this->textboxHour,     room != NULL && !room->timeGlobal);
        Element_Condition(&this->textboxMin,      room != NULL && !room->timeGlobal);
        Element_Condition(&this->sliderTimeSpeed, room != NULL);
        
        if (Element_Textbox(&this->textboxHour))
            room->timeHour = clamp(sint(this->textboxHour.txt), 0, 24);
        if (Element_Textbox(&this->textboxMin))
            room->timeMinute = clamp(sint(this->textboxMin.txt), 0, 59);
        if (Element_Slider(&this->sliderTimeSpeed))
            room->timeSpeed = Element_Slider_GetValue(&this->sliderTimeSpeed);
        
    }
    Element_Box(BOX_END, &this->panelTime);
    
    if (Element_Box(BOX_START, &this->panelEnvironment, "Environment")) {
        Element_Row(&this->buttonSkybox, 0.5f, &this->buttonSunMoon, 0.5f);
        Element_Row(&this->comboBeha1, 1.0f);
        Element_Row(&this->comboBeha2, 1.0f);
        Element_Row(&this->sliderEcho, 1.0f);
        
        if (room) {
            Element_Button_SetProperties(&this->buttonSkybox, NULL, true, !room->skyBox.disableSky);
            Element_Button_SetProperties(&this->buttonSunMoon, NULL, true, !room->skyBox.disableSunMoon);
            Element_Slider_SetValue(&this->sliderEcho, room->echo);
            
            Arli_Set(&this->behaviour1, room->behaviour.val1);
            Arli_Set(&this->behaviour2, room->behaviour.val2);
        }
        
        Element_Condition(&this->comboBeha1,    room != NULL);
        Element_Condition(&this->comboBeha2,    room != NULL);
        Element_Condition(&this->buttonSkybox,  room != NULL);
        Element_Condition(&this->buttonSunMoon, room != NULL);
        Element_Condition(&this->sliderEcho,    room != NULL);
        
        if (Element_Button(&this->buttonSkybox))
            room->skyBox.disableSky = !this->buttonSkybox.state;
        if (Element_Button(&this->buttonSunMoon))
            room->skyBox.disableSunMoon = !this->buttonSunMoon.state;
        if (Element_Slider(&this->sliderEcho))
            room->echo = Element_Slider_GetValue(&this->sliderEcho);
        
        if (Element_Combo(&this->comboBeha1))
            room->behaviour.val1 = this->behaviour1.cur;
        if (Element_Combo(&this->comboBeha2))
            room->behaviour.val2 = this->behaviour2.cur;
    }
    Element_Box(BOX_END, &this->panelEnvironment);
}

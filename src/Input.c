#include <z64scene.h>

GlobalContext* pGlobalCtx;

void Input_Init(GlobalContext* globalCtx) {
	pGlobalCtx = globalCtx;
}

void Input_Update(GlobalContext* globalCtx) {
	Input* input = &globalCtx->input;
	
	for (s32 i = 0; i < KEY_MAX; i++) {
		input->key[i].press = (input->key[i].prev == 0 && input->key[i].hold);
		input->key[i].prev = input->key[i].hold;
	}
	
	input->mouse.clickL.press = (input->mouse.clickL.prev == 0 && input->mouse.clickL.hold);
	input->mouse.clickL.prev = input->mouse.clickL.hold;
	input->mouse.clickR.press = (input->mouse.clickR.prev == 0 && input->mouse.clickR.hold);
	input->mouse.clickR.prev = input->mouse.clickR.hold;
	input->mouse.clickMid.press = (input->mouse.clickMid.prev == 0 && input->mouse.clickMid.hold);
	input->mouse.clickMid.prev = input->mouse.clickMid.hold;
	
	if (glfwGetKey(globalCtx->app.mainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(globalCtx->app.mainWindow, true);
	
	/* time since last execution */
	{
		static double last = 0;
		double cur = glfwGetTime();
		input->deltaTimeSec = (cur - last);
		last = cur;
	}
	
	Vec2i* mPos = &input->mouse.pos;
	Vec2i* mVel = &input->mouse.vel;
	Vec2i* mPrev = &input->mouse.prevPos;
	
	mVel->x = mPos->x - mPrev->x;
	mVel->y = mPos->y - mPrev->y;
	*mPrev = *mPos;
}

void Input_KeyCallback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods) {
	Input* input = &pGlobalCtx->input;
	int hold = action != GLFW_RELEASE;
	s32 pressed = action == GLFW_RELEASE;
	
	input->key[key].hold = hold;
}

void Input_CursorCallback(GLFWwindow* window, f64 xpos, f64 ypos) {
	Input* input = &pGlobalCtx->input;
	
	input->mouse.pos.x = xpos;
	input->mouse.pos.y = ypos;
}

void Input_MouseClickCallback(GLFWwindow* window, s32 button, s32 action, s32 mods) {
	Input* input = &pGlobalCtx->input;
	s32 pressed = action != GLFW_RELEASE;
	s32 hold = action == GLFW_PRESS;
	
	switch (button) {
	    case GLFW_MOUSE_BUTTON_RIGHT:
		    input->mouse.clickR.hold = hold;
		    break;
		    
	    case GLFW_MOUSE_BUTTON_LEFT:
		    input->mouse.clickL.hold = hold;
		    break;
	    case GLFW_MOUSE_BUTTON_MIDDLE:
		    input->mouse.clickMid.hold = hold;
	}
}
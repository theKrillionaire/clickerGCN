#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

enum ROOMS {
	HOUSE,
	SHOP,
	STARTSCREEN
};

void *Initialise();
void quit(int code) {
	printf("\n\n\n\n\n\n\n\n\nQuitting game! thanks for playing! X3\n");
	int timer = 0;
	for(int i = 0; i < 20; i++) {
		VIDEO_WaitVSync();
	}
	exit(code);
}
void win(int* clicks, int* cpe) {
	consoleClear();
	printf("good job!! you beat the game with %i clicks, at %i Clicks per Press!",*clicks, *cpe);
	quit(0);
}

void screenTransition(int room) {
	char text[7];
	if(room == HOUSE) strncpy(text,"HOUSE \0",7);
	else if(room == SHOP) strncpy(text," SHOP \0",7);
	else if(room == STARTSCREEN) strncpy(text, " GAME \0",7);
	int i = 0;
	consoleClear();
	printf("\n\n\n\n\n\n\n\n\n\n\n                                  moving to\n                                    %s",text);
	for(int i = 0; i < 20; i++) {
		VIDEO_WaitVSync();
	}
}

void drawScreen(int* clicks, int* cpe, int* room, int* menuOption, char menuOptions[][36]) {
	consoleClear();
	if(*room == HOUSE) { 
		printf("HOUSE\n\nClicks: %i, CpP: %i", *clicks, *cpe); 
	}
	else if(*room == SHOP) {
		printf("SHOP\n\n\nClicks: %i, CpP: %i\n\n", *clicks, *cpe);
		for(int i = 0; i < 5; i++) {
			if(i == *menuOption) printf("> ");
			printf(menuOptions[i]);
		}
	}
	else if(*room == STARTSCREEN) {
		printf("\nWelcome to Krillionaire's terrible bad GCN clicker game!!\n\nPress ANY Button on the HOUSE screen to click\nPress left or right to go to different screens\nPress up or down on the SHOP screen to select, and press A to confirm.\n\nPress START now to begin!");
	}
}


int main(int argc, char **argv) {

	xfb = Initialise();
	
	int movedStickAlready = false;
	int room = STARTSCREEN;
	int clicks = 0;
	int cpe = 1;
	int menuOption = 0;
	char menuOptions[5][36] = {
		"Upgrade 1: +1 CpE, Cost: 50\n",
		"Upgrade 2: +2 CpE, Cost: 100\n",
		"Upgrade 3: +10 CpE, Cost: 500\n",
		"Upgrade 4: +100 CpE, Cost: 5,000\n",
		"WIN!!! Cost: 1,024,024"
	};
	
	while(SYS_MainLoop()) {
		VIDEO_WaitVSync();
		PAD_ScanPads();
		
		// joy controls here ↓ :333
		int joy_YL = PAD_StickY(0);
		int joy_XL = PAD_StickX(0);
		if(joy_YL <= -10 && room == SHOP && !movedStickAlready) {
			menuOption++;
		} else if(joy_YL >= 10 && room == SHOP && !movedStickAlready) {
			menuOption--;
		}
		if(menuOption < 0) menuOption = 4;
		else if(menuOption > 4) menuOption = 0;
		if(joy_YL <= -10 || joy_YL >= 10 || joy_XL <= -10 || joy_XL >= 10) movedStickAlready = true;
		else movedStickAlready = false;
		
		if(joy_XL <= -10 && room == HOUSE) { 
			screenTransition(SHOP); 
			room = SHOP; 
		}
		else if(joy_XL >= 10 && room == SHOP) { 
			screenTransition(HOUSE); 
			room = HOUSE; 
		}
		
		// button controls here ↓ :3
		int buttonsDown = PAD_ButtonsDown(0);
		if(room == HOUSE) {
			if( buttonsDown & (PAD_BUTTON_A | PAD_BUTTON_B | PAD_BUTTON_X | PAD_BUTTON_Y) ) {
			clicks += cpe;
			} 
		} else if(room == SHOP) {
			if (buttonsDown &PAD_BUTTON_A) {
				if(menuOption == 0 && clicks >= 1) { cpe += 100; clicks -= 200; }
				else if(menuOption == 1 && clicks >= ) { cpe += 500; clicks -= 1000; }
				else if(menuOption == 2 && clicks >= 2) { cpe += 1000; clicks -= 2000; }
				else if(menuOption == 3 && clicks >= 10) { cpe += 10000; clicks -= 20000; }
				else if(menuOption == 4 && clicks >= 1024024) { win(&clicks, &cpe); }
			}
		}
		if(room == STARTSCREEN) {
			if (buttonsDown & PAD_BUTTON_START) {
				screenTransition(STARTSCREEN); 
				room = HOUSE;
			}
		} else {
			if (buttonsDown & PAD_BUTTON_START) {
				quit(0);
			}
		}
		drawScreen(&clicks, &cpe, &room, &menuOption, menuOptions);
	}

	return 0;
}

void * Initialise() {

	void *framebuffer;

	VIDEO_Init();
	PAD_Init();
	
	rmode = VIDEO_GetPreferredMode(NULL);

	framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(framebuffer,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(framebuffer);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	return framebuffer;

}

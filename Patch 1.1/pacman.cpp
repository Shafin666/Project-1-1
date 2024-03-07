# include "iGraphics.h"
# include <math.h>
# include <windows.h>
# include <mmsystem.h>
# pragma comment(lib,"winmm.lib")

const int d = 40;

//---------------
#define MENU 1
#define LEADERBOARD 2
#define INGAME_LVL_1 3
#define INGAME_LVL_2 4
#define INGAME_LVL_3 5
#define NAME_INPUT 6
#define TRANSITION 7
int GAMESTATE = 1;
//---------------

// essential functions
double min(double a, double b) { return (a < b)? a : b; }

double max(double a, double b) { return (a > b)? a : b; }

int toAxis_x(int x, int y) { return y * 16 + d; }
int toAxis_y(int x, int y) { return 512 - x * 16 + d; }

int toArray_x(int x, int y) { return (512 - (y - d)) / 16; }
int toArray_y(int x, int y) { return (x - d) / 16; }

char *toString(int n) {
	int len = 0, m = n;
	while(m) { len++; m /= 10; }
	char *str = (char *) malloc(len + 1);
	for(int i = 0; i < len; i++, n /= 10)
		str[len - i - 1] = n % 10 + '0';
	
	str[len] = '\0';
	return str;
}

bool hit(int i, int j, int player);
void shortestPath(void);
void showPacman(void);
void movePacman(void);
void moveGhost(void);

//-----------------------------------

int score[3];
int khawa[40][40][3];
int ghost_x[4], ghost_y[4];
int dist[40][40][3];
int last[4] = {-1, -1, -1, -1};

int giveUp;

int mouse_x, mouse_y;

int pacman_x = 16 + d, pacman_y = 512 - 30 * 16 + d;
bool pacman_mouth_toggle = 0;

int pacman_dir = 0; // 0 = left, 1 = up, 2 = right, 3 = down;
int buffer = 5;

int WIN = 0, LOSE = 0;

char grid[40][40][3];
int okay[40][40][3];

// timers
int pacman_mouth_t, pacman_motion_t;
int ghost_motion_t[4];

// ------------------------------

//motion
int forward_right = 0, forward_up = 0;
// -------------------------------


//music
int menuMusic = 0;
int deathMusic = 0, winMusic = 0;
int hitMusic = 0;
int musicLvl[3];
//--------------------------------

int hpLvl[3] = {2, 2, 2};
int showCherry[3];

int timeShuru[3], timeNow[3];

// MENU
#define MENU_START 1
#define MENU_DIFFICULTY 2
#define MENU_LEADERBOARD 3
#define MENU_EXIT 4
#define SUBMENU_EASY 5
#define SUBMENU_MEDIUM 6
#define SUBMENU_HARD 7

int selectedOption = MENU_START;
int submenuSelectedOption = 0;
int insideSubmenu = 0;
int numGhost = 1;


typedef struct Portal {
	int x, y;
};
Portal portals[4];

void drawMenu() {
    iClear();

	iShowBMP(8, 0, "assets/menu_bg.bmp");

    iSetColor(255, 255, 255);

    if (insideSubmenu) {
        iText(480, 275, "EASY", (submenuSelectedOption == SUBMENU_EASY) ? GLUT_BITMAP_HELVETICA_18 : GLUT_BITMAP_HELVETICA_12);
        iText(480, 245, "MEDIUM", (submenuSelectedOption == SUBMENU_MEDIUM) ? GLUT_BITMAP_HELVETICA_18 : GLUT_BITMAP_HELVETICA_12);
        iText(480, 215, "HARD", (submenuSelectedOption == SUBMENU_HARD) ? GLUT_BITMAP_HELVETICA_18 : GLUT_BITMAP_HELVETICA_12);
    }

	int r = 0, g = 0, b = 0;
	if(submenuSelectedOption == SUBMENU_EASY) g = 255;
	else if(submenuSelectedOption == SUBMENU_HARD) r = 255;
	else if(submenuSelectedOption == SUBMENU_MEDIUM) g = 255, r = 255;

	iSetColor(r, g, b);
	iFilledCircle(450, 250, 10, 10);

    iSetColor(255, 0, 0);
    switch (selectedOption) {
        case MENU_START:
			iShowBMP(223, 668 - 381, "assets/menu_bg_start.bmp");
            break;
        case MENU_DIFFICULTY:
			iShowBMP(168 + 8, 668 - 449, "assets/menu_bg_difficulty.bmp");
            break;
        case MENU_LEADERBOARD:
			iShowBMP(154 + 8, 668 - 516, "assets/menu_bg_leaderboard.bmp");
            break;
        case MENU_EXIT:
			iShowBMP(239 + 8, 668 - 578, "assets/menu_bg_exit.bmp");
            break;
    }
}

#define MAX_NAME_LENGTH 50
#define MAX_ENTRIES 10

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
} Entry;


void swapEntries(Entry *entry1, Entry *entry2) {
    Entry temp;

    // Swap names
    strcpy(temp.name, entry1->name);
    strcpy(entry1->name, entry2->name);
    strcpy(entry2->name, temp.name);

    // Swap scores
    temp.score = entry1->score;
    entry1->score = entry2->score;
    entry2->score = temp.score;
}

void showLeaderBoard() {
	iClear();

	iShowBMP(0, 0, "assets\\leaderboard.bmp");

	iSetColor(255, 255, 255);

	Entry entries[MAX_ENTRIES];
    int numEntries = 0;

    FILE *file = fopen("data.txt", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    while (fscanf(file, "%s %d", entries[numEntries].name, &entries[numEntries].score) == 2) {
        numEntries++;
        if (numEntries >= MAX_ENTRIES) {
            printf("Maximum number of entries reached.\n");
            break;
        }
    }

    fclose(file);

	for(int i = 0; i < numEntries; i++) {
		for(int j = i+1; j < numEntries; j++) {
			if(entries[i].score < entries[j].score)
				swapEntries(&entries[i], &entries[j]);
		}
	}


	for (int i = 0, x = 180, y = 468; i < numEntries; i++, y -= 46) {
		iText(x, y, entries[i].name, GLUT_BITMAP_TIMES_ROMAN_24);
		iText(x + 270, y, toString(entries[i].score), GLUT_BITMAP_TIMES_ROMAN_24);
    }
}

char cs[15], s[15];

void level() {
	iClear();

	if(score[0] == 302) {
		iClear();

		GAMESTATE = TRANSITION;
		return;
	}

	if(LOSE) {
		iClear();

		score[1] = score[2] = 0;
		timeShuru[1] = timeShuru[2] = 0;
		GAMESTATE = NAME_INPUT;

		return;
	}
	
	for(int i = 0; i < numGhost; i++) {

		if(hit(i, 0, 1)) {

			if(!hitMusic) {
				PlaySound(TEXT("music\\hit.wav"), NULL, SND_ASYNC);
				hitMusic = 1;
				musicLvl[0] = 0;
			}

			hpLvl[0]--;

			if(hpLvl[0] == 0) {
				LOSE = 1;
				return;
			}
			else {
				pacman_x = toAxis_x(30, 1);
				pacman_y = toAxis_y(30, 1);
				pacman_dir = 0;
				forward_right = forward_up = 0;
			}
		}
	}


	iShowBMP(0, 0, "assets/level_bg_1.bmp");

	if(showCherry[0] == 1) {
		iShowBMP(toAxis_x(16, 16), toAxis_y(16, 16), "assets/cherry.bmp");
	}

	if(hpLvl[0] > 0) iShowBMP(350, 18, "assets/heart.bmp");
	if(hpLvl[0] > 1) iShowBMP(400, 18, "assets/heart.bmp");
	if(hpLvl[0] > 2) iShowBMP(450, 18, "assets/heart.bmp");

	iText(492, 600, toString(score[0]), GLUT_BITMAP_TIMES_ROMAN_24);

	sprintf(cs, "%0*d", 2, timeNow[0] % 100);
	sprintf(s, "%d", timeNow[0] / 100);

	strcat(s, ".");
	strcat(s, cs);
	iText(90, 600, s, GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++) {
			if(!khawa[i][j][0])
				iPoint(toAxis_x(i, j) + 16, toAxis_y(i, j) + 16, 2);
		}
	}

	if (pacman_mouth_toggle) {
		if (pacman_dir == 0) iShowBMP(pacman_x, pacman_y, "assets/pacman_open_right.bmp");
		if (pacman_dir == 1) iShowBMP(pacman_x, pacman_y, "assets/pacman_open_up.bmp");
		if (pacman_dir == 2) iShowBMP(pacman_x, pacman_y, "assets/pacman_open_left.bmp");
		if (pacman_dir == 3) iShowBMP(pacman_x, pacman_y, "assets/pacman_open_down.bmp");
	}
	else {
		if (pacman_dir == 0) iShowBMP(pacman_x, pacman_y, "assets/pacman_close_right.bmp");
		if (pacman_dir == 1) iShowBMP(pacman_x, pacman_y, "assets/pacman_close_up.bmp");
		if (pacman_dir == 2) iShowBMP(pacman_x, pacman_y, "assets/pacman_close_left.bmp");
		if (pacman_dir == 3) iShowBMP(pacman_x, pacman_y, "assets/pacman_close_down.bmp");
	}

	if(numGhost > 0) iShowBMP(ghost_x[0], ghost_y[0], "assets/bhoot_0.bmp");
	if(numGhost > 1) iShowBMP(ghost_x[1], ghost_y[1], "assets/bhoot_1.bmp");
	if(numGhost > 2) iShowBMP(ghost_x[2], ghost_y[2], "assets/bhoot_2.bmp");
	if(numGhost > 3) iShowBMP(ghost_x[3], ghost_y[3], "assets/bhoot_3.bmp");

}

void level2() {
	iClear();

	iSetColor(20, 210, 60);

	if(score[1] == 292) {
		iClear();

		GAMESTATE = TRANSITION;

		return;
	}

	if(LOSE) {
		iClear();

		GAMESTATE = NAME_INPUT;

		return;
	}
	
	for(int i = 0; i < numGhost; i++) {

		if(hit(i, 0, 1)) {

			if(!hitMusic) {
				PlaySound(TEXT("music\\hit.wav"), NULL, SND_ASYNC);
				hitMusic = 1;
				musicLvl[1] = 0;
			}

			hpLvl[1]--;

			if(hpLvl[1] == 0) {
				LOSE = 1;
				return;
			}
			else {
				pacman_x = toAxis_x(30, 1);
				pacman_y = toAxis_y(30, 1);
				pacman_dir = 0;
				forward_right = forward_up = 0;
			}

			// LOSE = 1;
			// return;
		}
	}

	iShowBMP(0, 0, "assets/level_bg_2.bmp");

	if(showCherry[1] == 1) {
		iShowBMP(toAxis_x(16, 16), toAxis_y(16, 16), "assets/cherry.bmp");
	}

	if(hpLvl[1] > 0) iShowBMP(350, 18, "assets/heart.bmp");
	if(hpLvl[1] > 1) iShowBMP(400, 18, "assets/heart.bmp");
	if(hpLvl[1] > 2) iShowBMP(450, 18, "assets/heart.bmp");

	iText(500, 600, toString(score[1]), GLUT_BITMAP_TIMES_ROMAN_24);

	sprintf(cs, "%0*d", 2, timeNow[1] % 100);
	sprintf(s, "%d", timeNow[1] / 100);

	strcat(s, ".");
	strcat(s, cs);
	iText(90, 600, s, GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++) {
			if(!khawa[i][j][1])
				iPoint(toAxis_x(i, j) + 16, toAxis_y(i, j) + 16, 2);
		}
	}

	if (pacman_mouth_toggle) {
		if (pacman_dir == 0) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_open_right.bmp");
		if (pacman_dir == 1) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_open_up.bmp");
		if (pacman_dir == 2) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_open_left.bmp");
		if (pacman_dir == 3) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_open_down.bmp");
	}
	else {
		if (pacman_dir == 0) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_close_right.bmp");
		if (pacman_dir == 1) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_close_up.bmp");
		if (pacman_dir == 2) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_close_left.bmp");
		if (pacman_dir == 3) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_close_down.bmp");
	}

	if(numGhost > 0) iShowBMP(ghost_x[0], ghost_y[0], "assets/bhoot_0.bmp");
	if(numGhost > 1) iShowBMP(ghost_x[1], ghost_y[1], "assets/bhoot_1.bmp");
	if(numGhost > 2) iShowBMP(ghost_x[2], ghost_y[2], "assets/bhoot_2.bmp");
	if(numGhost > 3) iShowBMP(ghost_x[3], ghost_y[3], "assets/bhoot_3.bmp");
}

void level3() {
	iClear();

	iSetColor(150, 130, 10);

	if(score[2] == 303) {
		iClear();

		WIN = 1;
		GAMESTATE = NAME_INPUT;

		return;
	}

	if(LOSE) {
		iClear();

		GAMESTATE = NAME_INPUT;

		return;
	}
	
	for(int i = 0; i < numGhost; i++) {

		if(hit(i, 0, 1)) {
			if(!hitMusic) {
				PlaySound(TEXT("music\\hit.wav"), NULL, SND_ASYNC);
				hitMusic = 1;
				musicLvl[2] = 0;
			}

			hpLvl[2]--;

			if(hpLvl[2] == 0) {
				LOSE = 1;
				return;
			}
			else {
				pacman_x = toAxis_x(30, 1);
				pacman_y = toAxis_y(30, 1);
				pacman_dir = 0;
				forward_right = forward_up = 0;
			}

			// LOSE = 1;
			// return;
		}
	}

	iShowBMP(0, 0, "assets/level_bg_3.bmp");

	if(showCherry[2] == 1) {
		iShowBMP(toAxis_x(16, 16), toAxis_y(16, 16), "assets/cherry.bmp");
	}

	if(hpLvl[2] > 0) iShowBMP(350, 18, "assets/heart.bmp");
	if(hpLvl[2] > 1) iShowBMP(400, 18, "assets/heart.bmp");
	if(hpLvl[2] > 2) iShowBMP(450, 18, "assets/heart.bmp");

	iText(500, 600, toString(score[2]), GLUT_BITMAP_TIMES_ROMAN_24);

	sprintf(cs, "%0*d", 2, timeNow[2] % 100);
	sprintf(s, "%d", timeNow[2] / 100);

	strcat(s, ".");
	strcat(s, cs);
	iText(90, 600, s, GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++) {
			if(!khawa[i][j][2])
				iPoint(toAxis_x(i, j) + 16, toAxis_y(i, j) + 16, 2);

			if(grid[i][j][2] == 'O')
				iFilledRectangle(toAxis_x(i, j), toAxis_y(i, j), 20, 28);
		}
	}

	if (pacman_mouth_toggle) {
		if (pacman_dir == 0) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_open_right.bmp");
		if (pacman_dir == 1) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_open_up.bmp");
		if (pacman_dir == 2) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_open_left.bmp");
		if (pacman_dir == 3) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_open_down.bmp");
	}
	else {
		if (pacman_dir == 0) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_close_right.bmp");
		if (pacman_dir == 1) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_close_up.bmp");
		if (pacman_dir == 2) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_close_left.bmp");
		if (pacman_dir == 3) iShowBMP(pacman_x+2, pacman_y+1, "assets/pacman_close_down.bmp");
	}

	if(numGhost > 0) iShowBMP(ghost_x[0], ghost_y[0], "assets/bhoot_0.bmp");
	if(numGhost > 1) iShowBMP(ghost_x[1], ghost_y[1], "assets/bhoot_1.bmp");
	if(numGhost > 2) iShowBMP(ghost_x[2], ghost_y[2], "assets/bhoot_2.bmp");
	if(numGhost > 3) iShowBMP(ghost_x[3], ghost_y[3], "assets/bhoot_3.bmp");

	for(int i = 0; i < 4; i++) {
		if(i & 1) iShowBMP(toAxis_x(portals[i].x, portals[i].y), toAxis_y(portals[i].x, portals[i].y), "assets/portal_left.bmp");
		else iShowBMP(toAxis_x(portals[i].x, portals[i].y), toAxis_y(portals[i].x, portals[i].y), "assets/portal_right.bmp");
	}
}


void resetGhost() {
	ghost_x[0] = toAxis_x(18, 13);
	ghost_y[0] = toAxis_y(18, 13);

	ghost_x[1] = toAxis_x(18, 16);
	ghost_y[1] = toAxis_y(18, 16);

	ghost_x[2] = toAxis_x(15, 16);
	ghost_y[2] = toAxis_y(15, 16);

	ghost_x[3] = toAxis_x(15, 13);
	ghost_y[3] = toAxis_y(15, 13);
}

char str[100], str2[100];
int len;
int mode;

void drawTextBox()
{
	iSetColor(150, 150, 150);
	iRectangle(190, 290, 250, 30);

	iText(190, 340, "Enter your Nickname: ", GLUT_BITMAP_9_BY_15);
}

void iDraw() {
	iClear();

    if(GAMESTATE == MENU) {
        drawMenu();
        return;
    }

	if(GAMESTATE == LEADERBOARD) {
		showLeaderBoard();
		return;
	}

	if(GAMESTATE == INGAME_LVL_1) {
		if (submenuSelectedOption == SUBMENU_EASY) numGhost = 1;
		else if (submenuSelectedOption == SUBMENU_MEDIUM) numGhost = 2;
		else if (submenuSelectedOption == SUBMENU_HARD) numGhost = 3;

		if(!timeShuru) {
			timeShuru[0] = 1;
			timeNow[0] = 0;
		}
		giveUp = 0;

		if(!musicLvl[0]) {
			if(hitMusic) Sleep(1000);

			menuMusic = 0;
			hitMusic = 0;

			PlaySound(TEXT("music\\level_1_bgm.wav"), NULL, SND_LOOP | SND_ASYNC);
			musicLvl[0] = 1;
		}

		level();
		return;
	}

	if(GAMESTATE == INGAME_LVL_2) {
		if (submenuSelectedOption == SUBMENU_EASY) numGhost = 1;
		else if (submenuSelectedOption == SUBMENU_MEDIUM) numGhost = 2;
		else if (submenuSelectedOption == SUBMENU_HARD) numGhost = 3;

		if(!timeShuru[1]) {
			timeShuru[1] = 1;
			timeNow[1] = 0;
		}
		giveUp = 0;

		if(!musicLvl[1]) {
			if(hitMusic) Sleep(1000);

			menuMusic = 0;
			hitMusic = 0;

			PlaySound(TEXT("music\\level_2_bgm.wav"), NULL, SND_LOOP | SND_ASYNC);
			musicLvl[1] = 1;
		}

		level2();
		return;
	}

	if(GAMESTATE == INGAME_LVL_3) {
		if (submenuSelectedOption == SUBMENU_EASY) numGhost = 2;
		else if (submenuSelectedOption == SUBMENU_MEDIUM) numGhost = 3;
		else if (submenuSelectedOption == SUBMENU_HARD) numGhost = 4;

		if(!timeShuru[2]) {
			timeShuru[2] = 1;
			timeNow[2] = 0;
		}
		giveUp = 0;

		if(!musicLvl[2]) {
			if(hitMusic) Sleep(1000);

			menuMusic = 0;
			hitMusic = 0;

			PlaySound(TEXT("music\\level_3_bgm.wav"), NULL, SND_LOOP | SND_ASYNC);
			musicLvl[2] = 1;
		}

		level3();
		return;
	}

	if(GAMESTATE == NAME_INPUT) {
		iClear();

		if(WIN) {
			if(!winMusic) {
				PlaySound(TEXT("music\\youwin.wav"), NULL, SND_ASYNC);
				winMusic = 1;
			}
			iShowBMP(0, 0, "assets\\win.bmp");
		}
		else {
			if(!deathMusic) {
				PlaySound(TEXT("music\\youdied.wav"), NULL, SND_ASYNC);
				deathMusic = 1;
			}

			if(!timeShuru[1]) iShowBMP(0, 0, "assets\\death_1.bmp");
			else if(!timeShuru[2]) iShowBMP(0, 0, "assets\\death_2.bmp");
			else iShowBMP(0, 0, "assets\\death_3.bmp");
		}

		mode = 1;
		if(mode == 1)
		{
			iSetColor(255, 255, 255);
			iText(250, 325, str, GLUT_BITMAP_9_BY_15);
		}

	}

	if(GAMESTATE == TRANSITION) {
		iShowBMP(0, 0, "assets/transition.bmp");
	}
}

void iMouseMove(int mx, int my) {
	printf("x = %d, y= %d\n",mx,my);

	mouse_x = mx;
	mouse_y = my;
}

void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		
		pacman_x = mx;
		pacman_y = my;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

	}

	if(mx >= 50 && mx <= 300 && my >= 250 && my <= 280 && mode == 0)
	{
		mode = 1;
	}
}

void iKeyboard(unsigned char key) {
    if(GAMESTATE == MENU) {

		if(key == 'l') GAMESTATE = NAME_INPUT;

        if(key == 27) {
            if (!insideSubmenu) {
                exit(0);
            } 
			else {
                selectedOption = MENU_DIFFICULTY;
                insideSubmenu = 0;
                submenuSelectedOption = 0;
                // drawMenu(); 
            }
        }

        if(key == 13) {
			if(insideSubmenu) {
				selectedOption = MENU_DIFFICULTY;
                insideSubmenu = 0;
				return;
			}

            switch (selectedOption) {
                case MENU_START:
                    printf("Starting game...\n");
					GAMESTATE = INGAME_LVL_1;
					// loadLevel();
                    break;
                case MENU_DIFFICULTY:
                    printf("Changing difficulty...\n");

                    insideSubmenu = 1;
                    submenuSelectedOption = SUBMENU_EASY;

                    break;
                case MENU_LEADERBOARD:

					GAMESTATE = LEADERBOARD;
                    printf("Viewing leaderboard...\n");
                    break;
                case MENU_EXIT:
                    exit(0);
                    break;
            }
        }
    }

	if(GAMESTATE == LEADERBOARD) {
		if(key == 27) {
			insideSubmenu = 0;
			selectedOption = MENU_LEADERBOARD;
			submenuSelectedOption = 0;
			GAMESTATE = MENU;
			
			return;
		}
	}

	if(GAMESTATE == NAME_INPUT) {
		int i;
		if(mode == 1)
		{
			if(key == '\r')
			{
				mode = 0;
				strcpy(str2, str);
				// printf("%s\n", str2);

				int totalScore = 0;
				if(WIN) totalScore = 18000 - timeNow[0]/100 - timeNow[1]/100 - timeNow[2]/100;
				else totalScore = 500 + 5 * timeNow[0]/100 + 10 * score[0] + timeNow[1]/100 * 8 + score[1] * 12 + timeNow[2]/100 * 10 + score[2] * 15;

				totalScore += giveUp * 500;

				printf("%d %d %d %d %d %d\n", score[0], timeNow[0], score[1], timeNow[1], score[2], timeNow[2]);

				FILE* file = fopen("data.txt", "a");
				fprintf(file, "%s %d\n", str2, totalScore);
				fclose(file);

				for(i = 0; i < len; i++) str[i] = 0;
				len = 0;

				GAMESTATE = LEADERBOARD;
			}
			else str[len++] = key;
		}
	}
	
	if(GAMESTATE == TRANSITION) {
		if(key == 'y') {
			if(timeNow[1]) GAMESTATE = INGAME_LVL_3;
			else if(timeNow[0]) GAMESTATE = INGAME_LVL_2;
		}
		else {
			LOSE = 1;
			giveUp = 1;
			GAMESTATE = NAME_INPUT;
		}
	}
}

void iSpecialKeyboard(unsigned char key) {

	if (key == GLUT_KEY_END) {
		exit(0);
	}

    switch (GAMESTATE) 
    {
    case MENU:
        switch (key) {
        case GLUT_KEY_UP:
            if (insideSubmenu)
                submenuSelectedOption = (submenuSelectedOption == SUBMENU_EASY) ? SUBMENU_HARD : submenuSelectedOption - 1;
            else
                selectedOption = (selectedOption == MENU_START) ? MENU_EXIT : selectedOption - 1;

            break;
       
	    case GLUT_KEY_DOWN:
            if (insideSubmenu)
                submenuSelectedOption = (submenuSelectedOption == SUBMENU_HARD) ? SUBMENU_EASY : submenuSelectedOption + 1;
			else 
                selectedOption = (selectedOption == MENU_EXIT) ? MENU_START : selectedOption + 1;
            
			break;
    }

    case (INGAME_LVL_1):
    case (INGAME_LVL_2):
    case (INGAME_LVL_3):
        if (key == GLUT_KEY_RIGHT) {
            // forward_up = 0;
            // forward_right = 1;

			buffer = 0;
        }
        
        if (key == GLUT_KEY_LEFT) {
            // forward_up = 0;
            // forward_right = -1;

			buffer = 2;
        }

        if(key == GLUT_KEY_UP) {
            // forward_right = 0;
            // forward_up = 1;

			buffer = 1;
        }

        if(key == GLUT_KEY_DOWN) {
            // forward_right = 0;
            // forward_up = -1;

			buffer = 3;
        }
        break;
    }
    
}

void showPacman(void) {
	pacman_mouth_toggle ^= 1;
}

int last_right, last_up;

void movePacman(void) {

	int lvl = 0;
	if(GAMESTATE == INGAME_LVL_1) lvl = 0;
	if(GAMESTATE == INGAME_LVL_2) lvl = 1;
	if(GAMESTATE == INGAME_LVL_3) lvl = 2;

	if(buffer == 0) forward_right = 1, forward_up = 0;
	else if(buffer == 1) forward_right = 0, forward_up = 1;
	else if(buffer == 2) forward_right = -1, forward_up = 0;
	else if(buffer == 3) forward_right = 0, forward_up = -1;

	pacman_x += 16 * forward_right;
	pacman_y += 16 * forward_up;

	int here_x = toArray_x(pacman_x, pacman_y);
	int here_y = toArray_y(pacman_x, pacman_y);

	if(!okay[here_x][here_y][lvl]) {
		pacman_x -= 16 * forward_right;
		pacman_y -= 16 * forward_up;

		pacman_x += 16 * last_right;
		pacman_y += 16 * last_up;

		here_x = toArray_x(pacman_x, pacman_y);
		here_y = toArray_y(pacman_x, pacman_y);

		if(!okay[here_x][here_y][lvl]) {
			pacman_x -= 16 * last_right;
			pacman_y -= 16 * last_up;
		}
	}
	else {
		last_right = forward_right;
		last_up = forward_up;
		buffer = 5;
	}

	if(last_right > 0) pacman_dir = 0;
	if(last_right < 0) pacman_dir = 2;
	if(last_up > 0) pacman_dir = 1;
	if(last_up < 0) pacman_dir = 3;

	if(pacman_x < d + 1) pacman_x = 512 + d - 32;
	if(pacman_x > 512 + d - 32) pacman_x = d;

	here_x = toArray_x(pacman_x, pacman_y);
	here_y = toArray_y(pacman_x, pacman_y);

	score[lvl] += !khawa[here_x][here_y][lvl];
	khawa[here_x][here_y][lvl] = 1;

	if(score[lvl] == 100 && showCherry[lvl] == 0) showCherry[lvl] = 1;

	if(here_x == 16 && here_y == 16 && showCherry[lvl]) {
		showCherry[lvl] = 2; //eaten
		hpLvl[lvl]++;
	}
}

bool hit(int i, int j, int player) {
	int ax1 = ghost_x[i];
	int ax2 = ax1 + 28;
	int ay1 = ghost_y[i];
	int ay2 = ghost_y[i] + 28;

	int bx1 = ghost_x[j];
	int bx2 = bx1 + 28;
	int by1 = ghost_y[j];
	int by2 = ghost_y[j] + 28;

	if(player) {
		bx1 = pacman_x;
		bx2 = bx1 + 28;
		by1 = pacman_y;
		by2 = by1 + 28;
	}

	int ret = 1;
	ret &= (ax1 <= bx2);
	ret &= (ax2 >= bx1);
	ret &= (ay1 <= by2);
	ret &= (ay2 >= by1);

	return ret;
}

void moveGhost(void) {
	int lvl;
	if(GAMESTATE == INGAME_LVL_1) lvl = 0;
	if(GAMESTATE == INGAME_LVL_2) lvl = 1;
	if(GAMESTATE == INGAME_LVL_3) lvl = 2;

	for(int j = 0; j < 4; j++) {
		int dx[4] = {0, 1, 0, -1};
		int dy[4] = {1, 0, -1, 0};

		int now_x = toArray_x(ghost_x[j], ghost_y[j]);
		int now_y = toArray_y(ghost_x[j], ghost_y[j]);

		for(int iter = 0; iter < 20; iter++) {
			int p = rand() % 4;

			if(rand() % 3 == 0) {
				int mn = 500;
				for(int i = 0; i < 4; i++) {
					if(dist[now_x + dx[i]][now_y + dy[i]][lvl] <= mn) {
						if(now_x + dx[i] < 0) continue;
						if(now_y + dy[i] < 0) continue;

						mn = dist[now_x + dx[i]][now_y + dy[i]][lvl];
						p = i;
					}
				}
			}

			if(now_x + dx[p] < 0 || now_x + dx[p] > 30) continue;
			if(now_y + dy[p] < 0 || now_y + dy[p] > 30) continue;
			
			ghost_x[j] = toAxis_x(now_x + dx[p], now_y + dy[p]);
			ghost_y[j] = toAxis_y(now_x + dx[p], now_y + dy[p]);

			int flag = 0;
			for(int i = 0; i < 4; i++) {
				if(i == j) continue;
				if(hit(i, j, 0)) flag = 1;
			}
			
			if(flag) {
				ghost_x[j] = toAxis_x(now_x, now_y);
				ghost_y[j] = toAxis_y(now_x, now_y);
				last[j] = (last[j] + 2) % 4;
				break;
			}

			if(okay[now_x + dx[p]][now_y + dy[p]][lvl] && last[j] != (p+2) % 4) {

				ghost_x[j] = toAxis_x(now_x + dx[p], now_y + dy[p]);
				ghost_y[j] = toAxis_y(now_x + dx[p], now_y + dy[p]);

				last[j] = p;
				break;
			}
		}
	}
}

void shortestPath(void) {
	int lvl;
	if(GAMESTATE == INGAME_LVL_1) lvl = 0;
	if(GAMESTATE == INGAME_LVL_2) lvl = 1;
	if(GAMESTATE == INGAME_LVL_3) lvl = 2;

	for(int i = 0; i <= 32; i++)
		for(int j = 0; j <= 32; j++)
			dist[i][j][lvl] =  5000;

	dist[toArray_x(pacman_x, pacman_y)][toArray_y(pacman_x, pacman_y)][lvl] = 0;

	for(int k = 0; k < 512; k++) {
		for(int i = 0; i < 32; i++) {
			for(int j = 0; j < 32; j++) {
				if(!okay[i][j][lvl]) continue;

				if(i) dist[i][j][lvl] = min(dist[i][j][lvl], dist[i-1][j][lvl] + 1);
				if(j) dist[i][j][lvl] = min(dist[i][j][lvl], dist[i][j-1][lvl] + 1);
				dist[i][j][lvl] = min(dist[i][j][lvl], dist[i+1][j][lvl] + 1);
				dist[i][j][lvl] = min(dist[i][j][lvl], dist[i][j+1][lvl] + 1);
			}
		}
	}
}

void update() {
	if (GAMESTATE == MENU) {
		resetGhost();

		pacman_x = 16 + d, pacman_y = 512 - 30 * 16 + d;
		pacman_mouth_toggle = 0;

		WIN = 0, LOSE = 0;

		pacman_dir = 0; 
		forward_right = forward_up = 0;

		score[0] = score[1] = score[2] = 0;

		for(int lvl = 0; lvl < 3; lvl++) {
			for(int i = 0; i < 32; i++) {
				for(int j = 0; j < 32; j++) {
					if(okay[i][j][lvl] && grid[i][j][lvl] == '.') khawa[i][j][lvl] = 0;
					else khawa[i][j][lvl] = 1;
				}
			}
		}

		timeShuru[0] = timeShuru[1] = timeShuru[2] = 0;
		timeNow[0] = timeNow[1] = timeNow[2] = 0;
		score[0] = score[1] = score[2] = 0;

		if(menuMusic == 0) {
			PlaySound(TEXT("music\\shops.wav"), NULL, SND_ASYNC | SND_LOOP);
			menuMusic = 1;
		}

		musicLvl[0] = musicLvl[1] = musicLvl[2] = 0;
		deathMusic = 0, winMusic = 0, hitMusic = 0;

		hpLvl[0] = hpLvl[1] = hpLvl[2] = 2;
		showCherry[0] = showCherry[1] = showCherry[2] = 0;
	}
	if (GAMESTATE == TRANSITION) {
		resetGhost();

		pacman_x = 16 + d, pacman_y = 512 - 30 * 16 + d;
		pacman_mouth_toggle = 0;

		WIN = 0, LOSE = 0;

		pacman_dir = 0; 
		forward_right = forward_up = 0;
	}

	if (GAMESTATE == INGAME_LVL_3) {
		int here_x = toArray_x(pacman_x, pacman_y);
		int here_y = toArray_y(pacman_x, pacman_y);

		for(int i = 0; i < 4; i++) {
			// if(here_x == portals[i].x && here_y == portals[i].y) {

			int ax1 = pacman_x;
			int ax2 = ax1 + 10;
			int ay1 = pacman_y;
			int ay2 = ay1 + 18;

			int bx1 = toAxis_x(portals[i].x, portals[i].y);
			int bx2 = bx1 + 12;
			int by1 = toAxis_y(portals[i].x, portals[i].y);
			int by2 = by1 + 15;

			int ret = 1;
			ret &= (ax1 <= bx2);
			ret &= (ax2 >= bx1);
			ret &= (ay1 <= by2);
			ret &= (ay2 >= by1);

			if(ret == 0) continue;

			if (i == 3) here_x = portals[0].x, here_y = portals[0].y + 1;
			if (i == 0) here_x = portals[3].x, here_y = portals[3].y - 1;
			if (i == 1) here_x = portals[2].x, here_y = portals[2].y + 1;
			if (i == 2) here_x = portals[1].x, here_y = portals[1].y - 1;
			// }
		}

		pacman_x = toAxis_x(here_x, here_y);
		pacman_y = toAxis_y(here_x, here_y);
	}
}

void timeInc() {
	if(GAMESTATE == INGAME_LVL_1) timeNow[0]++;
	if(GAMESTATE == INGAME_LVL_2) timeNow[1]++;
	if(GAMESTATE == INGAME_LVL_3) timeNow[2]++;
}

int main() {
	//place your own initialization codes here.

	portals[0] = {8, 6};
	portals[1] = {8, 25};
	portals[2] = {23, 6};
	portals[3] = {23, 25};

	len = 0;
	mode = 0;
	str[0]= 0;

	srand(time(NULL));

	resetGhost();

	for(int lvl = 0; lvl < 3; lvl++) {
		FILE *file; 

		if(lvl == 0) file = fopen("levels/level_grid.txt", "r");
		if(lvl == 1) file = fopen("levels/level_grid_2.txt", "r");
		if(lvl == 2) file = fopen("levels/level_grid_3.txt", "r");

		char line[50];
		int row = 0;
		while(fgets(line, sizeof(line), file) && row < 32) {
			int len = strlen(line);
			if(line[len-1] == '\n') line[len-1] = '\0';

			for(int i = 0; line[i]; i++)
				grid[row][i][lvl] = line[i];
			
			row++;
		}

		for(int i = 0; i < 32; i++) {
			for(int j = 0; j < 32; j++) {
				if(grid[i][j][lvl] == '#') continue;
				if(grid[i-1][j][lvl] == '#') continue;
				if(grid[i-1][j+1][lvl] == '#') continue;
				if(grid[i][j+1][lvl] == '#') continue;

				okay[i][j][lvl] = 1;
			}
		}

		for(int i = 0; i < 32; i++) {
			for(int j = 0; j < 32; j++) {
				if(okay[i][j][lvl] && grid[i][j][lvl] == '.') khawa[i][j][lvl] = 0;
				else khawa[i][j][lvl] = 1;
			}
		}

		for(int i = 0; i < 32; i++) {
			for(int j = 0; j < 32; j++)
				printf("%d", okay[i][j][lvl]);
			printf("\n");
		}


		fclose(file);
	}

	iSetTimer(15, update);
	pacman_mouth_t = iSetTimer(200, showPacman);
	pacman_motion_t = iSetTimer(100, movePacman);
	ghost_motion_t[0] = iSetTimer(150, moveGhost);
	iSetTimer(350, shortestPath);
	iSetTimer(10, timeInc);

	// PlaySound(TEXT("music\\shops.wav"), NULL, SND_ASYNC | SND_LOOP);

	iInitialize(600, 700, "pacman");
	
	return 0;
}
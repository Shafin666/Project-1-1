# include "iGraphics.h"
# include <math.h>

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

int khawa[40][40], score = 0;
int ghost_x[4], ghost_y[4];
int dist[40][40];
int last[4] = {-1, -1, -1, -1};

int khawa2[40][40], score2 = 0;
int dist2[40][40];

int giveUp;

int mouse_x, mouse_y;

int pacman_x = 16 + d, pacman_y = 512 - 30 * 16 + d;
bool pacman_mouth_toggle = 0;

int WIN = 0, LOSE = 0;

int pacman_dir = 0; // 0 = left, 1 = up, 2 = right, 3 = down;

char grid[40][40],	grid2[40][40];
int okay[40][40],	okay2[40][40];

// timers
int pacman_mouth_t, pacman_motion_t;
int ghost_motion_t[4];

// ------------------------------

//motion
int forward_right = 0, forward_up = 0;
// -------------------------------

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
int numGhost = 4;

int timeShuru = 0, timeNow = 0;
int timeShuru2 = 0, timeNow2 = 0;

void drawMenu() {
    iClear();

	iShowBMP(0, 0, "assets/menu_bg.bmp");

    // Draw menu options
    iSetColor(255, 255, 255); // Set color to white
    // iText(200, 400, "START", GLUT_BITMAP_HELVETICA_18);
    // iText(200, 350, "DIFFICULTY", GLUT_BITMAP_HELVETICA_18);
    // iText(200, 300, "LEADERBOARD", GLUT_BITMAP_HELVETICA_18);
    // iText(200, 250, "EXIT", GLUT_BITMAP_HELVETICA_18);

    if (insideSubmenu) {
        iText(450, 390, "EASY", (submenuSelectedOption == SUBMENU_EASY) ? GLUT_BITMAP_HELVETICA_18 : GLUT_BITMAP_HELVETICA_12);
        iText(450, 360, "MEDIUM", (submenuSelectedOption == SUBMENU_MEDIUM) ? GLUT_BITMAP_HELVETICA_18 : GLUT_BITMAP_HELVETICA_12);
        iText(450, 330, "HARD", (submenuSelectedOption == SUBMENU_HARD) ? GLUT_BITMAP_HELVETICA_18 : GLUT_BITMAP_HELVETICA_12);
    }

	int r = 0, g = 0, b = 0;
	if(submenuSelectedOption == SUBMENU_EASY) g = 255;
	else if(submenuSelectedOption == SUBMENU_HARD) r = 255;
	else if(submenuSelectedOption == SUBMENU_MEDIUM) g = 255, r = 255;

	iSetColor(r, g, b);
	iFilledCircle(390, 365, 10, 10);

    // Draw selection arrow next to the selected option
    iSetColor(255, 0, 0);
    switch (selectedOption) {
        case MENU_START:
            // iText(150, 400, ">", GLUT_BITMAP_HELVETICA_18);
			iShowBMP(120, 400, "assets/arrow.bmp");
            break;
        case MENU_DIFFICULTY:
            // iText(150, 350, ">", GLUT_BITMAP_HELVETICA_18);
			iShowBMP(120, 350, "assets/arrow.bmp");
            break;
        case MENU_LEADERBOARD:
            // iText(150, 300, ">", GLUT_BITMAP_HELVETICA_18);
			iShowBMP(120, 300, "assets/arrow.bmp");
            break;
        case MENU_EXIT:
            // iText(150, 250, ">", GLUT_BITMAP_HELVETICA_18);
			iShowBMP(120, 250, "assets/arrow.bmp");
            break;
    }
}



#define MAX_NAME_LENGTH 50
#define MAX_ENTRIES 100

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
	iSetColor(255, 255, 255);
	// iText(500, 500, "eta leaderboard", GLUT_BITMAP_TIMES_ROMAN_10);

	Entry entries[MAX_ENTRIES];
    int numEntries = 0;

    // Open the file for reading
    FILE *file = fopen("data.txt", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    // Read entries from the file
    while (fscanf(file, "%s %d", entries[numEntries].name, &entries[numEntries].score) == 2) {
        numEntries++;
        if (numEntries >= MAX_ENTRIES) {
            printf("Maximum number of entries reached.\n");
            break;
        }
    }

    // Close the file
    fclose(file);

    // Print the entries
    // printf("Name\tScore\n");
    for (int i = 0; i < numEntries; i++) {
        // printf("%s\t%d\n", entries[i].name, entries[i].score);
		// iText(500, 500, entries[i].name, GLUT_BITMAP_TIMES_ROMAN_24);
    }

	iText(200, 500, "NAME", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(350, 500, "SCORE", GLUT_BITMAP_TIMES_ROMAN_24);

	for(int i = 0; i < numEntries; i++) {
		for(int j = i+1; j < numEntries; j++) {
			if(entries[i].score < entries[j].score)
				swapEntries(&entries[i], &entries[j]);
		}
	}


	for (int i = 0, x = 200, y = 400; i < numEntries; i++, y -= 50) {
        // printf("%s\t%d\n", entries[i].name, entries[i].score);
		iText(x, y, entries[i].name, GLUT_BITMAP_TIMES_ROMAN_24);
		iText(x + 145, y, toString(entries[i].score), GLUT_BITMAP_TIMES_ROMAN_24);
    }
}


void level() {
	iClear();

	iShowBMP(17, 17, "assets/level_bg_2.bmp");

	if(score == 302) {
		iClear();
		// iShowBMP(0, 0, "win.bmp");

		GAMESTATE = TRANSITION;

		return;
	}

	if(LOSE) {
		iClear();

		score2 = 0;
		timeShuru2 = 0;
		GAMESTATE = NAME_INPUT;

		return;
	}
	
	for(int i = 0; i < numGhost; i++) {

		if(hit(i, 0, 1)) {
			LOSE = 1;
			return;
		}
	}

	iShowBMP(150, 600, "assets/title.bmp");
	iText(500, 600, toString(score), GLUT_BITMAP_TIMES_ROMAN_24);
	iText(100, 600, toString(timeNow), GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++) {
			if(!khawa[i][j])
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

	if(score2 == 292) {
		iClear();
		// iShowBMP(0, 0, "win.bmp");

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
			LOSE = 1;
			return;
		}
	}

	for(int i = d; i < 512 + d; i++) {
		for(int j = d; j < 512 + d; j++) {
			if(grid2[toArray_x(i, j)][toArray_y(i, j)] == '#')
				iPoint(i, j+15, 1);
		}
	}

	iShowBMP(150, 600, "assets/title.bmp");
	iText(500, 600, toString(score2), GLUT_BITMAP_TIMES_ROMAN_24);
	iText(100, 600, toString(timeNow2), GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++) {
			if(!khawa2[i][j])
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


char str[100], str2[100];
int len;
int mode;

void drawTextBox()
{
	iSetColor(150, 150, 150);
	iRectangle(190, 290, 250, 30);

	iText(190, 340, "Enter your Nickname: ", GLUT_BITMAP_HELVETICA_18);
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
		numGhost = max(1, submenuSelectedOption - 3);
		if(!timeShuru) {
			timeShuru = 1;
			timeNow = 0;
		}
		giveUp = 0;

		level();
		return;
	}

	if(GAMESTATE == INGAME_LVL_2) {
		numGhost = max(1, submenuSelectedOption - 3);
		if(!timeShuru2) {
			timeShuru2 = 1;
			timeNow2 = 0;
		}
		giveUp = 0;

		level2();
		return;
	}

	if(GAMESTATE == NAME_INPUT) {
		iClear();

		if(WIN) iText(240, 500, "YOU WON", GLUT_BITMAP_TIMES_ROMAN_24);
		else iText(240, 500, "YOU LOSE", GLUT_BITMAP_TIMES_ROMAN_24);

		drawTextBox();
		mode = 1;
		if(mode == 1)
		{
			iSetColor(255, 255, 255);
			iText(250, 300, str);
		}

	}

	if(GAMESTATE == TRANSITION) {
		iText(200, 500, "Go Further?", GLUT_BITMAP_TIMES_ROMAN_24);
		iText(200, 470, "[Y] Continue", GLUT_BITMAP_TIMES_ROMAN_10);
		iText(200, 450, "[ANYTHING ELSE] Give up", GLUT_BITMAP_TIMES_ROMAN_10);
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
            } else {
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
				if(WIN) totalScore = 20000 - timeNow - timeNow2;
				else totalScore = 500 + 10 * timeNow + 20 * score + timeNow2 * 10 + score2 * 20;

				totalScore += giveUp * 500;

				printf("%d %d %d %d\n", score, timeNow, score2, timeNow2);

				FILE* file = fopen("data.txt", "a");
				fprintf(file, "%s %d\n", str2, totalScore);
				fclose(file);

				for(i = 0; i < len; i++)
					str[i] = 0;
				len = 0;

				GAMESTATE = LEADERBOARD;
			}
			else
			{
				str[len] = key;
				len++;
			}
		}
	}
	
	if(GAMESTATE == TRANSITION) {
		if(key == 'y') GAMESTATE = INGAME_LVL_2;
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
            if (insideSubmenu) {
                // Navigate submenu options
                submenuSelectedOption = (submenuSelectedOption == SUBMENU_EASY) ? SUBMENU_HARD : submenuSelectedOption - 1;
            } else {
                selectedOption = (selectedOption == MENU_START) ? MENU_EXIT : selectedOption - 1;
            }
            break;
        case GLUT_KEY_DOWN:
            if (insideSubmenu) {
                // Navigate submenu options
                submenuSelectedOption = (submenuSelectedOption == SUBMENU_HARD) ? SUBMENU_EASY : submenuSelectedOption + 1;
            } else {
                selectedOption = (selectedOption == MENU_EXIT) ? MENU_START : selectedOption + 1;
            }
            break;
    }

    case (INGAME_LVL_1):
    case (INGAME_LVL_2):
    case (INGAME_LVL_3):
        if (key == GLUT_KEY_RIGHT) {
            forward_up = 0;
            forward_right = 1;
        }
        
        if (key == GLUT_KEY_LEFT) {
            forward_up = 0;
            forward_right = -1;
        }

        if(key == GLUT_KEY_UP) {
            forward_right = 0;
            forward_up = 1;
        }

        if(key == GLUT_KEY_DOWN) {
            forward_right = 0;
            forward_up = -1;
        }
        break;
    }
    
}

void showPacman(void) {
	pacman_mouth_toggle ^= 1;
}

void movePacman(void) {
	if(GAMESTATE == INGAME_LVL_1) {
		int here_x = toArray_x(pacman_x, pacman_y);
		int here_y = toArray_y(pacman_x, pacman_y);

		if(forward_right > 0) {
			pacman_dir = 0;
			pacman_x += 16;

			if(!okay[here_x][here_y + 1])
				pacman_x -= 16;
		}
		
		if(forward_right < 0) {
			pacman_dir = 2;
			pacman_x -= 16;

			if(!okay[here_x][here_y - 1])
				pacman_x += 16;
		}

		if(forward_up > 0) {
			pacman_dir = 1;
			pacman_y += 16;

			if(!okay[here_x - 1][here_y])
				pacman_y -= 16;
		}

		if(forward_up < 0) {
			pacman_dir = 3;
			pacman_y -= 16;

			if(!okay[here_x + 1][here_y])
				pacman_y += 16;
		}

		if(pacman_x < d + 1) pacman_x = 512 + d - 32;
		if(pacman_x > 512 + d - 32) pacman_x = d;

		here_x = toArray_x(pacman_x, pacman_y);
		here_y = toArray_y(pacman_x, pacman_y);

		score += !khawa[here_x][here_y];
		khawa[here_x][here_y] = 1;
	}
	
	
	
	if(GAMESTATE == INGAME_LVL_2) {
		int here_x = toArray_x(pacman_x, pacman_y);
		int here_y = toArray_y(pacman_x, pacman_y);

		if(forward_right > 0) {
			pacman_dir = 0;
			pacman_x += 16;

			if(!okay2[here_x][here_y + 1])
				pacman_x -= 16;
		}
		
		if(forward_right < 0) {
			pacman_dir = 2;
			pacman_x -= 16;

			if(!okay2[here_x][here_y - 1])
				pacman_x += 16;
		}

		if(forward_up > 0) {
			pacman_dir = 1;
			pacman_y += 16;

			if(!okay2[here_x - 1][here_y])
				pacman_y -= 16;
		}

		if(forward_up < 0) {
			pacman_dir = 3;
			pacman_y -= 16;

			if(!okay2[here_x + 1][here_y])
				pacman_y += 16;
		}

		if(pacman_x < d + 1) pacman_x = 512 + d - 32;
		if(pacman_x > 512 + d - 32) pacman_x = d;

		here_x = toArray_x(pacman_x, pacman_y);
		here_y = toArray_y(pacman_x, pacman_y);

		score2 += !khawa2[here_x][here_y];
		khawa2[here_x][here_y] = 1;
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
	if(GAMESTATE == INGAME_LVL_1) {
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
						if(dist[now_x + dx[i]][now_y + dy[i]] <= mn) {
							mn = dist[now_x + dx[i]][now_y + dy[i]];
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


				if(okay[now_x + dx[p]][now_y + dy[p]] && last[j] != (p+2) % 4) {

					ghost_x[j] = toAxis_x(now_x + dx[p], now_y + dy[p]);
					ghost_y[j] = toAxis_y(now_x + dx[p], now_y + dy[p]);

					last[j] = p;
					break;
				}
			}
		}
	}
	
	if(GAMESTATE == INGAME_LVL_2) {
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
						if(dist2[now_x + dx[i]][now_y + dy[i]] <= mn) {
							mn = dist2[now_x + dx[i]][now_y + dy[i]];
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


				if(okay2[now_x + dx[p]][now_y + dy[p]] && last[j] != (p+2) % 4) {

					ghost_x[j] = toAxis_x(now_x + dx[p], now_y + dy[p]);
					ghost_y[j] = toAxis_y(now_x + dx[p], now_y + dy[p]);

					last[j] = p;
					break;
				}
			}
		}
	}
}

void shortestPath(void) {

	if(GAMESTATE == INGAME_LVL_1) {

		memset(dist, 0x3f, sizeof dist);

		dist[toArray_x(pacman_x, pacman_y)][toArray_y(pacman_x, pacman_y)] = 0;

		for(int k = 0; k < 512; k++) {
			for(int i = 0; i < 32; i++) {
				for(int j = 0; j < 32; j++) {
					if(!okay[i][j]) continue;

					if(i) dist[i][j] = min(dist[i][j], dist[i-1][j] + 1);
					if(j) dist[i][j] = min(dist[i][j], dist[i][j-1] + 1);
					dist[i][j] = min(dist[i][j], dist[i+1][j] + 1);
					dist[i][j] = min(dist[i][j], dist[i][j+1] + 1);
				}
			}
		}
	}

	if(GAMESTATE == INGAME_LVL_2) {
		memset(dist2, 0x3f, sizeof dist2);

		dist2[toArray_x(pacman_x, pacman_y)][toArray_y(pacman_x, pacman_y)] = 0;

		for(int k = 0; k < 512; k++) {
			for(int i = 0; i < 32; i++) {
				for(int j = 0; j < 32; j++) {
					if(!okay2[i][j]) continue;

					if(i) dist2[i][j] = min(dist2[i][j], dist2[i-1][j] + 1);
					if(j) dist2[i][j] = min(dist2[i][j], dist2[i][j-1] + 1);
					dist2[i][j] = min(dist2[i][j], dist2[i+1][j] + 1);
					dist2[i][j] = min(dist2[i][j], dist2[i][j+1] + 1);
				}
			}
		}
	}
}

void update() {
	if(GAMESTATE == MENU) {
		ghost_x[0] = toAxis_x(18, 13);
		ghost_y[0] = toAxis_y(18, 13);

		ghost_x[1] = toAxis_x(18, 16);
		ghost_y[1] = toAxis_y(18, 16);

		ghost_x[2] = toAxis_x(15, 16);
		ghost_y[2] = toAxis_y(15, 16);

		ghost_x[3] = toAxis_x(15, 13);
		ghost_y[3] = toAxis_y(15, 13);

		pacman_x = 16 + d, pacman_y = 512 - 30 * 16 + d;
		pacman_mouth_toggle = 0;

		WIN = 0, LOSE = 0;

		pacman_dir = 0; 

		score = 0;
		for(int i = 0; i < 32; i++) {
			for(int j = 0; j < 32; j++) {
				if(okay[i][j] && grid[i][j] == '.') khawa[i][j] = 0;
				else khawa[i][j] = 1;
			}
		}

		timeShuru = timeShuru2 = 0;
		timeNow = timeNow2 = 0;
		score = score2 = 0;
	}
	if(GAMESTATE == TRANSITION) {
		ghost_x[0] = toAxis_x(18, 13);
		ghost_y[0] = toAxis_y(18, 13);

		ghost_x[1] = toAxis_x(18, 16);
		ghost_y[1] = toAxis_y(18, 16);

		ghost_x[2] = toAxis_x(15, 16);
		ghost_y[2] = toAxis_y(15, 16);

		ghost_x[3] = toAxis_x(15, 13);
		ghost_y[3] = toAxis_y(15, 13);

		pacman_x = 16 + d, pacman_y = 512 - 30 * 16 + d;
		pacman_mouth_toggle = 0;

		WIN = 0, LOSE = 0;

		pacman_dir = 0; 
	}
}

void timeInc() {
	timeNow++;
	if(GAMESTATE == INGAME_LVL_2) timeNow2++;
}


int main() {
	//place your own initialization codes here.

	len = 0;
	mode = 0;
	str[0]= 0;

	srand(time(NULL));

	ghost_x[0] = toAxis_x(18, 13);
	ghost_y[0] = toAxis_y(18, 13);

	ghost_x[1] = toAxis_x(18, 16);
	ghost_y[1] = toAxis_y(18, 16);

	ghost_x[2] = toAxis_x(15, 16);
	ghost_y[2] = toAxis_y(15, 16);

	ghost_x[3] = toAxis_x(15, 13);
	ghost_y[3] = toAxis_y(15, 13);

	FILE *file = fopen("level_grid.txt", "r");
	char line[50];
	int row = 0;
	while(fgets(line, sizeof(line), file) && row < 32) {
		int len = strlen(line);
		if(line[len-1] == '\n') line[len-1] = '\0';

		for(int i = 0; line[i]; i++)
			grid[row][i] = line[i];
		
		row++;
	}

	// for(int i = 0; i < 32; i++) {
	// 	for(int j = 0; j < 32; j++)
	// 		printf("%c", grid[i][j]);
	// 	printf("\n");
	// }


	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++) {
			if(grid[i][j] == '#') continue;
			if(grid[i-1][j] == '#') continue;
			if(grid[i-1][j+1] == '#') continue;
			if(grid[i][j+1] == '#') continue;

			okay[i][j] = 1;
		}
	}

	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++) {
			if(okay[i][j] && grid[i][j] == '.') khawa[i][j] = 0;
			else khawa[i][j] = 1;
		}
	}

	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++)
			printf("%d", okay[i][j]);
		printf("\n");
	}


	FILE *file2 = fopen("level_grid_2.txt", "r");
	char line2[50];
	int row2 = 0;
	while(fgets(line2, sizeof(line2), file2) && row2 < 32) {
		int len = strlen(line2);
		if(line2[len-1] == '\n') line2[len-1] = '\0';

		for(int i = 0; line2[i]; i++)
			grid2[row2][i] = line2[i];
		
		row2++;
	}

	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++) {
			if(grid2[i][j] == '#') continue;
			if(grid2[i-1][j] == '#') continue;
			if(grid2[i-1][j+1] == '#') continue;
			if(grid2[i][j+1] == '#') continue;

			okay2[i][j] = 1;
		}
	}

	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++) {
			if(okay2[i][j] && grid2[i][j] == '.') khawa2[i][j] = 0;
			else khawa2[i][j] = 1;
		}
	}

	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++)
			printf("%d", okay2[i][j]);
		printf("\n");
	}

	iSetTimer(15, update);
	pacman_mouth_t = iSetTimer(200, showPacman);
	pacman_motion_t = iSetTimer(100, movePacman);
	ghost_motion_t[0] = iSetTimer(120, moveGhost);
	iSetTimer(300, shortestPath);
	iSetTimer(1000, timeInc);

	iInitialize(600, 700, "pagman");
	
	return 0;
}
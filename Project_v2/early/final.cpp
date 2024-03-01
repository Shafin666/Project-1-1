# include "iGraphics.h"
# include <math.h>
#include <bits/stdc++.h>

const int d = 40;

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

bool hit(int i, int j);

//-----------------------------------

//---------------
#define MENU 1
#define LEADERBOARD 2
#define INGAME_LVL_1 3
#define INGAME_LVL_2 4
#define INGAME_LVL_3 5
#define NAME_INPUT 6
int GAMESTATE = INGAME_LVL_1;
//---------------

int mouse_x, mouse_y;
int grid[40][40], okay[40][40], khawa[40][40];
int grid2[40][40], okay2[40][40], khawa2[40][40];



typedef struct Pacman {
    double x, y;
    double v;

    int fwd_rgt;
    int fwd_up;
};
Pacman P;





void iDraw() {
	iClear();

    iText(40, 40, "helo bro", GLUT_BITMAP_TIMES_ROMAN_10);
    iText(55, 40, "hellobro", GLUT_BITMAP_TIMES_ROMAN_24);

    iSetColor(50, 30, 130);
    for(int i = d; i < 512 + d; i++) {
        for(int j = d; j < 512 + d; j++) {
            if(grid2[toArray_x(i, j)][toArray_y(i, j)] == '#')
                iPoint(i-5, j-5, 2);
        }
    }

    iSetColor(255, 255, 0);
    for(int i = 0; i < 32 ; i++) {
        for(int j = 0; j < 32; j++) {
            iPoint(toAxis_x(i, j), toAxis_y(i, j), 2);
        }
    }



    iSetColor(255, 255, 255);

    iFilledCircle(P.x, P.y, 10, 10);
}

void iMouseMove(int mx, int my) {
	printf("x = %d, y= %d\n",mx,my);

	mouse_x = mx;
	mouse_y = my;
}

void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

	}
}

void iKeyboard(unsigned char key) {
	if (key == 'q') {
		exit(0);
	}
}

void iSpecialKeyboard(unsigned char key) {

	if (key == GLUT_KEY_END) {
		exit(0);
	}

	if (key == GLUT_KEY_RIGHT) {
        P.fwd_rgt = 1;
        P.fwd_up = 0;
    }
    
    if (key == GLUT_KEY_LEFT) {
        P.fwd_up = 0;
        P.fwd_rgt = -1;
    }

    if(key == GLUT_KEY_UP) {
        P.fwd_rgt = 0;
        P.fwd_up = 1;
    }

    if(key == GLUT_KEY_DOWN) {
        P.fwd_rgt = 0;
        P.fwd_up = -1;
    }
}


void showPacman(void) {
	
}

void movePacman(void) {
	
}

bool hit(int i, int j) {
	// int ax1 = ghost_x[i];
	// int ax2 = ax1 + 28;
	// int ay1 = ghost_y[i];
	// int ay2 = ghost_y[i] + 28;

	// int bx1 = ghost_x[j];
	// int bx2 = bx1 + 28;
	// int by1 = ghost_y[j];
	// int by2 = ghost_y[j] + 28;

	int ret = 1;
	// ret &= (ax1 <= bx2);
	// ret &= (ax2 >= bx1);
	// ret &= (ay1 <= by2);
	// ret &= (ay2 >= by1);

	return ret;
}

// void moveGhost(void) {
// 	for(int j = 0; j < 4; j++) {
// 		int dx[4] = {0, 1, 0, -1};
// 		int dy[4] = {1, 0, -1, 0};

// 		int now_x = toArray_x(ghost_x[j], ghost_y[j]);
// 		int now_y = toArray_y(ghost_x[j], ghost_y[j]);

// 		for(int iter = 0; iter < 20; iter++) {
// 			int p = rand() % 4;

// 			if(rand() % 4 == 0) {
// 				int mn = 500;
// 				for(int i = 0; i < 4; i++) {
// 					if(dist[now_x + dx[i]][now_y + dy[i]] <= mn) {
// 						mn = dist[now_x + dx[i]][now_y + dy[i]];
// 						p = i;
// 					}
// 				}
// 			}

// 			if(now_x + dx[p] < 0 || now_x + dx[p] > 30) continue;
// 			if(now_y + dy[p] < 0 || now_y + dy[p] > 30) continue;
			
// 			ghost_x[j] = toAxis_x(now_x + dx[p], now_y + dy[p]);
// 			ghost_y[j] = toAxis_y(now_x + dx[p], now_y + dy[p]);

// 			int flag = 0;
// 			for(int i = 0; i < 4; i++) {
// 				if(i == j) continue;
// 				if(hit(i, j)) flag = 1;
// 			}
			
// 			if(flag) {
// 				ghost_x[j] = toAxis_x(now_x, now_y);
// 				ghost_y[j] = toAxis_y(now_x, now_y);
// 				last[j] = (last[j] + 2) % 4;
// 				break;
// 			}


// 			if(okay[now_x + dx[p]][now_y + dy[p]] && last[j] != (p+2) % 4) {

// 				ghost_x[j] = toAxis_x(now_x + dx[p], now_y + dy[p]);
// 				ghost_y[j] = toAxis_y(now_x + dx[p], now_y + dy[p]);

// 				last[j] = p;
// 				break;
// 			}
// 		}
// 	}
// }

// void shortestPath(void) {
// 	memset(dist, 0x3f, sizeof dist);

// 	dist[toArray_x(pacman_x, pacman_y)][toArray_y(pacman_x, pacman_y)] = 0;

// 	for(int k = 0; k < 512; k++) {
// 		for(int i = 0; i < 32; i++) {
// 			for(int j = 0; j < 32; j++) {
// 				if(!okay[i][j]) continue;

// 				if(i) dist[i][j] = min(dist[i][j], dist[i-1][j] + 1);
// 				if(j) dist[i][j] = min(dist[i][j], dist[i][j-1] + 1);
// 				dist[i][j] = min(dist[i][j], dist[i+1][j] + 1);
// 				dist[i][j] = min(dist[i][j], dist[i][j+1] + 1);
// 			}
// 		}
// 	}
// }

void update() {
    if(GAMESTATE == INGAME_LVL_1) {
        P.x += P.v * P.fwd_rgt;
        P.y += P.v * P.fwd_up;

        if(!okay2[toArray_x(P.x, P.y)][toArray_y(P.x, P.y)]) {
            P.x -= P.v * P.fwd_rgt;
            P.y -= P.v * P.fwd_up;
        }
    }
}


int main() {
	//place your own initialization codes here.

    P.x = d + 16, P.y = d + 16;
    P.v = 16.0/6;
    P.fwd_rgt = P.fwd_up = 0;

	srand(time(NULL));

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
    fclose(file);

    FILE *file2 = fopen("level_grid_2.txt", "r");
	row = 0;
	while(fgets(line, sizeof(line), file2) && row < 32) {
		int len = strlen(line);
		if(line[len-1] == '\n') line[len-1] = '\0';

		for(int i = 0; line[i]; i++)
			grid2[row][i] = line[i];
		
		row++;
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
			else khawa[i][j] = 1;
		}
	}

    freopen("out.txt", "w", stdout);
    for(int i = 0; i < 32; i++) {
        for(int j = 0; j < 32; j++)
            printf("%d ", okay2[i][j]);
        printf("\n");
    }

    iSetTimer(15, update);

	iInitialize(600, 700, "pagman");
	
	return 0;
}
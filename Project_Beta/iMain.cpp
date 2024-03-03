# include "iGraphics.h"
# include <math.h>

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

int khawa[40][40], score = 0;
int ghost_x[4], ghost_y[4];
int dist[40][40];
int last[4] = {-1, -1, -1, -1};

int mouse_x, mouse_y;

int pacman_x = 16 + d, pacman_y = 512 - 30 * 16 + d;
bool pacman_mouth_toggle = 0;

int WIN = 0, LOSE = 0;

int pacman_dir = 0; // 0 = left, 1 = up, 2 = right, 3 = down;

char grid[40][40];
int okay[32][32];

// timers
int pacman_mouth_t, pacman_motion_t;
int ghost_motion_t[4];

// ------------------------------

//motion
int forward_right = 0, forward_up = 0;
// -------------------------------


void iDraw() {
	iClear();

	iShowBMP(17, 17, "level_bg_2.bmp");

	if(score == 302) WIN = 1;

	iShowBMP(150, 600, "title.bmp");
	iText(500, 600, toString(score), GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++) {
			if(!khawa[i][j])
				iPoint(toAxis_x(i, j) + 16, toAxis_y(i, j) + 16, 2);
		}
	}

	if (pacman_mouth_toggle) {
		if (pacman_dir == 0) iShowBMP(pacman_x, pacman_y, "pacman_open_right.bmp");
		if (pacman_dir == 1) iShowBMP(pacman_x, pacman_y, "pacman_open_up.bmp");
		if (pacman_dir == 2) iShowBMP(pacman_x, pacman_y, "pacman_open_left.bmp");
		if (pacman_dir == 3) iShowBMP(pacman_x, pacman_y, "pacman_open_down.bmp");
	}
	else {
		if (pacman_dir == 0) iShowBMP(pacman_x, pacman_y, "pacman_close_right.bmp");
		if (pacman_dir == 1) iShowBMP(pacman_x, pacman_y, "pacman_close_up.bmp");
		if (pacman_dir == 2) iShowBMP(pacman_x, pacman_y, "pacman_close_left.bmp");
		if (pacman_dir == 3) iShowBMP(pacman_x, pacman_y, "pacman_close_down.bmp");
	}

	iShowBMP(ghost_x[0], ghost_y[0], "bhoot_0.bmp");
	iShowBMP(ghost_x[1], ghost_y[1], "bhoot_1.bmp");
	iShowBMP(ghost_x[2], ghost_y[2], "bhoot_2.bmp");
	iShowBMP(ghost_x[3], ghost_y[3], "bhoot_3.bmp");

	if(WIN) {
		iClear();
		iShowBMP(0, 0, "win.bmp");
	}

	if(LOSE) {
		iClear();
		iShowBMP(0, 0, "lose.bmp");
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

		if(WIN) exit(0);
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
}


void showPacman(void) {
	pacman_mouth_toggle ^= 1;
}

void movePacman(void) {
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
	if(pacman_x > 512 + d - 32) pacman_x = d ;

	here_x = toArray_x(pacman_x, pacman_y);
	here_y = toArray_y(pacman_x, pacman_y);

	score += !khawa[here_x][here_y];
	khawa[here_x][here_y] = 1;
}

bool hit(int i, int j) {
	int ax1 = ghost_x[i];
	int ax2 = ax1 + 28;
	int ay1 = ghost_y[i];
	int ay2 = ghost_y[i] + 28;

	int bx1 = ghost_x[j];
	int bx2 = bx1 + 28;
	int by1 = ghost_y[j];
	int by2 = ghost_y[j] + 28;

	int ret = 1;
	ret &= (ax1 <= bx2);
	ret &= (ax2 >= bx1);
	ret &= (ay1 <= by2);
	ret &= (ay2 >= by1);

	return ret;
}

void moveGhost(void) {
	for(int j = 0; j < 4; j++) {
		int dx[4] = {0, 1, 0, -1};
		int dy[4] = {1, 0, -1, 0};

		int now_x = toArray_x(ghost_x[j], ghost_y[j]);
		int now_y = toArray_y(ghost_x[j], ghost_y[j]);

		for(int iter = 0; iter < 20; iter++) {
			int p = rand() % 4;

			if(rand() % 4 == 0) {
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
				if(hit(i, j)) flag = 1;
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

void shortestPath(void) {
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

int main() {
	//place your own initialization codes here.

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

	pacman_mouth_t = iSetTimer(200, showPacman);
	pacman_motion_t = iSetTimer(100, movePacman);
	ghost_motion_t[0] = iSetTimer(120, moveGhost);

	iSetTimer(300, shortestPath);

	iInitialize(600, 700, "pagman");
	
	return 0;
}
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#include "Character.h"

//#define DEBUG
#define MOVE_UP 0
#define MOVE_RIGHT 1
#define MOVE_DOWN 2
#define MOVE_LEFT 3

const int key_escape = 27;
const int key_enter = 10;
const int delay_duration = 25;
const char *ready_message = "Are you ready? (ESC - exit, Enter - yes)";
const int row_gametext = 1;

void clearStdscrBuff()
{
	char garbage;

	while( (garbage = getch()) != EOF )
		;
}

void printRowChar(int y, int max_column, int symbol)
{
	move(y, 0);
	for(int i = 0; i < max_column; i++)
		addch(symbol);
}

void printGameText(int max_column, const char *msg)
{
	printRowChar(row_gametext, max_column, ' ');
	mvprintw(row_gametext, (max_column-strlen(msg))/2, msg);
	refresh();
}

void printStep(int step, int max_column)
{
	const char *msg_step;
	if(step == MOVE_UP) msg_step = "up";
	else if(step == MOVE_RIGHT) msg_step = "right";
	else if(step == MOVE_DOWN) msg_step = "down";
	else msg_step = "left";

	printGameText(max_column, msg_step);
}

void showSteps(int max_y, int max_x, int *arr, int amount)
{
	for(int i = 0; i < amount; i++) {
		printGameText(max_y, "          ");
		struct timespec tw = {0, 100000000};
		nanosleep(&tw, NULL);

		arr[i] = rand() % 4;

		printStep(arr[i], max_x);
		sleep(1);
	}
	printRowChar(row_gametext, max_x, ' ');
	refresh();
}

void HandlingUserMove(Character &Player)
{
	int key = getch();
	int y = Player.getY();
	int x = Player.getX();
	switch(key) {
		case 'w':
		case 'W': { // up
			Player.goYX(y-1, x);
			break;
		}
		case 'd':
		case 'D': { // right
			Player.goYX(y, x+1);
			break;
		}
		case 's':
		case 'S': { // down
			Player.goYX(y+1, x);
			break;
		}
		case 'a':
		case 'A': { // left
			Player.goYX(y, x-1);
			break;
		}

		default: break;

		/*case KEY_RESIZE: {
			endwin();
			return 1;
		}*/
	}
}

// 1 tick = 1 milliseconds
uint32_t getTick()
{
	struct timespec ts; // for timer
		/*struct timespec {
	        time_t   tv_sec; // seconds
	        long     tv_nsec; // nanoseconds
		};*/
	uint32_t Tick = 0U; // or unsigned int?
	clock_gettime(CLOCK_MONOTONIC, &ts); // didn't handle error
	Tick = ts.tv_nsec / 1000000;
	Tick += ts.tv_sec * 1000;
	return Tick;
}

bool boom(const int peace_y, const int peace_x,
	const int max_y, const int max_x, const Character& Hero)
{
	bool died = false;

	for(int y = row_gametext+2; y < max_y+1; y++)
		printRowChar(y, max_x+1, 'x');

	if(Hero.getX() == peace_x && Hero.getY() == peace_y) // if player stay in right position
		mvprintw(peace_y, peace_x, "%c", Hero.getSkin());
	else {
		died = true;
		mvprintw(peace_y, peace_x, " ");
	}

	refresh();
	return died;
}

void clear_boom(int peace_y, int peace_x,
	const int max_y, const int max_x, Character& Hero, bool &died)
{
	for(int y = row_gametext+2; y < max_y+1; y++)
		printRowChar(y, max_x+1, ' ');

	#ifdef DEBUG
		if(died) {
			Hero.goYX(peace_y, peace_x);
			died = false;
		}
	#endif
	if(died) {
		printGameText(max_x, "Haha. You lose!");
		sleep(2);
	}
	else mvprintw(peace_y, peace_x, "%c", Hero.getSkin());
	refresh();
}

void changePeace(int step, int &peace_y, int &peace_x)
{
	if(step == MOVE_UP) peace_y--;
	else if(step == MOVE_RIGHT) peace_x++;
	else if(step == MOVE_DOWN) peace_y++;
	else if(step == MOVE_LEFT) peace_x--;
}

int main(int argc, char **argv)
{
	// initialize screen
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(false);

	int rows, columns;
	getmaxyx(stdscr, rows, columns);
	// check normal screen: (#todo)

	// create user, peace coords:
	int peace_y = rows/2; // no boom in these coordinates
	int peace_x = (columns-1)/2;
	Character Player(peace_y, peace_x, '#'); // go in center

	printRowChar(row_gametext+1, columns, ACS_HLINE);
	printGameText(columns, ready_message); // have refresh() in func

	// ask user if he is ready
	int key;
	while((key = getch()) != key_enter) {
		#ifdef DEBUG
			mvprintw(rows-1, 0, "       ");
			mvprintw(rows-1, 0, "%d", key);
			refresh();
		#endif

		if(key == key_escape) {
			endwin();
			return 1;
		}
	}

	int number_steps = 2;
	int steps[16];
	int *ptr_steps = &(steps[0]);
	srand( time(NULL) );

	timeout(delay_duration);
	int now_step;
	bool died = false;
	while(true)
	{
		for(int j=0; j < 16; j++)
			steps[j] = -1;

		showSteps(rows, columns, ptr_steps, number_steps);

		uint32_t boom_timer = getTick();
		now_step = 0;

		changePeace(steps[now_step], peace_y, peace_x);
		clearStdscrBuff();
		while(now_step < number_steps) {
			HandlingUserMove(Player);

			if(getTick() > boom_timer+2500) {
				died = boom(peace_y, peace_x, rows, columns, Player);
				now_step++;

				struct timespec tw = {0, 100000000};
			   	nanosleep(&tw, NULL);

			   	boom_timer = getTick();
			   	clear_boom(peace_y, peace_x, rows, columns, Player, died);
			   	changePeace(steps[now_step], peace_y, peace_x);
			   	if(died) break;
			}
		}
		if(died) break;

		number_steps++;
		sleep(1);
		if(number_steps == 5) break;
	}

	endwin();
	return 0;
}
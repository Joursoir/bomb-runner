#include <ncurses.h>

#include "Character.h"

void Character::goYX(int go_y, int go_x) {
	mvprintw(y, x, " ");
	y = go_y; x = go_x;
	mvprintw(y, x, "%c", skin);
	refresh();
}
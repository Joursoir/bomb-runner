#ifndef CHARACTER_H
#define CHARACTER_H

class Character {
	int y, x;
	char skin;
public:
	Character(int g_y, int g_x, char g_skin) : skin(g_skin)
		{ goYX(g_y, g_x); }
	char getSkin() const { return skin; }
	int getY() const { return y; }
	int getX() const { return x; }
	void goYX(int go_y, int go_x);
};

#endif
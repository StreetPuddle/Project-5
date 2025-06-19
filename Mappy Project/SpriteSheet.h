#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "NPC.h"
#include <iostream>
using namespace std;

class Sprite
{
	friend int bottomCollision(int x, int y, int frameWidth, int frameHeight);
	friend int rightCollision(int x, int y, int frameWidth, int frameHeight);
	friend int leftCollision(int x, int y, int frameWidth, int frameHeight);
	friend int topCollision(int x, int y, int frameWidth, int frameHeight);
	friend bool endValue(int x, int y, int frameHeight, int frameWidth);

public:

	Sprite();
	~Sprite();
	void InitSprites();
	void UpdateSprites(int width, int height, int dir, bool collided); //dir 1 = right, 0 = left, 2 = Standing Still
	void DrawSprites(int xoffset, int yoffset);
	float getX() const { return x; }
	float getY() const { return y; }
	void setX(int sX) { x = sX; }
	void setY(int sY) { y = sY; }
	int getDir() const { return animationDirection; }
	void setAnimationDir(int x) { animationDirection = x; }
	void setRunningSpeed(int running) { runningSpeed = running; }
	int getWidth() const { return frameWidth; }
	int getHeight() const { return frameHeight; }
	bool CollisionEndBlock();

private:

	float x;
	float y;
	int directionalFrames[3];
	int index;
	int speed;
	int runningSpeed;
	int maxFrame;
	int curFrame;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;
	int animationRows;
	int animationDirection;

	ALLEGRO_BITMAP* image;
};
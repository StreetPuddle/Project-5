#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro.h>
#include <iostream>
#include "NPC.h"
using namespace std;

class Sprite
{
	friend int bottomCollision(int x, int y, int frameWidth, int frameHeight);
	friend int rightCollision(int x, int y, int frameWidth, int frameHeight);
	friend int leftCollision(int x, int y, int frameWidth, int frameHeight);
	friend int topCollision(int x, int y, int frameWidth, int frameHeight);
	friend bool endValue(int x, int y, int frameHeight, int frameWidth);

public:

	void UpdateSprites(int width, int height, int dir, bool collided);
	void setRunningSpeed(int running) { runningSpeed = running; }
	void setAnimationDir(int x) { animationDirection = x; }
	int getDir() const { return animationDirection; }
	bool explodingAnim() const { return exploding; }
	int getHeight() const { return frameHeight; }
	int getWidth() const { return frameWidth; }
	void DrawSprites(int xoffset, int yoffset);
	float getX() const { return x; }
	float getY() const { return y; }
	void Sprite::UpdateExplosion();
	void Sprite::StartExplosion();
	void setX(int sX) { x = sX; }
	void setY(int sY) { y = sY; }
	bool CollisionEndBlock();
	void InitSprites();
	~Sprite();
	Sprite();

private:


	int directionalFrames[3];
	int animationDirection;
	double explosionTime;
	int animationColumns;
	int explosionFrameW;
	int explosionFrameH;
	int explosionFrame;
	int animationRows;
	int runningSpeed;
	int frameHeight;
	bool exploding;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int maxFrame;
	int curFrame;
	int index;
	int speed;
	float x;
	float y;

	ALLEGRO_BITMAP* explode;
	ALLEGRO_BITMAP* image;
	ALLEGRO_SAMPLE* sample;
};
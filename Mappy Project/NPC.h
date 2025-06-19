#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <iostream>
#include <string>
class NPC
{

public:
	NPC();
	~NPC();
	float getX() const { return x; }
	float getY() const { return y; }
	void setX(int sX) { x = sX; }
	void setY(int sY) { y = sY; }
	int getWidth() const { return frameWidth; }
	int getHeight() const { return frameHeight; }
	void InitNPC(std::string fileName, int width, int height);
	void updateNPC(int dir);
	void DrawNPC(int xoffset, int yoffset);

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


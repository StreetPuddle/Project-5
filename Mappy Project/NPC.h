#pragma once
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro.h>
#include <iostream>
#include <string>
class NPC
{

public:

	void InitNPC(std::string fileName, int width, int height);
	int getHeight() const { return frameHeight; }
	int getWidth() const { return frameWidth; }
	void DrawNPC(int xoffset, int yoffset);
	float getX() const { return x; }
	float getY() const { return y; }
	void setX(int sX) { x = sX; }
	void setY(int sY) { y = sY; }
	void updateNPC(int dir);
	~NPC();
	NPC();

private:

	int directionalFrames[3];
	int animationDirection;
	int animationColumns;
	int animationRows;
	int runningSpeed;
	int frameHeight;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int maxFrame;
	int curFrame;
	int index;
	int speed;
	float x;
	float y;
	
	ALLEGRO_BITMAP* image;

};


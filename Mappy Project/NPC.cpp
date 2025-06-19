
#include "NPC.h"

//default constructor
NPC::NPC()
{
	image = NULL;
}

//deconstructor
NPC::~NPC() {
	al_destroy_bitmap(image);
}

//constrcutor
void NPC::InitNPC(std::string fileName, int x, int y)
{
	this->x = x;
	this->y = y;
	maxFrame = 9;
	curFrame = 0;
	index = 0;
	speed = 2;
	runningSpeed = 0;
	frameCount = 0;
	frameDelay = 6;
	frameWidth = 32;
	frameHeight = 48;
	animationColumns = 1;
	animationDirection = 1;
	image = al_load_bitmap(fileName.c_str());
}

//update the sprite to face the player
void NPC::updateNPC(int dir) {

	switch (dir) {//renders still frame of last standing direction
	case 0: curFrame = 2; break;//face right
	case 1: curFrame = 1; break;//face left
	case 3: curFrame = 0; break;//face up
	case 4: curFrame = 3; break;//face right
	}
}

//updates the way the NPC is facing
void NPC::DrawNPC(int xoffset, int yoffset)
{
	int fx = (curFrame % animationColumns) * frameWidth;
	int fy = (curFrame / animationColumns) * frameHeight;
	al_draw_bitmap_region(image, fx, fy, frameWidth, frameHeight, x - xoffset, y - yoffset, 0);
}
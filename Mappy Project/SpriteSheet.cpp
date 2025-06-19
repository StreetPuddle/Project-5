#include "SpriteSheet.h"

//default constructror
Sprite::Sprite()
{
	image = NULL;
}

//deconstructor
Sprite::~Sprite()
{
	al_destroy_bitmap(explode);
	al_destroy_sample(sample);
	al_destroy_bitmap(image);
}

//constrcutor
void Sprite::InitSprites()
{
	explode = al_load_bitmap("explosion.png");
	image = al_load_bitmap("spriteSheet.png");
	sample = al_load_sample("bump.wav");
	animationDirection = 1;
	explosionFrameH = 48;
	explosionFrameW = 56;
	animationColumns = 3;
	explosionTime - 0.0;
	explosionFrame = 0;
	exploding = false;
	frameHeight = 46;
	runningSpeed = 0;
	frameWidth = 32;
	frameDelay = 6;
	frameCount = 0;
	maxFrame = 9;
	curFrame = 0;
	index = 0;
	speed = 2;
	y = 250;
	x = 50;	
}

//this function handles the logic of sprite movement in all directions and tests for collision
void Sprite::UpdateSprites(int width, int height, int dir, bool collided)
{
	int oldx = x;
	int oldy = y;

	if (dir == 1 || dir == 0) {//animate left or right

		if (dir == 1) {//left
			animationDirection = dir;
			speed = 2 + runningSpeed;
		}
		else {//right
			animationDirection = dir;
			speed = -2 + -runningSpeed;
		}
		directionalFrames[0] = 6;//frames get flipped depending on direction in draw function
		directionalFrames[1] = 7;
		directionalFrames[2] = 8;
		x += speed;//movement speed

		if (++frameCount > frameDelay)
		{
			frameCount = 0;
			curFrame = directionalFrames[index];//current frame to render
			index++;//moves onto the next frame index
			if (index > 2) {//bounds of array
				index = 0;
			}
		}
	}
	else if (dir == 4) {//animate moving down
		animationDirection = dir;
		directionalFrames[0] = 0;
		directionalFrames[1] = 1;
		directionalFrames[2] = 2;
		y += speed + runningSpeed;

		if (++frameCount > frameDelay)
		{
			frameCount = 0;
			curFrame = directionalFrames[index];
			index++;
			if (index > 2) {
				index = 0;
			}
		}
	}
	else if (dir == 3) {//animate moving up
		animationDirection = dir;
		directionalFrames[0] = 3;
		directionalFrames[1] = 4;
		directionalFrames[2] = 5;
		y -= speed - -runningSpeed;

		if (++frameCount > frameDelay)
		{
			frameCount = 0;
			curFrame = directionalFrames[index];
			index++;
			if (index > 2) {
				index = 0;
			}
		}
	}
	else {
		switch (animationDirection) {//renders still frame of last standing direction
		case 0: curFrame = 7; break;//face left
		case 1: curFrame = 7; break;//face right
		case 3: curFrame = 4; break;//face up
		case 4: curFrame = 1; break;//face down
		}
	}
	speed = 2;//resets walking speed after player stops running

	//check for collided with foreground tiles and against NPCs
	if (animationDirection == 0) {//left
		if (leftCollision(x, y, frameWidth, frameHeight) || collided) {
			al_play_sample(sample, 0.3, 0.0, 1.5, ALLEGRO_PLAYMODE_ONCE, NULL);
			x = oldx + 2;
			y = oldy;
		}
	}
	else if (animationDirection == 1) {//right
		if (rightCollision(x, y, frameWidth, frameHeight) || collided) {
			al_play_sample(sample, 0.3, 0.0, 1.5, ALLEGRO_PLAYMODE_ONCE, NULL);
			x = oldx - 2;
			y = oldy;
		}
	}
	else if (animationDirection == 3) {//up
		if (topCollision(x, y, frameWidth, frameHeight) || collided) {
			al_play_sample(sample, 0.3, 0.0, 1.5, ALLEGRO_PLAYMODE_ONCE, NULL);
			x = oldx;
			y = oldy +2;
		}
	}
	else if (animationDirection == 4) {//down
		if (bottomCollision(x, y, frameWidth, frameHeight) || collided) {
			al_play_sample(sample, 0.3, 0.0, 1.5, ALLEGRO_PLAYMODE_ONCE, NULL);
			x = oldx;
			y = oldy - 2;
		}
	}
}

//test if the sprite has collided with an end level block
bool Sprite::CollisionEndBlock()
{
	if (endValue(x, y, frameHeight, frameWidth))
		return true;
	else
		return false;
}

//renders sprite at current targeted frame to the display
void Sprite::DrawSprites(int xoffset, int yoffset)
{
	int fx = (curFrame % animationColumns) * frameWidth;
	int fy = (curFrame / animationColumns) * frameHeight;

	//renders explosion after a loss
	if (exploding) {
		al_draw_bitmap_region(
			explode, explosionFrame * explosionFrameW, 0, explosionFrameW, explosionFrameH,
			x + frameWidth / 2 - explosionFrameW / 2 - xoffset, y + frameHeight / 2 - explosionFrameH / 2 - yoffset, 0);
		al_convert_mask_to_alpha(explode, al_map_rgb(255, 255, 255));
	}
	else {//normal movement
		if (animationDirection == 1) {//right
			al_draw_bitmap_region(image, fx, fy, frameWidth, frameHeight, x - xoffset, y - yoffset, ALLEGRO_FLIP_HORIZONTAL);
		}
		else if (animationDirection == 0) {//left
			al_draw_bitmap_region(image, fx, fy, frameWidth, frameHeight, x - xoffset, y - yoffset, 0);
		}
		else if (animationDirection == 2) {//still
			al_draw_bitmap_region(image, 0, 0, frameWidth, frameHeight, x - xoffset, y - yoffset, 0);
		}
		else if (animationDirection == 3) {//up
			al_draw_bitmap_region(image, fx, fy, frameWidth, frameHeight, x - xoffset, y - yoffset, 0);
		}
		else if (animationDirection == 4) {//down
			al_draw_bitmap_region(image, fx, fy, frameWidth, frameHeight, x - xoffset, y - yoffset, 0);
		}
	}
}

//initiates explosion animation after loss
void Sprite::StartExplosion() {
	exploding = true;
	explosionFrame = 0;
	explosionTime = al_get_time();
}

//logic for exploding frames
void Sprite::UpdateExplosion() {

	if (!exploding)
		return;

	double currentTime = al_get_time();

	//gets respective  explosion fram
	if (currentTime - explosionTime > 0.1) {
		explosionFrame++;
		explosionTime = currentTime;
	}

	if (explosionFrame >= 7) {
		exploding = false;
	}
}
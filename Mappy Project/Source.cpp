#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro.h>
#include "SpriteSheet.h"
#include "mappy_A5.h"
#include "NPC.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

//tests if a block is an end level block, returns true or false
bool endValue(int x, int y, int frameHeight, int frameWidth);

//tests if top of block is solid
int topCollision(int x, int y, int frameWidth, int frameHeight);

//tests if bottom of block is solid
int bottomCollision(int x, int y, int frameWidth, int frameHeight);

//tests if right side of block is solid
int rightCollision(int x, int y, int frameWidth, int frameHeight);

//tests if left side of block is solid
int leftCollision(int x, int y, int frameWidth, int frameHeight);

//if the user's last 8 inputs match the cheatcode vectors, the player will be teleported to the end of the maze
void enteredCheatCode(std::vector<int>& input, Sprite& player, int level);

//tests if the player has collided with an npc
bool npcCollision(const Sprite& player, const std::vector<NPC*>& npcs);

void createNPCs(NPC npc, std::vector<NPC*>& npcs);

int main(void)
{
	const int WIDTH = 900;
	const int HEIGHT = 480;
	int level = 0;//used to render layers and save time within recordedTimes array
	int FPS = 60;
	double countDown = 0.0;//time limit
	double timePassed = 0.0;//time counter
	float recordedTImes[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	bool keys[] = { false, false, false, false, false, false };
	enum KEYS { UP, DOWN, LEFT, RIGHT, SPACE, SHIFT };
	bool done = false;
	bool render = false;
	char mapFile[12];
	srand(time(0));

	std::vector<int> lastEightKeys;

	//Player Variable
	Sprite player;
	NPC npc;
	std::vector<NPC*> npcs;

	//allegro variable
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_EVENT_QUEUE* event_queue = NULL;
	ALLEGRO_TIMER* timer;
	ALLEGRO_FONT* font;

	//program init
	if (!al_init())										//initialize Allegro
		return -1;

	//addon init
	al_install_keyboard();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_init_native_dialog_addon();

	player.InitSprites();
	createNPCs(npc, npcs);

	

	display = al_create_display(WIDTH, HEIGHT);			//create our display object
	if (!display)										//test display object
		return -1;

	font = al_load_ttf_font("NiseJSRF.TTF", 27, 0);
	if (!font)
		return -2;

	int xOff = 0;
	int yOff = 0;

	sprintf(mapFile, "sample%d.fmp", level);
	if (MapLoad(mapFile, 1)) exit(1);//loads fmp/map by level

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);//60fps

	//registering event sources
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	//intro to game
	al_show_native_message_box(display, "Welcome!", "\t\tHow to Play!", "- Move with arrows\n- Hold shift to run\n"
		"- Enter the barn to complete a level!\n- Cheat Code : Up Up Down Down Left Right Left Right", 0, 0);

	al_start_timer(timer);
	while (!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			timePassed += 1.0 / FPS;//incrementing time counter by 10th of a second
			countDown = 100.0 - timePassed;//time left
			render = true;

			//directional movement
			if (keys[UP])
				player.UpdateSprites(WIDTH, HEIGHT, 3, npcCollision(player, npcs));
			else if (keys[DOWN])
				player.UpdateSprites(WIDTH, HEIGHT, 4, npcCollision(player, npcs));
			else if (keys[LEFT])
				player.UpdateSprites(WIDTH, HEIGHT, 0, npcCollision(player, npcs));
			else if (keys[RIGHT])
				player.UpdateSprites(WIDTH, HEIGHT, 1, npcCollision(player, npcs));
			else
				player.UpdateSprites(WIDTH, HEIGHT, 2, npcCollision(player, npcs));


			//block to test if player has reach endBlock or if time has run out to goto next level or finish the game
			if (player.CollisionEndBlock() || countDown <= 0.0) {
				recordedTImes[level] = timePassed;//index time it took to complete level
				level += 2;//next level
				
				if (level <= 4) {//if a valid level, loads next fmp file, resets player and timer
					sprintf(mapFile, "sample%d.fmp", level);
					if (MapLoad(mapFile, 1)) exit(1);
					createNPCs(npc, npcs);
					if (countDown <= 0.0)
						al_show_native_message_box(display, "Out of Time", "Time ran out!!", "Onto the next level!", 0, 0);
					else
						al_show_native_message_box(display, "Finished Level!", "Nice job!!", "Onto the next level!", 0, 0);
					player.setX(80);
					player.setY(250);
					player.setRunningSpeed(0);
					for (int i = 0; i < 6; i++) {//reset keys, otherwise sprite moves during popup
						keys[i] = false;
					}
					timePassed = 0.0;
				}
				else//game ends
				{
					int lvl = 0;
					std::ostringstream endResults;
					endResults << std::fixed << std::setprecision(1);//formats to 0.0 format
					for (int i = 0; i < 6; ++i) {
						if (i % 2 == 0) {//times saved at indices 0, 2, and 4
							lvl++;
							endResults << "Level " << lvl << ": " << (recordedTImes[i]) << "s\n\n";
						}
					}
					al_show_native_message_box(display, "Game Over", "Times!", endResults.str().c_str(), 0, 0);
					done = true;
				}
			}
			render = true;
		}

		//closes the game
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}

		//key press actions
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_UP:
				keys[UP] = true;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = true;
				break;
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = true;
				break;
			case ALLEGRO_KEY_LSHIFT:
				keys[SHIFT] = true;
				player.setRunningSpeed(2);//speeds player up
				break;
			}
			lastEightKeys.push_back(ev.keyboard.keycode);//records last keypress
			enteredCheatCode(lastEightKeys, player, level);//checks if player has entered the cheat code
		}

		//key release actions
		else if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_UP:
				keys[UP] = false;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = false;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = false;
				break;
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = false;
				break;
			case ALLEGRO_KEY_LSHIFT:
				keys[SHIFT] = false;
				player.setRunningSpeed(0);//sets player back to walking speed
			}
		}

		//render to display
		if (render && al_is_event_queue_empty(event_queue))
		{
			render = false;

			//update the map scroll position
			xOff = player.getX() + player.getWidth() - WIDTH / 2;
			yOff = player.getY() + player.getHeight() - HEIGHT / 2;

			//avoid moving beyond the map edge
			if (xOff < 0) xOff = 0;
			if (xOff > (mapwidth * mapblockwidth - WIDTH))
				xOff = mapwidth * mapblockwidth - WIDTH;
			if (yOff < 0) yOff = 0;
			if (yOff > (mapheight * mapblockheight - HEIGHT))
				yOff = mapheight * mapblockheight - HEIGHT;

			//draw the background tiles
			MapChangeLayer(level);
			MapDrawBG(xOff, yOff, 0, 0, WIDTH, HEIGHT);

			//draw foreground tiles
			MapChangeLayer(level + 1);
			MapDrawFG(xOff, yOff, 0, 0, WIDTH, HEIGHT, 0);

			player.DrawSprites(xOff, yOff);
			for (NPC* npc : npcs) {
				npc->DrawNPC(xOff, yOff);
			}
			al_draw_textf(font, al_map_rgb(255, 255, 255), 308, 5, 0, "TIME LEFT: %.1f", countDown);//visual timer
			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}
	}
	for (NPC* npc : npcs) {
		delete npc;
	}
	npcs.clear();
	MapFreeMem();
	al_destroy_timer(timer);
	al_destroy_font(font);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);						//destroy our display object
	return 0;
}

//tests if right side of block is solid
int rightCollision(int x, int y, int frameWidth, int frameHeight) {

	int blockTR = y / mapblockheight;//top right
	int blockBR = (y + frameHeight) / mapblockheight;//bottom right
	int blockX = (x + frameWidth) / mapblockheight;//right side
	BLKSTR* blockdataTL = MapGetBlock(blockX, blockTR);

	if (blockdataTL && blockdataTL->tl)//tests current block's collision value
		return blockdataTL->tl;
	BLKSTR* blockdataBL;
	blockdataBL = MapGetBlock(blockX, blockBR);
	if (blockdataBL && blockdataBL->tl)
		return blockdataBL->tl;
	return 0;
}

//tests if left side of block is solid
int leftCollision(int x, int y, int frameWidth, int frameHeight) {

	int blockTL = y / mapblockheight;//top left
	int blockBL = (y + frameHeight) / mapblockheight;//bottom left
	int tileX = x / mapblockheight;//left side
	BLKSTR* blockdataTL = MapGetBlock(tileX, blockTL);

	if (blockdataTL && blockdataTL->tl)//tests current block's collision value
		return blockdataTL->tl;
	BLKSTR* blockdataBL;
	blockdataBL = MapGetBlock(tileX, blockBL);
	if (blockdataBL && blockdataBL->tl)
		return blockdataBL->tl;
	return 0;
}

//tests if top of block is solid
int topCollision(int x, int y, int frameWidth, int frameHeight) {

	int blockTL = x / mapblockwidth;//top left
	int blockTR = (x + frameWidth) / mapblockwidth;//top right
	int blockY = y / mapblockheight;//height of surface
	BLKSTR* blockdataL = MapGetBlock(blockTL, blockY);

	if (blockdataL->bl)//tests current block's collision value
		return blockdataL->bl;
	BLKSTR* blockdataR = MapGetBlock(blockTR, blockY);
	if (blockdataR->bl)
		return blockdataR->bl;
	return 0;
}

//test if bottom of a block is solid or not
int bottomCollision(int x, int y, int frameWidth, int frameHeight) {

	int blockBL = x / mapblockwidth;//bottom left
	int blockBR = (x + frameWidth) / mapblockwidth;//bottom right
	int blockY = (y + frameHeight) / mapblockheight;//surface height
	BLKSTR* blockdataL = MapGetBlock(blockBL, blockY);

	if (blockdataL && blockdataL->tl)//tests current block's collision value
		return blockdataL->tl;
	BLKSTR* blockdataR = MapGetBlock(blockBR, blockY);
	if (blockdataR && blockdataR->tl)
		return blockdataR->tl;
	return 0;
}

bool npcCollision(const Sprite& player, const std::vector<NPC*>& npcs) {
	float playerX = player.getX();
	float playerY = player.getY();
	float playerXBound = playerX + player.getWidth();
	float playerYBound = playerY + player.getHeight();

	for (NPC* npc : npcs) {
		float npcX = npc->getX();
		float npcY = npc->getY() + 5;
		float npcXBound = npcX + npc->getWidth();
		float npcYBound = npcY + npc->getHeight() - 10;

		if (playerXBound > npcX && playerX < npcXBound &&
			playerYBound > npcY && playerY < npcYBound) {
			npc->updateNPC(player.getDir());
			return true;
		}
	}
	return false;
}

//tests if a block is an end level block, returns true or false
bool endValue(int x, int y, int frameHeight, int frameWidth)
{

	int tileLeftX = x / mapblockwidth;
	int tileRightX = (x + frameWidth) / mapblockwidth;
	int tileY = (y - 5) / mapblockheight;
	BLKSTR* data = MapGetBlock(tileLeftX, tileY);

	if (data && data->user1 == 8)
		return true;
	return false;
}

//if the user's last 8 inputs match the cheatcode vectors, the player will be teleported to the end of the maze
void enteredCheatCode(std::vector<int>& input, Sprite& player, int level) {

	std::vector<int> cheatCode = { ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
	ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT };

	if (input.size() > cheatCode.size())//when input reaches capacity (8), the first vector is erased
		input.erase(input.begin());

	if (input == cheatCode) {

		if (level == 0) {
			player.setX(2400);
			player.setY(128);
		}
		else if (level == 2) {
			player.setX(3340);
			player.setY(34);
		}
		else if (level == 4) {
			player.setX(4700);
			player.setY(160);
		}
	}
}

void createNPCs(NPC npc, std::vector<NPC*>& npcs) {
	int sectionX = 100;
	int sectionXBound = 200;
	npcs.clear();
	for (int i = 0; i < 5; i++) {
		
		int x = rand() % (sectionXBound - sectionX + 1) + sectionX; // Random x in [100, 200]
		int y = rand() % (350 - 80 + 1) + 80;

		NPC* npc = new NPC();
		std::string fileName = "NPC_" + std::to_string(i) + ".png";
		npc->InitNPC(fileName, x, y);
		npcs.push_back(npc);

		sectionX += 100;
		sectionXBound += 100;
	}
}
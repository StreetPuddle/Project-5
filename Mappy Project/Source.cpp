#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro.h>
#include "SpriteSheet.h"
#include "mappy_A5.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <ctime>
#include "NPC.h"

using namespace std;

//if the user's last 8 inputs match the cheatcode vectors, the player will be teleported to the end of the maze
void enteredCheatCode(std::vector<int>& input, Sprite& player, int level, int& wins);

//creates a targeted amount of NPCS and adjusts goal; per level
void createNPCs(NPC npc, std::vector<NPC*>& npcs, int level, int& goal);

//tests if the player has collided with an npc
bool npcCollision(const Sprite& player, const std::vector<NPC*>& npcs);

//tests if bottom of block is solid
int bottomCollision(int x, int y, int frameWidth, int frameHeight);

//tests if right side of block is solid
int rightCollision(int x, int y, int frameWidth, int frameHeight);

//proximity detection to engage with an NPC
bool nearNPC(const Sprite& player, const std::vector<NPC*>& npcs);

//tests if left side of block is solid
int leftCollision(int x, int y, int frameWidth, int frameHeight);

//tests if top of block is solid
int topCollision(int x, int y, int frameWidth, int frameHeight);

//tests if block is end level block, returns true or false
bool endValue(int x, int y, int frameHeight, int frameWidth);

int main(void)
{
	bool keys[] = { false, false, false, false, false, false ,false};
	enum KEYS { UP, DOWN, LEFT, RIGHT, SPACE, SHIFT , ENTER};
	float recordedTImes[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	std::vector<int> lastEightKeys;
	bool lostMessage = false;
	bool firstMessage = true;
	bool winMessage = false;
	double timePassed = 0.0;
	const int HEIGHT = 480;
	bool explosion = false;
	double countDown = 0.0;
	const int WIDTH = 900;
	bool render = false;
	int menuSelect = 0;
	bool done = false;
	bool inMenu = 0;
	char mapFile[12];
	int level = 0;
	int wins = 0;
	int FPS = 60;
	int goal = 3;
	int xOff = 0;
	int yOff = 0;
	srand(time(0));

	//Player Variable
	std::vector<NPC*> npcs;
	Sprite player;
	NPC npc;
	

	//allegro variable
	ALLEGRO_EVENT_QUEUE* event_queue = NULL;
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_SAMPLE* sample = NULL;
	ALLEGRO_SAMPLE* menu = NULL;
	ALLEGRO_SAMPLE* boom = NULL;
	ALLEGRO_TIMER* timer = NULL;
	ALLEGRO_FONT* font = NULL;

	//program init
	if (!al_init())//initialize Allegro
		return -1;

	//addon init
	al_init_native_dialog_addon();
	al_init_primitives_addon();
	al_install_keyboard();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	
	if (!al_install_audio())
		return -1;
	if (!al_init_acodec_addon())
		return -1;
	if (!al_reserve_samples(4))
		return -1;
	sample = al_load_sample("overworld.wav");
	if (!sample)
		exit(9);
	menu = al_load_sample("menu.wav");
	if (!menu)
		exit(9);
	boom = al_load_sample("boom.wav");
	if (!boom)
		return(9);

	font = al_load_ttf_font("Pokemon Classic.ttf", 18, 0);
	if (!font)
		return -2;

	display = al_create_display(WIDTH, HEIGHT);//create our display object
	if (!display)//test display object
		return -1;

	sprintf(mapFile, "sample%d.fmp", level);
	if (MapLoad(mapFile, 1)) exit(1);//loads fmp/map by level

	player.InitSprites();
	createNPCs(npc, npcs, 0, goal);

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);//60fps

	//registering event sources
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	//intro to game
	al_show_native_message_box(display, "Welcome!", "\t\tHow to Play!", "- Move with arrows\n- Hold shift to run\n"
		"- Within 90 seconds, get as many wins as there are people to be able\n  to enter the barn by going up to them and pressing enter!\n"
		"- Enter the barn to advance to the next level\n- Winning gets a point, losing loses you a point\n"
		"- Cheat Code : Up Up Down Down Left Right Left Right", 0, 0);

	al_start_timer(timer);
	al_play_sample(sample, 0.5, 0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
	while (!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			if(!inMenu)
				timePassed += 1.0 / FPS;//incrementing time counter by 10th of a second
			countDown = 90.0 - timePassed;//time left
			player.UpdateExplosion();
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
			if ((player.CollisionEndBlock() && wins >= goal) || countDown <= 0.0) {
				recordedTImes[level] = timePassed;//index time it took to complete level
				level += 2;//next level
				wins = 0;

				//if a valid level, loads next fmp file, resets player and timer
				if (level <= 4) {

					//if player ran out of time or successfully beat level
					if (countDown <= 0.0)
						al_show_native_message_box(display, "Out of Time", "Time ran out!!", "Onto the next level!", 0, 0);
					else
						al_show_native_message_box(display, "Finished Level!", "Nice job!!", "Onto the next level!", 0, 0);
					
					for (int i = 0; i < 6; i++) {//reset keys, otherwise sprite moves during popup
						keys[i] = false;
					}

					sprintf(mapFile, "sample%d.fmp", level);
					if (MapLoad(mapFile, 1))//validity check
						exit(1);

					createNPCs(npc, npcs, level, goal);
					player.setRunningSpeed(0);
					timePassed = 0.0;
					player.setY(250);
					player.setX(80);
					
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
			done = true;

		//key press actions
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{

			//if engaged with an NPC
			if (inMenu) {
				switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_UP:
					menuSelect = !menuSelect;//flips between 0/1
					al_play_sample(menu, 0.3, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					break;
				case ALLEGRO_KEY_DOWN:
					menuSelect = !menuSelect;//flips between 0/1
					al_play_sample(menu, 0.3, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					break;
				case ALLEGRO_KEY_ENTER:
					
					if (menuSelect == 0) {
						inMenu = false;

						//25% chance of winning
						bool win = rand() % 4 == 1;
						wins += win ? 1 : 0;
						
						al_draw_filled_rectangle(0, 350, WIDTH, HEIGHT, al_map_rgb(240, 240, 240));
						al_draw_rectangle(15, 365, WIDTH - 15, HEIGHT - 15, al_map_rgb(80, 80, 80), 2);
						al_flip_display();
						al_rest(1);
						
						for (int i = 0; i < 3; i++) {//anticipation.....
							std::string message = std::string(i * 2, ' ') + ".";
							al_draw_text(font, al_map_rgb(0, 0, 0), 50, 390, 0, message.c_str());
							al_flip_display();
							al_rest(1);
						}

						//reveal who won
						al_draw_filled_rectangle(0, 350, WIDTH, HEIGHT, al_map_rgb(240, 240, 240));
						al_draw_rectangle(15, 365, WIDTH - 15, HEIGHT - 15, al_map_rgb(80, 80, 80), 2);
						al_draw_text(font, al_map_rgb(0, 0, 0), 50, 390, 0,
							win ? "Darn! You won...!" : "Haha! I won, you lost!!!");
						al_flip_display();
						al_rest(3);

						//upon losing, player will lose a win and explode lol
						if (!win) {
							player.StartExplosion();
							al_play_sample(boom, 0.7, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
							wins--;
						}
					}
					else {
						inMenu = false;
						firstMessage = true;
					}
					break;
				}
			}

			//while key is pressed
			else {
				if (!player.explodingAnim()) {
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
					case ALLEGRO_KEY_ENTER:
						keys[ENTER] = true;
						//tests if player can engage in a battle
						if (nearNPC(player, npcs)) {
							inMenu = true;
							menuSelect = 0;
						}
						break;
					}
				}
			}
			lastEightKeys.push_back(ev.keyboard.keycode);//records last keypress
			enteredCheatCode(lastEightKeys, player, level, wins);//checks if player has entered the cheat code
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
				break;
			case ALLEGRO_KEY_ENTER:
				keys[ENTER] = false;
				break;
			}
		}

		//render to display
		if (render && al_is_event_queue_empty(event_queue))
		{
			render = false;
			
			if (inMenu) {//engaged with an NPC

				if (firstMessage) {
					firstMessage = false;
					al_draw_filled_rectangle(0, 350, WIDTH, HEIGHT, al_map_rgb(240, 240, 240));
					al_draw_rectangle(15, 365, WIDTH - 15, HEIGHT - 15, al_map_rgb(80, 80, 80), 2);

					al_draw_text(font, al_map_rgb(0, 0, 0), 50, 390, 0, "Heads I win, tail you lose!");
					al_flip_display();
					al_rest(3);
				}
				al_draw_filled_rectangle(0, 350, WIDTH, HEIGHT, al_map_rgb(240, 240, 240));
				al_draw_rectangle(15, 365, WIDTH - 15, HEIGHT - 15, al_map_rgb(80, 80, 80), 2);
				al_draw_text(font, al_map_rgb(0, 0, 0), 50, 390, 0, "Risk your honor?");
				al_flip_display();

				//prompting player yes or no
				al_draw_filled_rectangle(750, 270, 890, 340, al_map_rgb(240, 240, 240));
				al_draw_rectangle(750, 270, 890, 340, al_map_rgb(80, 80, 80), 2);
				al_draw_text(font, menuSelect == 0 ? al_map_rgb(50, 50, 50) : al_map_rgb(100, 100, 100),
					770, 265, 0, menuSelect == 0 ? "- OK" : "  OK");
				al_draw_text(font, menuSelect == 1 ? al_map_rgb(50, 50, 50) : al_map_rgb(100, 100, 100),
					770, 295, 0, menuSelect == 1 ? "- No!" : "  No!");
				al_flip_display();

			}
			else {

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

				//render player and NPCs
				player.DrawSprites(xOff, yOff);
				for (NPC* npc : npcs) {
					npc->DrawNPC(xOff, yOff);
				}

				//HUD for timer and score
				al_draw_textf(font, al_map_rgb(255, 255, 255), 220, 0, 0, "WINS: %d           TIME LEFT: %.1f", wins, countDown);
				MapUpdateAnims();
				al_flip_display();
			}
		}
	}
	//end of game, clear memory
	for (NPC* npc : npcs) {
		delete npc;
	}

	//deallocating
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);
	al_destroy_sample(sample);
	al_destroy_sample(menu);
	al_destroy_sample(boom);
	al_destroy_timer(timer);
	al_destroy_font(font);
	player.~Sprite();
	npcs.clear();
	MapFreeMem();
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

//tests if the player has collided with an npc
bool npcCollision(const Sprite& player, const std::vector<NPC*>& npcs) {

	//player bounds
	float playerX = player.getX();
	float playerY = player.getY();
	float playerXBound = playerX + player.getWidth();
	float playerYBound = playerY + player.getHeight();

	for (NPC* npc : npcs) {

		//npc bounds
		float npcX = npc->getX();
		float npcY = npc->getY() + 5;
		float npcXBound = npcX + npc->getWidth();
		float npcYBound = npcY + npc->getHeight() - 10;

		//if collided, npc will turn to look at the player
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
void enteredCheatCode(std::vector<int>& input, Sprite& player, int level, int& wins) {

	std::vector<int> cheatCode = { ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
	ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT };

	if (input.size() > cheatCode.size())//when input reaches capacity (8), the first vector is erased
		input.erase(input.begin());

	if (input == cheatCode) {

		wins++;//increment wins
	}
}

//creates a targeted amount of NPCS and adjusts goal; per level
void createNPCs(NPC npc, std::vector<NPC*>& npcs, int level, int& goal) {

	//collumn where NPC can spawn
	int sectionX = 100;
	int sectionXBound = 200;
	int buffer = 10;

	//difficulty adjustments
	int amount = level + 3;
	goal = amount;

	npcs.clear();
	for (int i = 0; i < amount; i++) {
		
		int x = rand() % (sectionXBound - sectionX + 1) + sectionX;
		int y = rand() % (350 - 80 + 1) + 80;
		if (i > 6)//avoid barn
			y = 300;
		int sprite = rand() % 11;//random sprite

		NPC* npc = new NPC();
		std::string fileName = "NPC_" + std::to_string(sprite) + ".png";
		npc->InitNPC(fileName, x, y);
		npcs.push_back(npc);

		sectionX += 100 + buffer;
		sectionXBound += 100;
	}
}

//proximity detection to engage with an NPC
bool nearNPC(const Sprite& player, const std::vector<NPC*>& npcs) {
	
	//player bounds
	float playerX = player.getX();
	float playerY = player.getY();
	float playerXBound = playerX + player.getWidth();
	float playerYBound = playerY + player.getHeight();

	for (NPC* npc : npcs) {

		//npc bounds
		float npcX = npc->getX();
		float npcY = npc->getY();
		float npcXBound = npcX + npc->getWidth();
		float npcYBound = npcY + npc->getHeight();

		//detecting proximity
		bool triggerX = (playerXBound >= npcX - 5) && (playerX <= npcXBound + 5);
		bool triggerY = (playerYBound >= npcY - 5) && (playerY <= npcYBound + 5);

		//returns true if within proximity
		if (triggerX && triggerY)
			return true;
	}
	return false;
}
#include <avr/pgmspace.h>
#include <Gamby.h>
#include "knight.h"
#include "level.h"
#include "level_bitmap.h"
#include "level_data.h"
#include "font_bitmap.h"
#include "enemy.h"
#include "egg.h"
#include "lava.h"
#include "helper.h"

#define LEVEL_WIDTH 12
#define LEVEL_HEIGHT 8
#define LEVEL_SIZE 96

extern GambyGraphicsMode gamby;
extern long score;
extern char life;
extern char game_state;
extern char eggs;
extern char enemies;

LevelElement levelElements[MAX_ELEMENT_RECORDS];
char element_count = 0;

char level_current = 0;
char level_total = 0;
char enemy_speed = 2;

void level_reset()
{
  level_current = 0;
  level_total = 0;
  enemy_speed = 2;
}

void level_update()
{ 
   gamby.clearScreen();
   
   level_draw();
   knight_draw();
   
   level_element_handle();
   
   //draw score and lives on the screen
   helper_print_number(font_bitmap,25,60,score);

   for (char i=0; i<life; i++)
   {
	helper_draw_char(font_bitmap,64+(i*4), 60, 43);
   }
   
   gamby.update();
}

void level_calculate()
{
  char next_lvl = level_total + 1;
  eggs = 0;
  enemies = 0;
  
  if (next_lvl % 5 == 0)
  {
    eggs = 2;
    if (next_lvl > 5) eggs++;
  } else {
    enemies = 1;
    if (next_lvl > 3) enemies++;
    if (next_lvl > 5) enemies++;
  }
}

void level_advance()
{
  
  level_current++;
  level_total++;
  //loop through each of the level patterns as they advance
  if (level_current > 9) level_current = 2;
    
  //clear the array of the previous contents
  memset(levelElements, 0, sizeof(levelElements));
  element_count = 0;
  
  //0 = knight egg
  level_element_add(TYPE_EGG_KNIGHT, 0, 0, STATE_HIDDEN, 1);
  
  if (level_total > 2)
  {
    //add in lava
    level_element_add(TYPE_LAVA, 1, 56, STATE_VISIBLE, 1);
    level_element_add(TYPE_LAVA, 8, 56, STATE_VISIBLE, 1);
    level_element_add(TYPE_LAVA, 80, 56, STATE_VISIBLE, 1);
    level_element_add(TYPE_LAVA, 88, 56, STATE_VISIBLE, 1);
  }
  
  //every 5th level is an 'egg wave'
  if (level_total % 5 == 0)
  {
    //add in other elements
    level_element_add(TYPE_EGG_ENEMY, 88, 8, EGG_STATE_FALL, 2);
    if (level_total > 5) 
	{
	   level_element_add(TYPE_EGG_ENEMY, 0, 22, EGG_STATE_FALL, 2);
	}
    level_element_add(TYPE_EGG_ENEMY, 88, 34, EGG_STATE_FALL, 2);
	
    //enemies speed up on each egg wave, until they are all fast
    if (enemy_speed > 0) enemy_speed --;
  } else {
    //add in other elements
    level_element_add(TYPE_BIRD_RIDER, 88, 8, STATE_MOVE_LEFT, enemy_speed);
    if (level_total > 5) 
	{
	   level_element_add(TYPE_BIRD_RIDER, 0, 22, STATE_MOVE_RIGHT, enemy_speed);
	}
    if (level_total > 3)
	{
	   level_element_add(TYPE_BIRD_RIDER, 88, 34, STATE_MOVE_LEFT, enemy_speed);
	}
  }
  
  knight_init(48,39);
  level_draw();
}

void level_draw()
{  
   //read the level data and draw the level on the screen
   for (char y=0; y < LEVEL_HEIGHT; y++)
   {
      for (char x=0; x < LEVEL_WIDTH; x++)
      {
          char current_block = level_get_block(x, y);
          if (current_block > 0) gamby.drawSprite(x * 8, y * 8, level_bitmap, current_block-1);
      }
   }
}

//compare the given x,y coordinate against the level data
char level_check_move(char x, char y)
{
   char level_x = x / 8;
   char level_y = y / 8;
   if ((level_x > 11) || (level_y > 8)) return 0;
   return level_get_block(level_x, level_y);
}


char level_check_move_h(char x, char y, char &tile_y)
{
   char tile_x_pixels = (x - (x % 8)); 	//calculate the x position in pixels we are checking against
   char testend = x + 8; //calculate the end of testing
   tile_y = y >> 3; //calculate the y position (map coordinates) of the tiles we want to test   
   char tile_x = tile_x_pixels >> 3; //calculate map x coordinate for first tile
 
    //loop while the start point of the test tile is inside the bounding box
    while(tile_x_pixels <= testend){
       if(level_get_block(tile_x, tile_y) > 0)	//is a solid tile is found at tile_x, tile_y?
			return 1;	
			
       tile_x++;		//increase tile x map coordinate
       tile_x_pixels +=8;	//increase tile x pixel coordinate
	}

	return 0;
}

char level_check_move_v(char x, char y, char &tile_x)
{
   char tile_y_pixels = (y - (y % 8)); 	//calculate the x position in pixels we are checking against
   char testend = y + 8; //calculate the end of testing
   tile_x = x >> 3; //calculate the y position (map coordinates) of the tiles we want to test   
   char tile_y = tile_y_pixels >> 3; //calculate map x coordinate for first tile
 
    //loop while the start point of the test tile is inside the bounding box
    while(tile_y_pixels <= testend){
       if(level_get_block(tile_x, tile_y) > 0)	//is a solid tile is found at tile_x, tile_y?
			return 1;	
			
       tile_y++;		//increase tile x map coordinate
       tile_y_pixels +=8;	//increase tile x pixel coordinate
	}

	return 0;
}

//return the block at a given 'level' x,y coordinate in the current level
char level_get_block(char level_x, char level_y)
{
   return pgm_read_byte_near (level_data + (level_current * LEVEL_SIZE) + level_x + (level_y * LEVEL_WIDTH));
}

void level_element_add(char type, char x, char y, char state, char speed)
{
      levelElements[element_count].id = element_count;
      levelElements[element_count].type = type;
      levelElements[element_count].x = x;
      levelElements[element_count].y = y;
      levelElements[element_count].step = 0;
      levelElements[element_count].state = state;
      levelElements[element_count].counter = COUNTER_START;
      levelElements[element_count].speed = speed;
      levelElements[element_count].speed_counter = 0;
		  
      element_count++;  
}

void level_element_handle()
{
  char active_count = 0;
  
  for (char i=0; i < element_count; i++)
  {
    //if the element is hidden, skip past it
    if (levelElements[i].state > STATE_HIDDEN)
    {  
      if (levelElements[i].type <= 50)
      {	
         if (level_test_element(levelElements[i], knight_get_x(), knight_get_y()))
         {
           //must be 'playing' and not 'dead' to trigger a hit event
           if (knight_get_state() == 0)
           {
             if (levelElements[i].type < 9)
             {
               //if the knight is higher he 'wins'
               if (knight_get_y() < levelElements[i].y)
               {
                 levelElements[i] = enemy_hit(levelElements[i]);
               } else {
                    if (life > 0){
                       knight_hit();
                       active_count++;
                       life--;
       	          } else {
    	             game_state = 8;
    	          }
    	       }
             } else {
                  if (life > 0){
                      knight_hit();
                      active_count++;
                      life--;
       	          } else {
    	             game_state = 8;
    	          }
               
             }
           } else {
             if (levelElements[i].type < 9)
             {
               levelElements[i] = enemy_move(levelElements[i]);
               active_count++;
             } else {
               levelElements[i] = lava_move(levelElements[i]);
             }
           }
         } else {
           if (levelElements[i].type < 9)
           {
             levelElements[i] = enemy_move(levelElements[i]);
             active_count++;
           } else {
             levelElements[i] = lava_move(levelElements[i]);
           }
         }
      } else {
         
         if (knight_get_state() == 0)
         {
           if (level_test_element(levelElements[i], knight_get_x(), knight_get_y()))
           {
  		levelElements[i] = egg_hit(levelElements[i]);
           } else {
                levelElements[i] = egg_move(levelElements[i]);
                active_count++;
           }      
         } else {
           levelElements[i] = egg_move(levelElements[i]);
           active_count++;
         }
	 
      }
    }
  }
  //if nothing we are tracking is active, advance the level
  if ((active_count == 0) && (game_state < 8 )) game_state = 9;
}

//test for collision in level elements
bool level_test_element (LevelElement element, char testX, char testY)
{
   if ((element.y + 8) < testY) return false;
   if (element.y > (testY + 8)) return false;
   if ((element.x + 8) < testX) return false;
   if (element.x > (testX + 8)) return false;
   return true;
}

LevelElement level_get_element (char id)
{
  return levelElements[id];
}

void level_update_element (LevelElement element)
{
  levelElements[element.id] = element;
}


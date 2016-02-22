#include <avr/pgmspace.h>
#include <Gamby.h>
#include "enemy.h"
#include "enemy_bitmap.h"
#include "level.h"
#include "knight.h"
#include "sound.h"

extern GambyGraphicsMode gamby;
extern long score;

#define STEP_LENGTH  2

LevelElement enemy_move(LevelElement element)
{
  char adjust;
  
  //draw new monster bitmap
  if (element.state > STATE_HIDDEN)
  {
  
    if (element.speed_counter == element.speed)
    {
        element.speed_counter = 0;
        element.step++;
        if (element.step > 1) element.step = 0;
       
        switch (element.state)
        {
    	 
    	  case STATE_MOVE_LEFT:
               element.state = STATE_VISIBLE;
               if (element.x > STEP_LENGTH)
               {
                 if (level_check_move_v(element.x - STEP_LENGTH, element.y, adjust) == 0)
                 {
                    element.x -= STEP_LENGTH;
                    element.state = STATE_MOVE_LEFT;
                 }
                 else
                 {
                   //check above; if it is possible to move up, then do so
                   if (level_check_move_h(element.x, element.y-STEP_LENGTH, adjust) == 0)
                   {
                     element.y -= STEP_LENGTH;
                     element.state = STATE_MOVE_LEFT;
                   }
                   else
                   {
                     //if it is possible to move down, then try that as well
                     if (level_check_move_h(element.x, element.y+STEP_LENGTH, adjust) == 0)
                     {
                        element.y += STEP_LENGTH;
                        element.state = STATE_MOVE_LEFT;
                     }
                   }
                 }
               } else {
    		     element.x = 88;
                         element.state = STATE_MOVE_LEFT;
    		   }
    	   break;	   
    	   
    	  case STATE_MOVE_RIGHT:
               element.state = STATE_VISIBLE;
               if (element.x < 86)
               {
                 if (level_check_move_v(element.x + 8 + STEP_LENGTH, element.y, adjust) == 0)
                 {
                    element.x += STEP_LENGTH;
                    element.state = STATE_MOVE_RIGHT;
                 }
                 else
                 {
                   //check above; if it is possible to move up, then do so
                   if (level_check_move_h(element.x + 8, element.y-STEP_LENGTH, adjust) == 0)
                   {
                     element.y -= STEP_LENGTH;
                     element.state = STATE_MOVE_RIGHT;
                   }
                   else
                   {
                     //if it is possible to move down, then try that as well
                     if (level_check_move_h(element.x + 8, element.y+STEP_LENGTH, adjust) == 0)
                     {
                        element.y += STEP_LENGTH;
                        element.state = STATE_MOVE_RIGHT;
                     }
                   }
                 }
                 
               } else {
    		     element.x = 0;
                         element.state = STATE_MOVE_RIGHT;
    		   }
    	   break;		      
        }
       }
       else
       {
         element.speed_counter++;
       }
       gamby.drawSprite(element.x, element.y, enemy_bitmap, (element.type + (element.state - 2) + element.step)); 
	
	//decrement counter, if active
	if (element.counter > 0) element.counter--;
  }
  return element;
}

LevelElement enemy_hit(LevelElement element)
{
  char newX = knight_get_x();
  
  if (element.type == 0)
  {
    switch (knight_get_facing())
	{
	  case FACING_LEFT:
	    if (newX > 12 + knight_get_y_velocity())
		{
		  newX -= 12 + knight_get_y_velocity();
		} else {
		  newX = 88;
		}
                level_element_add(TYPE_BIRD, newX, element.y, STATE_MOVE_LEFT, 0);
		//level_element_add(TYPE_EGG_ENEMY, newX, element.y + 8, EGG_STATE_FALL, 3);
		score += (100 * element.speed);
		sound_play_sfx(1);
		break;


	  case FACING_RIGHT:
  	    if ((newX + 12 + knight_get_y_velocity()) < 81)
		{
		  newX += 12 + knight_get_y_velocity();
		} else {
		  newX = 0;
		}
                level_element_add(TYPE_BIRD, newX, element.y, STATE_MOVE_RIGHT, 0);
		//level_element_add(TYPE_EGG_ENEMY, newX, element.y + 8, EGG_STATE_FALL, 3);
		score += (100 * element.speed);
		sound_play_sfx(1);
		break;

    }		
  } else {
    score += 100;
  }
   
  element.state = STATE_HIDDEN;
   
  return element;
}




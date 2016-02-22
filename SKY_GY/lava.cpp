#include <avr/pgmspace.h>
#include <Gamby.h>
#include "lava.h"
#include "lava_bitmap.h"

extern GambyGraphicsMode gamby;

LevelElement lava_move(LevelElement element)
{
  char adjust;
  
  //draw new monster bitmap
  if (element.state > STATE_HIDDEN)
  {
    if (element.state == LAVA_STATE_BURN)
    {
      if (element.speed_counter == element.speed)
      {
          element.speed_counter = 0;
         
  	element.step++;
  	if (element.step > 3) element.step = 0;
  
      }
         else
      {
           element.speed_counter++;
      }
    } else {
      //set an initial, random animation frame
      element.step = random() % 4;
      element.state = LAVA_STATE_BURN;
    }
    
    gamby.drawSprite(element.x, element.y, lava_bitmap, element.step); 
  }
  return element;
}




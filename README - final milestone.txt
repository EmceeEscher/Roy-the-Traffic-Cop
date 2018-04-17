Nicholas Chan, 39146113
Theo Chitayat, 57110132
Yao Liu, 36521110
Haoyu (Mason) Yang, 10467158
Jordan Roth, 46305108
Jacob Brunette, 12861143

Changes made between milestone 4 (robust play) and final submission:

-Added ambulances. From level 4 onward, sometimes ambulances will come across the screen (higher chance to happen at later levels). They will also spawn from the beginning in hardcore mode.
	First a warning symbol will appear at an outgoing lane, then after a few seconds, an ambulance will cross the screen before exiting the other side. 
	It will collide with any cars in its path (using the same bounding box/mesh collision detection as the rest of our cars). It also does a slight curve through
	the intersection so it can switch lanes (so it will always be an outgoing lane), but we did this manually instead of using a Bezier curve, because the Bezier
	curve didn't work very well for the type of turning we wanted to do.
-Added a high score system. The game will save your top 3 high scores and display them on game over. We had lots of trouble trying to render text, so instead we ended up using the 
	number texture that we used for the in-game score on the high score screen as well. 
-Added weather. Enabled at level 3 and beyond (or immediately in hardcore mode), adds to game immersion as well as provides some difficulty as weather can be obstructive to placard indicator.
	There are a number of weather types, including darkness, fog, as well as dawn and twilight lighting. The game will choose between these randomly depending on level and 
	time of year (as shown in the date in the top right). 
-Added headlights to the cars. These lights will blink in the direction the car is going to turn (or will stay on if going straight). This was implemented in the fragment shader.
-Finetuned our level system. Levels now control number of active lanes, number of points required to progress, duration cars will wait before turning automatically, 
	probability of villain cars, probability of ambulances, and type of weather. Number of spacebar presses to clear intersection is no longer tied to level.
-Fixed a number of bugs, including:
	-memory leak related to car deletion
	-game reset not properly resetting all variables
	-SDL exception on game reset
	-timer/score not stopping on game over
	-clipping with splash screens

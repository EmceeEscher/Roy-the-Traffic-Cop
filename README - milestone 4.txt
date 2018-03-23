Nicholas Chan, 39146113
Theo Chitayat, 57110132
Yao Liu, 36521110
Haoyu (Mason) Yang, 10467158
Jordan Roth, 46305108
Jacob Brunette, 12861143

Items implemented for the fourth milestone (robust play):
- Added basic levels
  - After scoring a certain number of points, a splash screen will appear, and the player
    will move to the next level
- Added pause, game start, and reset buttons now (assigned to P, G, and R keys respectively)
- Added a score display (with animated coin icon) that keeps track of how many cars
  you've successfully moved through an intersection (crashed cars don't count towards score)
- Added a mechanic to remove crashed cars from the intersection by repeatedly pressing spacebar
  (an image will pop up on screen to signify when to press the spacebar)
- Added some more sound effects and music tracks
- Cleaned up a number of AI bugs:
  - Cars won't turn in a different direction than their turn arrow now
  - Villain cars won't change their turn direction when they are first in line now
- Cleaned up a number of bugs / issues with collision:
  - fixed bug with converting local coordinates of mesh vertices to world coordinates
  - fixed bug where it wouldn't recognize collisions with second car in line
    - both of these fixes mean there are now much fewer missed collisions
  - also made it so now both cars in a collision will bounce and spin instead of just one

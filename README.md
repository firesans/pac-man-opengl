GRAPHICS ASSIGNMENT 1  (Pacman 2-D Game)
----------------------

EXECUTION 
--------- 
 Make sure you have opengl libraries installed. 
  
For installation on "Ubuntu" 
- sudo apt install libglew-dev libglfw3-dev libglm-dev cmake pkg-config

After required installations : 
- Create a folder '/build' (mkdir build)
- Change the directory to '/build' (cd build)
- Run 'cmake ..'
- Run 'make all'
- Run the executable to start playing 


CONTROLS : 
-------------
- The player can be navigated using the A and D for left and right.
- The player can be made to jump using space button.
- Zooming and panning are implemented. On pressing the 'up' button, the screen zooms in and on pressing the 'down' button, the screen zooms out.
- For panning, on pressing the 'right' key, the screen pans right, and on pressing the 'left' button the screen pans left.
- Virtually 'infinite' world is created.

FEATURES IMPLEMENTED :
------------------------
- Balls are randomly generated of different colors and sizes.
- The balls upon being destroyed, dissapear. The score is added depending upon the colour and sixex of ball destroyed.
- When the ball hits the surface of the pond, its speed decreases.
- Upon reaching the circular surface, it follows the trajectory of the pond, and reaches the bottom.
- When the ball encounters a deflector, it gets reflected following the laws of reflection.
- When the ball encounters a magnet, it gets attracted to it.
- The deflectors and magnets occur randomly.
- When the ball hits the trampoline, it bounces back up with a higher speed, and to a higher height.
- Moving porcupines are implemented.
- Energy bar is implemented at the right side of the screen, which shows the energy remaining for the player. On striking the moving porcupines or being attracted by the magnet, 
  the energy pointer reduces.
- When it reaches zero, the screen becomes blank and the game is over.
- The ball can be dragged using the mouse.
- When the window is panned left or right, the player can move.



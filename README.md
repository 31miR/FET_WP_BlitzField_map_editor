# What is this useful for
This was made as a tool for building another app [https://github.com/31miR/FET_WP_BlitzField].\
Basically, it's a helper for making a map for a top down game.\
You can open a .bmp file inside of it (on which a top down map is drawn), and then you can mark the actual objects on the map.

## What exactly can I mark?
You can mark high or low objects, node and node connections.\
A high object is any object that cannot be jumped over.\
A low object is any object that can be jumped over.\
A node is a place on a map where an enemy can stand or move to (or even spawn).\
A node connection is a straight line between two nodes and represents the path that an enemy can take.

## How do I use it?
Open a bmp file from the drop down menu. Once a valid menu has been opened you can mark over it.\
When the app is first started, you are in SELECT mode. There are different operating modes and here is what they mean:
- SELECT - for moving around the image
- DRAW_HIGH - for drawing high objects
- DRAW_LOW - for drawing low objects
- DRAW_NODE - for drawing node objects
- DRAW_CONNECTION - for drawing node connections

To enter these modes press the corresponding button on the keyboard:
- SELECT - s
- DRAW_HIGH - h
- DRAW_LOW - l
- DRAW_NODE - n
- DRAW_CONNECTION - c

When in SELECT mode, use your mouse to move around the image. Press and hold left mouse button and then move the mouse. You will move in opposite direction of the mouse movement.\
When in DRAW_HIGH or DRAW_LOW mode, press and hold left mouse button to start drawing a rectangle, move the mouse and release the left mouse button when the rectangle is the way you want it to be. Your new rectangle represents the object.\
When in DRAW_NODE mode, press left mouse button to place the node.\
When in DRAW_CONNECTION mode, press left mouse button on one node to select it. Press the left mouse button again on a different node to select that one also. The selected nodes will be connected.

## Can I draw a map inside of this?
NO! You should draw the map with the walls and tables and everything else in another tool such as photoshop. Then, just open the .bmp image and draw your high and low objects over those walls and tables.

## I've marked what I needed, what now?
You can save the map object coordinates file from a drop down menu. Make sure to write the extension when typing the name of the file you are generating.

## Can I use this in any game?
Not quite, this is written specially for the game that was referenced at the top of this readme. In order to use this you would have to implement the same map logic as the one in the referenced game.

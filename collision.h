#ifndef COLLISION_H
#define COLLISION_H

//Collision Layers
#define COL_FOREG 0x1			//All foreground tiles
#define COL_LEFTH 0x2			//Lefthand boundary foreground tiles
#define COL_RIGHTH 0x4			//Righthand boundary foreground tiles
#define COL_ENEMY 0x8			//Enemy sprites
#define COL_BOMB 0x10			//Bombs
#define COL_EXPLOSION 0x20		//Explosions
#define COL_COIN 0x40			//Coins
#define COL_GEM 0x80				//Gems
#define COL_PLAYER 0x100			//Players
#define COL_DOOR 0x200			//Door (Exit)
#define COL_MINE 0x400			//Mines
#define COL_PICKUP 0x800			//Pick ups
#define COL_ROOF 0x1000			//Ceilings
#define COL_DESTROY_TILE 0x2000 	//Destructable Background Tiles

#endif


void InitCollisions(int n);
void ResetCollisions(int n);
void ShowCollisionLayer(int z);
int CollideImage(SDL_Surface* src,int x,int y,int sx,int sy,int w,int h,int cm);
void CollideImage(SDL_Surface* src,int x,int y,int sx,int sy,int w,int h,int cm,int wm,char id);


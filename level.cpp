#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_mixer.h>
#include "level.h"
#include "collision.h"
#include "wheelie.h"
#include "DestructTile.h"
#include "bomb.h"
#include "explosion.h"
#include "smoke.h"
#include "mine.h"
#include "dynamite.h"
#include "reward.h"
#include "grog.h"
#include "slinky.h"
#include "player.h"
#include "gem.h"
#include "coin.h"
#include "pickup.h"
#include "pickedup16.h"
#include "pickedup32.h"
#include "hiscore.h"

extern void Text(char* strg,int x,int y);
extern void SmallText(char* strg,int x,int y);

extern SDL_Surface* tilesBitmap;
extern SDL_Surface* screen;

extern Mix_Music *music;
extern int soundpresent;
extern int background_on;

extern Twheelie wheelie;
extern Tgrog grog;
extern Tslinky slinky;
extern TDestructTile DestructTile;
extern Tbomb bomb;
extern Texplosion explosion;
extern Tsmoke smoke;
extern Tmine mine;
extern Treward reward;
extern Tdynamite dynamite;
extern Tplayer player;
extern Tgem gem;
extern Tcoin coin;
extern Tpickup pickup;
extern Tpickedup16 pickedup16;
extern Tpickedup32 pickedup32;
extern Thiscore hiscore;

extern char map[4] [40] [30];

int avetime[]={5,5,5,5,5,5,5,5,5,5};

//level class
/*
****
Level Bank format (5032 bytes)
-----------------

HEADER
======
Data									Offset			Size
----									------			----
number of levels						0				Int
level size							4				Int

LEVELDATA
=========
Player 1 x pos						0				Int
Player 1 y pos						4				Int
player 2 x pos						8				Int
player 2 y pos						12				Int
Number of enemies						16				Int
enemy 1 x pos							20				Int
enemy 1 y pos							24				Int
enemy 1 dir							28				Int
enemy 1 type							32				Int
enemy 1 active						36				Int
..... continue enemies (room for 8) ......
exit door x pos						180				int
exit door y pos						184				int
10 spare ints							188 - 227		Int
level background number				228				Int
layer 0 tile data						232				Byte
layer 1 tile data						1432				Byte
layer 2 tile data						2632				Byte
tile boundary data					3832 to 5031		Byte
****
*/

Tlevel::Tlevel()
{
}

int Tlevel::calctime(int t)
{
	int tot=0;
	for (int z=0;z<=8;z++)
	{
		avetime[z]=avetime[z+1];
		tot+=avetime[z];
	}
	avetime[9]=t;
	tot+=t;
	tot/=10;
	return tot;

}

int Tlevel::loadgfx(SDL_Surface** s, char* n)
{
    SDL_Surface* tempsurf;
    SDL_Surface* tempsurf2;
    tempsurf = IMG_Load(n);
    if(!tempsurf)
	{
		printf("SDL %s not found.\n",n);								// debug output example for serial cable
		return 0;
	}
	else
	{
	    *s = SDL_ConvertSurface(tempsurf, screen->format, SDL_HWSURFACE);
	    tempsurf2 = SDL_ConvertSurface(tempsurf, screen->format, SDL_HWSURFACE);
        SDL_SetColorKey( *s, SDL_SRCCOLORKEY, SDL_MapRGB(tempsurf2->format, 255, 0, 255) );
        SDL_FreeSurface(tempsurf);
        SDL_FreeSurface(tempsurf2);
	}
	return 1;

}



void Tlevel::drawTile(SDL_Surface* imageSurface, int x, int y, int t)
{

	SDL_Rect srcRect;
	srcRect.y = t/(imageSurface->w/16);
	srcRect.x = t-(srcRect.y*(imageSurface->w/16));
	srcRect.x = srcRect.x*16;
	srcRect.y = srcRect.y*16;
	srcRect.w = 16;
	srcRect.h = 16;

	//printf("tile %d  x %d  y %d\n",t,srcRect.x,srcRect.y);

	SDL_Rect dstRect;
	dstRect.x = x;
	dstRect.y = y;
	dstRect.w = 16;
	dstRect.h = 16;

	SDL_BlitSurface(imageSurface, &srcRect, screen, &dstRect);
}

int Tlevel::getTileX(SDL_Surface* imageSurface, int t)
{

	SDL_Rect srcRect;
	srcRect.y = t/(imageSurface->w/16);
	srcRect.x = t-(srcRect.y*(imageSurface->w/16));
	srcRect.x = srcRect.x*16;
	return srcRect.x;

}

int Tlevel::getTileY(SDL_Surface* imageSurface, int t)
{

	SDL_Rect srcRect;
	srcRect.y = t/(imageSurface->w/16);
	srcRect.y = srcRect.y*16;
	return srcRect.y;

}


void Tlevel::drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height)
{
	SDL_Rect srcRect;
	srcRect.x = srcX;
	srcRect.y = srcY;
	srcRect.w = width;
	srcRect.h = height;

	SDL_Rect dstRect;
	dstRect.x = dstX;
	dstRect.y = dstY;
	dstRect.w = width;
	dstRect.h = height;

    SDL_BlitSurface(imageSurface, &srcRect, screenSurface, &dstRect);

}


char Tlevel::PeekByte(char* mem,int addr)
{
    return mem[addr] & 0xFF;
}

int Tlevel::PeekInt(char* mem,int addr)
{
    return (mem[addr] | (mem[addr+1]<<8) | (mem[addr+2]<<16) | (mem[addr+3]<<24)) & 0xFFFFFFFF;
}

void Tlevel::Init()
{
    loadgfx(&exitpost_appear,(char*)"graphics/exitpost_appear.png");
    loadgfx(&exitpost_back,(char*)"graphics/exitpost_back.png");
    loadgfx(&exitpost_overlay,(char*)"graphics/exitpost_overlay.png");
    loadgfx(&exitpost_collision,(char*)"graphics/exitpost_collision.png");
    loadgfx(&exitpost_disappear,(char*)"graphics/exitpost_disappear.png");
    loadgfx(&tilesBitmap,(char*)"LEVELDATA.bmp");
    loadgfx(&backscreen0,(char*)"graphics/area01_bkg0-test.png");
    loadgfx(&backscreen1,(char*)"graphics/area01_bkg1-test.png");
    background_on=0;
    levelbank=(char*)malloc(150968);
    FILE * f;
    f=fopen("LEVELDATA.dat","rb");
    fread(levelbank,1,150968,f);
    fclose(f);
    NUMBER_LEVELS=PeekInt(levelbank,0);



    //**** Storage of mapdata will be using a Blitz Bank in due course ****
    LEVEL_SIZE=PeekInt(levelbank,4);

}

void Tlevel::Create(int l)
{
    int x,y,offset,i;
    id=l;								//Store level id
    num_enemy=0;
    door_appear=0;
    playersin=0;


    offset=8+(id * LEVEL_SIZE)+232;		//calculate offset for layer 0
    i=0;
    for (y=0;y<=29;y++)
			for (x=0;x<=39;x++)
			{
				map[0] [x] [y]=PeekByte(levelbank,offset+i);			// Read in map data
				i=i+1;
			}


    offset=8+(id * LEVEL_SIZE)+1432;		//calculate offset for layer 1
    i=0;
    for (y=0;y<=29;y++)
			for (x=0;x<=39;x++)
			{
				map[1] [x] [y]=PeekByte(levelbank,offset+i);			//Read in map data
				i=i+1;
			}



    offset=8+(id * LEVEL_SIZE)+2632;		//calculate offset for layer 2
    i=0;
	for (y=0;y<=29;y++)
	{
			for (x=0;x<=39;x++)
			{
				map[2] [x] [y]=PeekByte(levelbank,offset+i);			//Read in map data
				i=i+1;
			}
	}

        DestructTile.ClearList();
        ResetCollisions(0);
		offset=8+(id * LEVEL_SIZE)+3832;		//calculate offset for tile boundary
		i=0;
		for (y=0;y<=29;y++)
		{
			for (x=0;x<=39;x++)
			{
				map[3] [x] [y]=PeekByte(levelbank,offset+i);		//Read in bounds data
				if ((map[3] [x] [y]) & 192) DestructTile.spawn(x,y,map[1] [x] [y]);
                if (map[3] [x] [y]>0)
                {
                    if (map[3] [x] [y]==2) CollideImage (tilesBitmap,x*16,y*16,getTileX(tilesBitmap,map[1] [x] [y]),getTileY(tilesBitmap,map[1] [x] [y]),16,16,0,COL_LEFTH,255);
                    if (map[3] [x] [y]==4) CollideImage (tilesBitmap,x*16,y*16,getTileX(tilesBitmap,map[1] [x] [y]),getTileY(tilesBitmap,map[1] [x] [y]),16,16,0,COL_ROOF,255);
                    if (map[3] [x] [y]==8) CollideImage (tilesBitmap,x*16,y*16,getTileX(tilesBitmap,map[1] [x] [y]),getTileY(tilesBitmap,map[1] [x] [y]),16,16,0,COL_FOREG,255);
                    if (map[3] [x] [y]==16) CollideImage (tilesBitmap,x*16,y*16,getTileX(tilesBitmap,map[1] [x] [y]),getTileY(tilesBitmap,map[1] [x] [y]),16,16,0,COL_RIGHTH,255);
                    if ((map[3] [x] [y]) & 96) CollideImage (tilesBitmap,x*16,y*16,getTileX(tilesBitmap,map[1] [x] [y]),getTileY(tilesBitmap,map[1] [x] [y]),16,16,0,COL_ENEMY,255);
                }
				i=i+1;
			}
		}






    offset=8+(id * LEVEL_SIZE);
    int bimage=PeekInt(levelbank,offset+196);
		switch (bimage)
		{
		case 0:
			backdrop=backscreen0;				//Draw picture for backdro
			middrop=backscreen1;
			break;
		default:
			backdrop=backscreen0;				//Draw picture for backdro
			middrop=backscreen1;
			break;
		}



		p1x=PeekInt(levelbank,offset+0)-16;		//Load initial player start positions
		p1y=PeekInt(levelbank,offset+4)-16;
		p2x=PeekInt(levelbank,offset+8)-16;
		p2y=PeekInt(levelbank,offset+12)-16;
		door_x=PeekInt(levelbank,offset+180)-32;	//Load position for door to appear
		door_y=PeekInt(levelbank,offset+184)-32;

}

void Tlevel::ScrollToNext(void)
{
		int z,x,y,i;
		int row_count=0;
		int trans_row=0;
		int trans_scroll=0;
		char tempmap[3] [40] [30];
		for (z=0;z<=2;z++)
		{
			for (y=0;y<=29;y++)
			{
				for (x=0;x<=39;x++)
				{
					tempmap[z] [x] [y]=map[z] [x] [y];
				}
			}
		}
		id+=1;
		Create(id);
		while (row_count<30)
		{
			if (background_on==1)
			{
                drawSprite(backdrop,screen,0,0,0,mainscroll,backdrop->w,backdrop->h);
                drawSprite(backdrop,screen,0,0,0,mainscroll+480,backdrop->w,backdrop->h);

				drawSprite(middrop,screen,0,0,0,mainscroll2,middrop->w,middrop->h);
                drawSprite(middrop,screen,0,0,0,mainscroll2+480,middrop->w,middrop->h);
            }
            else
            {
                SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,96,96,160));
            }


			mainscroll=mainscroll+1;
			if (mainscroll>0) mainscroll=-479;
			mainscroll2=mainscroll2+2;
			if (mainscroll2>0) mainscroll2=-478;

			i=0;
			for (y=row_count;y<=29;y++)
			{
				for (x=0;x<=39;x++)
				{
					for (z=0;z<=2;z++)
					{
						if (tempmap[z] [x] [y]>0) drawTile(tilesBitmap,x*16,i*16-trans_scroll,tempmap[z] [x] [y]);
					}
				}
				i+=1;
			}
			for (y=0;y<=row_count;y++)
			{
				for (x=0;x<=39;x++)
				{
					for (z=0;z<=2;z++)
					{
						if (map[z] [x] [y]>0) drawTile(tilesBitmap,x*16,i*16-trans_scroll,map[z] [x] [y]);
					}
				}
				i+=1;
			}
			SDL_Flip(screen);
			trans_scroll+=4;
			if (trans_scroll>15)
			{
				row_count+=1;
				trans_scroll=0;
			}
		}

   /*     ResetCollisions(COL_DESTROY_TILE);
		for (y=0;y<=29;y++)
		{
			for (x=0;x<=39;x++)
			{
				if ((map[3] [x] [y]) & 192) DestructTile.spawn(x,y,map[1] [x] [y]);
			}
		}
*/
}


void Tlevel::AddEnemies()
{
		int offset=8+(id * LEVEL_SIZE)+16;
		num_enemy=PeekInt(levelbank,offset);			//Retrieve num of enemies
		offset=offset+4;			//set offset to 1st enemy data
		int z;
		for (z=0;z<=7;z++)
		{
			if (PeekInt(levelbank,offset+16))
			{
				switch (PeekInt(levelbank,offset+12))
				{
				case 0:
					wheelie.spawn(PeekInt(levelbank,offset)-16,PeekInt(levelbank,offset+4)-16,PeekInt(levelbank,offset+8));
					break;
				case 1:
					grog.spawn(PeekInt(levelbank,offset)-16,PeekInt(levelbank,offset+4)-16,PeekInt(levelbank,offset+8));
					break;
				case 2:
					slinky.spawn(PeekInt(levelbank,offset)-16,PeekInt(levelbank,offset+4)-16,PeekInt(levelbank,offset+8));
					break;
				default:
				break;
				}
			}
			offset=offset+20;
		}
}


void Tlevel::Fadein()
{
		int x,y,c,showenemy;
		c=0;
		showenemy=0;
		float scale=0;
		while (scale<1)
		{

        if (c<255)
        {
            SDL_SetAlpha(tilesBitmap,SDL_SRCALPHA,c);
            SDL_SetAlpha(backdrop,SDL_SRCALPHA,c);
            SDL_SetAlpha(middrop,SDL_SRCALPHA,c);
        }

		if (background_on==1)
		{
			drawSprite(backdrop,screen,0,0,0,mainscroll,backdrop->w,backdrop->h);
			drawSprite(backdrop,screen,0,0,0,mainscroll+480,backdrop->w,backdrop->h);


			drawSprite(middrop,screen,0,0,0,mainscroll2,middrop->w,middrop->h);
			drawSprite(middrop,screen,0,0,0,mainscroll2+480,middrop->w,middrop->h);

		}
		else
		{
			SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,96,96,160));
		}

		mainscroll=mainscroll+1;
		if (mainscroll>0) mainscroll=-479;
		mainscroll2=mainscroll2+2;
		if (mainscroll2>0) mainscroll2=-478;

			for (y=0;y<=29;y++)
				for (x=0;x<=39;x++)
				{
					if (map[0] [x] [y]>0) drawTile(tilesBitmap,x*16,y*16,map[0] [x] [y]);
					if (map[1] [x] [y]>0) drawTile(tilesBitmap,x*16,y*16,map[1] [x] [y]);
					if (map[2] [x] [y]>0) drawTile(tilesBitmap,x*16,y*16,map[2] [x] [y]);

				}


			if (showenemy)
			{

                wheelie.Appear(scale);
                grog.Appear(scale);
                slinky.Appear(scale);
				scale=scale+.1;
			}


			SDL_Flip(screen);
			c=c+8;
			if (c>=255)
			{
				c=255;
				showenemy=1;
				SDL_SetAlpha(tilesBitmap,0,c);
                SDL_SetAlpha(backdrop,0,c);
                SDL_SetAlpha(middrop,0,c);
			}
		}
		DestructTile.AddToCollisionLayer();
}

void Tlevel::Fadeout()					//Fade level out
{
		int x,y,c,v;
		c=255;						//Set initial colour intensity
		v=50;							//Set inital volume
//		if (soundpresent) Mix_FadeOutMusic(5000);
		while (c>0)					//Loop until colour faded to black
        {
			SDL_SetAlpha(tilesBitmap,SDL_SRCALPHA,c);				//set colour drawing intensity
			SDL_SetAlpha(backdrop,SDL_SRCALPHA,c);
            SDL_SetAlpha(middrop,SDL_SRCALPHA,c);

        if (background_on==1)
		{
			drawSprite(backdrop,screen,0,0,0,mainscroll,backdrop->w,backdrop->h);
			drawSprite(backdrop,screen,0,0,0,mainscroll+480,backdrop->w,backdrop->h);


			drawSprite(middrop,screen,0,0,0,mainscroll2,middrop->w,middrop->h);
			drawSprite(middrop,screen,0,0,0,mainscroll2+480,middrop->w,middrop->h);

		}
		else
		{
			SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,96,96,160));
		}

		mainscroll=mainscroll+1;
		if (mainscroll>0) mainscroll=-479;
		mainscroll2=mainscroll2+2;
		if (mainscroll2>0) mainscroll2=-478;

			for (y=0;y<=29;y++)
				for (x=0;x<=39;x++)
				{
					if (map[0] [x] [y]>0) drawTile(tilesBitmap,x*16,y*16,map[0] [x] [y]);
					if (map[1] [x] [y]>0) drawTile(tilesBitmap,x*16,y*16,map[1] [x] [y]);
					if (map[2] [x] [y]>0) drawTile(tilesBitmap,x*16,y*16,map[2] [x] [y]);

				}

			SDL_Flip(screen);
			c=c-8;						//Reduce colour intensity variable
			v=v-2;						//Reduce MOD volume variable
			if (v<0) v=0;
			//If soundpresent Then BASSMOD_SetVolume(v)			//Set MOD volume
			if (c<0) c=0;
        }
        SDL_SetAlpha(tilesBitmap,0,c);
        SDL_SetAlpha(backdrop,0,c);
        SDL_SetAlpha(middrop,0,c);
		//If soundpresent Then BASSMOD_MusicStop()			//Stop MOD playing
		//If soundpresent Then BASSMOD_MusicFree()
		//if (soundpresent) Mix_PauseMusic();
}

void Tlevel::Play()
{

		int x,y,l;
		long timing=SDL_GetTicks();

		//SetColor 255,255,255

		if (background_on==1)
		{
			drawSprite(backdrop,screen,0,0,0,mainscroll,backdrop->w,backdrop->h);
			drawSprite(backdrop,screen,0,0,0,mainscroll+480,backdrop->w,backdrop->h);


			drawSprite(middrop,screen,0,0,0,mainscroll2,middrop->w,middrop->h);
			drawSprite(middrop,screen,0,0,0,mainscroll2+480,middrop->w,middrop->h);

		}
		else
		{
			SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,96,96,160));
		}

		mainscroll=mainscroll+1;
		if (mainscroll>0) mainscroll=-479;
		mainscroll2=mainscroll2+2;
		if (mainscroll2>0) mainscroll2=-478;

		ResetCollisions(0x3FF8);  //need to limit to appropriate layers
		//ResetCollisions(COL_BOMB);

			for (y=0;y<=29;y++)
				for (x=0;x<=39;x++)
				{
				if (map[0] [x] [y]>0) drawTile(tilesBitmap,x*16,y*16,map[0] [x] [y]);
                if (map[1] [x] [y]>0) drawTile(tilesBitmap,x*16,y*16,map[1] [x] [y]);
                if ((map[3] [x] [y]) & 96) CollideImage (tilesBitmap,x*16,y*16,getTileX(tilesBitmap,map[1] [x] [y]),getTileY(tilesBitmap,map[1] [x] [y]),16,16,0,COL_ENEMY,255);
				}

        DestructTile.Update();


		/*SetColor 255,255,255
		SetBlend (MASKBLEND)
*/
		if (rand() % 1000==1)					//Randomly drop coins
		{
			coin.spawn((rand()%508)+50,-16,4,rand()%2,(rand()%90)+10);
        }

		if (rand() % 1000==1)					//Randomly drop gems
		{
			gem.spawn((rand()%508)+50,-16,4,rand()%6,(rand()%90)+10);
        }
		if (rand() % 1000==1)					//Randomly drop gems
		{
			pickup.spawn((rand()%508)+50,-16,4,rand()%2,(rand()%90)+10);
        }


		if (door_appear==2)				//Draw door frame under player
		{
			drawSprite(exitpost_back,screen,door_frame*64,0,door_x,door_y,64,64);
			CollideImage(exitpost_back,door_x,door_y,door_frame*64,0,64,64,0,COL_DOOR,255);
			CollideImage(exitpost_overlay,door_x,door_y,door_frame*64,0,64,64,0,COL_DOOR,255);
			CollideImage(exitpost_collision,door_x,door_y-18,0,0,64,28,0,COL_FOREG,255);
			door_timer=door_timer+1;
			if (door_timer>=2) door_timer=0;
			if (door_timer==0) door_frame=door_frame+1;
			if (door_frame==8) door_frame=0;
		}



        explosion.Update();
        explosion.Draw();

        bomb.Update();

        mine.Update();
        mine.Draw();

        wheelie.Update();
        wheelie.Draw();

        grog.Update();
        grog.Draw();

        slinky.Update();
        slinky.Draw();




        coin.Update();
        coin.Draw();

        gem.Update();
        gem.Draw();

        pickup.Update();
        pickup.Draw();

        player.Update();
        player.CheckExit();
        player.Draw();
        player.ShowStats();

        pickedup16.Update();
        pickedup16.Draw();

        pickedup32.Update();
        pickedup32.Draw();


		if (door_appear==1) 				//Draw door appearing animation
		{
			drawSprite(exitpost_appear,screen,door_frame*64,0,door_x,door_y,64,64);
			door_timer=door_timer+1;
			if (door_timer>=1) door_timer=0;
			if (door_timer==0) door_frame=door_frame+1;
			if (door_frame==40)
			{
				door_frame=0;
				door_appear=2;
			}
		}

		if (door_appear==3) 				//Draw door appearing animation
		{
			drawSprite (exitpost_disappear,screen,door_frame*64,0,door_x,door_y,64,64);
			door_timer=door_timer+1;
			if (door_timer>=4) door_timer=0;
			if (door_timer==0) door_frame=door_frame+1;
			if (door_frame==32)
			{
				door_frame=0;
				door_appear=4;
			}
		}



        dynamite.Update();
        dynamite.Draw();

        reward.Update();
        reward.Draw();

        bomb.Draw();
	//	SetBlend (MASKBLEND)


        smoke.Update();
        smoke.Draw();

		if (door_appear==2)
		{
			drawSprite (exitpost_overlay,screen,door_frame*64,0,door_x,door_y,64,64);	//Draw door overlay over player
		}

		if ((playersin>=player.CountList()) && (door_appear==2))
		{
			door_appear=3;
			door_frame=0;
			door_timer=0;
		}

		//SetBlend (MASKBLEND)
		for (y=0;y<=29;y++)
				for (x=0;x<=39;x++)
				{
				if (map[2] [x] [y]>0) drawTile(tilesBitmap,x*16,y*16,map[2] [x] [y]);

 /*               char buffer[4];
                sprintf(buffer,"%d\0",map[3] [x] [y]);
                SmallText(buffer,x*16,y*16);
*/

				}


        hiscore.Update();




		if ((wheelie.CountList()==0) && (grog.CountList()==0) && (slinky.CountList()==0) && (door_appear==0))
		{
			door_appear=1;
			door_frame=0;
			door_timer=0;
		}

		timing=calctime(SDL_GetTicks()-timing);
        char buffer[20];
        sprintf(buffer,"MSECS: %d\n",timing);
		Text(buffer,300,0);






}

int Tlevel::GetPx(int i)
{
    if (i==0) return p1x; else return p2x;
}

int Tlevel::GetDoorX()
{
    return door_x;
}

int Tlevel::GetDoorY()
{
    return door_y;
}

int Tlevel::GetPy(int i)
{
    if (i==0) return p1y; else return p2y;
}

void Tlevel::IncPlayersIn(void)
{
    playersin++;
}

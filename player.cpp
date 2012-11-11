#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_mixer.h>
#include "player.h"
#include "collision.h"
#include "bomb.h"
#include "mine.h"
#include "dynamite.h"
#include "explosion.h"
#include "level.h"
#include "gem.h"
#include "coin.h"
#include "pickup.h"
#include "reward.h"
#include "pickedup16.h"
#include "pickedup32.h"
#include "hiscore.h"

extern Tbomb bomb;
extern Tmine mine;
extern Tdynamite dynamite;
extern Tlevel level;
extern Texplosion explosion;
extern Tgem gem;
extern Tcoin coin;
extern Tpickup pickup;
extern Treward reward;
extern Tpickedup16 pickedup16;
extern Tpickedup32 pickedup32;
extern Thiscore hiscore;
SDL_Surface *mine_floor;
SDL_Surface *bomb_floor;
SDL_Surface *dynamite_floor;

extern Mix_Chunk *collect_snd;
extern Mix_Chunk *jump_snd;

extern SDL_Surface* screen;
extern SDL_Surface* spritemasks;

struct CONTROLS {
    int joy;
    int left,right,jump,cycle,fire;
    int leftdown,rightdown,jumpdown,cycledown,firedown,quitdown;

};

extern CONTROLS playercontrols[2];

extern void SmallText(char* strg,int x,int y);
extern void Text(char* strg,int x,int y);
extern void PutNumberLeadingZeros(char* b,int v,int n);
extern int get_key(int pi);
extern int CheckScore(int s);

extern char map[4] [40] [30];

static char fade_values[20]={0,10,20,30,40,50,60,70,90,110,130,150,180,190,200,210,220,230,240,250};

Tplayer::Tplayer()
{
    item_count=0;
}

void Tplayer::init()
{
    loadgfx(&snipe_stand_left,(char*)"graphics/snipe.stand_left.png");
    loadgfx(&snipe_stand_right,(char*)"graphics/snipe.stand_right.png");
    loadgfx(&snipe_run_left,(char*)"graphics/snipe.run_left.png");
    loadgfx(&snipe_run_right,(char*)"graphics/snipe.run_right.png");
    loadgfx(&snipe_skid_halt_left,(char*)"graphics/snipe.skid_halt_left.png");
    loadgfx(&snipe_skid_halt_right,(char*)"graphics/snipe.skid_halt_right.png");
    loadgfx(&snipe_turn_left_to_right,(char*)"graphics/snipe.turn_left_to_right.png");
    loadgfx(&snipe_turn_right_to_left,(char*)"graphics/snipe.turn_right_to_left.png");
    loadgfx(&snipe_blink_facing_left,(char*)"graphics/snipe.blink_facing_left.png");
    loadgfx(&snipe_blink_facing_right,(char*)"graphics/snipe.blink_facing_right.png");
    loadgfx(&snipe_facing_left_drop_bomb,(char*)"graphics/snipe.facing_left_drop_bomb.png");
    loadgfx(&snipe_facing_right_drop_bomb,(char*)"graphics/snipe.facing_right_drop_bomb.png");
    loadgfx(&snipe_facing_left_drop_mine,(char*)"graphics/snipe.facing_left_drop_mine.png");
    loadgfx(&snipe_facing_right_drop_mine,(char*)"graphics/snipe.facing_right_drop_mine.png");
    loadgfx(&snipe_facing_left_drop_dynamite,(char*)"graphics/snipe.facing_left_drop_dynamite.png");
    loadgfx(&snipe_facing_right_drop_dynamite,(char*)"graphics/snipe.facing_right_drop_dynamite.png");
    loadgfx(&snipe_trigger_dynamite_left,(char*)"graphics/snipe.trigger_dynamite_left.png");
    loadgfx(&snipe_trigger_dynamite_right,(char*)"graphics/snipe.trigger_dynamite_right.png");
    loadgfx(&snipe_jump_left,(char*)"graphics/snipe.jump_left.png");
    loadgfx(&snipe_jump_right,(char*)"graphics/snipe.jump_right.png");
    loadgfx(&snipe_die,(char*)"graphics/snipe.die.png");


    loadgfx(&gripe_stand_left,(char*)"graphics/gripe.stand_left.png");
    loadgfx(&gripe_stand_right,(char*)"graphics/gripe.stand_right.png");
    loadgfx(&gripe_run_left,(char*)"graphics/gripe.run_left.png");
    loadgfx(&gripe_run_right,(char*)"graphics/gripe.run_right.png");
    loadgfx(&gripe_skid_halt_left,(char*)"graphics/gripe.skid_halt_left.png");
    loadgfx(&gripe_skid_halt_right,(char*)"graphics/gripe.skid_halt_right.png");
    loadgfx(&gripe_turn_left_to_right,(char*)"graphics/gripe.turn_left_to_right.png");
    loadgfx(&gripe_turn_right_to_left,(char*)"graphics/gripe.turn_right_to_left.png");
    loadgfx(&gripe_blink_facing_left,(char*)"graphics/gripe.blink_facing_left.png");
    loadgfx(&gripe_blink_facing_right,(char*)"graphics/gripe.blink_facing_right.png");
    loadgfx(&gripe_facing_left_drop_bomb,(char*)"graphics/gripe.facing_left_drop_bomb.png");
    loadgfx(&gripe_facing_right_drop_bomb,(char*)"graphics/gripe.facing_right_drop_bomb.png");
    loadgfx(&gripe_facing_left_drop_mine,(char*)"graphics/gripe.facing_left_drop_mine.png");
    loadgfx(&gripe_facing_right_drop_mine,(char*)"graphics/gripe.facing_right_drop_mine.png");
    loadgfx(&gripe_facing_left_drop_dynamite,(char*)"graphics/gripe.facing_left_drop_dynamite.png");
    loadgfx(&gripe_facing_right_drop_dynamite,(char*)"graphics/gripe.facing_right_drop_dynamite.png");
    loadgfx(&gripe_trigger_dynamite_left,(char*)"graphics/gripe.trigger_dynamite_left.png");
    loadgfx(&gripe_trigger_dynamite_right,(char*)"graphics/gripe.trigger_dynamite_right.png");
    loadgfx(&gripe_jump_left,(char*)"graphics/gripe.jump_left.png");
    loadgfx(&gripe_jump_right,(char*)"graphics/gripe.jump_right.png");
    loadgfx(&gripe_die,(char*)"graphics/gripe.die.png");

    loadgfx(&mine_floor,(char*)"graphics/mine_floor.png");
    loadgfx(&dynamite_floor,(char*)"graphics/dynamite_floor.png");
    loadgfx(&bomb_floor,(char*)"graphics/bomb_floor.png");

    loadgfx(&icons,(char*)"graphics/icons.png");


}

int Tplayer::loadgfx(SDL_Surface** s, char* n)
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

void Tplayer::drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id)
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

    if ((item_store[id].zx==1) && (item_store[id].zy==1))
    {
        SDL_BlitSurface(imageSurface, &srcRect, screenSurface, &dstRect);
    }
    else
    {
        SDL_Rect tempRect;
        tempRect.x = 0;
        tempRect.y = 0;
        tempRect.w = width;
        tempRect.h = height;
        SDL_Surface* tempSurface=SDL_CreateRGBSurface(SDL_SRCCOLORKEY | SDL_SWSURFACE,width,height,16,0,0,0,0);
        SDL_FillRect(tempSurface,NULL,SDL_MapRGB(tempSurface->format, 255, 0, 255));
        SDL_BlitSurface(imageSurface, &srcRect, tempSurface, &tempRect);
        SDL_Surface *zoomimage=zoomSurface(tempSurface,(double)item_store[id].zx,(double)item_store[id].zy,0);
        //SDL_SetColorKey( zoomimage, SDL_SRCCOLORKEY, SDL_MapRGB(zoomimage->format, 0, 0, 0) );
        SDL_SetAlpha(zoomimage,SDL_SRCCOLORKEY,255);
        SDL_SetColorKey( zoomimage, SDL_SRCCOLORKEY, SDL_MapRGB(zoomimage->format, 255, 0, 255) );
        tempRect.w = zoomimage->w;
        tempRect.h = zoomimage->h;
        SDL_BlitSurface(zoomimage, &tempRect, screenSurface, &dstRect);
    }
}

void Tplayer::SetScale(float xscale,float yscale,int id)
{
    item_store[id].zx=xscale;
    item_store[id].zy=yscale;
}



int Tplayer::create(int i)
{
    int slot=-1;
    //First Check if any free slots
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            if (item_store[z].id==-1) //free slot
            {
                slot=z;
            }
        }
    }
    if (slot<0)
    {
        item_count++;
        if (item_count==1)
        {
            item_store=(class_struct*)malloc(sizeof(class_struct));
        }
        else
        {
            item_store=(class_struct*)realloc(item_store,sizeof(class_struct)*item_count);
        }
        slot=item_count-1;
    }
    item_store[slot].id=i;
    return slot;
}

int Tplayer::remove(int id)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            if (item_store[z].id==id) //match
            {
                item_store[z].id=-1; //deacivate
                return z;
            }
        }
    }
    return -1;
}

int Tplayer::spawn(int i,int px,int py,int s,int j,int b,int l,int fd)
{
    int p;
    p=create(i);
    if (p>=0)  //valid slot
    {
    item_store[p].x=px;
    item_store[p].y=py;
    item_store[p].topspeed=s;
    item_store[p].speed=0;
    item_store[p].jump=j;
    item_store[p].canplacebomb=b;
    item_store[p].lives=l;
    item_store[p].coins=0;
    item_store[p].gems=0;
    item_store[p].facingdir=fd;
    item_store[p].isdead=0;
    item_store[p].indoor=0;
    item_store[p].grip=.2;
    item_store[p].skid=.08;
    item_store[p].weapon=1;
    item_store[p].weaponfade=0;
    item_store[p].num_mine=1;
    item_store[p].num_tnt=1;
    item_store[p].tnt_laid=0;
    item_store[p].canland=1;
    item_store[p].haslanded=0;
    item_store[p].currframe=0;
    item_store[p].invincible=0;
    item_store[p].invflipflop=0;
    item_store[p].zx=1;
    item_store[p].zy=1;
    item_store[p].score=0;
    if (i==0)
    {
        if (fd==0) item_store[p].image=snipe_stand_left; else snipe_stand_right;
    }
    else
    {
        if (fd==0) item_store[p].image=gripe_stand_left; else gripe_stand_right;
    }
    return i;
    }
    else return -1;
}

void Tplayer::Appear(float s)
{
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
        }
    }
}

void Tplayer::Update()
{
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
            Update2(&item_store[z]);
        }
    }
}

void Tplayer::Update2(class_struct *is)
{
        int p;
		int joymoved=0,jumppressed=0,kickpressed=0,dropbomb=0,cyclepressed=0;
		int keydata;


		is->walkleft=0;
		is->walkright=0;
		keydata=get_key(is->id);
		//if JoyDown(1,id)=0 And KeyDown(pfire[id])=0 Then firepressed=0
		//If JoyDown(2,id)=0 And KeyDown(pcycle[id])=0 Then cycledown=0
		if ((keydata & MY_BUTT_B)==0) is->firepressed=0;
		if ((keydata & MY_BUTT_Y)==0) is->cycledown=0;

		if ((is->isdead==0) && (is->indoor==0))
		{
			if (is->facingdir==0)
			{
				if (is->id==1) is->image=snipe_stand_left; else is->image=gripe_stand_left;
			}
			else
			{
				if (is->id==1) is->image=snipe_stand_right; else is->image=gripe_stand_right;
			}

			if (is->jump==0)
			{
				int z=0;
				int oldfall=is->fall;
				for (z=0;z<=5;z++)	//How many lines to fall each frame
				{
					is->fall=0;
					if (CollideImage((SDL_Surface*)spritemasks,(int)is->x,(int)is->y,0,0,32,32,COL_FOREG)==0)
					{
						is->y+=1;
						is->canland=1;
						is->onground=0;
						is->fall=1;
					}
					else
					{
						if (is->canland==0)
						{
							is->fall=1;
							is->y+=1;
							is->onground=0;
						}
						else
						{
							if (CollideImage(spritemasks,(int)is->x,(int)is->y-5,4*32,0,32,32,COL_FOREG)==0)
							{
								is->onground=1;
							}
							else
							{
								is->fall=1;
								is->y+=1;
								is->onground=0;
							}
						}
					}
				}

/*'				For z=0 To 3
'					If Not CollideImage(spritemasks,x,y-1,0,1,0) Or CollideImage(spritemasks,x,y-3,0,1,0) Or y<-24 Then
'						y=y+1
'						fall=1
'					End If
'				Next*/

				if (is->y>=480) is->y=-32;
				if ((oldfall==1) && (is->fall==0))   //Set vars to show player has landed a after jumping/falling
				{
					is->haslanded=1;
					is->animcount=-2;
				}
/*				'DrawImage spritemasks,x,y-1,0
				'DrawImage spritemasks,x,y-3,0*/
			}


			jumppressed=0;
			kickpressed=0;
			dropbomb=0;
			joymoved=0;
			//If joysticks>id Then
				if (is->fall==0)
				{
					if (playercontrols[is->id-1].leftdown)
					{
						joymoved=1;
						is->walkleft=1;
					}

					if (playercontrols[is->id-1].rightdown)
					{
						joymoved=1;
						is->walkright=1;
					}

				}


				if (playercontrols[is->id-1].jumpdown) jumppressed=1;

				if ((playercontrols[is->id-1].firedown) && (is->firepressed==0))
				{
        			kickpressed=1;
					is->firepressed=1;
				}

				if ((playercontrols[is->id-1].cycledown) && (is->cycledown==0)) cyclepressed=1;


                if (keydata & MY_BUTT_A) ShowCollisionLayer(13);
			//EndIf
/*
			if (joymoved==0)
			{
				if (is->fall==0)
				{

				}
					If KeyDown(pleft[id]) Then
						walkleft=1
					Else If KeyDown(pright[id]) Then
						walkright=1
					End If
				End If

				If KeyDown(pjump[id]) Then jumppressed=1

				If KeyDown(pfire[id]) And firepressed=0 Then
					kickpressed=1
					firepressed=1
				End If

				If KeyDown(pcycle[id]) And cycledown=0 Then cyclepressed=1
			End If
*/
			if (is->placingbomb!=0 )
			{
				is->walkleft=0;
				is->walkright=0;
			}

			if ((is->walkleft==0) && (is->walkright==0))
			{
				if (is->speed>0)
				{
					is->speed=is->speed-is->skid;
					if (is->facingdir==0)
					{
						if (is->id==1) is->image=snipe_skid_halt_left; else is->image=gripe_skid_halt_left;
					}
					else
					{
						if (is->id==1) is->image=snipe_skid_halt_right; else is->image=gripe_skid_halt_right;
					}
					if (is->animcount==0) is->currframe=is->currframe+1;
					if (is->currframe>3) is->currframe=3;
				}
				if (is->speed<=.5) is->speed=0;

			}

			if (is->walkleft==1)
			{
				is->blink=0;
				if (is->facingdir==0)
				{
					is->speed=is->speed+is->grip;
					if (is->speed>is->topspeed) is->speed=is->topspeed;
					if (is->animcount==0) is->currframe=is->currframe+1;
					if (is->currframe>=8) is->currframe=0;
					if (is->id==1) is->image=snipe_run_left; else is->image=gripe_run_left;
				}
				else
				{
					if (is->speed>=.5)
					{
						if (is->animcount==0) is->currframe=is->currframe+1;
						is->speed=is->speed-is->grip;
						if (is->speed<.5) is->currframe=0;
						if (is->currframe>3) is->currframe=3;
						if (is->id==1) is->image=snipe_skid_halt_right; else is->image=gripe_skid_halt_right;
					}
					else
					{
						is->currframe=is->currframe+1;
						if (is->id==1) is->image=snipe_turn_right_to_left; else is->image=gripe_turn_right_to_left;
						if (is->currframe>3)
						{
							is->facingdir=0;
							if (is->id==1) is->image=snipe_stand_left; else is->image=gripe_stand_left;
							is->currframe=0;
						}
					}
				}
			}

			if (is->walkright==1)
			{
				is->blink=0;
				if (is->facingdir==1)
				{
					is->speed=is->speed+is->grip;
					if (is->speed>is->topspeed) is->speed=is->topspeed;
					if (is->animcount==0) is->currframe=is->currframe+1;
					if (is->currframe>=8) is->currframe=0;
					if (is->id==1) is->image=snipe_run_right; else is->image=gripe_run_right;
				}
				else
				{
					if (is->speed>=.5)
					{
						if (is->animcount==0) is->currframe=is->currframe+1;
						is->speed=is->speed-is->grip;
						if (is->speed<.5) is->currframe=0;
						if (is->currframe>3) is->currframe=3;
						if (is->id==1) is->image=snipe_skid_halt_left; else is->image=gripe_skid_halt_left;
					}
					else
					{
						is->currframe=is->currframe+1;
						if (is->id==1) is->image=snipe_turn_left_to_right; else is->image=gripe_turn_left_to_right;
						if (is->currframe>3)
						{
							is->facingdir=1;
							if (is->id==1) is->image=snipe_stand_right; else is->image=gripe_stand_right;
							is->currframe=0;
						}
					}
				}
			}

			if (is->facingdir==0) is->x=is->x-is->speed; else is->x=is->x+is->speed;

			if (is->x>=626) is->x=-14;
			if (is->x<=-16) is->x=624;

			if ((jumppressed) && (is->jump==0) && (is->fall==0) && (is->placingbomb==0) && (CollideImage(is->image,(int)is->x,(int)is->y,0,0,32,32,COL_DOOR)==0) && (is->onground))
			{
			/*	'Local jump_channel=CueSound(jump_snd)
				'SetChannelVolume (jump_channel,0.3)
				'ResumeChannel(jump_channel)*/
				if (CollideImage(gripe_jump_left,(int)is->x,(int)is->y-8,0,0,32,32,COL_ROOF)==0)
				{
					channel=Mix_PlayChannel(-1,jump_snd,0);
					is->jump=1;
					is->blink=0;
					is->jumpspeed=-8;
					is->canland=0;
					is->onground=0;
					if (is->walkleft)
					{
						is->speed=is->topspeed;
						is->facingdir=0;
					}
					if (is->walkright)
					{
						is->speed=is->topspeed;
						is->facingdir=1;
					}
				}
				else
				{
					jumppressed=0;
				}
			}


			if (is->jump)
			{
				is->fall=0;
				is->blink=0;
				is->y=is->y+is->jumpspeed;
				if (CollideImage(gripe_jump_left,(int)is->x,(int)is->y,0,0,32,32,COL_ROOF))
				{
					is->jump=0;
					is->y-=is->jumpspeed;
				}
				else
				{
					if (is->id==1)
					{
						if (is->facingdir==0) is->image=snipe_jump_left; else is->image=snipe_jump_right;
					}
					else
					{
						if (is->facingdir==0) is->image=gripe_jump_left; else is->image=gripe_jump_right;
					}
					is->currframe=0;
				}
				//'If jumppressed Then jumpspeed#=jumpspeed#+.32 Else jumpspeed#=jumpspeed#+.64
				is->jumpspeed=is->jumpspeed+.40;

				if ((is->jump==1) && (is->jumpspeed>0)) is->jump=2;

				if ((is->jump==2) && (CollideImage(spritemasks,(int)is->x,(int)is->y,0,0,32,32,COL_FOREG)==0)) is->canland=1;

				//'If jumpspeed#>=4 Then jumpspeed#=4
				if (is->jumpspeed>=4) is->jump=0;
				if ((CollideImage(spritemasks,(int)is->x,(int)is->y,0,0,32,32,COL_FOREG)) && (is->canland==1))
				{
					is->jump=0;
				}




			}


			if ((is->walkleft==0) && (is->walkright==0) && (is->jump==0) && (is->blink==0) && (is->speed==0))
			{
				if (rand() % 500==1)
				{
					is->blink=1;
					is->currframe=0;
				}
			}

			if (is->blink)
			{
				if (is->facingdir==0)
				{
					if (is->id==1) is->image=snipe_blink_facing_left; else is->image=gripe_blink_facing_left;
				}
				else
				{
					if (is->id==1) is->image=snipe_blink_facing_right; else is->image=gripe_blink_facing_right;
				}
				if (is->animcount==0) is->currframe=is->currframe+1;
				if (is->currframe>7)
				{
					is->blink=0;
					is->currframe=0;
				}
			}

			if (is->triggermine)
			{
				if (is->facingdir==0)
				{
					if (is->id==1) is->image=snipe_trigger_dynamite_left; else is->image=gripe_trigger_dynamite_left;
				}
				else
				{
					if (is->id==1) is->image=snipe_trigger_dynamite_right; else is->image=gripe_trigger_dynamite_right;
				}

				if (is->animcount==0) is->currframe=is->currframe+1;
				if (is->currframe==2)
				{
					int temptnt=dynamite.Find(is->id); //Used to hold the id of which player placed mine
					if (temptnt)
					{
                        explosion.spawn(dynamite.GetX(temptnt)-24,dynamite.GetY(temptnt)-48,temptnt);
                        dynamite.Destroy(temptnt);
					}
				}
				if (is->currframe>7)
				{
					is->currframe=0;
					is->triggermine=0;
					is->tnt_laid=0;
				}
			}

			if (is->placingbomb)
			{
				switch (is->weapon)
				{
					case 2:
						if (is->facingdir==0)
						{
							if (is->id==1) is->image=snipe_facing_left_drop_mine; else is->image=gripe_facing_left_drop_mine;
						}
						else
						{
							if (is->id==1) is->image=snipe_facing_right_drop_mine; else is->image=gripe_facing_right_drop_mine;
						}
						break;
					case 3:
						if (is->facingdir==0)
						{
							if (is->id==1) is->image=snipe_facing_left_drop_dynamite; else is->image=gripe_facing_left_drop_dynamite;
						}
						else
						{
							if (is->id==1) is->image=snipe_facing_right_drop_dynamite; else is->image=gripe_facing_right_drop_dynamite;
						}
						break;
					default:
						if (is->facingdir==0)
						{
							if (is->id==1) is->image=snipe_facing_left_drop_bomb; else is->image=gripe_facing_left_drop_bomb;
						}
						else
						{
							if (is->id==1) is->image=snipe_facing_right_drop_bomb; else is->image=gripe_facing_right_drop_bomb;
						}
						break;
				}

				if (is->animcount==0) is->currframe=is->currframe+1;
				if (is->currframe>3)
				{
					is->placingbomb=0;
					switch (is->weapon)
					{
						case 2:
							if (is->facingdir==0) mine.spawn((int)is->x-14,(int)is->y+16,0,3,4,is->id); else mine.spawn((int)is->x+30,(int)is->y+16,0,3,4,is->id);
							is->num_mine-=1;
							break;
						case 3:
							if (is->tnt_laid==0)
							{
								if (is->facingdir==0) dynamite.spawn((int)is->x-14,(int)is->y+16,0,3,4,is->id); else dynamite.spawn((int)is->x+30,(int)is->y+16,0,3,4,is->id);
								is->tnt_laid=1;
								is->num_tnt-=1;
							}
							break;
						default:
							if (is->facingdir==0)
							{
							bomb.spawn((int)is->x-4,(int)is->y+16,-is->speed,is->grip,200,0,3,4,is->id);
							}
							else
							{
							bomb.spawn((int)is->x+16,(int)is->y+16,is->speed,is->grip,200,0,3,4,is->id);
							}
						break;
					}
					is->currframe=0;
				}
			}

			if ((cyclepressed) && (is->placingbomb==0))
			{
				is->weapon=is->weapon+1;
				if (is->weapon>3) is->weapon=1;
				is->cycledown=1;
				is->weaponfade=255;
			}

			if (is->haslanded==1)
			{
				if (is->id==1)
				{
					if (is->facingdir==0) is->image=snipe_skid_halt_left; else is->image=snipe_skid_halt_right;
				}
				else
				{
					if (is->facingdir==0) is->image=gripe_skid_halt_left; else is->image=gripe_skid_halt_right;
				}
				is->currframe=2;
			}

			if (is->fall==1)
			{
				if (is->id==1)
				{
					if (is->facingdir==0) is->image=snipe_jump_left; else is->image=snipe_jump_right;
				}
				else
				{
					if (is->facingdir==0) is->image=gripe_jump_left; else is->image=gripe_jump_right;
				}
				is->currframe=0;
			}

			if ((is->image==snipe_stand_left) || (is->image==gripe_stand_left) || (is->image==snipe_stand_right) || (is->image==gripe_stand_right)) is->currframe=0;

			//Test collision with lefthand boundary
			int checkpos=is->y+8;
			if (checkpos<1) checkpos=1;
			if (CollideImage(spritemasks,(int)is->x,checkpos,0,0,32,32,COL_LEFTH))
			{
				//'x=x+topspeed#
				is->x+=is->speed;
				is->speed=0;
			}
			//Test collision with righthand boundary
			if (CollideImage(spritemasks,(int)is->x,checkpos,0,0,32,32,COL_RIGHTH))
			{
				//'x=x-topspeed#
				is->x-=is->speed;
				is->speed=0;
			}

			//'DrawImage spritemasks,x,checkpos,0

			//'Collision detection for enemies
			if ((CollideImage(is->image,(int)is->x,(int)is->y,is->currframe*32,0,32,32,COL_ENEMY)) && (is->invincible==0)) is->isdead=1;


			int cc;
			//Collision detection for Gems
			cc=CollideImage(is->image,(int)is->x,(int)is->y,is->currframe*32,0,32,32,COL_GEM);
			if (cc)
			{
                is->score=is->score+gem.GetValue(cc);
                reward.spawn(gem.GetX(cc),gem.GetY(cc),gem.GetValue(cc));
                pickedup16.spawn(gem.GetX(cc),gem.GetY(cc));
                is->gems=is->gems+1;
                if (is->gems>99) is->gems=99;
                gem.Destroy(cc);
                channel=Mix_PlayChannel(-1,collect_snd,0);
			}



			//Collision detection for Coins
			cc=CollideImage(is->image,(int)is->x,(int)is->y,is->currframe*32,0,32,32,COL_COIN);
			if (cc)
			{
                is->score=is->score+coin.GetValue(cc);
                reward.spawn(coin.GetX(cc),coin.GetY(cc),coin.GetValue(cc));
                pickedup16.spawn(coin.GetX(cc),coin.GetY(cc));
                is->coins=is->coins+1;
                if (is->coins>99) is->coins=99;
                coin.Destroy(cc);
                channel=Mix_PlayChannel(-1,collect_snd,0);
			}

			//Collision detection for pick-ups
			cc=CollideImage(is->image,(int)is->x,(int)is->y,is->currframe*32,0,32,32,COL_PICKUP);
			if (cc)
			{
                pickedup32.spawn(pickup.GetX(cc),pickup.GetY(cc));
                pickup.Destroy(cc);
                channel=Mix_PlayChannel(-1,collect_snd,0);
			}


			//Collision detection for mines
			cc=CollideImage(is->image,(int)is->x,(int)is->y,is->currframe*32,0,32,32,COL_MINE);
			if (cc)
			{
			    if (mine.GetTime(cc)==0)
			    {
                    explosion.spawn(mine.GetX(cc),mine.GetY(cc),mine.GetPlayerID(cc));
                    mine.Destroy(cc);
			    }
			}

			//Collision detection for Exploding bombs
			cc=CollideImage(is->image,(int)is->x,(int)is->y,is->currframe*32,0,32,32,COL_EXPLOSION);
			if ((cc) && (is->invincible==0))
			{
			/*	'For Local tempbomb:Tbomb = EachIn bomb_list
				'	If tempbomb=cc[0] Then
				'		whosbomb?=tempbomb=id
				'	End If
				'Next*/
				is->isdead=1;
			}

			//Collision detection for unexploded Bombs/for bomb kick routine
			cc=CollideImage(is->image,(int)is->x,(int)is->y,is->currframe*32,0,32,32,COL_BOMB);
			if (cc)
			{
				if ((kickpressed) && (is->fall==0) && (is->jump==0))
				{
                    if (bomb.GetKicked(cc)==0)
                    {
                        bomb.SetDY(cc,-2);
                        if (is->facingdir==0) bomb.SetDX(cc,-2); else bomb.SetDX(cc,2);
                        bomb.SetKicked(cc);
                    }
				}
			}
			else
			{
				if (kickpressed)
				{
					if ((is->weapon==3) && (is->tnt_laid==1))
					{
						if ((is->jump==0) && (is->triggermine==0) && (is->fall==0))
						{
							is->triggermine=1;
							is->currframe=0;
						}
					}
					else if ((is->fall==0) && (is->jump==0) && (is->canplacebomb==0))
                    {
                        if ((is->weapon==1) || (is->speed==0))
                        {
							if (((is->weapon==2) && (is->num_mine>0)) || ((is->weapon==3) && (is->num_tnt>0)) || (is->weapon==1))
							{
								is->placingbomb=1;
								is->canplacebomb=40;
								is->currframe=0;
							}
                        }
                    }
					is->firepressed=1;
				}
			}



			if (is->invincible>0)
			{
				is->invincible=is->invincible-1;
				is->invflipflop=1-is->invflipflop;
			}
			else
			{
				is->invincible=0;
				is->invflipflop=0;
			}
			if (is->canplacebomb>0) is->canplacebomb=is->canplacebomb-1;

			is->animcount=is->animcount+1;
			if (is->animcount>2)
			{
				is->animcount=0;
				is->haslanded=0;
			}

			}

		if (is->isdead==1)	//Set up start of death sequence
		{
			is->isdead=2;
			is->currframe=0;
			if (is->id==1) is->image=snipe_die; else is->image=gripe_die;
			is->animcount=0;
			is->zoom=1;
		}

		if (is->isdead==2) 	//process player death sequence
		{
			is->animcount+=1;
			if (is->animcount>2) is->animcount=0;
			if (is->animcount==0) is->currframe+=1;
			if (is->currframe>3)
			{
				is->currframe=3;
				is->isdead=3;
			}
			is->y-=2;
			is->zoom+=.01;
		}

		if (is->isdead==3)
		{
			is->animcount+=1;
			if (is->animcount>2) is->animcount=0;
			if (is->animcount==0) is->currframe+=1;
			if (is->currframe>3) is->currframe=0;
			is->y+=8;
			if (is->y>480) is->isdead=4;
		}

		if (is->isdead==4)		// **** Reset player after death!
		{
			is->x=level.GetPx(is->id-1);
			is->y=level.GetPy(is->id-1);
			is->jump=0;
			is->fall=0;
			is->speed=0;
			is->canplacebomb=0;
			is->isdead=0;
			is->lives=is->lives-1;
			is->invincible=150;
			is->currframe=0;
			if (is->lives==0)
			{
				int hpos=CheckScore(is->score);
				if (hpos<10) hiscore.Spawn(is->id,is->score,hpos);
				remove(is->id);
			}

		}

}

void Tplayer::CheckExit()
{
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
        if ((CollideImage(item_store[z].image,(int)item_store[z].x,(int)item_store[z].y,item_store[z].currframe*32,0,32,32,COL_DOOR)) && (item_store[z].indoor==0))
        {
			if (((int)item_store[z].x>=level.GetDoorX()+8) && ((int)item_store[z].x<=level.GetDoorX()+24) && ((int)item_store[z].y>=level.GetDoorY()+12) && ((int)item_store[z].y<=level.GetDoorY()+32))
			{
				item_store[z].indoor=1;
				level.IncPlayersIn();
			}
        }
        }
    }
}

void Tplayer::Draw()
{
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
		//SDL_SetAlpha(item_store[z].image,0,255);
		//'SetScale (-1,1,z)

		CollideImage (item_store[z].image,(int)item_store[z].x,(int)item_store[z].y,item_store[z].currframe*32,0,32,32,0,COL_PLAYER,item_store[z].id);
		//drawSprite(item_store[z].image,screen,item_store[z].currframe*32,0,(int)item_store[z].x,(int)item_store[z].y,32,32,item_store[z].id);
		if (item_store[z].invflipflop==0)
		{
			if (item_store[z].isdead>0) SetScale(item_store[z].zoom,item_store[z].zoom,item_store[z].id);
			if (!item_store[z].indoor)  drawSprite(item_store[z].image,screen,item_store[z].currframe*32,0,(int)item_store[z].x,(int)item_store[z].y,32,32,z);
			if (item_store[z].weaponfade>0)
			{
			    SDL_Surface* tempw;

				switch(item_store[z].weapon)
				{
					case 2:
						tempw=mine_floor;
						break;
					case 3:
						tempw=dynamite_floor;
						break;
					default:
						tempw=bomb_floor;
						break;
				}
				SDL_SetAlpha(tempw,SDL_SRCALPHA,item_store[z].weaponfade);
				drawSprite(tempw,screen,0,0,item_store[z].x+10,item_store[z].y-14,16,16,z);
				SDL_SetAlpha(tempw,0,255);

				item_store[z].weaponfade=item_store[z].weaponfade-8;
			}
			SetScale(1,1,item_store[z].id);
		}


		//'SetScale (1,1,z);

        }
    }
}


void Tplayer::ShowStats()
{
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
        //If weapon=2 And num_mine=0 Then weapon=3
		//If weapon=3 And num_tnt=0 Then weapon=1
		int sx;
		if (item_store[z].id==1) sx=8; else sx=536;
		char buffer[10];
		PutNumberLeadingZeros(&buffer[0],item_store[z].score,6);
		Text(buffer,sx,8);
		SetScale(1,1,z);
		if (item_store[z].id==1)
		{
			sx=sx+16;
			drawSprite(icons,screen,0,0,sx,32,16,16,z);
		}
		else
		{
			sx=sx+16;
			drawSprite(icons,screen,16,0,sx,32,16,16,z);
		}
		PutNumberLeadingZeros(&buffer[0],item_store[z].lives,2);
		SmallText(buffer,sx,48);
		drawSprite(icons,screen,32,0,sx+24,32,16,16,z);
		PutNumberLeadingZeros(&buffer[0],item_store[z].gems,2);
		SmallText(buffer,sx+24,48);
		drawSprite(icons,screen,48,0,sx+48,32,16,16,z);
		PutNumberLeadingZeros(&buffer[0],item_store[z].coins,2);
		SmallText(buffer,sx+48,48);

		if (item_store[z].weapon!=1)
		{
			SDL_SetAlpha(bomb_floor,SDL_SRCALPHA,96);
		}
		else
		{
			SDL_SetAlpha(bomb_floor,0,255);
		}
		drawSprite(bomb_floor,screen,0,0,sx,58,16,16,z);
		if (item_store[z].weapon!=2)
		{
			SDL_SetAlpha(mine_floor,SDL_SRCALPHA,96);
		}
		else
		{
			SDL_SetAlpha(mine_floor,0,255);
		}
		drawSprite(mine_floor,screen,0,0,sx+24,58,16,16,z);
		PutNumberLeadingZeros(&buffer[0],item_store[z].num_mine,1);
		SmallText(buffer,sx+28,63);
		if (item_store[z].weapon!=3)
		{
			SDL_SetAlpha(dynamite_floor,SDL_SRCALPHA,96);
		}
		else
		{
			SDL_SetAlpha(dynamite_floor,0,255);
		}
		drawSprite(dynamite_floor,screen,0,0,sx+48,58,16,16,z);
		PutNumberLeadingZeros(&buffer[0],item_store[z].num_tnt,1);
		SmallText(buffer,sx+52,63);
		SDL_SetAlpha(bomb_floor,0,255);
		SDL_SetAlpha(mine_floor,0,255);
		SDL_SetAlpha(dynamite_floor,0,255);
        }
    }
}

void Tplayer::InitialPosition()
{
    int z;
    for (z=0;z<item_count;z++)
    {
    if (item_store[z].id>=0)
    {
		item_store[z].x=level.GetPx(item_store[z].id-1);
		item_store[z].y=level.GetPy(item_store[z].id-1);
		//item_store[z].x=level.GetPx(z);
        //item_store[z].y=level.GetPy(z);
		item_store[z].indoor=0;
		item_store[z].jump=0;
		item_store[z].speed=0;
		item_store[z].tnt_laid=0;
    }
    }
}

void Tplayer::ClearList(void)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            remove(item_store[z].id);
        }
    }
}

int Tplayer::CountList(void)
{
    int c=0;
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            if (item_store[z].id>=0) c++;
        }
    }
    return c;
}


void Tplayer::AddScore(int v,int id)
{
    if (item_count<1) return;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==id)
        {
            item_store[z].score+=v;
        }
    }
}

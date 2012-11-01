#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_mixer.h>
#include "bomb.h"
#include "collision.h"
#include "explosion.h"

extern SDL_Surface* screen;
extern SDL_Surface* coin_collide;

extern Texplosion explosion;

extern Mix_Chunk * bomb_land_snd;

Tbomb::Tbomb()
{
    item_count=0;
}

void Tbomb::init()
{
    loadgfx(&bomb_floor,(char*)"graphics/bomb_floor.png");
}

int Tbomb::loadgfx(SDL_Surface** s, char* n)
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

void Tbomb::drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id)
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

void Tbomb::SetScale(float xscale,float yscale,int id)
{
    item_store[id].zx=xscale;
    item_store[id].zy=yscale;
}



int Tbomb::create()
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
    item_store[slot].id=slot+1; //+1 for coll detect
    return slot;
}

int Tbomb::remove(int id)
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

int Tbomb::spawn(float px,float py,float pdx,float g,int time,int sframe,int eframe,int acount,int bel) //Create new bomb);
{
    int b;
    b=create();
    //printf("Spawn bomb %d\n",b);
    if (b>=0)  //valid slot
    {
    item_store[b].x=px;						//Set X coord
    item_store[b].y=py;						//Set Y coord
    item_store[b].dx=pdx;
    item_store[b].dy=0;
    item_store[b].grip=g/4;
    item_store[b].t=time;						//Set time (fuse length)
    item_store[b].belongsto=bel;				//Record which player placed bomb
    item_store[b].currframe=sframe;			//Current anim frame
    item_store[b].startframe=sframe;			//Start anim frame
    item_store[b].endframe=eframe;				//End anim frame
    item_store[b].framecount=acount;			//Timer for animation speed
    item_store[b].framecounttotal=acount;		//Total for animation speed timer
    item_store[b].image=bomb_floor;			//Image gfx
    item_store[b].fall=0;						//Is bomb falling
    item_store[b].kicked=false;				//Has bomb been kicked
    item_store[b].falltime=0;					//Time bomb spends falling (for sfx)
    item_store[b].zx=1;
    item_store[b].zy=1;

    return item_store[b].id;
    }
    else return -1;
}

void Tbomb::Update()
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

void Tbomb::Update2(class_struct *is)
{
		is->framecount=is->framecount-1;		//Adjust framecounter (time till frame changes)
		if (is->framecount==0)			//When framecounter=0
		{
			is->framecount=is->framecounttotal;		//Reset framecounter
			is->currframe=is->currframe+1;			//Increase frame
			if (is->currframe>is->endframe) is->currframe=is->startframe;
		}

		if (is->dx!=0)
		{
			is->x=is->x+is->dx;
			if (CollideImage(is->image,(int)is->x,(int)is->y,is->currframe*16,0,16,16,COL_LEFTH | COL_RIGHTH))
			{
				is->x=is->x-is->dx;
				is->dx=-is->dx;
			}
            if ((is->dx>-.5) && (is->dx<.5)) is->dx=0;
		}

		if (is->dy!=0)
		{
			is->y=is->y+is->dy;
			if (is->dy<0)
			{
				is->dy=is->dy+.08;
				if (is->dy>-.07)
				{
					is->dy=0;
					is->kicked=0;
				}
			}
		}

		if (is->kicked==0)
		{
			int z=0;
			is->fall=0;
			for (z=0;z<=3;z++)
			{
				if ((CollideImage(coin_collide,(int)is->x,(int)is->y,0,0,16,16,COL_FOREG)==0) || (CollideImage(coin_collide,(int)is->x,(int)is->y-4,0,0,16,16,COL_FOREG)))
                {
					is->y=is->y+1;
					is->fall=1;
					if (is->dx>0) is->dx=is->dx-.01;
                    if (is->dx<0) is->dx=is->dx+.01;
                }
                else
                {
                    if (is->dx>0) is->dx=is->dx-is->grip;
                    if (is->dx<0) is->dx=is->dx+is->grip;
                }
			}
            if (is->fall)		//Bomb is falling ground
            {
				is->falltime=is->falltime+1;
            }
			else				//Bomb has hit/is on ground
			{
				if (is->falltime>4) channel=Mix_PlayChannel(-1,bomb_land_snd,0);;
				is->falltime=0;
			}
		}

		CollideImage (is->image,(int)is->x,(int)is->y,is->currframe*16,0,16,16,0,COL_BOMB,is->id);

		is->t=is->t-1;						//Countdown bomb timer (fuse)
		if ((CollideImage(is->image,(int)is->x,(int)is->y,0,0,16,16,COL_EXPLOSION)) || (is->t==0)) 	//Once timer=0 or caught by explosion
		{
			explosion.spawn(is->x-24,is->y-48,is->belongsto);
			remove(is->id);
			is->currframe=3;					//Avoid trying to draw non-existent frame
		}
		if (is->y>480)					//If bomb drop off screen
		{
			remove(is->id);
			is->currframe=3;					//Avoid trying to draw non-existent frame
		}
}

void Tbomb::Draw()
{
        int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
        SDL_SetAlpha(item_store[z].image,0,0);
        if (item_store[z].t>0)
        {
			if ((item_store[z].t<50) && ((item_store[z].currframe==0) || (item_store[z].currframe==2))) SDL_SetAlpha(item_store[z].image,SDL_SRCALPHA,128);
			switch (item_store[z].currframe)
			{
                    case 0:
						SetScale(1,1,z);
						break;
					case 1:
						SetScale(1.125,1.125,z);
						break;
                    case 3:
						SetScale(1.125,1.125,z);
						break;
					case 2:
						SetScale(1.25,1.25,z);
						break;
                    default:
						SetScale(1,1,z);
						break;
			}
        }
        drawSprite(item_store[z].image,screen,item_store[z].currframe*16,0,(int)item_store[z].x,(int)item_store[z].y,16,16,z);
        SDL_SetAlpha(item_store[z].image,0,0);
		SetScale(1,1,z);
        }
    }
}

int Tbomb::GetPlayerID(int b)
{
    if (item_count<1) return 0;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==b)
        {
            return item_store[z].belongsto;
        }
    }
    return 0;
}

int Tbomb::GetKicked(int b)
{
    if (item_count<1) return 0;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==b)
        {
            return item_store[z].kicked;
        }
    }
    return 0;
}

void Tbomb::SetKicked(int b)
{
    if (item_count<1) return;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==b)
        {
            item_store[z].kicked=1;
        }
    }
}

void Tbomb::ClearKicked(int b)
{
    if (item_count<1) return;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==b)
        {
            item_store[z].kicked=0;
        }
    }
}


void Tbomb::SetDX(int b,float d)
{
    if (item_count<1) return;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==b)
        {
            item_store[z].dx=d;
        }
    }
}

void Tbomb::SetDY(int b,float d)
{
    if (item_count<1) return;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==b)
        {
            item_store[z].dy=d;
        }
    }
}

void Tbomb::Destroy(int id)
{
    if (item_count<1) return;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==id)
        {
            remove(item_store[z].id);
        }
    }
}

void Tbomb::ClearList(void)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            if (item_store[z].id>=0) remove(item_store[z].id);
        }
    }
}

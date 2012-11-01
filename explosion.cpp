#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_mixer.h>
#include "explosion.h"
#include "collision.h"
#include "DestructTile.h"
#include "smoke.h"

extern SDL_Surface* screen;
//extern SDL_Surface* coin_collide;

extern TDestructTile DestructTile;
extern Tsmoke smoke;

extern Mix_Chunk *explosion_snd;

Texplosion::Texplosion()
{
    item_count=0;
}

void Texplosion::init()
{
    loadgfx(&explosion_gfx,(char*)"graphics/explosion2.png");
}

int Texplosion::loadgfx(SDL_Surface** s, char* n)
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

void Texplosion::drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id)
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

void Texplosion::SetScale(float xscale,float yscale,int id)
{
    item_store[id].zx=xscale;
    item_store[id].zy=yscale;
}



int Texplosion::create()
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

int Texplosion::remove(int id)
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

int Texplosion::spawn(int ex, int ey, int b)
{
    int e;
    e=create();
    if (e>=0)  //valid slot
    {
    item_store[e].x=ex;
    item_store[e].y=ey;
    item_store[e].belongsto=b;
    item_store[e].currframe=0;
    item_store[e].framecount=4;
    item_store[e].framecounttotal=4;
    item_store[e].image=explosion_gfx;				//Set  image to explosion gfx
    item_store[e].zx=1;
    item_store[e].zy=1;

    channel=Mix_PlayChannel(-1,explosion_snd,0);
    return item_store[e].id;
    }
    else return -1;
}

void Texplosion::Update()
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

void Texplosion::Update2(class_struct *is)
{
		is->framecount=is->framecount-1;			//Adjust framecounter
		if (is->framecount==0)				//If framecounter=0 then
		{
			is->framecount=is->framecounttotal;		//Reset framecounter
			is->currframe=is->currframe+1;			//Increase frame
			if (is->currframe==4)
			{
				smoke.Spawn(is->x+24,is->y+28,0,0);
				smoke.Spawn(is->x,is->y+24,1,.125);
				smoke.Spawn(is->x+46,is->y+20,2,.25);
				smoke.Spawn(is->x+36,is->y+16,3,.125);
				smoke.Spawn(is->x+24,is->y+12,4,0);
				smoke.Spawn(is->x+12,is->y+16,5,-.125);
				smoke.Spawn(is->x+2,is->y+20,6,-.25);
				smoke.Spawn(is->x+12,is->y+24,7,-.125);
			}
			if (is->currframe>7)			//If end of frames
			{
				remove(is->id);
				is->currframe=7;					//Avoid trying to draw non-existent frame
			}
			else
			{
				if (is->currframe<4)
				{
					int cc=CollideImage(is->image,is->x,is->y,is->currframe*64,0,64,64,COL_DESTROY_TILE);
					if (cc) DestructTile.Destroy(cc);

					CollideImage (is->image,is->x,is->y,is->currframe*64,0,64,64,0,COL_EXPLOSION,is->id);
				}
			}
		}

}

void Texplosion::Draw()
{
        int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
        SDL_SetAlpha(item_store[z].image,0,0);
        drawSprite(item_store[z].image,screen,item_store[z].currframe*64,0,(int)item_store[z].x,(int)item_store[z].y,64,64,z);
        }
    }
}

int Texplosion::GetPlayerID(int b)
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

void Texplosion::ClearList(void)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            if (item_store[z].id>=0) remove(item_store[z].id);
        }
    }
}

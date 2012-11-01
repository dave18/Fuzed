#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include "smoke.h"
#include "collision.h"

extern SDL_Surface* screen;
//extern SDL_Surface* coin_collide;


Tsmoke::Tsmoke()
{
    item_count=0;
}

void Tsmoke::init()
{
    loadgfx(&smoke_16,(char*)"graphics/smoke_16x16.png");
}

int Tsmoke::loadgfx(SDL_Surface** s, char* n)
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

void Tsmoke::drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id)
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

void Tsmoke::SetScale(float xscale,float yscale,int id)
{
    item_store[id].zx=xscale;
    item_store[id].zy=yscale;
}



int Tsmoke::create()
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

int Tsmoke::remove(int id)
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

int Tsmoke::Spawn(float sx,float sy,int f,float sdx)
{
    int s;
    s=create();
    if (s>=0)  //valid slot
    {
        item_store[s].x=sx;
		item_store[s].y=sy;
		item_store[s].dx=sdx;
		item_store[s].alpha=128;
		item_store[s].currframe=f;
		item_store[s].framecount=0;
		item_store[s].framecounttotal=8;
		item_store[s].time=32;
		item_store[s].image=smoke_16;
		item_store[s].zx=1;
		item_store[s].zy=1;

        return item_store[s].id;
    }
    else
    {
        return -1;

    }
}

void Tsmoke::Update()
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

void Tsmoke::Update2(class_struct *is)
{
		is->y=is->y-.5;
		is->alpha=is->alpha-4;
		is->x=is->x+is->dx;

		is->framecount=is->framecount+1;
		if (is->framecount>is->framecounttotal)
        {
			is->currframe=is->currframe+1;
			if (is->currframe>7) is->currframe=0;
			is->framecount=0;
        }

		if ((is->y<-16) || (is->alpha<=0))
		{
			remove(is->id);
		}

}

void Tsmoke::Draw()
{
        int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
            SDL_SetAlpha(item_store[z].image,SDL_SRCALPHA,item_store[z].alpha);
            drawSprite(item_store[z].image,screen,item_store[z].currframe*16,0,(int)item_store[z].x,(int)item_store[z].y,16,16,z);
            SDL_SetAlpha(item_store[z].image,0,0);
        }
    }
}


void Tsmoke::ClearList(void)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            if (item_store[z].id>=0) remove(item_store[z].id);
        }
    }
}


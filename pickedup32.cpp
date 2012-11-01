#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include "pickedup32.h"
#include "collision.h"

extern SDL_Surface* screen;
extern SDL_Surface* coin_collide;


Tpickedup32::Tpickedup32()
{
    item_count=0;
}

void Tpickedup32::init()
{
    loadgfx(&picked_up_32x32,(char*)"graphics/invincibility_picked_up.png");
}

int Tpickedup32::loadgfx(SDL_Surface** s, char* n)
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

void Tpickedup32::drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id)
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

void Tpickedup32::SetScale(float xscale,float yscale,int id)
{
    item_store[id].zx=xscale;
    item_store[id].zy=yscale;
}



int Tpickedup32::create()
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

int Tpickedup32::remove(int id)
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

int Tpickedup32::spawn(int x, int y) //Create new pickedup32);
{
    int b;
    b=create();
    //printf("Spawn pickedup32 %d\n",b);
    if (b>=0)  //valid slot
    {
    item_store[b].x=x;						//Set X coord
    item_store[b].y=y;						//Set Y coord
    item_store[b].currframe=0;			//Current anim frame
    item_store[b].framecount=0;			//Timer for animation speed
    item_store[b].zx=1;
    item_store[b].zy=1;
    item_store[b].image=picked_up_32x32;

    return item_store[b].id;
    }
    else return -1;
}

void Tpickedup32::Update()
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

void Tpickedup32::Update2(class_struct *is)
{
		is->framecount+=1;
		if (is->framecount>2)
		{
			is->framecount=0;
			is->currframe+=1;
			if (is->currframe>5)
			{
				is->currframe=11;
				remove(is->id);
			}
		}

}

void Tpickedup32::Draw()
{
        int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
        //'If CollideImage(image,x,y,0,32,0) Then t=1
        drawSprite(item_store[z].image,screen,item_store[z].currframe*32,0,item_store[z].x,item_store[z].y,32,32,z);
        }
    }
}


void Tpickedup32::Destroy(int id)
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

void Tpickedup32::ClearList(void)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            if (item_store[z].id>=0) remove(item_store[z].id);
        }
    }
}

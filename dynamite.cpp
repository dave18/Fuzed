#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_mixer.h>
#include "dynamite.h"
#include "collision.h"
#include "explosion.h"

extern SDL_Surface* screen;
extern SDL_Surface* coin_collide;

extern Texplosion explosion;
extern Mix_Chunk* mine_arm_snd;

Tdynamite::Tdynamite()
{
    item_count=0;
}

void Tdynamite::init()
{
    loadgfx(&dynamite_floor,(char*)"graphics/dynamite_floor.png");
}

int Tdynamite::loadgfx(SDL_Surface** s, char* n)
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

void Tdynamite::drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id)
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

void Tdynamite::SetScale(float xscale,float yscale,int id)
{
    item_store[id].zx=xscale;
    item_store[id].zy=yscale;
}



int Tdynamite::create()
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


int Tdynamite::remove(int id)
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

int Tdynamite::spawn(int mx,int my,int sframe,int eframe,int acount,int bel) //Create new dynamite);
{
    int b;
    b=create();
    //printf("Spawn dynamite %d\n",b);
    if (b>=0)  //valid slot
    {
    item_store[b].x=mx;						//Set X coord
    item_store[b].y=my;						//Set Y coord
    item_store[b].belongsto=bel;				//Record which player placed dynamite
    item_store[b].currframe=sframe;			//Current anim frame
    item_store[b].startframe=sframe;			//Start anim frame
    item_store[b].endframe=eframe;				//End anim frame
    item_store[b].framecount=acount;			//Timer for animation speed
    item_store[b].framecounttotal=acount;		//Total for animation speed timer
    item_store[b].image=dynamite_floor;			//Image gfx
    item_store[b].zx=1;
    item_store[b].zy=1;

    channel=Mix_PlayChannel(-1,mine_arm_snd,0);

    return item_store[b].id;
    }
    else return -1;
}

void Tdynamite::Update()
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

void Tdynamite::Update2(class_struct *is)
{
		is->framecount=is->framecount-1;		//Adjust framecounter (time till frame changes)
		if (is->framecount==0)			//When framecounter=0
		{
			is->framecount=is->framecounttotal;		//Reset framecounter
			is->currframe=is->currframe+1;			//Increase frame
			if (is->currframe>is->endframe) is->currframe=is->startframe;
		}

		if (CollideImage(is->image,is->x,is->y,0,0,16,16,COL_EXPLOSION))	//Caught by explosion
        {
			explosion.spawn(is->x+24,is->y-48,is->belongsto);


            //player.ClearTnt(is->belongsto);

			remove(is->id);		//Remove dynamite from listq
			is->currframe=3;					//Avoid trying to draw non-existent frame
		}
}

void Tdynamite::Draw()
{
        int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
        //'If CollideImage(image,x,y,0,32,0) Then t=1
        drawSprite(item_store[z].image,screen,item_store[z].currframe*16,0,item_store[z].x,item_store[z].y,16,16,z);
        }
    }
}

int Tdynamite::GetPlayerID(int b)
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

int Tdynamite::GetY(int b)
{
    if (item_count<1) return 0;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==b)
        {
            return item_store[z].y;
        }
    }
    return 0;
}

int Tdynamite::GetX(int b)
{
    if (item_count<1) return 0;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==b)
        {
            return item_store[z].x;
        }
    }
    return 0;
}


void Tdynamite::Destroy(int id)
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


int Tdynamite::Find(int id)
{
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
            if (item_store[z].belongsto==id) return item_store[z].id;
        }
    }
    return 0;
}

void Tdynamite::ClearList(void)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            if (item_store[z].id>=0) remove(item_store[z].id);
        }
    }
}

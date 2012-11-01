#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include "collision.h"
#include "gem.h"


extern SDL_Surface* smallfont;
extern SDL_Surface* screen;

extern Tgem gem;

extern void SmallText(char* strg,int x,int y);


Tgem::Tgem()
{
    item_count=0;
}

void Tgem::init()
{
    loadgfx(&gem_10_points,(char*)"graphics/gem_10_points.png");
    loadgfx(&gem_20_points,(char*)"graphics/gem_20_points.png");
    loadgfx(&gem_40_points,(char*)"graphics/gem_40_points.png");
    loadgfx(&gem_80_points,(char*)"graphics/gem_80_points.png");
    loadgfx(&gem_100_points,(char*)"graphics/gem_100_points.png");
    loadgfx(&gem_150_points,(char*)"graphics/gem_150_points.png");
    loadgfx(&gem_collide,(char*)"graphics/gem_collide.png");

}

int Tgem::loadgfx(SDL_Surface** s, char* n)
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

void Tgem::drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id)
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

void Tgem::SetScale(float xscale,float yscale,int id)
{
    item_store[id].zx=xscale;
    item_store[id].zy=yscale;
}



int Tgem::create()
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

int Tgem::remove(int id)
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

int Tgem::spawn(int px,int py, int acount, int t, int ft) //Create new gem);
{
    int g;
    g=create();
    if (g>=0)  //valid slot
    {
		item_store[g].x=px;
		item_store[g].y=py;
		item_store[g].fall=1;
		item_store[g].falltime=ft;
		item_store[g].currframe=0;
		item_store[g].framecount=0;
		item_store[g].framecounttotal=acount;
		item_store[g].existfor=1000;
		item_store[g].zx=1;
		item_store[g].zy=1;
		switch (t)
		{
			case 1:
				item_store[g].value=20;
				item_store[g].image=gem_20_points;
				break;
			case 2:
				item_store[g].value=40;
				item_store[g].image=gem_40_points;
				break;
			case 3:
				item_store[g].value=80;
				item_store[g].image=gem_80_points;
				break;
			case 4:
				item_store[g].value=100;
				item_store[g].image=gem_100_points;
				break;
			case 5:
				item_store[g].value=150;
				item_store[g].image=gem_150_points;
				break;
			default:
				item_store[g].value=10;
				item_store[g].image=gem_10_points;
				break;
		}
    return item_store[g].id;
    }
    else return -1;
}

void Tgem::Update()
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

void Tgem::Update2(class_struct *is)
{
        if (is->framecount==0)
        {
			is->currframe=is->currframe+1;
			if (is->currframe>3) is->currframe=0;
        }
		is->framecount=is->framecount+1;
		if (is->framecount>=is->framecounttotal) is->framecount=0;

		if (is->fall==1)
		{
			if (is->falltime==0)
			{
				if (CollideImage(gem_collide,is->x,is->y+2,0,0,16,16,COL_FOREG)) is->fall=0;
			}
			else
			{
				is->falltime=is->falltime-1;
				if ((is->falltime==0) && (CollideImage(gem_collide,is->x,is->y,0,0,16,16,COL_FOREG))) is->falltime=1;
			}
		}


		if (is->fall==1) is->y=is->y+4;

		is->existfor=is->existfor-1;

		if ((is->y>480) || (is->existfor<1))
		{
			remove(is->id);
		}
		else
		{
			CollideImage(is->image,is->x,is->y,is->currframe*16,0,16,16,0,COL_GEM,is->id);
		}

}

void Tgem::Draw()
{
        int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
            drawSprite(item_store[z].image,screen,item_store[z].currframe*16,0,(int)item_store[z].x,(int)item_store[z].y,16,16,z);
        }
    }
}


int Tgem::GetValue(int id)
{
    if (item_count<1) return 0;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==id)
        {
            return item_store[z].value;
        }
    }
    return 0;
}

int Tgem::GetX(int id)
{
    if (item_count<1) return 0;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==id)
        {
            return item_store[z].x;
        }
    }
    return 0;
}

int Tgem::GetY(int id)
{
    if (item_count<1) return 0;
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id==id)
        {
            return item_store[z].y;
        }
    }
    return 0;
}

void Tgem::Destroy(int id)
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

void Tgem::ClearList(void)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            if (item_store[z].id>=0) remove(item_store[z].id);
        }
    }
}

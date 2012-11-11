#include <SDL/SDL.h>
#include "DestructTile.h"
#include "collision.h"

extern SDL_Surface* tilesBitmap;
extern char map[4] [40] [30];

TDestructTile::TDestructTile()
{
    item_count=0;
}

int TDestructTile::getTileX(SDL_Surface* imageSurface, int t)
{

	SDL_Rect srcRect;
	srcRect.y = t/(imageSurface->w/16);
	srcRect.x = t-(srcRect.y*(imageSurface->w/16));
	srcRect.x = srcRect.x*16;
	return srcRect.x;

}

int TDestructTile::getTileY(SDL_Surface* imageSurface, int t)
{

	SDL_Rect srcRect;
	srcRect.y = t/(imageSurface->w/16);
	srcRect.y = srcRect.y*16;
	return srcRect.y;

}

int TDestructTile::create(void)
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
    item_store[slot].id=slot+64; //+1 for coll detect
    return slot;
}

int TDestructTile::remove(int id)
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


int TDestructTile::spawn(int tx,int ty,int t)
{
    int b;
    b=create();
    if (b>=0)  //valid slot
    {
    item_store[b].x=tx;						//Set X coord
    item_store[b].y=ty;						//Set Y coord
    item_store[b].tile=t;
    switch (t)
    {
			case 51:
				item_store[b].replacewith=0;
				item_store[b].replaceunder=1;
				item_store[b].replaceunderwith=102;
				break;

			default:
				item_store[b].replacewith=0;
				item_store[b].replaceunder=0;
				break;
    }

    return item_store[b].id;
    }
    else return -1;
}

void TDestructTile::Update()
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

void TDestructTile::Update2(class_struct *is)
{
    CollideImage (tilesBitmap,is->x*16,is->y*16,getTileX(tilesBitmap,is->tile),getTileY(tilesBitmap,is->tile),16,16,0,COL_DESTROY_TILE,is->id);
}

void TDestructTile::AddToCollisionLayer()
{
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
            CollideImage (tilesBitmap,item_store[z].x*16,item_store[z].y*16,getTileX(tilesBitmap,item_store[z].tile),getTileY(tilesBitmap,item_store[z].tile),16,16,0,COL_DESTROY_TILE,item_store[z].id);
        }
    }
}


void TDestructTile::Destroy(int id)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            if (item_store[z].id==id) //match
            {

                map[3] [item_store[z].x] [item_store[z].y]=0;
                map[1] [item_store[z].x] [item_store[z].y]=(char)item_store[z].replacewith;
                if (item_store[z].replaceunder) map[1] [item_store[z].x] [item_store[z].y+1]=(char)item_store[z].replaceunderwith;

                remove(item_store[z].id); //deacivate
            }
        }
    }
}

void TDestructTile::ClearList(void)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            remove(item_store[z].id);
        }
    }
}

int TDestructTile::CountList(void)
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

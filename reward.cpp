#include <SDL/SDL.h>
#include "reward.h"


extern SDL_Surface* smallfont;

extern void SmallText(char* strg,int x,int y);


Treward::Treward()
{
    item_count=0;
}


int Treward::create()
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
    item_store[slot].id=slot;
    return slot;
}

int Treward::remove(int id)
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

int Treward::spawn(int rx,float ry,int v) //Create new reward);
{
    int b;
    b=create();
    //printf("Spawn reward %d\n",b);
    if (b>=0)  //valid slot
    {
    item_store[b].id=b;
    item_store[b].x=rx;						//Set X coord
    item_store[b].y=ry;						//Set Y coord
    item_store[b].value=v;
    item_store[b].alpha=255;

    return b;
    }
    else return -1;
}

void Treward::Update()
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

void Treward::Update2(class_struct *is)
{
        is->alpha=is->alpha-4;

		is->y=is->y-.5;

		if ((is->y<-8) || (is->alpha<=0)) remove(is->id);

}

void Treward::Draw()
{
        int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
            char buffer[6];
            sprintf(buffer,"%d\0",item_store[z].value);
            SDL_SetAlpha(smallfont,SDL_SRCALPHA,item_store[z].alpha);
            SmallText(buffer,item_store[z].x+4,(int)item_store[z].y-8);
            SDL_SetAlpha(smallfont,0,0);
        }
    }
}


void Treward::ClearList(void)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            remove(item_store[z].id);
        }
    }
}

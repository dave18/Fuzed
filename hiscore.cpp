#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include "hiscore.h"
#include "collision.h"
#include <string.h>

extern SDL_Surface* screen;
extern SDL_Surface* font;

extern void Text(char* strg,int x,int y);
extern void SmallText(char* strg,int x,int y);
extern void PutNumberLeadingZeros(char* b,int v,int n);
extern int get_key(int i);
extern void SetHighScore(int p,int s,char* t,int n);

/*extern int hiscores[100];
extern char *hiname[100];
extern char hinew[100];
*/

Thiscore::Thiscore()
{
    item_count=0;
}

void Thiscore::init()
{

}


int Thiscore::create()
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
    item_store[slot].id=slot+1;
    return slot;
}

int Thiscore::remove(int id)
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

int Thiscore::Spawn(int i,int sc,int p)
{
    int s;
    s=create();
    if (s>=0)  //valid slot
    {
        item_store[s].player=i;
        item_store[s].score=sc;
		item_store[s].entrytime=5000;
		strcpy(item_store[s].hstring,"AAA");
		item_store[s].alpha=1;
		item_store[s].adir=-16;
		item_store[s].blinkcount=9;
		item_store[s].blink=0;
		item_store[s].currletter=0;
		item_store[s].entrycount=0;
		item_store[s].position=p;


        return item_store[s].id;
    }
    else
    {
        return -1;

    }
}

void Thiscore::Update()
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

void Thiscore::Update2(class_struct *is)
{
		int x;
		switch (is->player)	//'Set position for Hi-score entry to be displayed
		{
		case 2:
			x=536;
			break;
        default:
			x=8;
			break;
		}

		is->alpha+=is->adir;
		if ((is->alpha<=0) || (is->alpha>=255))
		{
			is->adir=-is->adir;
			is->alpha+=is->adir;
		}
		SDL_SetAlpha(font,SDL_SRCALPHA,is->alpha);
		char buffer[10];
		PutNumberLeadingZeros(&buffer[0],is->score,6);
		Text(buffer,x,8);
		SDL_SetAlpha(font,0,255);
		if ((is->currletter!=0) || (is->blink==0))
		{
		    strncpy(buffer,&is->hstring[0],1);
		    buffer[1]=0;
		    Text(buffer,x+24,28);
		}
        if ((is->currletter!=1) || (is->blink==0))
		{
		    strncpy(buffer,&is->hstring[1],1);
		    buffer[1]=0;
		    Text(buffer,x+40,28);
		}
        if ((is->currletter!=2) || (is->blink==0))
		{
		    strncpy(buffer,&is->hstring[2],1);
		    buffer[1]=0;
		    Text(buffer,x+56,28);
		}


		is->blinkcount=(1+is->blinkcount) % 8;
		if (is->blinkcount==0) is->blink=!is->blink;

		if (is->entrycount>0) is->entrycount-=1;

		int joymoved=0;
		int lleft=0;
		int lright=0;
		int keydata=get_key(0);
		if ((is->entrycount==0) && (is->currletter<3))
		{
			if (keydata & MY_LEFT)
			{
				joymoved=1;
				lleft=1;
			}

			if (keydata & MY_RIGHT)
			{
				joymoved=1;
				lright=1;
			}


			if (keydata & MY_BUTT_B)
			{
				is->currletter+=1;
				lleft=0;
				lright=0;
				is->entrycount=8;
				if (is->currletter==3) is->entrytime=400;
			}

		}

		if ((lleft==1) && (lright==1))
		{
			lleft=0;
			lright=0;
		}

		if (lleft==1)
		{
			is->hstring[is->currletter]-=1;
			if (is->hstring[is->currletter]<32) is->hstring[is->currletter]=90;
			is->entrycount=8;
		}

		if (lright==1)
		{
			is->hstring[is->currletter]+=1;
			if (is->hstring[is->currletter]>90) is->hstring[is->currletter]=32;
			is->entrycount=8;
		}



		is->entrytime-=1;
		if (is->entrytime<=0)
		{
            is->hstring[3]=0;
            SetHighScore(is->position,is->score,is->hstring,1);
			remove (is->id);
		}
}

void Thiscore::Draw()
{

}


void Thiscore::ClearList(void)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            if (item_store[z].id>=0) remove(item_store[z].id);
        }
    }
}

int Thiscore::CountList(void)
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

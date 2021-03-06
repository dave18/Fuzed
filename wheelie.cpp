#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include "wheelie.h"
#include "collision.h"
#include "bomb.h"
#include "mine.h"
#include "explosion.h"
#include "reward.h"
#include "player.h"

extern Tplayer player;
extern Tbomb bomb;
extern Tmine mine;
extern Texplosion explosion;
extern Treward reward;

extern SDL_Surface* screen;
extern SDL_Surface* spritemasks;

extern void SmallText(char* strg,int x,int y);

extern char map[4] [40] [30];

static char fade_values[20]={0,10,20,30,40,50,60,70,90,110,130,150,180,190,200,210,220,230,240,250};

Twheelie::Twheelie()
{
    item_count=0;
}

void Twheelie::init()
{
    loadgfx(&wheelie_left,(char*)"graphics/wheelie_left.png");
    loadgfx(&wheelie_right,(char*)"graphics/wheelie_right.png");
    loadgfx(&wheelie_die_left,(char*)"graphics/wheelie_die_left.png");
    loadgfx(&wheelie_die_right,(char*)"graphics/wheelie_die_right.png");
}

int Twheelie::loadgfx(SDL_Surface** s, char* n)
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

void Twheelie::drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id)
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

void Twheelie::SetScale(float xscale,float yscale,int id)
{
    item_store[id].zx=xscale;
    item_store[id].zy=yscale;
}



int Twheelie::create()
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

int Twheelie::remove(int id)
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

int Twheelie::spawn(int px,int py,int dir)
{
    int e;
    e=create();
    if (e>=0)  //valid slot
    {
    item_store[e].x=px;
    item_store[e].y=py;
    item_store[e].dx=dir;					//X direction
    item_store[e].startframe=0;		//Start anim frame
    item_store[e].endframe=3;			//End anim frame
    item_store[e].framecount=8;		//Counter for animation delay
    item_store[e].value=100;
    item_store[e].currframe=item_store[e].startframe;		//Current anim frame
    item_store[e].framecounttotal=item_store[e].framecount;	//Max for animation delay counter
    item_store[e].explode=0;				//Is enemy currently exploding/dying
    if (item_store[e].dx==1) item_store[e].image=wheelie_right; else item_store[e].image=wheelie_left;
    item_store[e].dieflash=0;
    item_store[e].isjumping=0;
    item_store[e].hitwalltimer=4;
    item_store[e].hitbombtimer=0;
    item_store[e].fall=0;
    item_store[e].zx=1;
    item_store[e].zy=1;

    return item_store[e].id;
    }
    else return -1;
}

void Twheelie::Appear(float s)
{
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
            SetScale (4-(3*s),s,z);
            drawSprite(item_store[z].image,screen,item_store[z].currframe*32,0,item_store[z].x,item_store[z].y,32,32,z);
            SetScale(1,1,z);
        }
    }
}

void Twheelie::Update()
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

void Twheelie::Update2(class_struct *is)
{
		//Check for collision with explosion
		if (is->hitbombtimer>0) is->hitbombtimer-=1;
		if (is->hitwalltimer>0) is->hitwalltimer-=1;
		if (is->explode==0)
        {
			if (is->dx<0) is->image=wheelie_left; else is->image=wheelie_right;

			/*Local temp:Object[]*/
			int temp;
			temp=CollideImage(is->image,is->x,is->y,is->currframe*32,0,32,32,COL_EXPLOSION);
			if (temp)
			{
				//Cycle through bombs until we find the one that matches collision
				int tempid=explosion.GetPlayerID(temp); //Used to hold the id of which player placed bomb
				player.AddScore(is->value,tempid);
				is->explode=1;
				is->currframe=0;
				is->dietime=100;
				is->dieflash=0;
				reward.spawn(is->x-4,is->y,is->value);
			}

			//Collision detection for mines
			temp=CollideImage(is->image,is->x,is->y,is->currframe*32,0,32,32,COL_MINE);
			if (temp)
			{
			    if (mine.GetTime(temp)==0)
			    {
                    int tempmine=mine.GetPlayerID(temp); //Used to hold the id of which player placed mine
                    explosion.spawn(is->x-24,is->y-48,tempmine);
                    mine.Destroy(temp);
			    }
			}


        }
		if (is->explode==0)
		{
			int z=0;

			if (is->isjumping==0)
			{
				for (z=0;z<=3;z++)
				{
					is->fall=0;
					if (!CollideImage(spritemasks,is->x,is->y,0,0,32,32,COL_FOREG))
                    {
						is->y+=1;
						is->fall=1;
                    }
					else
					{
						if (CollideImage(spritemasks,is->x,is->y-5,4*32,0,32,32,COL_FOREG))
						{
							is->fall=1;
							is->y+=1;
						}
					}
				}
			}

			//Check for collision with unexploded bomb
			if ((CollideImage(is->image,is->x,is->y,is->currframe*32,0,32,32,COL_BOMB)) && (is->hitbombtimer==0))
			{
				if (is->fall==1)
				{
					is->y=is->y-1;
					is->fall=0;
				}
				else
				{
					is->dx=-is->dx;
					is->hitbombtimer=32;
				}
			}

			if (is->y>=480) is->y=-30;
			if (is->fall==0)
			{
				is->x=is->x+is->dx;
				if (is->x>=640) is->x=-30;
				if (is->x<=-32) is->x=638;
			}

			if ((CollideImage(is->image,is->x,is->y,is->currframe*32,0,32,32,COL_LEFTH)) && (is->hitwalltimer==0))
			{
				is->x=is->x-is->dx;
				is->dx=-is->dx;
				is->hitwalltimer=32;
			}
 	 		if ((CollideImage(is->image,is->x,is->y,is->currframe*32,0,32,32,COL_RIGHTH)) && (is->hitwalltimer==0))
			{
				is->x=is->x-is->dx;
				is->dx=-is->dx;
				is->hitwalltimer=32;
			}

			if ((((int)rand() % 300==5) || (is->tryjumpagain==1)) && (is->fall==0) && (is->isjumping==0))		//Will enemy jump up to next platform
			{
				int xgrid,ygrid;
				xgrid=((is->x) >> 4) + 1;
				if ((xgrid>=0) && (xgrid<40))
				{
					ygrid=((is->y) >> 4) +1;
					int isplatform=0;
					int topofcheck=ygrid-5;
					if (topofcheck<0) topofcheck=0;
					int botofcheck=ygrid-2;
					if (botofcheck>29) botofcheck=29;
					if (botofcheck>topofcheck)
					{
                        /*SDL_Rect r;
                        r.x=xgrid*16;
                        r.y=topofcheck*16;
                        r.w=16;
                        r.h=(botofcheck-topofcheck)*16;
                        SDL_FillRect(screen,&r,SDL_MapRGB(screen->format,255,255,255));*/

                        for (int z=topofcheck;z<=botofcheck;z++)
							if (map[3] [xgrid] [z]==8) isplatform=z;

						if (isplatform>0)
						{
							is->isjumping=1;
							is->jumplevel=(isplatform-1)*16;
							is->jumptimer=24;
							is->fatness=0;
							is->dx=0;
						}
					}
				}
			}

			if (is->fall==0) is->tryjumpagain=0;


			is->framecount=is->framecount-1;
			if (is->framecount==0)
			{
				is->framecount=is->framecounttotal;
				is->currframe=is->currframe+1;
				if (is->currframe>is->endframe) is->currframe=is->startframe;
			}


			if (is->isjumping==2)
			{
				is->y-=8;
				is->fatness=.5;
				if (is->y<=is->jumplevel-16)
				{
					is->isjumping=0;
					is->dx=rand() % 2;
					if (is->dx==0) is->dx=-1;
					is->tryjumpagain=rand() % 2;
				}
			}


			if (is->isjumping==1)
			{
				is->currframe=0;
				is->jumptimer-=1;
                if ((is->jumptimer >> 3) << 3 == is->jumptimer)
				{
					//is->jumpdir=1-is->jumpdir;
                    is->fatness+=.32;
				}
				if (is->jumpdir) is->image=wheelie_left; else is->image=wheelie_right;
				if (is->jumptimer==0) is->isjumping=2;
			}
		}
		else
		{
			if (is->dx<0) is->image=wheelie_die_left; else is->image=wheelie_die_right;

			is->dietime=is->dietime-1;
			if (is->dietime<40) is->dieflash=1-is->dieflash;
			if (is->dietime==0)
			{
				remove(is->id);
			}
		}
}

void Twheelie::Draw()
{
    int z;
    for (z=0;z<item_count;z++)
    {
        if (item_store[z].id>=0)
        {
        if (item_store[z].dieflash)
        {
			//SetBlend(LIGHTBLEND)
			if (item_store[z].dietime<20) SDL_SetAlpha(item_store[z].image,SDL_SRCALPHA,fade_values[item_store[z].dietime]);
        }
		else SDL_SetAlpha(item_store[z].image,0,255);

		if (item_store[z].isjumping==1)
		{
			SetScale(1+item_store[z].fatness,1-(item_store[z].fatness/2),z);
			SetScale(1,1-(item_store[z].fatness/2),z);
			drawSprite(item_store[z].image,screen,item_store[z].currframe*32,0,item_store[z].x,item_store[z].y+(int)(item_store[z].fatness*8),32,32,z);
		}
		if (item_store[z].isjumping==2)
		{
			SetScale(0.5,2,z);
			drawSprite(item_store[z].image,screen,item_store[z].currframe*32,0,item_store[z].x,item_store[z].y-16,32,32,z);
		}
		if (item_store[z].isjumping==0) drawSprite(item_store[z].image,screen,item_store[z].currframe*32,0,item_store[z].x,item_store[z].y,32,32,z);
		if (item_store[z].explode==0) CollideImage (item_store[z].image,item_store[z].x,item_store[z].y,item_store[z].currframe*32,0,32,32,0,COL_ENEMY,item_store[z].id);
		SDL_SetAlpha(item_store[z].image,0,255);
		SetScale(1,1,z);

/*		char buffer[20];
		sprintf(buffer,"%d\0",z);
		SmallText(buffer,item_store[z].x+4,item_store[z].y-8);
*/
        }
    }
}

void Twheelie::ClearList(void)
{
    if (item_count>0)
    {
        for (int z=0;z<item_count;z++)
        {
            if (item_store[z].id>=0) remove(item_store[z].id);
        }
    }
}

int Twheelie::CountList(void)
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


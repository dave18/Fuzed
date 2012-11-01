#include <SDL/SDL.h>
#include "collision.h"


#define MAX_LAYERS 15

SDL_Surface * collision_layer[MAX_LAYERS];
extern SDL_Surface * screen;

void InitCollisions(int n)
{
    SDL_Color colors[256];
    for (int p=0;p<256;p++)
    {
        colors[p].r=p;
        colors[p].g=p;
        colors[p].b=p;
    }
    if (n>MAX_LAYERS) n=MAX_LAYERS;
    for (int z=0;z<n;z++)
    {
        collision_layer[z]=SDL_CreateRGBSurface(SDL_SWSURFACE,640,480,8,0,0,0,0);
        SDL_SetPalette(collision_layer[z],SDL_LOGPAL | SDL_PHYSPAL,colors,0,256);
    }
    ResetCollisions(0);
}

void ResetCollisions(int n)
{
    for (int z=0;z<MAX_LAYERS;z++)
    {
    if (((1<<z) & n) || (n==0)) //layers to clear
    {
        //printf("layer %d\n",z);
        if (collision_layer[z])
        {
            //SDL_FillRect(collision_layer[z],NULL,0);
            SDL_LockSurface(collision_layer[z]);
            memset((char*)collision_layer[z]->pixels,0,640*480);
            SDL_UnlockSurface(collision_layer[z]);
        }
    }
    }
}

void ShowCollisionLayer(int z)
{

    SDL_Rect srcRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 640;
	srcRect.h = 480;

	SDL_Rect dstRect;
	dstRect.x = 0;
	dstRect.y = 0;
	dstRect.w = 640;
	dstRect.h = 480;

	SDL_BlitSurface(collision_layer[z], &srcRect, screen, &dstRect);


}

void SetPix ( SDL_Surface* pSurface , int x , int y , SDL_Color color )
{
  //convert color
  Uint32 col = SDL_MapRGB ( pSurface->format , color.r , color.g , color.b ) ;

  SDL_LockSurface(pSurface);

  //determine position
  char* pPosition = ( char* ) pSurface->pixels ;

  //offset by y
  pPosition += ( pSurface->pitch * y ) ;

  //offset by x
  pPosition += ( pSurface->format->BytesPerPixel * x ) ;

  //copy pixel data
  memcpy ( pPosition , &col , pSurface->format->BytesPerPixel ) ;
  SDL_UnlockSurface(pSurface);
}

int CollideImage(SDL_Surface* src,int x,int y,int sx,int sy,int w,int h,int cm)
{




int cl,cr,ct,cb,c1,cx,cy;
c1=SDL_MapRGB(src->format, 0xFF,0,0xFF);
for (int z=0;z<MAX_LAYERS;z++)
    {
    if (((1<<z) & cm) || (cm==0)) //layers to clear
    {
        if (collision_layer[z])
        {





	//**** Work out total region that may have a collision within ****
	cl=x;
	if (x<0) cl=0;
	cr=x+w;
	if (x+w>640) cr=640;
	ct=y;
	if (y<0) ct=0;
	cb=y+h;
	if (y+h>480) cb=480;


    Uint8* p1;
    Uint8* p2;

	//**** Go through the collision region and see if both gfx have an unmasked pixel ****
	for (cy=ct;cy<cb;cy++)
	{
        p2=(Uint8*)collision_layer[z]->pixels+(cy * collision_layer[z]->pitch) + cl;
		for (cx=cl;cx<cr;cx++)
		{
            if (*p2)
            {
                p1=(Uint8*)src->pixels+((cy-y)+sy) * src->pitch + ((cx-x)+sx) * 2;
                if ((*p1+(*(p1+1)<<8))!=c1) return *p2;
			}
			p2++;

		}
	}

        }
    }
    }


	//**** If we've got here then no collision detected so return zero ****
	return(0);
}


void CollideImage(SDL_Surface* src,int x,int y,int sx,int sy,int w,int h,int cm,int wm,char id)
{
    if (x<0) x=0;
    if (y<0) y=0;
    if (x+w>640) w=640-x;
    if (y+h>479) h=479-y;
    int mc=SDL_MapRGB(src->format,255,0,255);
    for (int z=0;z<MAX_LAYERS;z++)
    {
        if (wm & (1<<z))
        {
            if (collision_layer[z])
            {
                SDL_LockSurface(src);
                SDL_LockSurface(collision_layer[z]);



                for (int ty=0;ty<h;ty++)
                {
                    char* p2=(char*)collision_layer[z]->pixels;
                    p2+=(y+ty)*collision_layer[z]->pitch+x;
                    char* p1=(char*)src->pixels;
                    p1+=((sy+ty)*src->pitch)+(sx*src->format->BytesPerPixel);
                    for (int tx=0;tx<w;tx++)
                    {
                        int c=*p1 + (*(p1+1)<<8);

                        if (c!=mc) *p2=id;
                        p1++;
                        p1++;
                        p2++;
                    }

                }


                SDL_UnlockSurface(src);
                SDL_UnlockSurface(collision_layer[z]);
//                printf("z %d\n",z);
    /*if (z==1)
    {

    SDL_Rect srcRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 640;
	srcRect.h = 480;

	SDL_Rect dstRect;
	dstRect.x = 0;
	dstRect.y = 0;
	dstRect.w = 640;
	dstRect.h = 480;

	SDL_BlitSurface(collision_layer[z], &srcRect, screen, &dstRect);
    }*/
            }
        }
    }
}

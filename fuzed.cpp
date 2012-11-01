#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include <new>
#include "fuzed.h"
#include "level.h"
#include "player.h"
#include "collision.h"
#include "wheelie.h"
#include "grog.h"
#include "slinky.h"
#include "DestructTile.h"
#include "bomb.h"
#include "explosion.h"
#include "smoke.h"
#include "mine.h"
#include "reward.h"
#include "dynamite.h"
#include "gem.h"
#include "coin.h"
#include "pickup.h"
#include "pickedup16.h"
#include "pickedup32.h"
#include "hiscore.h"

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

struct CONTROLS {
    int joy;
    int left,right,jump,cycle,fire;
    int leftdown,rightdown,jumpdown,cycledown,firedown,quitdown;

};

SDL_Surface* screen;
char LEFTDOWN,RIGHTDOWN,ADOWN,BDOWN,XDOWN,YDOWN,UPDOWN,DOWNDOWN,STARTDOWN,LSDOWN,RSDOWN,PAUSEDOWN,QDOWN=0;
int mx=0;
int my=0;

int oldvol;
int startlevel;
int quitgame;
int soundpresent=1;
//Global joysticks=JoyCount()					'Count number of joysticks attached
Tlevel level;					//Create of global instance of level
Tplayer player;
Twheelie wheelie;
Tgrog grog;
Tslinky slinky;
TDestructTile DestructTile;
Tbomb bomb;
Texplosion explosion;
Tsmoke smoke;
Tmine mine;
Treward reward;
Tdynamite dynamite;
Tgem gem;
Tcoin coin;
Tpickup pickup;
Tpickedup16 pickedup16;
Tpickedup32 pickedup32;
Thiscore hiscore;

Mix_Chunk *mine_arm_snd;
Mix_Chunk *explosion_snd;
Mix_Chunk *collect_snd;
Mix_Chunk *jump_snd;
Mix_Chunk *bomb_land_snd;

Mix_Music *music;

SDL_Surface* font;
SDL_Surface* smallfont;
SDL_Surface* spritemasks;
SDL_Surface* coin_collide;
SDL_Surface* tilesBitmap;

CONTROLS playercontrols[2];

int background_on;

int hiscores[100];
char *hiname[100];
char hinew[100];

char map[4] [40] [30];							//Store level map

extern char* levelbank;


char PeekByte(char* mem,int addr)
{
    return mem[addr] & 0xFF;
}

int PeekInt(char* mem,int addr)
{
    return (mem[addr] | (mem[addr+1]<<8) | (mem[addr+2]<<16) | (mem[addr+3]<<24)) & 0xFFFFFFFF;
}

static void xtoa (
        unsigned long val,
        char *buf,
        unsigned radix,
        int is_neg
        )
{
        char *p;                /* pointer to traverse string */
        char *firstdig;         /* pointer to first digit */
        char temp;              /* temp char */
        unsigned digval;        /* value of digit */

        p = buf;

        if (is_neg) {
            /* negative, so output '-' and negate */
            *p++ = '-';
            val = (unsigned long)(-(long)val);
        }

        firstdig = p;           /* save pointer to first digit */

        do {
            digval = (unsigned) (val % radix);
            val /= radix;       /* get next digit */

            /* convert to ascii and store */
            if (digval > 9)
                *p++ = (char) (digval - 10 + 'a');  /* a letter */
            else
                *p++ = (char) (digval + '0');       /* a digit */
        } while (val > 0);
		*p=ENDCHAR;

        /* We now have the digit of the number in the buffer, but in reverse
           order.  Thus we reverse them now. */

        //*p-- = '\0';            /* terminate string; p points to last digit */
		*p--;

        do {
            temp = *p;
            *p = *firstdig;
            *firstdig = temp;   /* swap *p and *firstdig */
            --p;
            ++firstdig;         /* advance to next two digits */
        } while (firstdig < p); /* repeat until halfway */
}

/* Actual functions just call conversion helper with neg flag set correctly,
   and return pointer to buffer. */

char * nitoa (
        int val,
        char *buf,
        int radix
        )
{
        if (radix == 10 && val < 0)
            xtoa((unsigned long)val, buf, radix, 1);
        else
            xtoa((unsigned long)(unsigned int)val, buf, radix, 0);
        return buf;
}


void PutNumberLeadingZeros(char* b,int v,int n)
{
    nitoa(v,b,10);
    int l=strlen(b);
    if (l<n)
    {
        int z=0;
        for (z=0;z<=l;z++)
        {
            b[n-l+z]=b[z];
        }
        for (z=0;z<(n-l);z++)
        {
            b[z]='0';
        }
        b[n]=0;

    }

}


#ifdef KEYBOARD
int get_pc_keyboard()
{
    int pi;
    int pckeydata=0;
    int kinput=0;
    SDL_Event event;
	while( SDL_PollEvent( &event ) )
		{
                for (pi=0;pi<2;pi++)
                {
                    if ((event.type==SDL_JOYBUTTONUP) && (playercontrols[pi].joy<10))
                    {
                        if (event.jbutton.button==playercontrols[pi].fire) playercontrols[pi].firedown=0;
                        if (event.jbutton.button==playercontrols[pi].jump) playercontrols[pi].jumpdown=0;
                        if (event.jbutton.button==playercontrols[pi].cycle) playercontrols[pi].cycledown=0;
                        if (event.jbutton.button==playercontrols[pi].left) playercontrols[pi].leftdown=0;
                        if (event.jbutton.button==playercontrols[pi].right) playercontrols[pi].rightdown=0;
                    }

                    if ((event.type==SDL_JOYBUTTONDOWN) && (playercontrols[pi].joy<10))
                    {
                        if (event.jbutton.button==playercontrols[pi].fire) playercontrols[pi].firedown=1;
                        if (event.jbutton.button==playercontrols[pi].jump) playercontrols[pi].jumpdown=1;
                        if (event.jbutton.button==playercontrols[pi].cycle) playercontrols[pi].cycledown=1;
                        if (event.jbutton.button==playercontrols[pi].left) playercontrols[pi].leftdown=1;
                        if (event.jbutton.button==playercontrols[pi].right) playercontrols[pi].rightdown=1;
                    }

                    if ((event.type==SDL_KEYUP) && (playercontrols[pi].joy==10))
                    {
                        if (event.key.keysym.sym==playercontrols[pi].fire) playercontrols[pi].firedown=0;
                        if (event.key.keysym.sym==playercontrols[pi].jump) playercontrols[pi].jumpdown=0;
                        if (event.key.keysym.sym==playercontrols[pi].cycle) playercontrols[pi].cycledown=0;
                        if (event.key.keysym.sym==playercontrols[pi].left) playercontrols[pi].leftdown=0;
                        if (event.key.keysym.sym==playercontrols[pi].right) playercontrols[pi].rightdown=0;
                    }

                    if ((event.type==SDL_KEYDOWN) && (playercontrols[pi].joy==10))
                    {
                        kinput=event.key.keysym.sym;
                        if (event.key.keysym.sym==playercontrols[pi].fire) playercontrols[pi].firedown=1;
                        if (event.key.keysym.sym==playercontrols[pi].jump) playercontrols[pi].jumpdown=1;
                        if (event.key.keysym.sym==playercontrols[pi].cycle) playercontrols[pi].cycledown=1;
                        if (event.key.keysym.sym==playercontrols[pi].left) playercontrols[pi].leftdown=1;
                        if (event.key.keysym.sym==playercontrols[pi].right) playercontrols[pi].rightdown=1;
                    }
                }


                if (event.type== SDL_KEYUP)
                {																	// PC buttons
					switch( event.key.keysym.sym )
					{

						case SDLK_UP:
							UPDOWN=0;
							break;
						//case SDLK_HOME:
//							UPDOWN=0;
							//break;
						//case SDLK_RCTRL:
//							UPDOWN=0;
							//break;
						case SDLK_LEFT:
							LEFTDOWN=0;
							break;
						case SDLK_RIGHT:
							RIGHTDOWN=0;
							break;
                        case SDLK_END:
                            BDOWN=0;
                            break;
                        case SDLK_PAGEUP:
                            YDOWN=0;
                            break;
                        case SDLK_HOME:
                            ADOWN=0;
                            break;
                        case SDLK_PAGEDOWN:
                            XDOWN=0;
                            break;
                        case SDLK_l:
                            LSDOWN=0;
							break;
                        case SDLK_DOWN:
							DOWNDOWN=0;
							break;
//                        case SDLK_LCTRL:
//							SELECTDOWN=0;
							//break;
                        case SDLK_s:
                        case SDLK_LALT:
                            STARTDOWN=0;
							break;
                        case SDLK_q:
                            QDOWN=0;
							break;
                        case SDLK_p:
                            PAUSEDOWN=0;
							break;
						default:
							break;
					}
                }

				if (event.type== SDL_KEYDOWN)
				{																	// PC buttons
//printf("%d\n",event.key.keysym.sym);
                    switch( event.key.keysym.sym )
					{

						case SDLK_UP:
							UPDOWN=1;
							break;
						//case SDLK_HOME:
//							UPDOWN=1;
	//						break;
		//				case SDLK_RCTRL:
			//				UPDOWN=1;
				//			break;
						case SDLK_LEFT:
							LEFTDOWN=1;
							break;
						case SDLK_RIGHT:
							RIGHTDOWN=1;
							break;
                        case SDLK_END:
                            BDOWN=1;
                            break;
                        case SDLK_PAGEUP:
                            YDOWN=1;
                            break;
                        case SDLK_HOME:
                            ADOWN=1;
                            break;
                        case SDLK_PAGEDOWN:
                            XDOWN=1;
                            break;

                        case SDLK_l:
                            LSDOWN=1;
							break;
                        case SDLK_DOWN:
							DOWNDOWN=1;
							break;

//                        case SDLK_LCTRL:
	//						SELECTDOWN=1;
		//					break;
                        case SDLK_s:
                        case SDLK_LALT:
                            STARTDOWN=1;
							break;
                        case SDLK_q:
                            QDOWN=1;
							break;
                        case SDLK_p:
                            PAUSEDOWN=1;
							break;
						default:
							break;
					}
				}

				if (event.type==SDL_MOUSEMOTION)
				{
				    mx=event.motion.x;
				    my=event.motion.y;
				}

			}

        playercontrols[0].quitdown=0;
        playercontrols[1].quitdown=0;

		if (UPDOWN) pckeydata|=MY_UP;
		if (LEFTDOWN) pckeydata|=MY_LEFT;
		if (RIGHTDOWN) pckeydata|=MY_RIGHT;
		if (DOWNDOWN) pckeydata|=MY_DOWN;
		if (ADOWN) pckeydata|=MY_BUTT_A;
		if (YDOWN) pckeydata|=MY_BUTT_Y;
		if (BDOWN) pckeydata|=MY_BUTT_B;
		if (XDOWN) pckeydata|=MY_BUTT_X;
		if (LSDOWN) pckeydata|=MY_BUTT_SL;
		if (STARTDOWN) pckeydata|=MY_START;
//		if (SELECTDOWN) pckeydata|=MY_SELECT;
		if (PAUSEDOWN) pckeydata|=MY_PAUSE;
		if (QDOWN)
		{
		    pckeydata|=MY_QT;
		    playercontrols[0].quitdown=1;
		    playercontrols[1].quitdown=1;
		}

		/*if (kinput)
		{
		    int conv=0;
		    //printf("keycode: %d\n",kinput);
		    if (kinput==32) {kinput=1;conv=1;}
            if (kinput==46) {kinput=38;conv=1;}
		    if (kinput==33) {kinput=39;conv=1;}
		    if (kinput==13) {kinput=40;conv=1;}
		    if ((kinput>=97) && (kinput<=122)) {kinput-=95;conv=1;}
		    if ((kinput>=48) && (kinput<=57)) {kinput-=20;conv=1;}
		    if (conv==0) kinput=0;
		}*/

        return pckeydata;
}
#endif


int get_key(int pi)
{
    int keydata=0;
    #ifdef KEYBOARD
    keydata+=get_pc_keyboard();
    #endif

    /*#ifdef JOYSTICK
    if (p[pi].joy<10)
    {
    SDL_JoystickUpdate();
    //if (SDL_JoystickGetButton(joy,GP2X_BUTTON_UP)) printf("Up Pressed\n");
    //if (SDL_JoystickGetButton(joy,GP2X_BUTTON_DOWN)) printf("Down Pressed\n");
    //if (SDL_JoystickGetButton(joy,GP2X_BUTTON_LEFT)) printf("Left Pressed\n");
    //if (SDL_JoystickGetButton(joy,GP2X_BUTTON_RIGHT)) printf("Right Pressed\n");
    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_UP)) keydata|=MY_UP;
    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_DOWN)) keydata|=MY_DOWN;
    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_LEFT)) keydata|=MY_LEFT;
    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_RIGHT)) keydata|=MY_RIGHT;

    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_DOWNRIGHT))
    {
        keydata|=MY_RIGHT;
        keydata|=MY_DOWN;
    }

    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_UPRIGHT))
    {
        keydata|=MY_RIGHT;
        keydata|=MY_UP;
    }
    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_DOWNLEFT))
    {
        keydata|=MY_LEFT;
        keydata|=MY_DOWN;
    }
    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_UPLEFT))
    {
        keydata|=MY_LEFT;
        keydata|=MY_UP;
    }


    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_A)) keydata|=MY_BUTT_A;
    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_B)) keydata|=MY_BUTT_B;
    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_X)) keydata|=MY_BUTT_X;
//    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_Y)) keydata|=MY_BUTT_Y;
    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_L)) keydata|=MY_BUTT_SL;
    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_R)) keydata|=MY_BUTT_SR;

    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_START)) keydata|=MY_START;
//    if (SDL_JoystickGetButton(joy,GP2X_BUTTON_SELECT)) keydata|=MY_SELECT;

    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_VOLUP)) keydata|=MY_VOL_UP;
    if (SDL_JoystickGetButton(joy[p[pi].joy],GP2X_BUTTON_VOLDOWN)) keydata|=MY_VOL_DOWN;
    }
    if ((keydata & MY_VOL_UP) && (keydata & MY_VOL_DOWN)) keydata|=MY_QT;
    #endif
*/
    return keydata;
}


int images_collide(int dx1, int dy1, int dw1, int dh1, SDL_Surface * src1, int sx1, int sy1, int sw1, int sh1, int c1, int dx2, int dy2, int dw2, int dh2, SDL_Surface * src2, int sx2, int sy2, int sw2, int sh2, int c2)
{
	int close;
	close=0;
	int cl,cr,ct,cb,cx,cy;

	//**** Check to see whether the 2 gfx are even close - no point pixel checking if not ****

	if ((dx1 >= dx2-dw1) && (dx1 <= dx2+dw2) && (dy1 >= dy2-dh1) && (dy1 <= dy2+dh2)) close=1;
//	if ((dx2 >= dx1-dw2) && (dx2 <= dx1+dw1) && (dy2 >= dy1-dh2) && (dy2 <= dy1+dh1)) close=1;

	if (close==0) return (close);


	//**** Work out total region that may have a collision within ****
	cl=dx1;
	if (dx2 > dx1) cl=dx2;
	cr=dx1+dw1;
	if (dx2+dw2 < dx1+dw1) cr=dx2+dw2;
	ct=dy1;
	if (dy2 > dy1) ct=dy2;
	cb=dy1+dh1;
	if (dy2+dh2 < dy1+dh1) cb=dy2+dh2;

	c1=SDL_MapRGB(src1->format, (c1 >> 16) & 0xFF, (c1>>8) & 0xFF, c1 & 0xFF);
	c2=SDL_MapRGB(src2->format, (c2 >> 16) & 0xFF, (c2>>8) & 0xFF, c2 & 0xFF);

    Uint8* p1;
    Uint8* p2;

	//**** Go through the collision region and see if both gfx have an unmasked pixel ****
	for (cx=cl;cx<=cr;cx++)
		for (cy=ct;cy<=cb;cy++)
		{
			if ((cx>=dx1) && (cx<dx1+dw1) && (cy>=dy1) && (cy<dy1+dh1))
			{
				p1=(Uint8*)src1->pixels+((cy-dy1)+sy1) * src1->pitch + ((cx-dx1)+sx1) * 2;
                if ((*p1+(*(p1+1)<<8))!=c1)
				{
					if ((cx>=dx2) && (cx<dx2+dw2) && (cy>=dy2) && (cy<dy2+dh2))
					{
                        p2=(Uint8*)src2->pixels+((cy-dy2)+sy2) * src2->pitch + ((cx-dx2)+sx2) * 2;
                        if ((*p2+(*(p2+1)<<8))!=c2) return (1);
					}
				}
			}

		}

	//**** If we've got here then no collision detected so return zero ****
	return(0);
}

void SetPixel ( SDL_Surface* pSurface , int x , int y , SDL_Color color )
{
  //convert color
  Uint32 col = SDL_MapRGB ( pSurface->format , color.r , color.g , color.b ) ;

  //determine position
  char* pPosition = ( char* ) pSurface->pixels ;

  //offset by y
  pPosition += ( pSurface->pitch * y ) ;

  //offset by x
  pPosition += ( pSurface->format->BytesPerPixel * x ) ;

  //copy pixel data
  memcpy ( pPosition , &col , pSurface->format->BytesPerPixel ) ;
}

int images_collide_test(int dx1, int dy1, int dw1, int dh1, SDL_Surface *src1, int sx1, int sy1, int sw1, int sh1, int c1, int dx2, int dy2, int dw2, int dh2, SDL_Surface *src2, int sx2, int sy2, int sw2, int sh2, int c2)
{
	//**** This is a copy of the normal routine but actually plots the pixels it's checking against
	//so it is easy to detect bugs in the routine ****

	int close;
	close=0;
	int cl,cr,ct,cb,cx,cy,ofx1;

	SDL_Color color ;
    color.r = 24 ;
    color.g = 150 ;
    color.b = 220 ;

    SDL_Color color2 ;
    color2.r = 255 ;
    color2.g = 0 ;
    color2.b = 0 ;


	if ((dx1 >= dx2-dw1) && (dx1 <= dx2+dw2) && (dy1 >= dy2-dh1) && (dy1 <= dy2+dh2)) close=1;

	if (close==0) return (close);

	cl=dx1;
	if (dx2 > dx1) cl=dx2;
	cr=dx1+dw1;
	if (dx2+dw2 < dx1+dw1) cr=dx2+dw2;
	ct=dy1;
	if (dy2 > dy1) ct=dy2;
	cb=dy1+dh1;
	if (dy2+dh2 < dy1+dh1) cb=dy2+dh2;

	c1=SDL_MapRGB(src1->format, (c1 >> 16) & 0xFF, (c1>>8) & 0xFF, c1 & 0xFF);
	c2=SDL_MapRGB(src2->format, (c2 >> 16) & 0xFF, (c2>>8) & 0xFF, c2 & 0xFF);

    Uint8* p1;
    Uint8* p2;

	for (cx=cl;cx<=cr;cx++)
		for (cy=ct;cy<=cb;cy++)
		{
			if ((cx>=dx1) && (cx<dx1+dw1) && (cy>=dy1) && (cy<dy1+dh1))
			{
				ofx1=((cy-dy1)+sy1) * src1->pitch + ((cx-dx1)+sx1) * 2;
                p1=(Uint8*)src1->pixels+((cy-dy1)+sy1) * src1->pitch + ((cx-dx1)+sx1) * 2;
              //  if ((*p1+(*(p1+1)<<8))!=c1) SetPixel(screen,cx+100,cy+10,color);
                //printf("col=%x  v  %x\n",(*p1+(*(p1+1)<<8)),c1);
			}
			if ((cx>=dx2) && (cx<dx2+dw2) && (cy>=dy2) && (cy<dy2+dh2))
			{
				p2=(Uint8*)src2->pixels+((cy-dy2)+sy2) * src2->pitch + ((cx-dx2)+sx2) *2;
                if ((*p2+(*(p2+1)<<8))!=c2) SetPixel(screen,cx+100,cy+10,color2);
			}

		}


	return(0);
}



int loadgfx(SDL_Surface** s, char* n)
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

void Text(char* strg,int x,int y)						//Output text using graphics font
{
	int str_length=strlen(strg)-1;
    int scr_print;
	for (scr_print=0;scr_print<=str_length;scr_print++)
        drawSprite(font,screen,(strg[scr_print]-32)*16,0,(scr_print*16)+x,y,16,16);
}

void SmallText(char* strg,int x,int y)						//Output text (numbers only) using small graphics font
{
	int str_length=strlen(strg)-1;
    int scr_print;
	for (scr_print=0;scr_print<=str_length;scr_print++)
        drawSprite(smallfont,screen,(strg[scr_print]-48)*8,0,(scr_print*8)+x,y,8,8);
}

void ClearAllLists()
{
	bomb.ClearList();
	mine.ClearList();
	dynamite.ClearList();
	wheelie.ClearList();
	grog.ClearList();
	slinky.ClearList();
	gem.ClearList();
	coin.ClearList();
	pickedup16.ClearList();
	pickedup32.ClearList();
	reward.ClearList();
	explosion.ClearList();
	smoke.ClearList();
	pickup.ClearList();
}

int CheckScore(int s)
{
	int z=0;
	int beat=0;

	while ((z<100) && (beat==0))
	{
		if (s>hiscores[z]) beat=1;
		if (beat==0) z+=1;
	}

	if (beat)
	{
		for (int i=99;i>z;i--)
		{
			hiscores[i]=hiscores[i-1];
			memcpy(hiname[i],hiname[i-1],5);
			hinew[i]=hinew[i-1];
		}
		return z;
	}
	else
	{
		return 10;
	}

}

void SetHighScore(int p,int s,char* t,int n)
{
    hiscores[p]=s;
    memcpy(hiname[p],t,5);
    hinew[p]=n;
}

void SaveScores()
{
	FILE * f=fopen("Fuzed.hi","w");
	if (!f) return;
	for (int z=0;z<100;z++)
	{
	    fprintf(f,"SCORE:%d NAME: %s\n",hiscores[z],hiname[z]);
	}
	fclose (f);
}

void LoadScores()
{
	FILE * f=fopen("Fuzed.hi","r");
	if (!f) return;
	char buffer[5];
	for (int z=0;z<100;z++)
	{
		fscanf(f,"SCORE:%d NAME: %s\n",&hiscores[z],buffer);
		memcpy(hiname[z],buffer,5);
	}

	fclose (f);
}


void SaveSettings()
{
	FILE * f=fopen("Fuzed.cfg","w");
	if (!f) return;
	fprintf(f,"[FUZED]\n");
	fprintf(f,"\n");
	fprintf(f,"#KEY DEFINITIONS#\n");
    fprintf(f,"[P1 LEFT]=%c\n",playercontrols[0].left);
    fprintf(f,"[P1 RIGHT]=%c\n",playercontrols[0].right);
    fprintf(f,"[P1 JUMP]=%c\n",playercontrols[0].jump);
    fprintf(f,"[P1 FIRE]=%c\n",playercontrols[0].fire);
    fprintf(f,"[P1 CYCLE]=%c\n",playercontrols[0].cycle);
    fprintf(f,"[P2 LEFT]=%c\n",playercontrols[1].left);
    fprintf(f,"[P2 RIGHT]=%c\n",playercontrols[1].right);
    fprintf(f,"[P2 JUMP]=%c\n",playercontrols[1].jump);
    fprintf(f,"[P2 FIRE]=%c\n",playercontrols[1].fire);
    fprintf(f,"[P2 CYCLE]=%c\n",playercontrols[1].cycle);

    fprintf(f,"\n");
    fprintf(f,"[BACKGROUND]=%d\n",background_on);
    fprintf(f,"\n");
    fprintf(f,"[END]\n");

	fclose (f);
}

void LoadSettings()
{
	FILE * f=fopen("Fuzed.cfg","r");
	if (!f) return;
	fscanf(f,"[FUZED]\n");
	fscanf(f,"\n");
	fscanf(f,"#KEY DEFINITIONS#\n");
    fscanf(f,"[P1 LEFT]=%c\n",&playercontrols[0].left);
    fscanf(f,"[P1 RIGHT]=%c\n",&playercontrols[0].right);
    fscanf(f,"[P1 JUMP]=%c\n",&playercontrols[0].jump);
    fscanf(f,"[P1 FIRE]=%c\n",&playercontrols[0].fire);
    fscanf(f,"[P1 CYCLE]=%c\n",&playercontrols[0].cycle);
    fscanf(f,"[P2 LEFT]=%c\n",&playercontrols[1].left);
    fscanf(f,"[P2 RIGHT]=%c\n",&playercontrols[1].right);
    fscanf(f,"[P2 JUMP]=%c\n",&playercontrols[1].jump);
    fscanf(f,"[P2 FIRE]=%c\n",&playercontrols[1].fire);
    fscanf(f,"[P2 CYCLE]=%c\n",&playercontrols[1].cycle);

    fscanf(f,"\n");
    fscanf(f,"[BACKGROUND]=%d\n",&background_on);
    fscanf(f,"\n");
    fscanf(f,"[END]\n");


	fclose (f);
}


int main( int argc, char* args[] )
{

    int err,z;

    for (z=0;z<100;z++)
    {
        hiname[z]=(char*)malloc(5);
        //hiscores[z]=10000-z*100;
        hiscores[z]=0;
        hinew[z]=0;
        if (z&1) strcpy(hiname[z],"ED!"); else  strcpy(hiname[z],"FUZ");
    }


    #ifdef _WIN32
    _setmode( _fileno( stdin ), _O_BINARY );
    _setmode( _fileno( stdout ), _O_BINARY );
    #endif

    // Initialize all SDL subsystems
    err=SDL_Init( SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO );

    if (err ==-1)
    {
        printf(  "SDL Init error: %d\n",  err  );
        return 1;
    }
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF);
    SDL_ShowCursor(false);

    if (Mix_OpenAudio(44100, AUDIO_S16, MIX_DEFAULT_CHANNELS, 1024))
    {
        printf ("SDL Could not open sound hardware\n");
        soundpresent=0;
    }

    srand((int)SDL_GetTicks());

	// Do PC specific stuff
    #ifdef _WIN32
	// Set the window caption
	SDL_WM_SetCaption( "FuZeD", NULL );
    #endif // PLATFORM_PC

    SDL_Surface* intro_screen_scroll;
    SDL_Surface* intro_header;
    SDL_Surface* intro_footer;
    SDL_Surface* intro_icons;
    SDL_Surface* mouse_pointer;


    loadgfx(&intro_screen_scroll,(char*)"graphics/page_background_1280_scroll_wrap.png");
    loadgfx(&intro_header,(char*)"graphics/page_header.png");
    loadgfx(&intro_footer,(char*)"graphics/page_footer.png");

    loadgfx(&intro_icons,(char*)"graphics/intro_icons.png");
    loadgfx(&mouse_pointer,(char*)"graphics/mouse_pointer.png");

    loadgfx(&font,(char*)"graphics/16x16_font.png");
    loadgfx(&smallfont,(char*)"graphics/8x8_font.png");

    loadgfx(&spritemasks,(char*)"graphics/collmask.png");
    loadgfx(&coin_collide,(char*)"graphics/coin_collide.png");

    music = NULL;

    if (soundpresent)
    {

        mine_arm_snd=Mix_LoadWAV("audio/blips4.wav");
        explosion_snd=Mix_LoadWAV("audio/explosion_x.wav");
        collect_snd=Mix_LoadWAV("audio/sound005.wav");
        jump_snd=Mix_LoadWAV("audio/sound008.wav");
        bomb_land_snd=Mix_LoadWAV("audio/hammer.wav");
    }

    playercontrols[0].left=SDLK_LEFT;
    playercontrols[0].right=SDLK_RIGHT;
    playercontrols[0].cycle=SDLK_PAGEUP;
    playercontrols[0].jump=SDLK_PAGEDOWN;
    playercontrols[0].fire=SDLK_END;

    playercontrols[1].left=SDLK_z;
    playercontrols[1].right=SDLK_x;
    playercontrols[1].cycle=SDLK_c;
    playercontrols[1].jump=SDLK_t;
    playercontrols[1].fire=SDLK_f;

    playercontrols[0].joy=10; //keyboard
    playercontrols[1].joy=10;


InitCollisions(15);
level.Init();
wheelie.init();
grog.init();
slinky.init();
bomb.init();
explosion.init();
smoke.init();
mine.init();
dynamite.init();
player.init();
gem.init();
coin.init();
pickup.init();
pickedup16.init();
pickedup32.init();
hiscore.init();
LoadScores();
LoadSettings();
quitgame=0;
//level=new Tlevel;
while (quitgame==0)
{
    int keydata=0;
    int introscroll=0;
    int s1clip=640;
    long ticks=SDL_GetTicks();

    int exit_intro=0;
    int intro_option=0;
    startlevel=0;

    if (soundpresent)
    {
        if (music) Mix_FreeMusic(music);
        music = Mix_LoadMUS("audio/FuZED.title.Main.xm");
        if (!music) printf("SDL could not load music: %s\n" , SDL_GetError());
        if (music) Mix_PlayMusic(music, -1);
    }
    while (exit_intro<2)
    {
        introscroll+=4;
        if (introscroll>=1280) introscroll=0;
        s1clip=640-(introscroll-640);
        if (s1clip>640) s1clip=640;

        drawSprite(intro_screen_scroll, screen, introscroll, 0, 0, 96, s1clip, 352);
        if (s1clip<640) drawSprite(intro_screen_scroll, screen, 0, 0, s1clip, 96, 640-s1clip, 352);

        //drawSprite(intro_screen_scroll, screen, 0, 0, introscroll, 96, 1280, 352);
        //drawSprite(intro_screen_scroll, screen, 0, 0, 1280+introscroll, 96, 1280, 352);

        drawSprite(intro_header, screen, 0, 0, 0, 0, 640, 96);
        drawSprite(intro_footer, screen, 0, 0, 0, 448, 640, 32);

        drawSprite(intro_icons, screen, 0, 0, 32, 160, 96, 96);
        drawSprite(intro_icons, screen, 96, 0, 192, 160, 96, 96);
        drawSprite(intro_icons, screen, 192, 0, 352, 160, 96, 96);
        drawSprite(intro_icons, screen, 288, 0, 512, 160, 96, 96);

        drawSprite(intro_icons, screen, 288+96, 0, 32, 300, 96, 96);
        drawSprite(intro_icons, screen, 288+192, 0, 192, 300, 96, 96);
        drawSprite(intro_icons, screen, 288+288+96, 0, 352, 300, 96, 96);
        drawSprite(intro_icons, screen, 288+288, 0, 512, 300, 96, 96);


        drawSprite(mouse_pointer, screen, 0, 0, mx,my, 19, 31);

        if (images_collide(mx, my, 8, 8, mouse_pointer, 0, 0, 24, 31, 0xFF00FF, 32, 160, 96, 96, intro_icons, 0, 0, 96, 96, 0xFF00FF))
        {
            Text((char*)"SINGLE PLAYER",216,462);
            if (keydata & MY_BUTT_B)
            {
                exit_intro=2;
                intro_option=0;
            }
        }
        if (images_collide(mx, my, 8, 8, mouse_pointer, 0, 0, 24, 31, 0xFF00FF, 192, 160, 96, 96, intro_icons, 0, 0, 96, 96, 0xFF00FF))
        {
            Text((char*)"TWO PLAYER TEAM",200,462);
               if (keydata & MY_BUTT_B)
            {
                exit_intro=2;
                intro_option=1;
            }
        }
        //if (images_collide(mx, my, 8, 8, mouse_pointer, 0, 0, 24, 31, 0xFF00FF, 32, 160, 96, 96, intro_icons, 0, 0, 96, 96, 0xFF00FF)) Text((char*)"SINGLE PLAYER",216,462);
        //if (images_collide(mx, my, 8, 8, mouse_pointer, 0, 0, 24, 31, 0xFF00FF, 32, 160, 96, 96, intro_icons, 0, 0, 96, 96, 0xFF00FF)) Text((char*)"SINGLE PLAYER",216,462);
        if (images_collide(mx, my, 8, 8, mouse_pointer, 0, 0, 24, 31, 0xFF00FF, 512, 300, 96, 96, intro_icons, 0, 0, 96, 96, 0xFF00FF))
        {
            Text((char*)"EXIT GAME",248,462);
               if (keydata & MY_BUTT_B)
            {
                exit_intro=2;
                intro_option=7;
            }
        }


        SDL_Flip(screen);


        keydata=get_key(0);

//        printf("MS %d Millisecs\n",SDL_GetTicks()-ticks);
//        ticks=SDL_GetTicks();
    }

    if (soundpresent) Mix_HaltMusic();

    if ((intro_option>=0) && (intro_option<=3))
    {
		level.Create(startlevel);							//Create level (level id)
		player.spawn(1,level.GetPx(0),level.GetPy(0),2,0,0,3,1);		//Create Player 1
		if (intro_option>0) player.spawn(2,level.GetPx(1),level.GetPy(1),2,0,0,3,0);		//Create Player 2
		if (soundpresent)
		{
		    if (music) Mix_FreeMusic(music);
			if (startlevel<5) music = Mix_LoadMUS("audio/FuZED.lev.Checkers.xm");
			if ((startlevel>=5) && (startlevel<10)) Mix_LoadMUS("audio/FuZED.lev.Ancient.xm");

		}
		level.AddEnemies();

		level.Fadein();
		keydata=0;
        if ((soundpresent) && (music)) Mix_PlayMusic(music, -1);	//Start music playing
		while ((player.CountList()>0) && (QDOWN==0))
		{
		    while ((((player.CountList()>0) && (level.door_appear<4)) || hiscore.CountList()>0) && (QDOWN==0))
		    {
                level.Play();

                SDL_Flip(screen);
                keydata=get_key(0);
		    }
		    if ((player.CountList()>0) && (QDOWN==0))
		    {
				ClearAllLists();
				level.ScrollToNext();
				level.AddEnemies();
                player.InitialPosition();

		    }
		}

		level.Fadeout();
		ClearAllLists();
		player.ClearList();

    }


    if (intro_option==7) quitgame=1;
}
    Mix_FreeMusic(music);
    SaveScores();
    SaveSettings();
    SDL_Quit();

    return 0;

}

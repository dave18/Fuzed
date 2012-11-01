#ifndef FUZED_H
#define FUZED_H


#define GP2X_BUTTON_UP              (0)
#define GP2X_BUTTON_DOWN            (4)
#define GP2X_BUTTON_LEFT            (2)
#define GP2X_BUTTON_RIGHT           (6)
#define GP2X_BUTTON_UPLEFT          (1)
#define GP2X_BUTTON_UPRIGHT         (7)
#define GP2X_BUTTON_DOWNLEFT        (3)
#define GP2X_BUTTON_DOWNRIGHT       (5)
#define GP2X_BUTTON_CLICK           (18)
#define GP2X_BUTTON_A               (12)
#define GP2X_BUTTON_B               (13)
#define GP2X_BUTTON_X               (15)
#define GP2X_BUTTON_Y               (14)
#define GP2X_BUTTON_L               (11)
#define GP2X_BUTTON_R               (10)
#define GP2X_BUTTON_START           (8)
#define GP2X_BUTTON_SELECT          (9)
#define GP2X_BUTTON_VOLUP           (16)
#define GP2X_BUTTON_VOLDOWN         (17)

#define MY_LEFT                     (1)
#define MY_RIGHT                     (2)
#define MY_UP                     (4)
#define MY_DOWN                     (8)
#define MY_BUTT_A                   (16)
#define MY_BUTT_B                   (32)
#define MY_BUTT_X                     (64)
#define MY_BUTT_SL                   (128)
#define MY_BUTT_SR                   (256)
#define MY_START                     (512)
#define MY_QT                       (1024)
#define MY_BUTT_Y                     (2048)
#define MY_PAUSE                     (4096)




typedef unsigned char u8;







int get_key(int pi);
int get_pc_keyboard();
int loadgfx(SDL_Surface**, char*);
int images_collide(int, int, int, int, SDL_Surface *, int, int, int, int, int, int, int, int, int, SDL_Surface *, int, int , int, int, int);
int images_collide_test(int, int, int, int, SDL_Surface *, int, int, int, int, int, int, int, int, int, SDL_Surface *, int, int , int, int, int);
void SetPixel ( SDL_Surface* , int , int , SDL_Color);
char PeekByte(char* mem,int addr);
int PeekInt(char* mem,int addr);
void Text(char* strg,int x,int y);
void SmallText(char* strg,int x,int y);
void PutNumberLeadingZeros(char* b,int v,int n);
int CheckScore(int s);
void SetHighScore(int p,int s,char* t,int n);
void LoadScores(void);
void SaveScores(void);
void LoadSettings(void);
void SaveSettings(void);

void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height)
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

	SDL_BlitSurface(imageSurface, &srcRect, screenSurface, &dstRect);
}


char const ENDCHAR=0;


#endif

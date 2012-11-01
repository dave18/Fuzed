#ifndef SLINKY_H
#define SLINKY_H

//slinky class



class Tslinky
{

    private:

    char PeekByte(char* mem,int addr);
    int PeekInt(char* mem,int addr);
    int create(void);
    int remove(int id);
    void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id);
    int loadgfx(SDL_Surface** s, char* n);



    public:
    Tslinky();
        struct class_struct
        {
        int id;
        int x,y,dx,fall,value,hitbombtimer,hitwalltimer,oldy;
        int startframe,endframe,currframe,framecount,framecounttotal,fallframe;
        int explode;
        int dietime,dieflash;
        int isjumping,jumplevel,jumpdir,jumpsegs;
        float fatness,zx,zy;
        SDL_Surface* image;
        };

    private:
        class_struct * item_store;
        int item_count;

        SDL_Surface* slinky_all;
        SDL_Surface* slinky_fall;
        SDL_Surface* wheelie_die_left;
        SDL_Surface* wheelie_die_right;

        void Update2(class_struct* is);
        void SetScale(float xscale,float yscale,int id);
        int SnaptoGrid16(int v);


    public:

    void init(void);
    int spawn(int px,int py,int dir);
    void Appear(float s);
    void Update();
    void Draw();
    void ClearList(void);
    int CountList(void);

};

#endif



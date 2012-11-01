#ifndef GROG_H
#define GROG_H

//grog class



class Tgrog
{

    private:

    char PeekByte(char* mem,int addr);
    int PeekInt(char* mem,int addr);
    int create(void);
    int remove(int id);
    void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id);
    int loadgfx(SDL_Surface** s, char* n);



    public:
    Tgrog();
        struct class_struct
        {
        int id;
        int x,y,dx,fall,value,hitbombtimer,hitwalltimer;
        int startframe,endframe,currframe,framecount,framecounttotal;
        int explode;
        int dietime,dieflash;
        int isjumping,jumplevel,jumptimer,jumpdir,jumpface,tryjumpagain;
        float fatness,zx,zy;
        SDL_Surface* image;
        };

    private:
        class_struct * item_store;
        int item_count;

        SDL_Surface* grog_left;
        SDL_Surface* grog_right;
        SDL_Surface* grog_die_left;
        SDL_Surface* grog_die_right;

        void Update2(class_struct* is);
        void SetScale(float xscale,float yscale,int id);


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



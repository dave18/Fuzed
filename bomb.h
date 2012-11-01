#ifndef BOMB_H
#define BOMB_H

//bomb class



class Tbomb
{

    private:

    char PeekByte(char* mem,int addr);
    int PeekInt(char* mem,int addr);
    int create(void);
    int remove(int id);
    void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id);
    int loadgfx(SDL_Surface** s, char* n);
    int channel;



    public:
    Tbomb();
        struct class_struct
        {
        int id;
        int t,startframe,endframe,currframe,framecount,framecounttotal;
        int belongsto,fall,falltime;
        float x,y,dx,dy,grip;
        float zx,zy;
        int kicked;
        SDL_Surface* image;
        };

    private:
        class_struct * item_store;
        int item_count;

        SDL_Surface* bomb_floor;

        void Update2(class_struct* is);
        void SetScale(float xscale,float yscale,int id);


    public:

    void init(void);
    int spawn(float px,float py,float pdx,float g,int time,int sframe,int eframe,int acount,int bel); //Create new bomb);
    void Update();
    void Draw();
    int GetPlayerID(int b);
    int GetKicked(int b);
    void SetKicked(int b);
    void ClearKicked(int b);
    void SetDX(int b,float d);
    void SetDY(int b,float d);
    void ClearList(void);
    void Destroy(int id);

};

#endif



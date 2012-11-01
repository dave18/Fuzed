#ifndef MINE_H
#define MINE_H

//mine class



class Tmine
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
    Tmine();
        struct class_struct
        {
        int id;
        int startframe,endframe,currframe,framecount,framecounttotal;
        int belongsto,time;
        int x,y;
        SDL_Surface* image;
        float zx,zy;
        };

    private:
        class_struct * item_store;
        int item_count;

        SDL_Surface* mine_floor;

        void Update2(class_struct* is);
        void SetScale(float xscale,float yscale,int id);


    public:

    void init(void);
    int spawn(int mx,int my,int sframe,int eframe,int acount,int bel); //Create new mine);
    void Update();
    void Draw();
    int GetPlayerID(int b);
    int GetTime(int b);
    int GetX(int b);
    int GetY(int b);
    void Destroy(int id);
    void ClearList(void);

};


#endif




#ifndef DYNAMITE_H
#define DYNAMITEA_H

//dynamite class



class Tdynamite
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
    Tdynamite();
        struct class_struct
        {
        int id;
        int startframe,endframe,currframe,framecount,framecounttotal;
        int belongsto;
        int x,y;
        SDL_Surface* image;
        float zx,zy;
        };

    private:
        class_struct * item_store;
        int item_count;

        SDL_Surface* dynamite_floor;

        void Update2(class_struct* is);
        void SetScale(float xscale,float yscale,int id);


    public:

    void init(void);
    int spawn(int mx,int my,int sframe,int eframe,int acount,int bel); //Create new dynamite);
    void Update();
    void Draw();
    int GetPlayerID(int b);
    void Destroy(int id);
    int Find(int id);
    int GetX(int b);
    int GetY(int b);
    void ClearList(void);

};


#endif





#ifndef EXPLOSION_H
#define EXPLOSION_H

//bomb explosion



class Texplosion
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
    Texplosion();
        struct class_struct
        {
        int id;
        int currframe,framecount,framecounttotal;
        int y,x,belongsto;
        float zx,zy;
        SDL_Surface* image;
        };

    private:
        class_struct * item_store;
        int item_count;

        SDL_Surface* explosion_gfx;

        void Update2(class_struct* is);
        void SetScale(float xscale,float yscale,int id);


    public:

    void init(void);
    int spawn(int ex, int ey, int b); //Create new bomb);
    void Update();
    void Draw();
    int GetPlayerID(int b);
    void ClearList(void);

};

#endif




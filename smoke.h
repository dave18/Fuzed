#ifndef SMOKE_H
#define SMOKE_H

//smoke class



class Tsmoke
{

    private:

    int create(void);
    int remove(int id);
    void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id);
    int loadgfx(SDL_Surface** s, char* n);


    public:
    Tsmoke();
        struct class_struct
        {
        int id;
        int currframe,framecount,framecounttotal,time,alpha;
        float y,x,dx;
        float zx,zy;
        SDL_Surface* image;
        };

    private:
        class_struct * item_store;
        int item_count;

        SDL_Surface* smoke_16;

        void Update2(class_struct* is);
        void SetScale(float xscale,float yscale,int id);



    public:

    void init(void);
    int Spawn(float sx,float sy,int f,float sdx);
    void Update();
    void Draw();
    void ClearList(void);

};


#endif






#ifndef PICKUP_H
#define PICKUP_H

//pickup class



class Tpickup
{

    private:

    int create(void);
    int remove(int id);
    void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id);
    int loadgfx(SDL_Surface** s, char* n);


    public:
    Tpickup();
        struct class_struct
        {
        int id;
        int x,y,fall,falltime,existfor,value;
        int currframe,framecount,framecounttotal;
        float zx,zy;
        SDL_Surface* image;
        };

    private:
        class_struct * item_store;
        int item_count;

        SDL_Surface* invincibility_pickup;
        SDL_Surface* coin_collide;

        void Update2(class_struct* is);
        void SetScale(float xscale,float yscale,int id);



    public:

    void init(void);
    int spawn(int px,int py, int acount, int t, int ft); //Create new pickup);
    void Update();
    void Draw();
    int GetValue(int id);
    int GetX(int id);
    int GetY(int id);
    void Destroy(int id);
    void ClearList(void);

};


#endif






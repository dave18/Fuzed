#ifndef GEM_H
#define GEM_H

//gem class



class Tgem
{

    private:

    int create(void);
    int remove(int id);
    void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id);
    int loadgfx(SDL_Surface** s, char* n);


    public:
    Tgem();
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

        SDL_Surface* gem_10_points;
        SDL_Surface* gem_20_points;
        SDL_Surface* gem_40_points;
        SDL_Surface* gem_80_points;
        SDL_Surface* gem_100_points;
        SDL_Surface* gem_150_points;
        SDL_Surface* gem_collide;

        void Update2(class_struct* is);
        void SetScale(float xscale,float yscale,int id);



    public:

    void init(void);
    int spawn(int px,int py, int acount, int t, int ft); //Create new gem);
    void Update();
    void Draw();
    int GetValue(int id);
    int GetX(int id);
    int GetY(int id);
    void Destroy(int id);
    void ClearList(void);

};


#endif






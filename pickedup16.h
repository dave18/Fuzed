#ifndef PICKEDUP16_H
#define PICKEDUP16_H

//pickedup16 class



class Tpickedup16
{

    private:

    char PeekByte(char* mem,int addr);
    int PeekInt(char* mem,int addr);
    int create(void);
    int remove(int id);
    void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id);
    int loadgfx(SDL_Surface** s, char* n);



    public:
    Tpickedup16();
        struct class_struct
        {
        int id;
        int currframe,framecount;
        int x,y;
        SDL_Surface* image;
        float zx,zy;
        };

    private:
        class_struct * item_store;
        int item_count;

        SDL_Surface* picked_up_16x16;

        void Update2(class_struct* is);
        void SetScale(float xscale,float yscale,int id);


    public:

    void init(void);
    int spawn(int x, int y); //Create new pickedup16);
    void Update();
    void Draw();
    void Destroy(int id);
    void ClearList(void);

};


#endif





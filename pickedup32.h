#ifndef PICKEDUP32_H
#define PICKEDUP32_H

//pickedup32 class



class Tpickedup32
{

    private:

    char PeekByte(char* mem,int addr);
    int PeekInt(char* mem,int addr);
    int create(void);
    int remove(int id);
    void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id);
    int loadgfx(SDL_Surface** s, char* n);



    public:
    Tpickedup32();
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

        SDL_Surface* picked_up_32x32;

        void Update2(class_struct* is);
        void SetScale(float xscale,float yscale,int id);


    public:

    void init(void);
    int spawn(int x, int y); //Create new pickedup32);
    void Update();
    void Draw();
    void Destroy(int id);
    void ClearList(void);

};


#endif





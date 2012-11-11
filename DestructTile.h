#ifndef DESTRUCTTILE_H
#define DESTRUCTTILE_H



class TDestructTile
{

    private:

    char PeekByte(char* mem,int addr);
    int PeekInt(char* mem,int addr);
    int create(void);
    int remove(int id);

    public:
    TDestructTile();
        struct class_struct
        {
        int id;
        int x,y;
        int tile,replacewith;
        char replaceunder;
        int replaceunderwith;

        };

    private:
        class_struct * item_store;
        int item_count;
        int getTileX(SDL_Surface* imageSurface, int t);
        int getTileY(SDL_Surface* imageSurface, int t);
        void Update2(class_struct* is);


    public:

    int spawn(int x,int y,int tile);
    void AddToCollisionLayer(void);
    void Destroy(int id);
    int CountList(void);
    void ClearList(void);
    void Update(void);

};



#endif

#ifndef LEVEL_H
#define LEVEL_H


//level class



class Tlevel
{

    private:

    char PeekByte(char* mem,int addr);
    int PeekInt(char* mem,int addr);
    void drawTile(SDL_Surface* image,int x,int y,int tile);
    void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height);
    int loadgfx(SDL_Surface** s, char* n);
    int getTileX(SDL_Surface* imageSurface, int t);
    int getTileY(SDL_Surface* imageSurface, int t);
    int calctime(int t);

    public:
    Tlevel ();
    int mainscroll,mainscroll2,id;
    SDL_Surface* backdrop;
    SDL_Surface* middrop;
    int p1x,p1y,p2x,p2y;
    int num_enemy,is_complete,num_teleport;
    int playersin,door_appear,door_x,door_y,door_timer,door_frame; //(heh)
    int LEVEL_SIZE;
    int NUMBER_LEVELS;

    SDL_Surface* backscreen0;
    SDL_Surface* backscreen1;
    SDL_Surface* exitpost_appear;
    SDL_Surface* exitpost_back;
    SDL_Surface* exitpost_overlay;
    SDL_Surface* exitpost_collision;
    SDL_Surface* exitpost_disappear;


    char* levelbank;


    void Init(void);
    void Create(int l);
    void AddEnemies(void);
    void Fadein(void);
    void Fadeout(void);
    void Play(void);
    int GetPx(int i);
    int GetPy(int i);
    int GetDoorX(void);
    int GetDoorY(void);
    void ScrollToNext();
    void IncPlayersIn(void);


};

#endif

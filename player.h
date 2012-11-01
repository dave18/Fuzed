#ifndef PLAYER_H
#define PLAYER_H

//player class

#ifndef FUZED_H
#define FUZED_H


#define MY_LEFT                     (1)
#define MY_RIGHT                     (2)
#define MY_UP                     (4)
#define MY_DOWN                     (8)
#define MY_BUTT_A                   (16)
#define MY_BUTT_B                   (32)
#define MY_BUTT_X                     (64)
#define MY_BUTT_SL                   (128)
#define MY_BUTT_SR                   (256)
#define MY_START                     (512)
#define MY_QT                       (1024)
#define MY_BUTT_Y                     (2048)
#define MY_PAUSE                     (4096)

#endif


class Tplayer
{

    private:

    char PeekByte(char* mem,int addr);
    int PeekInt(char* mem,int addr);
    int create(int i);
    int remove(int id);
    void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height,int id);
    int loadgfx(SDL_Surface** s, char* n);


    int channel;



    public:
    Tplayer();
        struct class_struct
        {
        int id;
        float x,y,topspeed;
        int jump,lives,score,gems,coins,isdead,invincible,invflipflop;
        float jumpspeed,speed,grip,skid,zoom;
        int canland,fall,canplacebomb,haslanded,onground,indoor;
        int currframe,animcount;
        int walkleft,walkright,facingdir,blink,blinkframe;
        int placingbomb,firepressed,weapon,num_mine,num_tnt,tnt_laid,cycledown,triggermine;
        int weaponfade;
        float zx,zy;
        SDL_Surface* image;
        };

    private:
        class_struct * item_store;
        int item_count;

        SDL_Surface* snipe_stand_left;
        SDL_Surface* snipe_stand_right;
        SDL_Surface* snipe_run_left;
        SDL_Surface* snipe_run_right;
        SDL_Surface* snipe_skid_halt_left;
        SDL_Surface* snipe_skid_halt_right;
        SDL_Surface* snipe_turn_left_to_right;
        SDL_Surface* snipe_turn_right_to_left;
        SDL_Surface* snipe_blink_facing_left;
        SDL_Surface* snipe_blink_facing_right;
        SDL_Surface* snipe_facing_left_drop_bomb;
        SDL_Surface* snipe_facing_right_drop_bomb;
        SDL_Surface* snipe_facing_left_drop_mine;
        SDL_Surface* snipe_facing_right_drop_mine;
        SDL_Surface* snipe_facing_left_drop_dynamite;
        SDL_Surface* snipe_facing_right_drop_dynamite;
        SDL_Surface* snipe_trigger_dynamite_left;
        SDL_Surface* snipe_trigger_dynamite_right;
        SDL_Surface* snipe_jump_left;
        SDL_Surface* snipe_jump_right;
        SDL_Surface* snipe_die;


        SDL_Surface* gripe_stand_left;
        SDL_Surface* gripe_stand_right;
        SDL_Surface* gripe_run_left;
        SDL_Surface* gripe_run_right;
        SDL_Surface* gripe_skid_halt_left;
        SDL_Surface* gripe_skid_halt_right;
        SDL_Surface* gripe_turn_left_to_right;
        SDL_Surface* gripe_turn_right_to_left;
        SDL_Surface* gripe_blink_facing_left;
        SDL_Surface* gripe_blink_facing_right;
        SDL_Surface* gripe_facing_left_drop_bomb;
        SDL_Surface* gripe_facing_right_drop_bomb;
        SDL_Surface* gripe_facing_left_drop_mine;
        SDL_Surface* gripe_facing_right_drop_mine;
        SDL_Surface* gripe_facing_left_drop_dynamite;
        SDL_Surface* gripe_facing_right_drop_dynamite;
        SDL_Surface* gripe_trigger_dynamite_left;
        SDL_Surface* gripe_trigger_dynamite_right;
        SDL_Surface* gripe_jump_left;
        SDL_Surface* gripe_jump_right;
        SDL_Surface* gripe_die;

        SDL_Surface* icons;


        void Update2(class_struct* is);
        void SetScale(float xscale,float yscale,int id);


    public:

    void init(void);
    int spawn(int i,int px,int py,int s,int j,int b,int l,int fd);
    void Appear(float s);
    void Update(void);
    void CheckExit(void);
    void Draw(void);
    void ShowStats(void);
    void ClearList(void);
    int CountList(void);
    void InitialPosition(void);
    void AddScore(int v,int id);

};

#endif



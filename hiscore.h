#ifndef HISCORE_H
#define HISCORE_H

//hiscore class

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

#endif



class Thiscore
{

    private:

    char PeekByte(char* mem,int addr);
    int PeekInt(char* mem,int addr);
    int create(void);
    int remove(int id);
    struct class_struct
    {
    int id;
    int player;
	int score,position;
	int entrytime;
	char hstring[5];
	int alpha,adir;
	int blink,blinkcount;
	int currletter,entrycount;
    };





    public:
    Thiscore();

    private:
        class_struct * item_store;
        int item_count;


        void Update2(class_struct* is);


    public:

    void init(void);
    int Spawn(int i,int s,int p);
    void Update();
    void Draw();
    void ClearList(void);
    int CountList(void);

};

#endif




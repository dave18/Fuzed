#ifndef REWARD_H
#define REWARD_H

//reward class



class Treward
{

    private:

    int create(void);
    int remove(int id);


    public:
    Treward();
        struct class_struct
        {
        int id;
        int x,value;
        float y;
        int alpha;
        };

    private:
        class_struct * item_store;
        int item_count;

        void Update2(class_struct* is);



    public:

    void init(void);
    int spawn(int rx,float ry,int v); //Create new reward);
    void Update();
    void Draw();
    void ClearList(void);

};


#endif





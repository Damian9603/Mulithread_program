#include <mutex>

class Ball
{
private:
    int id;
    int x;                          //współrzędna kuli
    int y;                          //współrzędna kuli
    int directionX;                 //kierunek poruszania się kuli
    int directionY;                 //kierunek poruszania się kuli
    int hp;                         //aktualne hp kuli
    std::mutex mutex;               //mutex
    bool dead;
    time_t born;
    int lastHit;
public:
    Ball();
    Ball (int a, int b);
    ~Ball();
    void go();                      //metoda poruszania się kuli
    int getHp();
    void startAccess();             //metoda rozpoczynająca dostęp do kuli
    void endAccess();               //metoda kończąca dostęp do kuli
    int getX();
    int getY();
    void setDirectionX(int a);
    void setDirectionY(int b);
    bool checkDead();
    bool getDead();
    void setHP(int h);
    void setBorn(time_t t);
    void eat(int idx);
    int getLastHit();
    void setID(int d);
    void eaten();
};


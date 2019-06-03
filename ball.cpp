#include <unistd.h>
#include <math.h>

#include "ball.hpp"
#include "constants.hpp"

Ball::Ball()
{
    x=STARTX;
    y=STARTY;
    hp=0;
    mutex.unlock();
    dead = true;
    time(&born);
}

Ball::Ball(int a, int b)
{
    
    directionX=a;
    directionY=b;
    
}

Ball::~Ball()
{
}

void Ball::go()
{
    int way=abs(directionX)+abs(directionY);            //obliczanie sumarycznej drogi, którą kula ma przebyć podczas jednego wywołania metody
    if(way==0)
    {
        directionX++;                                   //zabezpiecznie przed dzieleniem przez 0 i staniem kuli w miejscu
        way++;
    }
    int time=100000;                                    //czas na wykonanie ruchu
    int speed=time/way;                                 //czas na poruszenie się o jedno pole
    int wayX=directionX;                                //ilosc pol do poruszenia się w x
    int wayY=directionY;                                //ilosc pol do poruszenia się w y
    while ((wayX!=0||wayY!=0)) //&& dead==false)             //dopóki kula nie wykonala wszystkich ruchów
    {
        if(x==CONSOLE_SIZE_X)                           //jeżeli natrafi na krawędź konsoli to odbija się od niej
        {
            wayX*=(-1);
            directionX*=(-1);
            x--;
        }
        if(y==CONSOLE_SIZE_Y)
        {
            wayY*=(-1);
            directionY*=(-1);
            y--;
        }
        if(x==0)
        {
            wayX*=(-1);
            directionX*=(-1);
            x++;
        }
        if(y==0)
        {
             wayY*=(-1);
            directionY*=(-1);
            y++;
        }

                                                        
        if(wayX>0)                                      //jeżeli droga do przejścia w x jest większa od zera to kula porusza się w dół
        {                                               //i zmniejsza zmienną ruchu, która dąży do 0
            x++;              
            wayX--;
            usleep(speed);
        }
        else if(wayX<0)                                 //jeżeli droga do przejścia w x jest mniejsza od zera to kula porusza się w górę
        {                                               //i zwiększa zmienną ruchu, która dąży do 0
            x--;
            wayX++;
            usleep(speed);
        }
        if(wayY>0)                                      //jeżeli droga do przejścia w y jest większa od zera to kula porusza się w prawo
        {                                               //i zmniejsza zmienną ruchu, która dąży do 0
            y++;
            wayY--;
            usleep(speed);
        }
        else if(wayY<0)                                 //jeżeli droga do przejścia w x jest większa od zera to kula porusza się w lewo
        {                                               //i zwiększa zmienną ruchu, która dąży do 0
            y--;
            wayY++;
            usleep(speed);
        }  
    }
}
 

int Ball::getHp()
{
    return hp;
}

void Ball::startAccess()
{
    mutex.lock();                                       //zablokowanie mutexu (żaden inny proces nie ma dostępu do tego obiektu)
}
void Ball::endAccess()
{
    mutex.unlock();                                     //odblokowanie mutexu
}

int Ball::getX()
{
    return x;
}

int Ball::getY()
{
    return y;
}

void Ball::setDirectionX(int a)
{
    directionX=a;
}

void Ball::setDirectionY(int b)
{
    directionY=b;
    dead=false;
    x=STARTX;
    y=STARTY;
}

bool Ball::checkDead()
{
    //jezeli hp jest wieksze od 3 to kula nie żyje
    if(hp>3)
    {
        dead = true;
        return true;
        
    }
    else
    {
        //sprawdzenie czy kula nie umarla ze starosci
        time_t now;
        time(&now);
        if(now-born>LIFETIME)
        {
            dead=true;
            hp=4;
            return true;
        }
    }
    return false;
    
}

bool Ball::getDead()
{
    return dead;
}

void Ball::setHP(int h)
{
    hp=h;
}

void Ball::setBorn(time_t t)
{
    born=t;
}

void Ball::eat(int idx)
{
    //ustawienie hp
    hp++;
}

void Ball::setID(int d)
{
    id=d;
}

void Ball::eaten()
{
    //ustawienie hp na hp kuli zjedzonej
    hp=EATEN;
    //wyrzucenie kuli poza terminal
    x=id;
    y=-1;
    //ustawienie, ze kuli nie zyje
    dead=true;
}
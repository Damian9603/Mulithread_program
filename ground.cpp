#include <cstdlib>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <algorithm>

#include "ground.hpp"
#include "constants.hpp"

bool endFlag=false;
bool firstFlag=true;

//funkcja realizujaca proces kulek
void ballBehavior(Ball * ball)
{
    ball->setHP(1);
    while(ball->getDead()!=true && endFlag==false)
    {
        ball->go();
        ball->checkDead();
    }
}

Ground::Ground(int num)
{
    srand(time(0));
    number=num;
    balls = new Ball[number];       //utworzenie kul

}

Ground::~Ground()
{   

}

void Ground::Start()
{
    
    initscr();
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair( 3, COLOR_RED, COLOR_BLACK );
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    
    curs_set(0);
    //utworzenie wątków kul
    while (endFlag!=true)
    {
        for (int i=0;i <number;i++)
        {
            if(balls[i].getDead()==true)
            {
                setBall(i);
                Ball * ball;
                ball = &(balls[i]);
                ballThreads.push_back(std::thread(ballBehavior, ball));
                time_t t;
                time(&t);
                balls[i].setBorn(t);
            }
            //rysowanie stanu kul
            DrawBalls(20);
        }
        firstFlag=false;
    }
    //zakończenie wątków kul
    for(int i=1;i<number;i++)
    {
        if(ballThreads[i].joinable()==true)
        {
            ballThreads[i].join();
        }
        
    }

    endwin();
}

void Ground::DrawBalls(int times)
{
    int j=0;
    auto start = std::chrono::high_resolution_clock::now();
    while(j<times)
    {
        auto check = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(check - start).count();
        if(duration>49)
        {
            //usleep(50000);          //opóźnienie pomocne w wyświetlaniu
            kbhit();                //sprawdzanie czy nie został wciśniety żaden klawisz
            clear();                //czyszczenie całego okna(aby usunąć stare pozycje kulek)
            //wyswietlanie granicy
            /*for(int i=0; i<CONSOLE_SIZE_X;i++)
            {
            mvprintw(i,CONSOLE_SIZE_Y, "|");
            }
            for(int i=0; i<CONSOLE_SIZE_Y;i++)
            {
            mvprintw(CONSOLE_SIZE_X,i, "-");
            }*/

            for(int i=0; i<number; i++) //wyświetlanie pozycji kolejnych kul
            {
                int color=i%4+1;
                attron( COLOR_PAIR( color ) );  //ustawienie koloru kuli
                balls[i].startAccess();         //rozpoczęcie dostępu do kuli (mutex)
                if(balls[i].getDead()==true && firstFlag==false)
                {
                    balls[i].endAccess(); 
                    if(ballThreads[i].joinable())
                    {
                        ballThreads[i].join();
                    }
                    continue;
                    
                    
                }
                int x=balls[i].getX();          //pobranie pozycji kuli
                int y=balls[i].getY();
                //wyświetlanie miejsca kulki
                //move(0,0);
                //printw("%d, %d",x,y);
                if(balls[i].getHp()==3)         //wyrysowanie odpowiedniego znaku zależnie od hp kuli
                {
                    mvprintw(x,y,THREE_HP);
                    refresh();
                }
                else if (balls[i].getHp()==2)
                {
                    mvprintw(x,y,TWO_HP);
                    refresh();
                }
                else if (balls[i].getHp()==1)
                {
                    mvprintw(x,y,ONE_HP);
                    refresh();
                }
                else
                {
                    mvprintw(x,y,DEAD);
                    refresh();
                }
                
                balls[i].endAccess();           //odblokowanie mutexa kuli
                attroff( COLOR_PAIR( color ) ); //wyłączenie koloru
            }
            j++;
        }
        
        checkHit();
    }
}


//funkcja sprawdzająca czy nie został naciśniety żaden klawisz (ze stacka)
void Ground::kbhit()
{
    int w;
    ioctl(0, 0x541B, &w);
    if( w > 0)
    {
        endFlag=true;
    }
}

void Ground::setBall(int num)
{
        usleep(100);
        //int x=rand()%10;            //i nadanie kuli losowego kierunku ruchu
        //int y=20 -rand()%41;
        int x=rand()%4;
        int y=4-rand()%9;
        //int x=1;
        //int y=2;
        balls[num].setDirectionX(x);
        balls[num].setDirectionY(y);
        balls[num].setID(num);
}

void Ground::checkHit()
{
    for(int i=0; i<number; i++)
    {
        if(balls[i].getDead()==false)
        {
            balls[i].startAccess();
            int iX=balls[i].getX();
            int iY=balls[i].getY();
            int iHP=balls[i].getHp();
            int iLastHit=balls[i].getLastHit();
            balls[i].endAccess();
            for (int j = i+1; j < number; j++)
            {
                if(balls[i].getDead()==false)
                {
                    balls[j].startAccess();
                    int jX=balls[j].getX();
                    int jY=balls[j].getY();
                    int jHP=balls[j].getHp();
                    int jLastHit=balls[j].getLastHit();
                    balls[j].endAccess();
                    if(iX==jX && iY==jY && iLastHit!=j && jLastHit!=i)
                    {
                        if(iHP<jHP)
                        {
                            balls[j].startAccess();
                            balls[j].eat(i);
                            balls[j].endAccess();
                            balls[i].startAccess();
                            balls[i].eaten();
                            balls[i].endAccess();
                        }
                        else
                        {
                            balls[i].startAccess();
                            balls[i].eat(j);
                            balls[i].endAccess();
                            balls[j].startAccess();
                            balls[j].eaten();
                            balls[j].endAccess();
                        }
                        
                        
                    }
                }
            }
        }
    }
}

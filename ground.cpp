#include <cstdlib>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <algorithm>

#include "ground.hpp"
#include "constants.hpp"

//flaga sygnalizujaca pierwsze wykonanie petli tworzacej watki
bool firstFlag=true;

//funkcja realizujaca proces kulek
void ballBehavior(Ball * ball)
{
    ball->setHP(1);
    while(ball->getDead()!=true)
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
    //inicjalizacja okna konsoli i stworzenie kilku podstawowych kolorow
    initscr();
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair( 3, COLOR_RED, COLOR_BLACK );
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    ballThreads.resize(number);
    curs_set(0);
    //utworzenie wątków kul
    while (true)
    {
        for (int i=0;i <number;i++)
        {
            //jezeli kula "umarla"
            if(balls[i].getDead()==true)
            {
                setBall(i);                                         //ustawienie wartosci poczatkowej kuli
                Ball * ball;
                ball = &(balls[i]);
                ballThreads[i]=(std::thread(ballBehavior, ball));   //stworzenie watku kuli
                time_t t;
                time(&t);
                balls[i].setBorn(t);                                //ustawienie poczatku zycia kuli
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
    //zakonczenie okna konsoli
    endwin();
}

//argument - ilosc powtorzen petli
void Ground::DrawBalls(int times)
{
    int j=0;
    auto start = std::chrono::high_resolution_clock::now(); //poczatek odmierzania czasu
    while(j<times)
    {
        auto check = std::chrono::high_resolution_clock::now(); //sprawdzanie czasu
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(check - start).count();   //ilosc milisekund od poczatku do punktu, w ktorym sprawdzany byl czas
        if(duration>49)
        {
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
                if(balls[i].getDead()==true && firstFlag==false)    //sprawdzanie czy kula "umarla", jak tak to konczenie watku
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
        //sprawdzanie czy wystapilo zderzenie kul 
        checkHit();
    }
}



void Ground::setBall(int num)
{
    usleep(100);                //odstęp czasowy
    int x=rand()%4;             //nadanie kulom losowego kierunku
    int y=4-rand()%9;
    balls[num].setDirectionX(x);
    balls[num].setDirectionY(y);
    balls[num].setID(num);
}

void Ground::checkHit()
{
    for(int i=0; i<number; i++)
    {
        //jezeli kula i nie jest "martwa"
        if(balls[i].getDead()==false)
        {
            //pobranie wpolrzednych kuli i, jej hp
            balls[i].startAccess();
            int iX=balls[i].getX();
            int iY=balls[i].getY();
            int iHP=balls[i].getHp();
            balls[i].endAccess();
            for (int j = i+1; j < number; j++)
            {
                //jezeli kula j nie jest martwa
                if(balls[i].getDead()==false)
                {
                    //pobranie wpolrzednych kuli j, jej hp
                    balls[j].startAccess();
                    int jX=balls[j].getX();
                    int jY=balls[j].getY();
                    int jHP=balls[j].getHp();
                    balls[j].endAccess();
                    //sprawdzanie czy kule maja takie same wspolrzedne
                    if(iX==jX && iY==jY)
                    {
                        //jezeli kula j ma wiecej hp to zjada kule i
                        if(iHP<jHP)
                        {
                            balls[j].startAccess();
                            balls[j].eat(i);
                            balls[j].endAccess();
                            balls[i].startAccess();
                            balls[i].eaten();
                            balls[i].endAccess();
                        }
                        //jezeli kula i ma wiecej lub tyle samo hp to zjada kule j
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

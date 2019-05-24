#include <vector>
#include <thread>
#include <chrono>
#include "ball.hpp"
class Ground
{
private:
    Ball * balls;                           //tablica kulek
    std::vector <std::thread> ballThreads;  //tablica procesu kulek
    int number;                             //ilosc kulek
public:
    Ground(int num);
    ~Ground();
    void Start();                           //rozpoczecie dzialania
    void DrawBalls(int times=INT32_MAX);    //rysowanie kulek
    void kbhit();                           //funkcja sprawdzajaca czy nacisniety zostal jakikolwiek klawisz
    void setBall(int num);
    void checkHit();
};



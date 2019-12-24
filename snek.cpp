#include <iostream>
#include <vector>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <queue>
#include <thread>
#include <X11/Xlib.h>
#include <mutex>
#include <time.h>

using namespace std;
using namespace sf;

mutex mx1;
queue <int> q1;
int end;
class kalakar;

class draw_list
{
    public:
    vector < pair <sf::Drawable* , string> > rlist;
    draw_list()
    {
        rlist.reserve(100);
    }
};

class loader
{
    public:
    sf::Texture texture_snake;
    sf::RectangleShape rect1;
    sf::Sprite snake1;
    sf::Font fnt1;
    sf::Text likh1,likh2;
    RenderWindow *drish;
    sf::CircleShape fd1;
    int render_mode = 0;

    vector <sf::RectangleShape> snek;
    vector <sf::Sprite> melt;

    loader(string lvl)
    {
        drish = new RenderWindow(VideoMode(768,480), "~SNEK~", sf::Style::Close);
        snek.reserve(100);
        melt.reserve(500);
        fd1.setRadius(5);
        fd1.setFillColor(Color::Blue);
    }

    void flush()
    {
        snek.clear();
        melt.clear();
        snek.reserve(100);
        melt.reserve(500);
        render_mode = 0;
    }

};

class kalakar
{
    public:
    loader *ld;
    draw_list *d;
    int vectX = 12;
    int vectY = 0;
    int gm_ov = 0;
    int mlt_on = 0;
    int count = 0;
    int score = 0;
    int repeat = 0;
    Texture gm_ov_tx;
    

    kalakar(loader *ld1, draw_list *d1)
    {
        ld = ld1;
        d = d1;

        ld->drish->setPosition(Vector2i(300,400));
        ld->snek.reserve(100);
        prep_snek(10);
    }

    private:
    void prep_snek(int sz)
    {
        d->rlist.emplace_back(make_pair(&ld->fd1, "food"));
        make_snek(sz);
        ld->snek[0].setPosition(Vector2f(240,240));
        for(int i = 1; i < ld->snek.size(); ++i)
        {
            ld->snek[i].setPosition(Vector2f(ld->snek[i-1].getPosition().x - 12, ld->snek[i-1].getPosition().y));
        }
        //cout<<"chala\n";
    }
    void make_snek(int sz)
    {
        for(int i=0; i<sz; i++)
        {
            ld->snek.emplace_back(Vector2f(10,10));
            d->rlist.emplace_back(make_pair(&ld->snek.back(), "snek"));
        }
    }

    public:
    void karo()
    {
        if(gm_ov == 1)
        {
            game_over_proc();
            return;
        } 

        int parent_positionX = 0;
        int parent_positionY = 0;
        int child_positionX = 0;
        int child_positionY = 0;
        float parent_orientaion = 0;
        float posX, posY;
        float fdx, fdy;

        mx1.lock();
        if(q1.size() != 0)
        {
            disha(q1.front());
            q1.pop();
        }
        mx1.unlock();
        
        for(int i=0; i < ld->snek.size(); ++i)
        {

            if(i == 0)
            {
                parent_positionX = ld->snek[0].getPosition().x;
                parent_positionY = ld->snek[0].getPosition().y;
                fdx = ld->fd1.getPosition().x;
                fdy = ld->fd1.getPosition().y;
                ld->snek[0].move(vectX, vectY);
                ld->snek[0].setFillColor(Color::White);
                if(fdx == 0 && fdy == 0)
                {
                    srand(time(0));
                    fdx = 1 + rand()%64;
                    srand(fdx);
                    fdy = rand()%40;
                    ld->fd1.setPosition(Vector2f(fdx*12, fdy*12));
                }
                else
                {
                    if(ld->snek[0].getPosition().x == fdx && ld->snek[0].getPosition().y == fdy)
                    {
                        //cout<<"khaliya\n";
                        srand(time(0));
                        fdx = 1 + rand()%64;
                        srand(fdx);
                        fdy = rand()%40;
                        ld->fd1.setPosition(Vector2f(fdx*12, fdy*12));
                        make_snek(1);
                        score+=(ld->snek.size());
                        //cout<<"score :: "<<score<<"\n";
                        ld->snek[0].setFillColor(Color::Blue);
                    }
                }
            }
            else
            {
                child_positionX = ld->snek[i].getPosition().x;
                child_positionY = ld->snek[i].getPosition().y;
                ld->snek[i].setPosition(Vector2f(parent_positionX, parent_positionY));
                parent_positionX = child_positionX;
                parent_positionY = child_positionY;

                if(parent_positionX == ld->snek[0].getPosition().x && parent_positionY == ld->snek[0].getPosition().y) game_over();
            }
        }
        warp_check(fdx, fdy);
    }

    private:
    void disha(int q1f)
    {
        if(q1f == 1 && vectY != -12)
        {
            vectX = 0;
            vectY = -12;
        }
        else if(q1f == 2 && vectX != 12)
        {
            vectX = 12;
            vectY = 0;
        }
        else if(q1f == 3 && vectY != 12)
        {
            vectX = 0;
            vectY = 12;
        }
        else if(q1f == 4 && vectX != -12)
        {
            vectX = -12;
            vectY = 0;
        }
    }
    void warp_check(int fdx, int fdy)
    {
        int pX = ld->snek[0].getPosition().x;
        int pY = ld->snek[0].getPosition().y;
        if(pX >= 768) ld->snek[0].setPosition(Vector2f(pX % 768, pY));
        if(pY >= 480) ld->snek[0].setPosition(Vector2f(pX, pY % 480));
        if(pX < 0) ld->snek[0].setPosition(Vector2f(768 + pX, pY));
        if(pY < 0) ld->snek[0].setPosition(Vector2f(pX, pY + 480));
        if(ld->snek[0].getPosition().x == fdx && ld->snek[0].getPosition().y == fdy)
        {
            cout<<"khaliya\n";
            srand(time(0));
            fdx = 1 + rand()%64;
            srand(fdx);
            fdy = rand()%40;
            ld->fd1.setPosition(Vector2f(fdx*12, fdy*12));
            make_snek(1);
            score+=(ld->snek.size());
            //cout<<"score :: "<<score<<"\n";
            ld->snek[0].setFillColor(Color::Blue);
        }
    }
    void game_over()
    {
        if(gm_ov == 0)
        {
            for(int i = 0; i < ld->snek.size(); ++i)
            {
                ld->snek[i].setFillColor(Color::Red);
            }
            gm_ov = 1; 
            mlt_on = 1;
        }
    }
    void game_over_proc()
    {
        if(mlt_on == 1)
        {
            //cout<<mlt_on<<"\n";
            for(int i = 0; i < d->rlist.size(); )
            {
                if(d->rlist[i].second == "snek" || d->rlist[i].second == "food")
                {
                    d->rlist.erase(d->rlist.begin() + i);
                }
                else
                {
                    ++i;
                }
                
            }
            gm_ov_tx.create(ld->drish->getSize().x, ld->drish->getSize().y);
            gm_ov_tx.update(*ld->drish);
            for(int i = 0; i < 768/2; i++)
            {
                Sprite s1;
                s1.setTexture(gm_ov_tx);
                s1.setTextureRect(IntRect(0 + i*2,0,2,480));
                s1.setPosition(0 + i*2, 0);
                ld->melt.emplace_back(s1);
                d->rlist.emplace_back(make_pair(&ld->melt[i], "melt"));
            }
            try
            {
                ld->fnt1.loadFromFile("./Xeliard.ttf");
                ld->likh1.setFont(ld->fnt1);
                ld->likh2.setFont(ld->fnt1);
                ld->likh1.setCharacterSize(50);
                ld->likh1.setString("GAME OVER\n\nSCORE :: " + to_string(score));
                ld->likh2.setString("Press R to replay , press E to exit");
                ld->likh1.setPosition(384-160, 240-25);
                ld->likh2.setPosition(384-260, 400);
                d->rlist.insert(d->rlist.begin() + 0, make_pair(&ld->likh1, "gm_ov_txt"));
                d->rlist.insert(d->rlist.begin() + 0, make_pair(&ld->likh2, "gm_ov_txtop"));
            }
            catch(...)
            {
                cout<<"error :: could not load font assets\n";
            }
            
            

            mlt_on = 0;
            ld->render_mode = 1;
        }
        //cout<<count++<<"\n";
        for(int i = 0; i < 768/2; ++i)
        {
            if(ld->melt[i].getPosition().y > 480) continue;
            float f = rand()%6 * 0.5;
            ld->melt[i].move(0,6*f);
        }
       return;
    }
};

class nivisht
{
    
    public:
    int rel;

    void take_input(sf::Event *e1, kalakar *k)
    {
        if(e1->type == sf::Event::KeyPressed) rel =1;
        if(e1->type == sf::Event::KeyReleased && e1->key.code == sf::Keyboard::Up && rel == 1)
        {
            mx1.lock();
            q1.emplace(1);
            mx1.unlock();
            rel = 0;
            //cout<<"up\n";
        }

        if(e1->type == sf::Event::KeyPressed) rel =1;
        if(e1->type == sf::Event::KeyReleased && e1->key.code == sf::Keyboard::Right && rel == 1)
        {
            mx1.lock();
            q1.emplace(2);
            mx1.unlock();
            rel = 0;
            //cout<<"right\n";
        }

        if(e1->type == sf::Event::KeyPressed) rel =1;
        if(e1->type == sf::Event::KeyReleased && e1->key.code == sf::Keyboard::Down && rel == 1)
        {
            mx1.lock();
            q1.emplace(3);
            mx1.unlock();
            rel = 0;
            //cout<<"down\n";
        }

        if(e1->type == sf::Event::KeyPressed) rel =1;
        if(e1->type == sf::Event::KeyReleased && e1->key.code == sf::Keyboard::Left && rel == 1)
        {
            mx1.lock();
            q1.emplace(4);
            mx1.unlock();
            rel = 0;
            //cout<<"left\n";
        }

        if(e1->type == sf::Event::KeyPressed) rel =1;
        if(e1->type == sf::Event::KeyReleased && e1->key.code == sf::Keyboard::E && rel == 1)
        {
            k->ld->drish->close();
            //cout<<"left\n";
        }

        if(e1->type == sf::Event::KeyPressed) rel =1;
        if(e1->type == sf::Event::KeyReleased && e1->key.code == sf::Keyboard::R && rel == 1)
        {
            k->repeat = 1;
            //k->ld->drish->close();
            //cout<<"left\n";
        }

    }
};

class rang
{
    public:
    kalakar *k;
    void rang_set(kalakar *k1)
    {
        k = k1;
    }

    void rend(nivisht n1)
    {
        Clock cl;

        k->ld->drish->setVerticalSyncEnabled(true);
        while(k->ld->drish->isOpen() && k->repeat == 0)
        {
            k->karo();
            k->ld->drish->clear();
            cl.restart();
            game_draw();
            k->ld->drish->display();
            //##############################
            if(k->ld->render_mode == 0)
            usleep(250000 - cl.restart().asMicroseconds());
            else
            usleep(50000 - cl.restart().asMicroseconds());;
            //##############################
        }
    }

    void game_draw()
    {
        for(int i=0; i < k->d->rlist.size(); ++i)
        {
            //cout<<k->d->rlist.size()<<"\n";
            k->ld->drish->draw(*k->d->rlist[i].first);
            //cout<<k->ld->fd1.getPosition().x<<" "<<k->ld->fd1.getPosition().y<<" "<<k->ld->snek[0].getPosition().x<<" "<<k->ld->snek[0].getPosition().y<<"\n";
        }
    }
};



void inp(nivisht *n1, kalakar *k)
{
    while(k->ld->drish->isOpen() && k->repeat == 0)
    {
        sf::Event e1;
        while(k->ld->drish->pollEvent(e1))
        {
            if(e1.type == sf::Event::Closed) k->ld->drish->close();
            n1->take_input(&e1, k);
        }
    }
}

int main()
{
    XInitThreads();
    int end = 0;
    kalakar *k1;
    rang r;
    loader ld("ld");
    do
    {
        draw_list d1;
        nivisht n1;
        
        kalakar k(&ld, &d1);
        r.rang_set(&k);
        thread t1(inp,&n1,&k);
        r.rend(n1);
        t1.join();/* code */
        k1 = &k;
        if(k.repeat == 1)
        {
            k.repeat = 0;
            ld.flush();
            continue;
        }
    } while (k1->ld->drish->isOpen());
}
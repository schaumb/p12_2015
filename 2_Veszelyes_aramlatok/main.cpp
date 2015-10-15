#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <set>
#include <tuple>
#include <locale>
#include <cmath>
#include <thread>

using namespace std;

int n=100;
vector<vector<tuple<short int, short int> > > aramlatok(n,vector<tuple<short int, short int> >(n));
vector<vector<bool> > hasznalt(n,vector<bool>(n,false));
vector<tuple<short int, short int> > hajokkezd, hajokcel;
vector<tuple<short int, short int> > ut;
vector<vector<tuple<short int, short int> > > hajoutak;



vector<tuple<short int, short int> > sebessegvektorok(short int x, short int y)
{
    vector<tuple<short int, short int> > cucc;
    for(short int i=-1;i<2;i++)
    {
        for(short int j=-1;j<2;j++)
        {
            if(x+i>=-5 && x+i<=5 && y+j>=-5 && y+j<=5)
                cucc.emplace_back(x+i,y+j);
        }
    }
    return cucc;
}

struct cucc2{

    cucc2(unsigned short int a, unsigned short int b, unsigned short int c, unsigned short int d)
    {
        sx=a;
        sy=b;
        cx=c;
        cy=d;
    }

    unsigned short int sx,sy,cx,cy;

    bool operator()(tuple<short int, short int> a, tuple<short int, short int> b) const
    {
        short int ax=sx+get<0>(a)+get<0>(aramlatok[sx][sy]);
        short int ay=sy+get<1>(a)+get<1>(aramlatok[sx][sy]);
        short int bx=sx+get<0>(b)+get<0>(aramlatok[sx][sy]);
        short int by=sy+get<1>(b)+get<1>(aramlatok[sx][sy]);

        if(!(ax>=0 && ay>=0 && bx>=0 && by>=0 && ax<n && ay<n && bx<n && by<n))
            return false;

        return (ax==cx && ay==cy)
            || ((abs(cx-ax)+abs(cy-ay)) < (abs(cx-bx)+abs(cy-by)))
            /*|| ((abs(cx-(ax+get<0>(aramlatok[ax][ay])))+abs(cy-(ay+get<1>(aramlatok[ax][ay])))) <
                (abs(cx-(bx+get<0>(aramlatok[bx][by])))+abs(cy-(by+get<1>(aramlatok[bx][by])))))*/

            || ((abs(cx-(ax+get<0>(a)+get<0>(aramlatok[ax][ay])))+abs(cy-(ay+get<1>(a)+get<1>(aramlatok[ax][ay])))) <
                (abs(cx-(bx+get<0>(b)+get<0>(aramlatok[bx][by])))+abs(cy-(by+get<1>(b)+get<1>(aramlatok[bx][by])))))
            ;

        return (abs(cx-(sx+get<0>(a)+get<0>(aramlatok[sx][sy])))+abs(cy-(sy+get<1>(a)+get<1>(aramlatok[sx][sy])))) <
                (abs(cx-(sx+get<0>(b)+get<0>(aramlatok[sx][sy])))+abs(cy-(sy+get<1>(b)+get<1>(aramlatok[sx][sy]))));
    }
};

void hanykodo(unsigned short int id, unsigned short int startx, unsigned short int starty, unsigned short int celx, unsigned short int cely, short int vektorx, short int vektory)
{
    if(startx!=celx || starty!=cely)
    {
        if(!hasznalt[startx][starty] && (hajoutak[id].size()==0 || ut.size()<hajoutak[id].size()))
        {
            vector<tuple<short int, short int> > cucc=sebessegvektorok(vektorx,vektory);

            hasznalt[startx][starty]=true;
            sort(cucc.begin(),cucc.end(),cucc2(startx,starty,celx,cely));
            for(unsigned int i=0;i<cucc.size();i++)
            {
                if(startx+get<0>(cucc[i])+get<0>(aramlatok[startx][starty])>=0 && startx+get<0>(cucc[i])+get<0>(aramlatok[startx][starty])<n &&
                   starty+get<1>(cucc[i])+get<1>(aramlatok[startx][starty])>=0 && starty+get<1>(cucc[i])+get<1>(aramlatok[startx][starty])<n)
                {
                    ut.emplace_back(get<0>(cucc[i]),get<1>(cucc[i]));
                    hanykodo(id, startx+get<0>(cucc[i])+get<0>(aramlatok[startx][starty]),starty+get<1>(cucc[i])+get<1>(aramlatok[startx][starty]),celx,cely,get<0>(cucc[i]),get<1>(cucc[i]));
                    ut.pop_back();
                }
            }
            hasznalt[startx][starty]=false;
        }
    }
    else
    {
        if(hajoutak[id].size()>ut.size() || hajoutak[id].size()==0)
        {
            hajoutak[id]=ut;
            //cout << "Megoldas:" << ut.size() << "| " << startx << ":" << starty << endl;
        }
    }
}

int main()
{
    //setLocal)
    ifstream aramlatokf("aramlas.txt");


    for(unsigned int i=0;i<n;i++)
    {
        for(unsigned int j=0;j<n;j++)
        {
            aramlatokf >> get<0>(aramlatok[n-1-i][j]);
            aramlatokf >> get<1>(aramlatok[n-1-i][j]);
        }
    }
    aramlatokf.close();


    ifstream hajokf("hajok.txt");
    short int startx,starty,celx,cely;

    while(hajokf>>startx && hajokf>>starty && hajokf>>celx && hajokf>>cely)
    {
        hajokkezd.emplace_back(startx,starty);
        hajokcel.emplace_back(celx,cely);
    }
    hajokf.close();

    hajoutak.resize(hajokkezd.size());

    //hanykodo(0,get<0>(hajokkezd[0]), get<1>(hajokkezd[0]), get<0>(hajokcel[0]), get<1>(hajokcel[0]),0,0);

    vector<thread> threads;

    unsigned int i=0;
    while(i<hajokkezd.size())
    {
        threads.emplace_back(hanykodo,i,get<0>(hajokkezd[i]), get<1>(hajokkezd[i]), get<0>(hajokcel[i]), get<1>(hajokcel[i]),0,0);
        if(i%20==0)
        {
            for (auto& th : threads) th.join();
            threads.clear();
        }

        i++;
    }
    for (auto& th : threads) th.join();

    ofstream kif("utvonalak.txt");

    for(unsigned int j=0;j<hajoutak.size();j++)
    {
        for(unsigned int k=0;k<hajoutak[j].size();k++)
        {
            kif << get<0>(hajoutak[j][k]) << " " << get<1>(hajoutak[j][k]) << " ";
        }
        kif << endl;
    }
    kif.close();

    return 0;
}

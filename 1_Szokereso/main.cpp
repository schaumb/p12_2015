#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <set>
#include <tuple>
#include <locale>

using namespace std;

unsigned int matrixn;
vector<vector<char> > matrix;
vector<vector<bool> > hasznalt;

vector<tuple<unsigned int, unsigned int> > szomszedok(unsigned int x, unsigned int y)
{
    vector<tuple<unsigned int, unsigned int> > cucc;

    for(unsigned int i=0; i<3; i++)
    {
        for(unsigned int j=0; j<3; j++)
        {
            if((i!=1 || j!=1) && (x+i-1>=0 && y+j-1>=0 && x+i-1<matrixn && y+j-1<matrixn))
                cucc.emplace_back(x+i-1,y+j-1);
        }
    }

    return cucc;
}

bool kirako(unsigned int x, unsigned int y, string szo)
{
    if(szo.size()==0)
        return true;

    bool megoldas = false;
    vector<tuple<unsigned int, unsigned int> > cucc=szomszedok(x,y);

    for(unsigned int i=0;i<cucc.size();i++)
    {
        if(matrix[get<0>(cucc[i])][get<1>(cucc[i])]==szo[0] && !hasznalt[get<0>(cucc[i])][get<1>(cucc[i])])
        {
            //cout << get<0>(cucc[i]) << ":" << get<1>(cucc[i]) << endl;
            hasznalt[get<0>(cucc[i])][get<1>(cucc[i])]=true;
            megoldas = megoldas || kirako(get<0>(cucc[i]),get<1>(cucc[i]),szo.substr(1));
            hasznalt[get<0>(cucc[i])][get<1>(cucc[i])]=false;

        }
    }

    return megoldas;
}

bool kirakhato(string szo)
{
    bool megoldas = false;
    for(unsigned int i=0; i<matrixn; i++)
    {
        for(unsigned int j=0; j<matrixn; j++)
        {
            if(matrix[i][j]==szo[0])
            {
                //cout << endl << i << ":" << j << endl;
                hasznalt[i][j]=true;
                megoldas = megoldas || kirako(i,j,szo.substr(1));
                hasznalt[i][j]=false;

            }
        }
    }
    return megoldas;
}

string strtoupper(string str)
{
    for (std::string::size_type i=0; i<str.length(); ++i)
        str[i]= std::toupper(str[i]);
    return str;
}

unsigned int pontszam(unsigned int hossz)
{
    if(hossz==2)
        return 1;
    else return (hossz*hossz-5*hossz+10)/2;
}

int main()
{
    //locale loc(setlocale(LC_ALL,""));
    setlocale(LC_ALL,"");
    //Szotar

    unsigned long int szotarn;

    ifstream szotarf("Szotar.txt");

    szotarf >> szotarn;
    vector<string> szotar(szotarn),szotarU(szotarn);

    for(unsigned long int i=0; i<szotarn; i++)
    {
        szotarf >> szotar[i];
        szotarU[i]=strtoupper(szotar[i]);
    }
    szotarf.close();

    sort(szotarU.begin(),szotarU.end());

    //cout << "haha" << endl;

    //Matrix



    ifstream matrixf("Matrix.txt");

    matrixf >> matrixn;
    matrix.resize(matrixn);
    hasznalt.resize(matrixn);

    for(unsigned int i=0; i<matrixn; i++)
    {
        matrix[i].resize(matrixn);
        hasznalt[i].resize(matrixn,false);
        for(unsigned int j=0; j<matrixn; j++)
        {
            matrixf >> matrix[i][j];
        }
    }
    matrixf.close();

    //cout << "haha" << endl;

    //Szavak

    unsigned int szavakn;

    ifstream szavakf("Szavak.txt");
    ofstream ertekelesf("Ertekeles.txt");

    szavakf >> szavakn;
    vector<string> szavak(szavakn);

    bool elso,masodik;
    unsigned long int c=0,pont;

    for(unsigned int i=0; i<szavakn; i++)
    {
        szavakf >> szavak[i];
        elso=kirakhato(szavak[i]);
        masodik=binary_search(szotarU.begin(),szotarU.end(),szavak[i]);
        if(elso && masodik && szavak.begin()+i==find(szavak.begin(),szavak.begin()+i,szavak[i]))
            pont=pontszam(szavak[i].size());
        else
            pont=0;

        c+=pont;
        ertekelesf << szavak[i] << " " << ((elso)?"1":"0") << " " << ((masodik)?"1":"0") << " " << pont << endl;
    }
    szavakf.close();
    ertekelesf << c;
    ertekelesf.close();

    ofstream megoldasf("Megoldas.txt");
    c=0;

    for(unsigned int i=0;i<szotar.size();i++)
    {
        if(kirakhato(strtoupper(szotar[i])))
        {
            c+=pontszam(szotar[i].size());
            megoldasf << szotar[i] << " ";
        }
    }
    megoldasf << endl << c;
    megoldasf.close();


    return 0;
}

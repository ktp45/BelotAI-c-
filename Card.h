#include <iostream>

using namespace std;
class Card
{
    unsigned char m_ucColor;
    unsigned char m_ucPower;
    public :
    Card();
    /*create card*/
    Card(unsigned char, unsigned char);
    /* Copy const*/
    Card(const Card&);
    /* operator = */
    Card& operator= (const Card&);

    bool operator==(const Card& right);

    /*get name*/
    unsigned char GetPower()const;
    /*get color*/
    unsigned char GetColor()const;
};


std::ostream& operator<<(ostream &out,const Card& c);

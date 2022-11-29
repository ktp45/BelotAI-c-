#include "Card.h"

using namespace std;


Card::Card()
{
    m_ucColor = 69;
    m_ucPower = 69;
}

    /* Copy const*/
Card::Card(const Card& other)
{
    m_ucColor = other.m_ucColor;
    m_ucPower = other.m_ucPower;
}
Card::Card(unsigned char power, unsigned char color)
{
    m_ucColor = color;
    m_ucPower = power;
}

unsigned char Card::GetPower()const
{
    return m_ucPower;
}
unsigned char Card::GetColor()const
{
    return m_ucColor;
}

std::ostream& operator<<(ostream &out, const Card& c) //the cards is printed as (' name and color')
{
	out << "(" << static_cast<int>(c.GetPower()) << ")" << "(" << static_cast<int>(c.GetColor()) << ")";
    return out;
}

Card& Card::operator=(const Card& other)
{
	if (this  ==  &other)
    {
        return *this;
    }

    this -> m_ucColor = other.m_ucColor;
    this -> m_ucPower = other.m_ucPower; // operator  =  same as above
    return *this;
}


bool Card::operator==(const Card& right) {
    return m_ucColor == right.m_ucColor && m_ucPower == right.m_ucPower;
}
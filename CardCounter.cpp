#include "CardCounter.h"
using namespace std;

void CardCounter::SortCardsByAnnounce()
{
    if(m_ucAnnounce > ALL_TRUMP_ANNOUNCE )
    {
        cerr<< "WRONG ANNOUNCE"<< endl;
    }
    
    for(int i = 0; i <= SPADES; i++)
    {
        if(i == m_ucAnnounce || m_ucAnnounce == ALL_TRUMP_ANNOUNCE)
        {
            cout << "ALL_TRUMP" << endl;
            m_helper.sort_by_power(m_vRemainingCards.at(i), "ALL_TRUMP");
        }
        else
        {
            m_helper.sort_by_power(m_vRemainingCards.at(i), "NO_TRUMP");
            cout << "NO_TRUMP" << endl;
        }

        reverse(m_vRemainingCards.at(i).begin(), m_vRemainingCards.at(i).end());
        //the most powerful card should be first for simplicity
    }
    
}

CardCounter::CardCounter(unsigned char announce)
{
    m_helper.InitHelper(announce);
    m_ucAnnounce = announce;
    m_ucPCardsCount = 0;
    for(int i = 0; i < NUMBER_OF_CARDS; i++)
    {
        m_aPlayedCards.at(i) = NULLCARD;
    }
    
    for (int i = 0; i <= SPADES; i++ )
    {
        vector<Card> tempColor;
        for (int j = 0; j <= Ace; j++ )
        {
            tempColor.push_back(Card(j, i));
             // TODO : Make deck generator func
        }
        m_vRemainingCards.push_back(tempColor); 
    }
    SortCardsByAnnounce();
}

void CardCounter::Update(array<Card, NUMBER_OF_PLAYERS> m_aPlayedTick)
{
    for(int i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        m_aPlayedCards.at(m_ucPCardsCount) = m_aPlayedTick.at(i);
        Card tempCard = m_aPlayedCards.at(m_ucPCardsCount);
        m_ucPCardsCount++;
        unsigned char remainingCardsSize = m_vRemainingCards.at(tempCard.GetColor()).size();

        for(int j = 0; j < remainingCardsSize; j++)
        {
    	    if(tempCard == m_vRemainingCards.at(tempCard.GetColor()).at(j))
            {
                m_vRemainingCards.at(tempCard.GetColor()).erase(m_vRemainingCards.at(tempCard.GetColor()).begin() + j);
                //vector:erase forses usage of iterator 
                break;
            }
        }

    }
}

vector<Card> CardCounter::GetPlayedCards()
{
    vector<Card> playedCards(m_aPlayedCards.begin(), m_aPlayedCards.begin() + m_ucPCardsCount);
    return playedCards;
}

vector<Card> CardCounter::GetPlayedbyTrumpCards(unsigned char trump)
{
    vector<Card> playedCards;
    for(int i = 0; i < m_ucPCardsCount; i++)
    {
        if(m_aPlayedCards.at(i).GetColor() == trump)
        {
            playedCards.push_back(m_aPlayedCards.at(i));
        }
    }

    return playedCards;
}


vector<Card> CardCounter::GetRemainingCards()
{
    vector<Card> remainingCards = m_vRemainingCards.at(0);
    remainingCards.reserve(NUMBER_OF_CARDS - m_ucPCardsCount);
    remainingCards.insert(remainingCards.end(), m_vRemainingCards.at(1).begin(), m_vRemainingCards.at(1).end());
    remainingCards.insert(remainingCards.end(), m_vRemainingCards.at(2).begin(), m_vRemainingCards.at(2).end());
    remainingCards.insert(remainingCards.end(), m_vRemainingCards.at(3).begin(), m_vRemainingCards.at(3).end());
    return remainingCards;
}


vector<Card> CardCounter::GetRemainingbyTrumpCards(unsigned char trump)
{
    return m_vRemainingCards.at(trump);
}

unsigned char CardCounter::EvaluateHand(array<Card, HAND_SIZE> hand)
{
   unsigned char winningCards = 0;
   unsigned char handSize = HAND_SIZE - m_ucPCardsCount / NUMBER_OF_PLAYERS;
   if (m_ucAnnounce >= NO_TRUMP_ANNOUNCE)
   {
        for(unsigned char i = 0; i <= SPADES; i++)
        {
            unsigned char j = 0 ;
            while(m_helper.findCard(hand, m_vRemainingCards.at(i).at(j), handSize) != ERROR)
            {
                winningCards++;
                j++;
            }   
            if(winningCards == handSize)
            {
                return winningCards;
            }
        }
   }
   else
   {
        unsigned char remainingTrump = m_vRemainingCards.at(m_ucAnnounce).size();
        unsigned char j = 0 ;
        while(m_helper.findCard(hand, m_vRemainingCards.at(m_ucAnnounce).at(j), handSize) != ERROR)
        {
            j++;
            winningCards++;
            if(winningCards == handSize)
            {
                return winningCards;
            }
            if(winningCards == remainingTrump)
            {
                break;
            }
        }
        if(winningCards == remainingTrump)
        {
            for(unsigned char i = 0; i <= SPADES; i++)
            {
                if(i == m_ucAnnounce)
                {
                    continue;
                }
                j = 0 ;
                while(m_helper.findCard(hand, m_vRemainingCards.at(i).at(j), handSize) != ERROR)
                {
                    winningCards++;
                    j++;
                }   
                if(winningCards == handSize)
                {
                    return winningCards;
                }
            }
        }
   }  
   return winningCards;  
}
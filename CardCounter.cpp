#include "CardCounter.h"
using namespace std;

CardCounter::CardCounter()
{

}

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
            m_helper.sort_by_power(m_vRemainingCards.at(i), "ALL_TRUMP");
        }
        else
        {
            m_helper.sort_by_power(m_vRemainingCards.at(i), "NO_TRUMP");
        }

        reverse(m_vRemainingCards.at(i).begin(), m_vRemainingCards.at(i).end());
        //the most powerful card should be first for simplicity
    }
    
}

bool CardCounter::InitCounter(unsigned char announce)
{
    m_vRemainingCards.clear();
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
    m_helper.InitHelper(announce);
    m_ucAnnounce = announce;
    SortCardsByAnnounce();
    return true;
}

void CardCounter::Update(const array<Card, NUMBER_OF_PLAYERS>& m_aPlayedTick)
{
    for(int i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        m_aPlayedCards.at(m_ucPCardsCount) = m_aPlayedTick.at(i);
        Card tempCard = m_aPlayedCards.at(m_ucPCardsCount);
        
        m_ucPCardsCount++;
        if(m_ucPCardsCount >= 32) return;
        if(tempCard.GetColor() == NULLCARD.GetColor())
        {
            cerr << "UPDATE WITH NULLCARD >?!?" << endl;
            throw __throw_bad_function_call;
        }

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

void CardCounter::UpdateByHands(const array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS> &hands,
                                 unsigned char playedCardsCount, bool updateplayedCards)
{
    m_ucPCardsCount = playedCardsCount;
    for (int i = 0; i <= SPADES; i++ )
    {
        fill(m_vRemainingCards.at(i).begin(), m_vRemainingCards.at(i).end(), NULLCARD);
    }

    for(int i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        for(int j = 0; j < HAND_SIZE; j++)
        {
            if(hands.at(i).at(j).GetColor() == NULLCARD.GetColor())
            {
                continue;
            }
            m_vRemainingCards.at(hands.at(i).at(j).GetColor()).at(hands.at(i).at(j).GetPower()) = hands.at(i).at(j);
        }
    }
    if (updateplayedCards)
    {
        for(int i = 0; i < NUMBER_OF_PLAYERS; i++)
        {
            for(int j = 0; j < HAND_SIZE; j++)
            {
                if(playedCardsCount == 0)
                {
                    break;
                }

                if(m_vRemainingCards.at(i).at(j).GetColor() == NULLCARD.GetColor())
                {
                    Card tempCard(j, i);
                    m_aPlayedCards.at(playedCardsCount) = tempCard;
                    playedCardsCount--;
                }
            }
        }
    }
    SortCardsByAnnounce();
}

void CardCounter::UpdateByCurrentDeal(const array<array<Card, NUMBER_OF_PLAYERS>, HAND_SIZE> &current_deal, 
                                        unsigned char playedCardsCount, bool updateplayedCards)
{
    m_ucPCardsCount = playedCardsCount;
    for (int i = 0; i <= SPADES; i++ )
    {
        fill(m_vRemainingCards.at(i).begin(), m_vRemainingCards.at(i).end(), NULLCARD);
    }

    for(int i = 0; i < HAND_SIZE; i++)
    {
        for(int j = 0; j < NUMBER_OF_PLAYERS; j++)
        {
            if(current_deal.at(i).at(j).GetColor() == NULLCARD.GetColor())
            {
                continue;
            }
            m_vRemainingCards.at(current_deal.at(i).at(j).GetColor()).at(current_deal.at(i).at(j).GetPower()) = current_deal.at(i).at(j);
        }
    }

    if (updateplayedCards)
    {
        for(int i = 0; i <= SPADES; i++)
        {
            for(int j = 0; j < HAND_SIZE; j++)
            {
                if(playedCardsCount == 0)
                {
                    break;
                }

                if(m_vRemainingCards.at(i).at(j).GetColor() == NULLCARD.GetColor())
                {
                    Card tempCard(j, i);
                    m_aPlayedCards.at(playedCardsCount) = tempCard;
                    playedCardsCount--;
                }
            }
        }
    }
    SortCardsByAnnounce();

}

vector<Card> CardCounter::GetPlayedCards()
{
    vector<Card> playedCards(m_aPlayedCards.begin(), m_aPlayedCards.begin() + m_ucPCardsCount);
    return playedCards;
}

vector<Card> CardCounter::GetPlayedbyTrumpCards(const unsigned char& trump)
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


vector<Card> CardCounter::GetRemainingbyTrumpCards(const unsigned char& trump)
{
    return m_vRemainingCards.at(trump);
}

unsigned char CardCounter::EvaluateHand(const array<Card, HAND_SIZE> &hand)
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
                if(m_vRemainingCards.at(i).at(j).GetColor() == NULLCARD.GetColor() )
                {
                    break; // don't count nullcards in power
                }                
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
        unsigned char remainingTrump = 0;
        for(int i = 0; i < HAND_SIZE; i++)
        {
            if(m_vRemainingCards.at(m_ucAnnounce).at(i).GetColor() != NULLCARD.GetColor())
            {
                remainingTrump++;
            }
        }
        unsigned char j = 0 ;
        while(m_helper.findCard(hand, m_vRemainingCards.at(m_ucAnnounce).at(j), handSize) != ERROR)
        {
            if(m_vRemainingCards.at(m_ucAnnounce).at(j).GetColor() == NULLCARD.GetColor() )
            {
                break; // don't count nullcards in power
            }

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
                j = 0;
                while(m_helper.findCard(hand, m_vRemainingCards.at(i).at(j), handSize) != ERROR)
                {
                    if(m_vRemainingCards.at(i).at(j).GetColor() == NULLCARD.GetColor() )
                    {
                        break; // don't count nullcards in power
                    }

                    winningCards++;
                    j++;
                    if(j == HAND_SIZE)
                    {
                        break; // there are no winning cards
                    }
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

bool CardCounter::IsHandClaimable(const array<Card, HAND_SIZE>& hand)
{
    unsigned char handSize = HAND_SIZE - m_ucPCardsCount / NUMBER_OF_PLAYERS;
    return handSize == EvaluateHand(hand);
}
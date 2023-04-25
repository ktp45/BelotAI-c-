#include "AnnounceTracker.h"
using namespace std;

AnnounceTracker::AnnounceTracker()
{
    cerr<<" AnnounceTracker Called Default constructor " << endl;
}

 // SORTED HAND ONLY
unsigned char AnnounceTracker::cardInARowChecker(array<Card, HAND_SIZE> hand)
{
    unsigned char maxCardsInARow = 0;
    unsigned char cardsInARow = 1;
    for(int j = 1; j < HAND_SIZE; j++)
    {
        if((hand.at(j-1).GetColor() == hand.at(j).GetColor()) && 
        (hand.at(j-1).GetPower() == (hand.at(j).GetPower() - 1)))
        {
            cardsInARow++;
        }
        else
        {
            cardsInARow = 1;
        }
        if(cardsInARow > maxCardsInARow)
        {
            maxCardsInARow = cardsInARow;
        }
    } 
    cout << (int)maxCardsInARow << "in row cards" << endl;
    return maxCardsInARow;
}

unsigned char AnnounceTracker::cardInARowCalculator(array<Card, HAND_SIZE> hand, unsigned char player_id, bool fillUsedCards)
{
    unsigned char score = 0;
    /*
        no way you get 3 in a row twice 
        and also a four of a kind which takes only one of them it must take both
        So it is either the cards in a row or the same cards
    */
    unsigned char cardsInARow = 0;
    for(int j = 1; j < HAND_SIZE; j++)
    {
        if((hand.at(j-1).GetColor() == hand.at(j).GetColor()) && 
        (hand.at(j-1).GetPower() == (hand.at(j).GetPower() - 1)))
        {
            cardsInARow++;
        }
        else
        {
            if(cardsInARow >= TERCA_CARDS 
                        && 
            !((hand.at(j+1).GetColor() == hand.at(j).GetColor()) && (hand.at(j+1).GetPower() == (hand.at(j).GetPower() + 1)))
            )
            {
                if(!fillUsedCards)
                {
                    if(UserOutput(Cards_In_A_Row_Names.at(cardsInARow)))
                    {
                        m_pTeamscores.at(player_id % 2) += Cards_In_A_Row_Points.at(cardsInARow);
                    }
                }
                else
                {
                    for(int used = j; used > (j - cardsInARow); used--)
                    {
                        m_aUsedCards.at(used) = true;
                    }
                    score += Cards_In_A_Row_Points.at(cardsInARow);
                }
            }
            cardsInARow = 1;
        }
    }
    return score; 
}

bool AnnounceTracker::sameCardChecker(array<Card, HAND_SIZE> hand)
{
    unsigned char sameCards = 0;
    for(int i = 0; i < HAND_SIZE; i++)
    {
        for(int j = 0; j < HAND_SIZE; j++)
        {
            if(hand.at(j).GetPower() == i)
            {
                sameCards++;
            }
        }
        if(sameCards == NUMBER_OF_PLAYERS)
        {
            return true;
        } 
        sameCards = 0;
    }
    return false;
}

unsigned char AnnounceTracker::sameCardCalculator(array<Card, HAND_SIZE> hand, unsigned char player_id, bool fillUsedCards)
{
    unsigned char score = 0;
    unsigned char sameCards = 0;
    for(int i = 0; i < HAND_SIZE; i++)
    {
        for(int j = 0; j < HAND_SIZE; j++)
        {
            if(hand.at(j).GetPower() == i)
            {
                sameCards++;
            }
        }

        if(sameCards == NUMBER_OF_PLAYERS)
        {
            if(!fillUsedCards)
            {
                if(UserOutput("Four of a Kind "))
                {
                    m_pTeamscores.at(player_id % 2) += Cards_In_A_Row_Points.at(i);
                }
            }
            else
            {
                for(int used = 0; used < HAND_SIZE; used++)
                {
                    if(hand.at(used).GetPower() == i)
                    {
                        if(m_aUsedCards.at(used))
                        {
                            m_bThereIsAnnounceConflict = true;
                        }
                        m_aUsedCards.at(used) = true;
                    }
                }
                score += Cards_In_A_Row_Points.at(i);
            }
        } 
        sameCards = 0;
    }
    return score;
}
 // SORTED HAND ONLY
unsigned char AnnounceTracker::belotCounter(array<Card, HAND_SIZE> hand)
{
    unsigned char belotCount = 0;
    for(int i = 1; i < HAND_SIZE; i++)
    {
        if(hand.at(i).GetPower() == King)
        {
            if(hand.at(i - 1).GetPower() == Queen && (hand.at(i).GetColor() == hand.at(i-1).GetColor()))
            {
                belotCount++;
            }
        }
    }
    return belotCount;
}

bool AnnounceTracker::UserOutput(string announce)
{
    int validnumber; // TODO ADD RETARD VALIDATION
    cout << "Do you want to say " << announce << "?" <<endl;
    cout << "Press 0 for NO any other number for YES" <<endl;

    cin >> validnumber;
    return validnumber != 0;

}

AnnounceTracker::AnnounceTracker(array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS> hands, string announce)
{
    m_sAnnounce = announce;
    m_aUsedCards.fill(false);

    for (int i = 0; i < TRUMP_NAMES.size(); i++)
    {
        if(announce == TRUMP_NAMES.at(i))
        {
            m_helper.InitHelper(i);
            m_sAnnounce = "TRUMP";
        }
    }

    for(int i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        m_helper.sort_hand(hands.at(i));
        cout << "Player " << i << endl;
        if(sameCardChecker(hands.at(i)) || (cardInARowChecker(hands.at(i)) >= TERCA_CARDS))
        {
            cout << "Player " << i << "has announce" <<endl;
            m_aIsThereTurnIAnnouncePerPlayer.at(i) = true; 
        }
        m_aBeloteCountPerPlayer.at(i) = belotCounter(hands.at(i));
    }

    m_pTeamscores.fill(0);
}

bool AnnounceTracker::IsThereTurnIAnnounce(unsigned char player_id)
{
    return m_aIsThereTurnIAnnouncePerPlayer.at(player_id);
}

unsigned char AnnounceTracker::GetBeloteCountPerPlayer(unsigned char player_id)
{
    return m_aBeloteCountPerPlayer.at(player_id);
}

void AnnounceTracker::MakeAnnounce(array<Card, HAND_SIZE> hand, unsigned char player_id)
{
    if(m_sAnnounce == "NO_TRUMP" || !IsThereTurnIAnnounce(player_id))
    {
        return;
    }

    if (cardInARowChecker(hand) && sameCardChecker(hand))
    {
        unsigned char scorecardInARow = cardInARowCalculator(hand, player_id, true);
        unsigned char scoreSameCards = sameCardCalculator(hand, player_id, true);
        if(!m_bThereIsAnnounceConflict)
        {
            if(UserOutput("KAPE " + to_string(scoreSameCards)))
            {
                m_pTeamscores.at(player_id % 2) += scoreSameCards;
            }
            if(UserOutput("Announce from Cards in a row " + to_string(scorecardInARow)))
            {
                m_pTeamscores.at(player_id % 2) += scorecardInARow;                
            }
        }
        else
        {
            string output = "Four of a Kind equal: " + to_string(scoreSameCards) + " or Announce from Cards in a row: " + to_string(scorecardInARow);
            if(UserOutput(output))
            {
                m_pTeamscores.at(player_id % 2) += scoreSameCards;
            }
            else
            {
                m_pTeamscores.at(player_id % 2) += scorecardInARow;
            }
        }
    }
    else if (cardInARowChecker(hand) >= TERCA_CARDS)
    {
        unsigned char scorecardInARow = cardInARowCalculator(hand, player_id);
        if(UserOutput("Announce from Cards in a row " + to_string(scorecardInARow)))
        m_pTeamscores.at(player_id % 2) += scorecardInARow;
    }
    else if (sameCardChecker(hand))
    {
        unsigned char scoreSameCards = sameCardCalculator(hand, player_id);
        if(UserOutput("KAPE " + to_string(scoreSameCards)))
        m_pTeamscores.at(player_id % 2) += scoreSameCards;
    }

}

bool AnnounceTracker::CanYouPlayBelote(array<Card, HAND_SIZE> hand, array<Card, NUMBER_OF_PLAYERS> playedCards, Card playedCard, unsigned char player_id)
{
    if((playedCard.GetPower() == Queen) || (playedCard.GetPower() == King))
    {
        if(GetBeloteCountPerPlayer(player_id) > 0)
        {
            if(m_sAnnounce == "ALL_TRUMP" || TRUMP_NAMES.at(playedCard.GetColor()) == m_sAnnounce)
            {
                bool isThereBoth = false;
                if(playedCard.GetPower() == Queen)
                {
                    for(int i = 0; i < HAND_SIZE; i++)
                    {
                        if((hand.at(i).GetPower() == King) && hand.at(i).GetColor() == playedCard.GetColor())
                        {
                            isThereBoth = true;
                        }
                    } 
                }
                else if(playedCard.GetPower() == Queen)
                {
                    for(int i = 0; i < HAND_SIZE; i++)
                    {
                        if((hand.at(i).GetPower() == King) && hand.at(i).GetColor() == playedCard.GetColor())
                        {
                            isThereBoth = true;
                        }
                    }                    
                }

                if(isThereBoth && ((playedCards.at(0).GetColor() == playedCard.GetColor())) || (TRUMP_NAMES.at(playedCard.GetColor()) == m_sAnnounce))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void AnnounceTracker::PlayBelote(unsigned char player_id)
{
    if(GetBeloteCountPerPlayer(player_id) <= 0 || m_sAnnounce == "NO_TRUMP")
    {
        return;
    }
    if(UserOutput("belot"))
    {
        m_aBeloteCountPerPlayer.at(player_id)--;
        m_pTeamscores.at(player_id % 2) += BELOT_POINTS;
    }
}



array<short, NUMBER_OF_TEAMS> AnnounceTracker::GetTeamscores()
{
    return m_pTeamscores;
}
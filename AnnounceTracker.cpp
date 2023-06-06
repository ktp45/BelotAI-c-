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
            cardsInARow = 0;
        }
        if(cardsInARow > maxCardsInARow)
        {
            maxCardsInARow = cardsInARow;
        }
    } 
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
            cardsInARow = 0;
        }
        if((cardsInARow >= TERCA_CARDS )
                    && 
            (
                (j == (HAND_SIZE - 1))
                            ||
                !((hand.at(j+1).GetColor() == hand.at(j).GetColor()) && (hand.at(j+1).GetPower() == (hand.at(j).GetPower() + 1)))
            )) /* next card is different or it is a last card */
        {
            bool newHightestCards = false;
            if(((m_aHightestCardsInRow.at(player_id)).back() <=  hand.at(j).GetPower()) || (cardsInARow > m_aHightestCardsInRow.at(player_id).size()))
            {   /* there is no highter announce */
                m_aHightestCardsInRow.at(player_id).erase();
                newHightestCards = true;
            }
            
            for(int used = j; used > (j - (cardsInARow + 1)); used--)
            {               
                if(fillUsedCards)
                {
                m_aUsedCards.at(used) = true;
                cout << "used " << used << endl;
                }
                if(newHightestCards)
                {
                    m_aHightestCardsInRow.at(player_id).insert(m_aHightestCardsInRow.at(player_id).begin(), hand.at(used).GetPower());
                }
            }
            score += Cards_In_A_Row_Points.at(cardsInARow);
        }
    }
    /*
        for (int i = 0 ; i < m_aHightestCardsInRow.at(player_id)).size(); i++)
        {
            cout << (int)m_aHightestCardsInRow.at(player_id)).at(i) ;
        }
        cout << endl; print hightest announce if needed
    */
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
            if(fillUsedCards)
            {
                for(int used = 0; used < HAND_SIZE; used++)
                {
                    if(hand.at(used).GetPower() == i)
                    {
                        if(m_aUsedCards.at(used))
                        {
                            m_bThereIsAnnounceConflict = true;
                            cout << "conflict same " << used << endl;
                        }
                        m_aUsedCards.at(used) = true;
                        cout << "used same " << used << endl; 
                    }
                }
            }
            score += Same_Cards_Points.at(i);
            m_aHightestSameCard.at(player_id) = i; /* if there are two four of a kinds the second will be always bigger */
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
    m_aHightestCardsInRow.fill("");
    m_aHightestSameCard.fill(0);
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
        if(sameCardChecker(hands.at(i)) || (cardInARowChecker(hands.at(i)) >= TERCA_CARDS))
        {
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

    unsigned char scorecardInARow = 0;
    unsigned char scoreSameCards = 0;

    if (cardInARowChecker(hand) && sameCardChecker(hand))
    {
        scorecardInARow = cardInARowCalculator(hand, player_id, true);
        scoreSameCards = sameCardCalculator(hand, player_id, true);
        
        if(!(m_bThereIsAnnounceConflict && ((scorecardInARow > 0) && (scoreSameCards > 0)))) 
        {
            /* conflict doesn't need to be resolved if the score from one of them is zero */
            if(UserOutput("Four of a Kind: " + to_string(scoreSameCards)))
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
            string output = "Four of a Kind equal: " + to_string(scoreSameCards) + " or Announce from Cards in a row: " + to_string(scorecardInARow)
                + "\n YES for Four of a Kind and NO for Announce from Cards in a row";
            if(UserOutput(output))
            {
                m_pTeamscores.at(player_id % 2) += scoreSameCards;
                m_aHightestCardsInRow.at(player_id % 2) = ""; /* reset hightest card */
            }
            else
            {
                m_pTeamscores.at(player_id % 2) += scorecardInARow;
                m_aHightestSameCard.at(player_id % 2) = 0; /* reset hightest card */
            }
        }
    }
    else if (cardInARowChecker(hand) >= TERCA_CARDS)
    {
        scorecardInARow = cardInARowCalculator(hand, player_id);
        if(UserOutput("Announce from Cards in a row " + to_string(scorecardInARow)))
        m_pTeamscores.at(player_id % 2) += scorecardInARow;
    }
    else if (sameCardChecker(hand))
    {
        scoreSameCards = sameCardCalculator(hand, player_id);
        if(UserOutput("KAPE " + to_string(scoreSameCards)))
        m_pTeamscores.at(player_id % 2) += scoreSameCards;
    }
    m_bThereIsAnnounceConflict = false;
    m_aUsedCards.fill(false);
    cout << (int)scorecardInARow << " " << (int)scoreSameCards << endl;
}

void AnnounceTracker::EvaluateAnnounces()
{
    unsigned char equal = 2;
    unsigned char teamWithHighterInRowAnnounce = 0; /* 0 = Team 0 1 = Team 1 2 = equal */
    unsigned char teamWithHighterSameCardAnnounce = 0;
    string currentHightestInRowAnnounce = "";
    unsigned char currentHightestSameCard = 0;
    
    for(int i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        if((m_aHightestCardsInRow.at(i).size() > currentHightestInRowAnnounce.size()) && (m_aHightestCardsInRow.at(i).back() > currentHightestInRowAnnounce.back()))
        {
            currentHightestInRowAnnounce = m_aHightestCardsInRow.at(i);
            teamWithHighterInRowAnnounce = i % 2;
        }
        else if ((m_aHightestCardsInRow.at(i).size() == currentHightestInRowAnnounce.size()) && 
                    (m_aHightestCardsInRow.at(i).back() == currentHightestInRowAnnounce.back()))
        {
            teamWithHighterInRowAnnounce = equal;
        }

        if(m_aHightestSameCard.at(i) > currentHightestSameCard)
        {
            currentHightestSameCard = m_aHightestSameCard.at(i);
            teamWithHighterSameCardAnnounce = i % 2;
        }
        else if(m_aHightestSameCard.at(i) > currentHightestSameCard)
        {
            teamWithHighterSameCardAnnounce = equal;
        }

        cout << m_aHightestCardsInRow.at(i).size() << endl;
    }

    cout << (int)teamWithHighterSameCardAnnounce << " " << (int)teamWithHighterInRowAnnounce << endl;

    if(teamWithHighterInRowAnnounce == equal)
    {
        for(int i = 0; i < NUMBER_OF_PLAYERS; i++)
        {
            if(m_aHightestCardsInRow.at(i).size() > 0)
            {
                m_pTeamscores.at(i % 2) -= Cards_In_A_Row_Points.at(m_aHightestCardsInRow.at(i).size() - 1);
            }
        }
    }
    else
    {
        for(int i = teamWithHighterInRowAnnounce; i < NUMBER_OF_PLAYERS; i+=2)
        {
            m_pTeamscores.at(i % 2) -= Cards_In_A_Row_Points.at(m_aHightestCardsInRow.at(i).size() - 1);
        }    
    }

    if(teamWithHighterSameCardAnnounce == equal)
    {
        for(int i = 0; i < NUMBER_OF_PLAYERS; i++)
        {
            m_pTeamscores.at(i % 2) -= Same_Cards_Points.at(m_aHightestSameCard.at(i));
        }
    }
    else
    {
        for(int i = teamWithHighterSameCardAnnounce; i < NUMBER_OF_PLAYERS; i+=2)
        {
            m_pTeamscores.at(i % 2) -= Same_Cards_Points.at(m_aHightestSameCard.at(i));
        }    
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
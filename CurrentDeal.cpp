#include "CurrentDeal.h"
using namespace std;

CurrentDeal::CurrentDeal()
{
    cerr<<" CurrentDeal Called Default constructor " << endl;
}

CurrentDeal::CurrentDeal(array< array<Card, NUMBER_OF_TICKS>, NUMBER_OF_PLAYERS> hands, string announce): 
 m_aPlayers{}, m_annTracker(hands, announce)
{ 
    for(int i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        m_aPlayers.at(i) = (HumanPlayer(hands.at(i), announce));
    }

    turn_id = 0;

    m_sAnnounce = announce;

    for (int i = 0; i < TRUMP_NAMES.size(); i++)
    {
        if(announce == TRUMP_NAMES.at(i))
        {
            m_helper.InitHelper(i);
            m_sAnnounce = "TRUMP";
            return;
        }
    }

    for(int i = 0; i < NUMBER_OF_TICKS; i++)
    {
        m_aCurrent_deal.at(i).fill(NULLCARD);
    }

    m_fResult = 0.0f;
}

void CurrentDeal::Start()
{
    unsigned char tick_id = turn_id / 4;
    while(tick_id < NUMBER_OF_TICKS)
    {
        for(int i = 0; i < NUMBER_OF_PLAYERS; i++)
        {
            if(turn_id < ANNOUNCE_TURNS * 4)
            {
                for(int h = 0; h < HAND_SIZE; h++)
                {
                    cout << m_aPlayers.at(i).GetHand().at(h) << endl;
                }

                if(m_annTracker.IsThereTurnIAnnounce(i))
                {
                    m_annTracker.MakeAnnounce(m_aPlayers.at(i).GetHand(), i);
                }
            }

            if(turn_id == ANNOUNCE_TURNS * 4)
            {
                m_annTracker.EvaluateAnnounces();
            }

            Card playedCard = m_aPlayers.at(i).PlayCard(m_aCurrent_deal.at(tick_id));

            if(m_annTracker.CanYouPlayBelote(m_aPlayers.at(i).GetHand(), m_aCurrent_deal.at(i), playedCard, i))
            {
                m_annTracker.PlayBelote(i);
            }
            m_aCurrent_deal.at(tick_id).at(i) = playedCard;
            turn_id++;
        }
        tick_id++;
    }

    Analyzer ResultCalc(m_helper);
    m_fResult = ResultCalc.AnalyzeDeal(m_aCurrent_deal, m_sAnnounce);
    m_fResult += m_annTracker.GetTeamscores().at(0);
    m_fResult += static_cast<float>(m_annTracker.GetTeamscores().at(1)) / 1000;
    cout << m_annTracker.GetTeamscores().at(0) << " " << m_annTracker.GetTeamscores().at(1) << endl;
}

array<array<Card, NUMBER_OF_PLAYERS>, NUMBER_OF_TICKS> CurrentDeal::GetCurrentDeal()
{
    return m_aCurrent_deal;
}

unsigned char CurrentDeal::GetTurnID()
{
    return turn_id;
}

float CurrentDeal::GetResult()
{
    return m_fResult;
}
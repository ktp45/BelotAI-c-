#include "HumanPlayer.h"
using namespace std;

class CurrentDeal
{
    unsigned char turn_id;

    Helper m_helper;

    string m_sAnnounce;

    array<array<Card, NUMBER_OF_PLAYERS>, NUMBER_OF_TICKS> m_aCurrent_deal;

    array<HumanPlayer, NUMBER_OF_PLAYERS> m_aPlayers;

    float m_fResult;

    public:

    CurrentDeal();

    CurrentDeal(array< array<Card, NUMBER_OF_TICKS>, NUMBER_OF_PLAYERS> hands,string announce);

    void Start();

    array<array<Card, NUMBER_OF_PLAYERS>, NUMBER_OF_TICKS> GetCurrentDeal();

    unsigned char GetTurnID();

    float GetResult();
};
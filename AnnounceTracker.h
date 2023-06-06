#include "Helper.h"
using namespace std;

class AnnounceTracker
{    
    bool m_bTeamWithHighterAnnounce;

    array<short, NUMBER_OF_TEAMS> m_pTeamscores;

    array<unsigned char, NUMBER_OF_PLAYERS> m_aBeloteCountPerPlayer;

    array<bool, NUMBER_OF_PLAYERS> m_aIsThereTurnIAnnouncePerPlayer;

    Helper m_helper;

    string m_sAnnounce;

    array<string, NUMBER_OF_PLAYERS> m_aHightestCardsInRow;

    array<unsigned char, NUMBER_OF_PLAYERS> m_aHightestSameCard;

    array<bool, HAND_SIZE> m_aUsedCards;

    bool m_bThereIsAnnounceConflict;

    unsigned char cardInARowChecker(array<Card, HAND_SIZE> hand);

    bool sameCardChecker(array<Card, HAND_SIZE> hand);

    unsigned char belotCounter(array<Card, HAND_SIZE> hand);

    unsigned char cardInARowCalculator(array<Card, HAND_SIZE> hand, unsigned char player_id, bool fillUsedCards = false);

    unsigned char sameCardCalculator(array<Card, HAND_SIZE> hand, unsigned char player_id, bool fillUsedCards = false);

    bool UserOutput(string announce);

    public:
    AnnounceTracker();

    AnnounceTracker(array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS> hands, string announce);

    bool IsThereTurnIAnnounce(unsigned char player_id);

    void MakeAnnounce(array<Card, HAND_SIZE> hand, unsigned char player_id);
    
    void EvaluateAnnounces();

    bool CanYouPlayBelote(array<Card, HAND_SIZE> hand, array<Card, NUMBER_OF_PLAYERS> playedCards, Card playedCard, unsigned char player_id);

    void PlayBelote(unsigned char player_id);

    unsigned char GetBeloteCountPerPlayer(unsigned char player_id);

    array<short, NUMBER_OF_TEAMS> GetTeamscores();
};
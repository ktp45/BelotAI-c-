#include "Analyzer.h"
using namespace std;
class HumanPlayer
{

    array <Card, HAND_SIZE> m_aHand;
    unsigned char m_ucHandSize;
    string m_sAnnounce;
    Helper m_helper;

    public:
    HumanPlayer();
    
    HumanPlayer(array <Card, HAND_SIZE> hand, const string &announce);

    HumanPlayer(const HumanPlayer &other);

    void SetAnnounce(const string &announce);

    array<Card, HAND_SIZE> GetPossibleOptions(const array<Card, NUMBER_OF_PLAYERS> &played_cards);

    Card PlayCard(const array<Card, NUMBER_OF_PLAYERS> &played_cards);

    Card PlayCard(const array<Card, NUMBER_OF_PLAYERS> &played_cards, unsigned char option);

    array<Card, HAND_SIZE> GetHand();

    unsigned char GetHandSize();


};
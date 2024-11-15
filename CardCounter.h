#include"Helper.h"
using namespace std;

class CardCounter{
// announce ID
unsigned char m_ucAnnounce;
// total Played card count
unsigned char m_ucPCardsCount;
// helper instance
Helper m_helper;
// remaining cards by color
vector<vector<Card>> m_vRemainingCards;
// played cards
array<Card, NUMBER_OF_CARDS> m_aPlayedCards;

public:
// default constr
CardCounter();

//  Sort remaining cards by power
void SortCardsByAnnounce();

// The constructor which should be used
bool InitCounter(unsigned char announce);

// Update after each tick
void Update(const array<Card, NUMBER_OF_PLAYERS> &m_aPlayedTick);

// Update by Remaining Cards
void UpdateByHands(const array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS> &hands, unsigned char playedCardsCount, bool updateplayedCards = true);

// Update by DEAL Cards
void UpdateByCurrentDeal(const array<array<Card, NUMBER_OF_PLAYERS>, HAND_SIZE> &current_deal, unsigned char playedCardsCount, bool updateplayedCards = true);

// get all played cards combined CLUBS->DIAMONDS->HEARTS->SPADES
vector<Card> GetPlayedCards();

// get all played cards by trump
vector<Card> GetPlayedbyTrumpCards(const unsigned char &trump);

// remaining cards combined CLUBS->DIAMONDS->HEARTS->SPADES
vector<Card> GetRemainingCards();

//  remaining cards by trump
vector<Card> GetRemainingbyTrumpCards(const unsigned char &trump);

//  calculate winning cards in had
unsigned char EvaluateHand(const array<Card, HAND_SIZE> &hand);

//  return if all cards are winning
bool IsHandClaimable(const array<Card, HAND_SIZE> &hand);
};
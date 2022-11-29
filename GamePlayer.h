#include "Helper.h"
#include <chrono>
using namespace std;
class GamePlayer{
Helper m_helper;

uint64_t first_deals;
vector<array<array<Card, NUMBER_OF_PLAYERS>, HAND_SIZE >> first_deals_combinations;
vector<array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS>> remaining_cards;

uint64_t second_deals; 
vector<array<array<Card, NUMBER_OF_PLAYERS> , HAND_SIZE>> second_deal_combinations;

uint64_t saved_deals; 
vector<array<array<Card, NUMBER_OF_PLAYERS>, HAND_SIZE>> played_deals;

array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS> m_aCards;

string announce;

uint64_t playedCombinations;
public:
/*default const */
GamePlayer();
/* const with 4 hands */
GamePlayer(array<Card, HAND_SIZE> hand1, array<Card, HAND_SIZE> hand2, array<Card, HAND_SIZE> hand3, array<Card, HAND_SIZE> hand4, string announce);
/* simple insertion sort for players cards "Метод на картоиграча" */
void sort_hands();
/* brute force play combinations */
bool play_recursive(unsigned char turn_id, const array<array<Card, NUMBER_OF_DEALS>, NUMBER_OF_PLAYERS>& all_hands, array<Card, NUMBER_OF_PLAYERS>& played_cards, 
                            array<array<Card, NUMBER_OF_PLAYERS>, NUMBER_OF_DEALS> current_deal, const unsigned char first_cards = 0, 
                            const unsigned char second_cards = 0, unsigned char current_player = 0);

/* manager of brute force */
void play_deals_fast(unsigned char first_cards = 0, unsigned char second_cards = 0, long long deal_id=-1);

/* separator between methods */
void play_separated_to_x_then_y(unsigned char cardsNumber);

long long GetFirstDealsCount();

long long GetSecondDealsCount();

/* final results */
bool return_when_all_cards_played();

void PrintHands();

bool StartProcessing();
};
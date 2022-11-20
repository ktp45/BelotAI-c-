#include "Helper.h"
#include <chrono>
using namespace std;
class GamePlayer{
Helper m_helper;
vector<vector<vector<Card>>> first_deals_combinations;  
vector<vector<vector<Card>>> second_deal_combinations;
vector<vector<vector<Card>>> played_deals;
vector<vector<vector<Card>>> remaining_cards;
vector<vector<Card>> m_vCards;
string announce;
long long playedCombinations;
public:
/*default const */
GamePlayer();
/* const with 4 hands */
GamePlayer(vector<Card> hand1, vector<Card> hand2, vector<Card> hand3, vector<Card> hand4, string announce);
/* simple insertion sort for players cards "Метод на картоиграча" */
void sort_hands();
/* brute force play combinations */
bool play_recursive(unsigned char turn_id, const vector<vector<Card>>& all_hands, vector<Card>& played_cards, 
                            vector<vector<Card>> current_deal, const unsigned char first_cards = 0, 
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
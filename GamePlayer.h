#include "CardCounter.h"
#include <chrono>
using namespace std;
struct ThreadData
{
    unsigned char first_cards;
    unsigned char second_cards;
    long long deal_id;
    unsigned char thread_id;
    string announce;
};

struct RecursiveData
{
    array<array<Card, NUMBER_OF_PLAYERS>, NUMBER_OF_DEALS> current_deal;
    unsigned char first_cards = 0;
    unsigned char second_cards = 0;
    unsigned char current_player = 0;
    unsigned char thread_id = 0;
    string announce = "NULL";
};


class GamePlayer{

unsigned char m_ucThreadNumber;

Helper m_helper;

vector<CardCounter> m_vCardCounters;

uint64_t m_ui64first_deals;
vector<array<array<Card, NUMBER_OF_PLAYERS>, HAND_SIZE >> m_vFirst_deals_combinations;
vector<array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS>> m_vRemaining_cards;

vector<uint64_t> m_vSecond_deals;
vector< vector<array<array<Card, NUMBER_OF_PLAYERS> , HAND_SIZE>>> m_vSecond_deal_combinations;

uint64_t m_ui64SavedDeals; 
vector<array<array<Card, NUMBER_OF_PLAYERS>, HAND_SIZE>> m_vPlayed_deals;

array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS> m_aCards;

string m_sAnnounce;

uint64_t m_ui64PlayedCombinations;
public:
/*default const */
GamePlayer();
/* const with 4 hands */
GamePlayer(array<Card, HAND_SIZE> hand1, array<Card, HAND_SIZE> hand2, array<Card, HAND_SIZE> hand3, array<Card, HAND_SIZE> hand4, 
            string announce, unsigned char threadNumber);
/* simple insertion sort for players cards "Метод на картоиграча" */
void sort_hands();
/* brute force play combinations */
bool play_recursive(unsigned char turn_id, const array<array<Card, NUMBER_OF_DEALS>, NUMBER_OF_PLAYERS>& all_hands, array<Card, NUMBER_OF_PLAYERS>& played_cards, 
                            array<array<Card, NUMBER_OF_PLAYERS>, NUMBER_OF_DEALS> current_deal, const unsigned char first_cards = 0, 
                            const unsigned char second_cards = 0, unsigned char current_player = 0, const unsigned char thread_id = 0,
                            string announce = "NULL");
                            
bool play_recursive(unsigned char turn_id, const array<array<Card, NUMBER_OF_DEALS>, NUMBER_OF_PLAYERS>& all_hands, array<Card, NUMBER_OF_PLAYERS>& played_cards,
                    RecursiveData data);

bool finishDeal(unsigned char turn_id, const array<array<Card, NUMBER_OF_DEALS>, NUMBER_OF_PLAYERS>& all_hands, array<Card, NUMBER_OF_PLAYERS>& played_cards, 
        array<array<Card, NUMBER_OF_PLAYERS>, NUMBER_OF_DEALS> current_deal, const unsigned char first_cards = 0, 
        const unsigned char second_cards = 0, unsigned char current_player = 0, const unsigned char thread_id = 0,
        string announce = "NULL");

/* manager of brute force */
void play_deals_fast(unsigned char first_cards = 0, unsigned char second_cards = 0, long long deal_id=-1, unsigned char thread_id = 0,
                    string announce = "n/a");

void play_deals_fast(ThreadData data);

/* separator between methods */
void play_separated_to_x_then_y(unsigned char cardsNumber);



long long GetFirstDealsCount();

long long GetSecondDealsCount(unsigned char thread_id);

long long GetSavedDealsCount();

long long GetPlayedCombinationsCount();

/* final results */
bool return_when_all_cards_played();

void PrintHands();

bool StartProcessing();
};
constants.h                                                                                         0000664 0001750 0001750 00000005070 14346337151 013110  0                                                                                                    ustar   default                         default                                                                                                                                                                                                                // import bitarray
#include <iostream>
#include <vector>
#include <utility>
#include <array>
#include "Card.h"
#include <thread>
#include <string>
#include <cstdlib>
#include <unistd.h>

// """0 equals to 0 and 1 to 1 =>  00101 is 8 of DIAMONDS and 11100 is Ace of CLUBS"""
// """First 3 digits represent the value and the last 2 represent the color"""

// enum TEAMS
// {
//     TEAM_1 = 0b00,
//     TEAM_2 = 0b01,
//     TEAM_1 = 0b10,
//     TEAM_2 = 0b11
// };

enum COLORS_TO_BYTES{
    CLUBS = 0b00,
    DIAMONDS = 0b01,
    HEARTS = 0b10,
    SPADES = 0b11
};


enum CARD_VALUE_TO_BYTES{
    seven = 0b000,
    eight = 0b001,
    nine = 0b010,
    ten = 0b011,
    Jack = 0b100,
    Queen = 0b101,
    King = 0b110,
    Ace = 0b111
};



const size_t ERROR = 69; 
const size_t HAND_SIZE = 8;
const size_t NUMBER_OF_DEALS = 8;
const size_t NUMBER_OF_PLAYERS = 4;
const size_t NUMBER_OF_CARDS = 32;

static const Card NULLCARD(ERROR, ERROR);
static const std::array<Card, HAND_SIZE> NULLHAND{NULLCARD, NULLCARD, NULLCARD, NULLCARD, NULLCARD, NULLCARD, NULLCARD, NULLCARD};

const size_t EXPECTED_FIRST_DEAL_COMBINATIONS     = 5000000;
const size_t EXPECTED_SECOND_DEAL_COMBINATIONS    = 10000000;
const size_t EXPECTED_SAVED_DEAL_COMBINATIONS    = 25000000;

static const std::vector <std::string> TRUMP_NAMES =
{
    "CLUBS",
    "DIAMONDS",
    "HEARTS",
    "SPADES"
};

static const std::vector <unsigned char> ALL_TRUMP_POINTS =
{
    0,  // seven 
    0, // eight
    14, // nine
    10, // ten
    20, // Jack 
    3, // Queen  
    4, // King 
    11  // Ace
};

static const std::vector <unsigned char> NO_TRUMP_POINTS =
{
    0,  // seven 
    0, // eight
    0, // nine
    10, // ten
    2, // Jack 
    3, // Queen  
    4, // King 
    11  // Ace
};



// BYTES_TO_COLORS = {tpl: color for color, tpl in COLORS_TO_BYTES.items()}
// BYTES_TO_CARD_VALUE = {tpl: value for value, tpl in CARD_VALUE_TO_BYTES.items()}


// """Generate card names dynamically based on dictionary keys"""
// CARD_TUPLES = [(value, color) for value in CARD_VALUE_TO_BYTES.keys() for color in COLORS_TO_BYTES.keys()]

// """Generate card bites representation dynamically based on value on color"""
// CARD_TO_BYTES = {card: bitarray.bitarray(CARD_VALUE_TO_BYTES[card[0]] + COLORS_TO_BYTES[card[1]]) for card in
//                  CARD_TUPLES}
// BYTES_TO_CARD = {tuple(bitarray.bitarray(CARD_VALUE_TO_BYTES[card[0]] + COLORS_TO_BYTES[card[1]])): card for card in
//                  CARD_TUPLES}

// if __name__ == '__main__':
//     import pprint

//     pprint.pprint(CARD_TO_BYTES)
//     pprint.pprint(BYTES_TO_CARD)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                        GamePlayer.h                                                                                        0000664 0001750 0001750 00000005423 14346341726 013127  0                                                                                                    ustar   default                         default                                                                                                                                                                                                                #include "Helper.h"
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
};                                                                                                                                                                                                                                             Helper.h                                                                                            0000664 0001750 0001750 00000003235 14346332042 012306  0                                                                                                    ustar   default                         default                                                                                                                                                                                                                #include "constants.h"
#include <algorithm>
#include <iterator>
using namespace std;
class Helper
{
    unsigned char color_trump;
    public:
    Helper();
    bool InitHelper(unsigned char);

    unsigned char findCard(array<Card, NUMBER_OF_DEALS> hand, Card card);
    //clever find

    bool compare_cards(Card card1, Card card2);
    //"""Simple compare for cards by color and power""" True if first > second

    bool compare_cards_power(Card card1, Card card2 , string announce);
    // True if first > second

    bool search_by_color(const array<Card, NUMBER_OF_DEALS>& hand, unsigned char color);
    // """Return True if the color is in the hand else False"""

    array <Card, HAND_SIZE> playableNoTrump
    (const array<Card, HAND_SIZE>& hand, const array<Card , NUMBER_OF_PLAYERS>& playedCards);
    array <Card, HAND_SIZE> playableAllTrump
    (const array<Card, HAND_SIZE>& hand, const array<Card , NUMBER_OF_PLAYERS>& playedCards);
    array <Card, HAND_SIZE> playableTrump
    (const array<Card, HAND_SIZE>& hand, const array<Card , NUMBER_OF_PLAYERS>& playedCards);

    array <Card, HAND_SIZE> playable_by_hand_and_played_cards 
    (const string& announce,const array<Card, NUMBER_OF_DEALS>& hand, const array<Card , NUMBER_OF_PLAYERS>& playedCards);
    //       """Logic about which cards are allowed to be played"""
    
    unsigned char calculate_winner_of_deal(const array<Card , NUMBER_OF_PLAYERS>& playedCards,const string& announce);

    unsigned char count_played_cards(const array<Card , NUMBER_OF_PLAYERS>& playedCards);

    unsigned char count_played_cards(const array<Card , HAND_SIZE>& playedCards);

    bool sort_hand(array<Card , HAND_SIZE>& hand);
};
                                                                                                                                                                                                                                                                                                                                                                   Card.cpp                                                                                            0000664 0001750 0001750 00000003033 14346331407 012273  0                                                                                                    ustar   default                         default                                                                                                                                                                                                                #include "Card.h"
#include <vector>
using namespace std;

static const std::vector <std::string> TRUMP_NAMES =
{
    "CLUBS",
    "DIAMONDS",
    "HEARTS",
    "SPADES"
};

static const std::vector <std::string> CARD_NAMES =
{
    "7",
    "8",
    "9",
    "10",
    "J",
    "Q",
    "K",
    "A"
};

Card::Card()
{
    m_ucColor = 69;
    m_ucPower = 69;
}

    /* Copy const*/
Card::Card(const Card& other)
{
    m_ucColor = other.m_ucColor;
    m_ucPower = other.m_ucPower;
}
Card::Card(unsigned char power, unsigned char color)
{
    m_ucColor = color;
    m_ucPower = power;
}

unsigned char Card::GetPower()const
{
    return m_ucPower;
}
unsigned char Card::GetColor()const
{
    return m_ucColor;
}

std::ostream& operator<<(ostream &out, const Card& c) //the cards is printed as (' name and color')
{
    if(!(static_cast<int>(c.GetColor() == 69)) && !(static_cast<int>(c.GetPower() == 69)))
    {
        out << "(" << CARD_NAMES.at(static_cast<int>(c.GetPower())) << ")" << "(" << TRUMP_NAMES.at(static_cast<int>(c.GetColor()))  << ")";
    }
    else 
    {   
        out << "(" << static_cast<int>(c.GetPower()) << ")" << "(" <<static_cast<int>(c.GetColor())  << ")";
    }
	
    return out;
}

Card& Card::operator=(const Card& other)
{
	if (this  ==  &other)
    {
        return *this;
    }

    this -> m_ucColor = other.m_ucColor;
    this -> m_ucPower = other.m_ucPower; // operator  =  same as above
    return *this;
}


bool Card::operator==(const Card& right) {
    return m_ucColor == right.m_ucColor && m_ucPower == right.m_ucPower;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     GamePlayer.cpp                                                                                      0000664 0001750 0001750 00000026064 14346341751 013464  0                                                                                                    ustar   default                         default                                                                                                                                                                                                                #include "GamePlayer.h"
#include <mutex>
#include <queue>

std::mutex mtx_queue;
std::mutex mtx_write_deal;
std::mutex mtx_reset_deal;
std::mutex mtx_update_combinations;

GamePlayer::GamePlayer()
{
    cerr << "Empty hands";
}


GamePlayer::GamePlayer(array<Card, HAND_SIZE> hand1, array<Card, HAND_SIZE> hand2, array<Card, HAND_SIZE> hand3, array<Card, HAND_SIZE> hand4, 
                        string announce, unsigned char threadNumber)
{
    m_ucThreadNumber = threadNumber; 
    m_vSecond_deal_combinations.resize(threadNumber);
    m_vSecond_deals.resize(threadNumber);

    m_aCards.at(0) = (hand1);
    m_aCards.at(1) = (hand2);
    m_aCards.at(2) = (hand3);
    m_aCards.at(3) = (hand4);
    sort_hands();

    m_ui64PlayedCombinations = 0;
    m_ui64first_deals = 0;

    m_ui64SavedDeals = 0;

    unsigned char colorTrump = ERROR;
    if(announce == "CLUBS")
    {
        colorTrump = CLUBS;
    }
    else if (announce == "DIAMONDS")
    {
        colorTrump = DIAMONDS;
    } 
    else if(announce == "HEARTS")
    {
        colorTrump = HEARTS;
    }
    else if(announce == "SPADES")
    {
        colorTrump = SPADES;
    }

    if (colorTrump != ERROR)
    {
        m_sAnnounce = "TRUMP";
    }
    else 
    {
        m_sAnnounce = announce;
    }

    m_helper.InitHelper(colorTrump);
   

    m_vFirst_deals_combinations.resize(EXPECTED_FIRST_DEAL_COMBINATIONS);
    m_vRemaining_cards.resize(EXPECTED_FIRST_DEAL_COMBINATIONS);

    for (int i = 0; i < threadNumber; i++)
    {
        m_vSecond_deal_combinations.at(i).resize(EXPECTED_SECOND_DEAL_COMBINATIONS);
        m_vSecond_deals.at(i) = 0;
    }
    
    m_vPlayed_deals.resize(EXPECTED_SAVED_DEAL_COMBINATIONS);
}

void GamePlayer::sort_hands()
{
    for(int hand = 0;  hand < m_aCards.size(); hand++)
    {
        m_helper.sort_hand(m_aCards.at(hand));        
    } 
}

/* brute force play combinations */ /*TODO MAKE CLASS*/
bool GamePlayer::play_recursive(unsigned char turn_id, const array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS>& all_hands, array<Card, NUMBER_OF_PLAYERS>& played_cards, 
                            array<array<Card, NUMBER_OF_PLAYERS>, HAND_SIZE> current_deal, const unsigned char first_cards, 
                            const unsigned char second_cards, unsigned char current_player, const unsigned char thread_id,
                            string announce)
{
    unsigned char playedCardsSize = m_helper.count_played_cards(played_cards);
    if(playedCardsSize == NUMBER_OF_PLAYERS)
    {
        current_player = m_helper.calculate_winner_of_deal(played_cards, announce);
        if(current_player == ERROR)
        {
            cerr << "NO WINNER OF DEAL" << endl; 
        }
        //"""TODO: create 24 bit deal bitarray + save + choose how much CPU"""
        current_deal.at((turn_id  + 1 ) / 4 - 1) = (played_cards); //turn id is counted from 0
        played_cards.fill(NULLCARD);
    } 

    if(first_cards != 0)
    {
        if(turn_id == NUMBER_OF_PLAYERS * first_cards)
        {
            m_vFirst_deals_combinations.at(m_ui64first_deals) = (current_deal);
            m_vRemaining_cards.at(m_ui64first_deals) = (all_hands);
            m_ui64first_deals++;

            return true;
        }
    }
    else if (second_cards != 0)
    {
        if(turn_id == NUMBER_OF_PLAYERS * second_cards)
        {
            mtx_update_combinations.lock();
            return_when_all_cards_played();
            m_vSecond_deal_combinations.at(thread_id).at(m_vSecond_deals.at(thread_id)) = (current_deal);
            m_vSecond_deals.at(thread_id)++;
            mtx_update_combinations.unlock();
            return true;
        }
    }
    else if (turn_id == NUMBER_OF_CARDS)
    {
        return_when_all_cards_played();
        return true;
    }
    current_player = current_player % 4;

    auto posibleOptions = m_helper.playable_by_hand_and_played_cards(announce, all_hands.at(current_player), played_cards);

    unsigned char posibleOptionsCount = m_helper.count_played_cards(posibleOptions);
    m_helper.sort_hand((posibleOptions));

    for (int i = 0; i < posibleOptionsCount; i++ )
    {
        array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS > newHands = all_hands;
        newHands.at(current_player).at(m_helper.findCard(newHands.at(current_player), posibleOptions.at(i))) = NULLCARD;

        array<Card, NUMBER_OF_PLAYERS> newPlayed_cards = played_cards;

        newPlayed_cards.at(m_helper.count_played_cards(newPlayed_cards)) = (posibleOptions.at(i));
        m_helper.sort_hand(newHands.at(current_player)); 
        play_recursive((turn_id + 1), newHands, newPlayed_cards, current_deal, first_cards, second_cards, current_player + 1, thread_id, announce); // play the next card
    }
    return true;
}

/* manager of brute force */
void GamePlayer::play_deals_fast(ThreadData data)
{
    unsigned char first_cards = data.first_cards;
    unsigned char second_cards = data.second_cards;
    long long deal_id = data.deal_id;
    unsigned char thread_id = data.thread_id;
    string announce = data.announce;

    play_deals_fast(first_cards, second_cards, deal_id, thread_id, announce);
}

void GamePlayer::play_deals_fast(unsigned char first_cards, unsigned char second_cards, long long deal_id, unsigned char thread_id,
                                 string announce)
{
    string current_announce;
    if(announce != "n/a")
    {
        current_announce = announce;
    }
    else
    {
        current_announce = m_sAnnounce;
    }
    sort_hands();

    array<Card, NUMBER_OF_PLAYERS> tempPlayedCards;
    array<array<Card, NUMBER_OF_PLAYERS>, HAND_SIZE> tempCurrentDeal;
    if(second_cards != 0)
    {
        if(deal_id == -1) //if no deal id
        {
            for(int i = 0 ; i < NUMBER_OF_PLAYERS ; i++)
            {
                while(m_helper.count_played_cards((m_aCards).at(i)) > second_cards)
                {
                    (m_aCards).at(i).at(m_helper.count_played_cards((m_aCards).at(i)) - 1) = NULLCARD ;
                }
            }
        }
        else
        {
            play_recursive(0, m_vRemaining_cards.at(deal_id), tempPlayedCards, tempCurrentDeal, 
                            first_cards, second_cards, 0, thread_id, current_announce);
            return;
        }
    }
    play_recursive(0, m_aCards, tempPlayedCards, tempCurrentDeal, first_cards, second_cards, 0, 0, current_announce);
}

/* separator between methods */
void GamePlayer::play_separated_to_x_then_y(unsigned char firstCardCount)
{
    auto start = chrono::steady_clock::now();
    int totalmaxed = 0 ;
    play_deals_fast(firstCardCount);
    long long FirstDealsCount = GetFirstDealsCount();
    cout << "Played Deals with "<< (int)firstCardCount << " : " << FirstDealsCount << endl;

    std::queue<int> RemainingDealIDs;
    for (int i = 0; i < FirstDealsCount; i++)
    {
        RemainingDealIDs.push(i);
    }

    vector<ThreadData> current_data;
    current_data.resize(m_ucThreadNumber);
    vector<thread> threads;
    threads.resize(m_ucThreadNumber);

    int thread_id = 0;
    for (thread_id = 0; thread_id < m_ucThreadNumber; thread_id++)
    { 
        current_data.at(thread_id).second_cards = HAND_SIZE - firstCardCount;
        current_data.at(thread_id).first_cards = 0;
        current_data.at(thread_id).thread_id = thread_id;
        current_data.at(thread_id).announce = m_sAnnounce;

        std::thread tempThread([&]
        { 
            while(true)
            {
                mtx_queue.lock();
                if(RemainingDealIDs.empty())
                {
                    mtx_queue.unlock();
                    break;
                }
                int currentDealID = RemainingDealIDs.front();
                RemainingDealIDs.pop();
                current_data.at(thread_id).deal_id = currentDealID;
                mtx_queue.unlock();
                
                play_deals_fast(current_data.at(thread_id));

                long long SecondDealsCount = GetSecondDealsCount(thread_id);
                for(long long i = 0; i < SecondDealsCount; i++)
                {
                    array<array<Card, NUMBER_OF_PLAYERS>, HAND_SIZE> mergedDeal;
                    for(unsigned char j = 0 ; j < HAND_SIZE; j++)
                    {
                        if(j < firstCardCount)
                        {
                            mergedDeal.at(j) = m_vFirst_deals_combinations.at(currentDealID).at(j);
                        }
                        else 
                        {
                            mergedDeal.at(j) = m_vSecond_deal_combinations.at(thread_id).at(i).at(j);
                        }
                    }

                    mtx_write_deal.lock();

                    if(GetSavedDealsCount() >= EXPECTED_SAVED_DEAL_COMBINATIONS)
                    {
                        totalmaxed ++;
                        m_ui64SavedDeals = 0;
                    }
                    
                    m_vPlayed_deals.at(GetSavedDealsCount()) = (mergedDeal);

                    m_ui64SavedDeals++;

                    mtx_write_deal.unlock();                     
                }
                mtx_reset_deal.lock();
                m_vSecond_deals.at(thread_id) = 0; 
                mtx_reset_deal.unlock(); 
                if(GetPlayedCombinationsCount() % 10 == 1)
                {
                    auto end = chrono::steady_clock::now();
                    cout << "Current Played: " << GetSavedDealsCount() << " time:" << chrono::duration_cast<chrono::seconds>(end - start).count() << " " << totalmaxed <<  endl;
                    
                    cout << "Remaining recursive calls : " << RemainingDealIDs.size()  << endl;                        
                }
            }
            
        });
        threads.at(thread_id) = std::move(tempThread);
    }

    thread_id = 0;

    for (thread_id = 0; thread_id < m_ucThreadNumber; thread_id++)
    {
        threads.at(thread_id).join();
    }
    auto end = chrono::steady_clock::now();
    cout << "Current Played: " << GetSavedDealsCount() << " time:" << chrono::duration_cast<chrono::seconds>(end - start).count() << " " << totalmaxed <<  endl;
    FirstDealsCount = FirstDealsCount - m_ucThreadNumber;
    cout << "Remaining recursive calls : " << FirstDealsCount - m_ucThreadNumber << endl;
}


long long GamePlayer::GetFirstDealsCount()
{
    return m_ui64first_deals;
}

long long GamePlayer::GetSecondDealsCount(unsigned char thread_id)
{
    return m_vSecond_deals.at(thread_id);
}

long long GamePlayer::GetSavedDealsCount()
{
    return m_ui64SavedDeals;
}

long long GamePlayer::GetPlayedCombinationsCount()
{
    return m_ui64PlayedCombinations;
}


/* final results */
bool GamePlayer::return_when_all_cards_played()
{
    m_ui64PlayedCombinations ++;

    if (GetPlayedCombinationsCount() % EXPECTED_FIRST_DEAL_COMBINATIONS == 0 )
    {
        cout << GetPlayedCombinationsCount() << endl;
    }
    return true;
}

void GamePlayer::PrintHands()
{
    for (int i = 0; i < m_aCards.size(); i++)
    {
        for (int j = 0; j < m_aCards.at(i).size(); j++)
        {
            cout << m_aCards.at(i).at(j) << " ";
        }
        cout << endl;
    }
}

bool GamePlayer::StartProcessing()
{
    play_separated_to_x_then_y(3);
    return true;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                            Helper.cpp                                                                                          0000664 0001750 0001750 00000025455 14346336727 012667  0                                                                                                    ustar   default                         default                                                                                                                                                                                                                #include "Helper.h"
using namespace std;

Helper::Helper()
{
    color_trump = ERROR;
}

bool Helper::InitHelper(unsigned char color)
{
    color_trump = color;
    return true;
}

/* first compare color than power */
bool Helper::compare_cards(Card card1, Card card2)
{
    if (card1.GetColor() > card2.GetColor())
    {
        return true;
    }
    else if (card1.GetColor() == card2.GetColor())
    {
        return card1.GetPower() > card2.GetPower();
    }
    return false;
}

unsigned char Helper::findCard(array<Card, HAND_SIZE> hand, Card card)
{
    if(card.GetColor() == HEARTS || card.GetColor() == SPADES)
    {
        for(int i = HAND_SIZE - 1; i >= 0 ; i--)
        {
            if(hand.at(i) == card)
            {
                return i;
            }
        }
    }
    else
    {
        for(int i = 0; i < HAND_SIZE ;i++)
        {
            if(hand.at(i) == card)
            {
                return i;
            }
        }
    }

    cerr << "Cannot FIND CARD" << endl;
    return ERROR;
}

bool Helper::compare_cards_power(Card card1, Card card2 , string announce)
{
    if(announce.empty())
    {
        cerr << "NO ANNOUNCE" << endl;
    }
        // any card is bigger than the nullcard
    if(card1.GetColor() == NULLCARD.GetColor())
    {
        return false;
    }

    if(card2.GetColor() == NULLCARD.GetColor())
    {           
        return true;
    }

    unsigned char points_first = ALL_TRUMP_POINTS.at(card1.GetPower());
    unsigned char points_second = ALL_TRUMP_POINTS.at(card2.GetPower()); // all trump by default 
    if (announce == "NO_TRUMP")
    {
        unsigned char points_first = NO_TRUMP_POINTS.at(card1.GetPower());
        unsigned char points_second = NO_TRUMP_POINTS.at(card2.GetPower());
    }

    if (points_first > points_second)
    {
        return true;
    }
    else if (points_first == points_second)
    {
        return card1.GetPower() > card2.GetPower();
    }
    return false;
}

bool Helper::search_by_color(const array<Card, HAND_SIZE>& hand, unsigned char color)
{
    for(int i = 0; i < HAND_SIZE; i++)
    {
        if (hand.at(i).GetColor() == color)
        {
            return true;
        }
    }
    return false;
}

array <Card, HAND_SIZE> Helper::playableNoTrump
(const array<Card, HAND_SIZE>& hand, const array<Card , NUMBER_OF_PLAYERS>& playedCards)
{
    unsigned char optionsCount = 0 , playedCardsSize = count_played_cards(playedCards);
    array <Card, HAND_SIZE> possible_options;
    if(!search_by_color(hand, playedCards.at(0).GetColor()))
    {
        return hand;
    }

    for (int i = 0; i < HAND_SIZE; i++)
    {
        if(playedCards.at(0).GetColor() == hand.at(i).GetColor())
        {
            possible_options.at(optionsCount) = (hand.at(i));
            optionsCount++;
        }
    }

    if(optionsCount == 0)
    {
        return hand;
    }
    else
    {
        for (int i = optionsCount; i < HAND_SIZE; i++)
        {
            possible_options.at(i) = NULLCARD;//filling with empty options
        }
    }        
    return possible_options;
}

array <Card, HAND_SIZE> Helper::playableAllTrump
(const array<Card, HAND_SIZE>& hand, const array<Card , NUMBER_OF_PLAYERS>& playedCards)
{
    unsigned char optionsCount = 0 , playedCardsSize = count_played_cards(playedCards);
    array <Card, HAND_SIZE> possible_options;
    string announce = "ALL_TRUMP";

    if(!search_by_color(hand, playedCards.at(0).GetColor()))
    {
        return hand;
    }

    bool have_bigger_trump = false;
    for (int i = 0; i < HAND_SIZE; i++)
    {
        if(playedCards.at(0).GetColor() == hand.at(i).GetColor() && compare_cards_power(hand.at(i), playedCards.at(playedCardsSize - 1), announce))
        {
            have_bigger_trump = true;
        }
    }

    for (int i = 0; i < HAND_SIZE; i++)
    {
        if(have_bigger_trump)
        {
            if(playedCards.at(0).GetColor() == hand.at(i).GetColor() && compare_cards_power(hand.at(i), playedCards.at(playedCardsSize - 1), announce))
            {
                possible_options.at(optionsCount) = (hand.at(i));
                optionsCount++;
            }
        }
        else if(playedCards.at(0).GetColor() == hand.at(i).GetColor())
        {
            possible_options.at(optionsCount) = (hand.at(i));
            optionsCount++;
        }
    }

    if(optionsCount == 0)
    {
        
        if (!have_bigger_trump)
        cerr << ( " BAD ALL TRUMP LOGIC problem \n \n");
        return hand;
    }
    else
    {
        for (int i = optionsCount; i < HAND_SIZE; i++)
        {
            possible_options.at(i) = NULLCARD;       //filling with empty options
        }
    }
    return possible_options;
}

array <Card, HAND_SIZE> Helper::playableTrump
(const array<Card, HAND_SIZE>& hand, const array<Card , NUMBER_OF_PLAYERS>& playedCards)
{
    unsigned char optionsCount = 0 , playedCardsSize = count_played_cards(playedCards);
    array <Card, HAND_SIZE> possible_options;
    if(color_trump == ERROR)
    {
        cerr << "NO TRUMP COLOR!" << endl;
    }

    if (!search_by_color(hand, playedCards.at(0).GetColor())) // if you dont have card with same color of first one
    {
        Card last_trump(0,ERROR);
        for (int i = 0; i < NUMBER_OF_PLAYERS; i++)
        {
            if(playedCards.at(i).GetColor() == color_trump && playedCards.at(i).GetPower() >= last_trump.GetPower())
            {
                last_trump = playedCards.at(i); // check if someone used trump
            }
        }

        bool teammate_trump = false;
        if(playedCardsSize > 2)
        {
            if(last_trump.GetPower() != playedCards.at(playedCardsSize - 2).GetPower())
            {
                teammate_trump = true;
            }
        }
        if(last_trump.GetColor() != ERROR && !teammate_trump) 
        {   
                    // if someone used trump and u have highter u must use it unless it's your teammate
            for(int i = 0; i < HAND_SIZE; i++)
            {
                if (hand.at(i).GetColor() == color_trump && hand.at(i).GetPower() > last_trump.GetPower())
                {
                    possible_options.at(optionsCount) = (hand.at(i));
                    optionsCount++;
                }
            }
        }
        else if(last_trump.GetColor() == ERROR)
        {
            for(int i = 0; i < HAND_SIZE; i++)
            {
                if (hand.at(i).GetColor() == color_trump)
                {
                    possible_options.at(optionsCount) = (hand.at(i));
                    optionsCount++;
                }
            }
        }

        if(optionsCount != 0)
        {
            for (int i = optionsCount; i < HAND_SIZE; i++)
            {
                possible_options.at(i) = NULLCARD;//filling with empty options
            }
            return possible_options;
        }
        return hand;
    }
    else if(playedCards.at(0).GetColor() == color_trump) // if you have card with same color an play on trump all trump logic aplies
    {
        return playableAllTrump(hand, playedCards);
    }
    else // if you have card with same color an play other than trump no trump logic aplies
    {
        return playableNoTrump(hand, playedCards);
    }
}

array <Card, HAND_SIZE> Helper::playable_by_hand_and_played_cards
(const string& announce, const array<Card, HAND_SIZE>& hand, const array<Card , NUMBER_OF_PLAYERS>& playedCards)
{
    if (announce.empty())
    {
        cerr << "NO ANNOUNCE" << endl;
    }
    
    if ((playedCards.at(0).GetColor() == NULLCARD.GetColor()) && (playedCards.at(0).GetPower() == NULLCARD.GetPower()))
    {
        return hand;
    }

    if (announce == "ALL_TRUMP")
    {
        return playableAllTrump(hand, playedCards);
    }
    if (announce == "NO_TRUMP")
    {
        return playableNoTrump(hand, playedCards);
    }
    if (announce == "TRUMP")
    {
        return playableTrump(hand, playedCards);
    }
    
    cerr << "INVALID ANNOUNCE: " << announce <<  endl;
    return hand;
}



unsigned char Helper::calculate_winner_of_deal(const array<Card , NUMBER_OF_PLAYERS>& playedCards,const string& announce)
{
    Card current_winner = playedCards.at(0);
    if(announce == "TRUMP")
    {
        for (int i = 0; i < NUMBER_OF_PLAYERS; i++)
        {
            if (playedCards.at(i).GetColor() == color_trump && current_winner.GetColor() != color_trump)
            {
                current_winner = playedCards.at(i);
            }
            else if (playedCards.at(i).GetColor() == color_trump && current_winner.GetColor() == color_trump)
            {
                if(compare_cards_power(playedCards.at(i), current_winner, "ALL_TRUMP"))
                {
                    current_winner = playedCards.at(i);
                }
            }
            else if (playedCards.at(i).GetColor() != color_trump && current_winner.GetColor() != color_trump)
            {
                if(compare_cards_power(playedCards.at(i), current_winner, "NO_TRUMP"))
                {
                    current_winner = playedCards.at(i);
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < NUMBER_OF_PLAYERS; i++)
        {
            if (compare_cards_power(playedCards.at(i), current_winner, announce))
            {
                current_winner = playedCards.at(i);
            }
        }
    }
    
    for (int winningCardID = 0 ; winningCardID < NUMBER_OF_PLAYERS; winningCardID++)
    {
        if(playedCards.at(winningCardID).GetColor() == current_winner.GetColor() && playedCards.at(winningCardID).GetPower() == current_winner.GetPower())
        {
            return winningCardID;
        }
    }
    cerr << "NO WINNER OF DEAL" << endl;
    return ERROR;
}

unsigned char Helper::count_played_cards(const array<Card , NUMBER_OF_PLAYERS>& playedCards)
{
    unsigned char playedCardsSize = 0 ;
    for(int i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        if(playedCards.at(i).GetColor() != NULLCARD.GetColor() && playedCards.at(i).GetPower() != NULLCARD.GetPower())
        {
            playedCardsSize++;
        }
    }
    return playedCardsSize;
}

unsigned char Helper::count_played_cards(const array<Card , HAND_SIZE>& playedCards)
{
    unsigned char playedCardsSize = 0 ;
    for(int i = 0; i < HAND_SIZE; i++)
    {
        if(playedCards.at(i).GetColor() != NULLCARD.GetColor() && playedCards.at(i).GetPower() != NULLCARD.GetPower())
        {
            playedCardsSize++;
        }
    }
    return playedCardsSize;
}

bool Helper::sort_hand(array<Card , HAND_SIZE>& hand)
{
    int i, j;
    for (i = 1; i < HAND_SIZE; i++)
    {
        Card key = hand.at(i);
        j = i - 1;

        // Move elements of arr[0..i-1], 
        // that are greater than key, to one
        // position ahead of their
        // current position
        while (j >= 0 && compare_cards(hand.at(j), key))
        {
            hand.at(j + 1) = hand.at(j);
            j = j - 1;
        }
        hand.at(j + 1) = key;
    }

    return true;
}                                                                                                                                                                                                                   Main.cpp                                                                                            0000664 0001750 0001750 00000001354 14346333120 012304  0                                                                                                    ustar   default                         default                                                                                                                                                                                                                #include "GamePlayer.h"
using namespace std;

int main()
{
    vector<Card> deck;
    for (int i = 0; i <= SPADES; i++ )
    {
        for (int j = 0; j <= Ace; j++ )
        {
            deck.push_back(Card(j, i));
        }
    }
    std::srand(std::time(0));
    random_shuffle(deck.begin(), deck.end());
    std::array<Card, HAND_SIZE> hand1, hand2, hand3, hand4;
    
    for(int i = 0; i < HAND_SIZE; i++)
    {
        hand1.at(i) = deck.at(HAND_SIZE * 0 + i);
        hand2.at(i) = deck.at(HAND_SIZE * 1 + i);
        hand3.at(i) = deck.at(HAND_SIZE * 2 + i);
        hand4.at(i) = deck.at(HAND_SIZE * 3 + i);
    }

    GamePlayer AI(hand1, hand2, hand3, hand4, "CLUBS", 8) ;
    AI.PrintHands();
    AI.StartProcessing();
    return 0;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
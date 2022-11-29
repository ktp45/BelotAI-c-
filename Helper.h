#include "constants.h"
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

    array <Card, HAND_SIZE> playable_by_hand_and_played_cards 
    (const string& announce,const array<Card, NUMBER_OF_DEALS>& hand, const array<Card , NUMBER_OF_PLAYERS>& playedCards);
    //       """Logic about which cards are allowed to be played"""
    
    unsigned char calculate_winner_of_deal(const array<Card , NUMBER_OF_PLAYERS>& playedCards,const string& announce);

    unsigned char count_played_cards(const array<Card , NUMBER_OF_PLAYERS>& playedCards);

    unsigned char count_played_cards(const array<Card , HAND_SIZE>& playedCards);

    bool sort_hand(array<Card , HAND_SIZE>& hand);
};

#include "constants.h"
#include <algorithm>
#include <iterator>
using namespace std;
class Helper
{
    unsigned char color_trump;
    public:
    Helper();
    Helper(unsigned char);
    bool compare_cards(pair <unsigned char, unsigned char> card1, pair <unsigned char, unsigned char> card2);
    //"""Simple compare for cards by color and power""" True if first > second

    bool compare_cards_power(pair <unsigned char, unsigned char> card1, pair <unsigned char, unsigned char> card2 , string announce);
    // True if first > second

    bool search_by_color(vector<pair <unsigned char, unsigned char>> hand, unsigned char color);
    // """Return True if the color is in the hand else False"""

    vector<pair <unsigned char, unsigned char>>  playable_by_hand_and_played_cards 
    (string announce,  vector<pair <unsigned char, unsigned char>> hand, vector<pair <unsigned char, unsigned char>> playedCards);
    //       """Logic about which cards are allowed to be played"""
    
    unsigned char calculate_winner_of_deal(vector<pair <unsigned char, unsigned char>> playedCards, string announce);
};

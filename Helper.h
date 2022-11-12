#include "constants.h"
#include "Card.h"
#include <algorithm>
#include <iterator>
using namespace std;
class Helper
{
    unsigned char color_trump;
    public:
    Helper();
    bool InitHelper(unsigned char);

    unsigned char findCard(vector<Card> hand, Card card);
    //clever find

    bool compare_cards(Card card1, Card card2);
    //"""Simple compare for cards by color and power""" True if first > second

    bool compare_cards_power(Card card1, Card card2 , string announce);
    // True if first > second

    bool search_by_color(const vector<Card>& hand, unsigned char color);
    // """Return True if the color is in the hand else False"""

    vector<Card>  playable_by_hand_and_played_cards 
    (const string& announce,  const vector<Card>& hand,const vector<Card>& playedCards);
    //       """Logic about which cards are allowed to be played"""
    
    unsigned char calculate_winner_of_deal(const vector<Card>& playedCards, const string& announce);

    //helper compare func
    bool equalCards(Card card1, Card card2);
};

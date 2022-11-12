#include "GamePlayer.h"
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
    random_shuffle(deck.begin(), deck.end());
    std::vector<Card> hand1(deck.begin(), deck.begin() + NUMBER_OF_DEALS);
    std::vector<Card> hand2(deck.begin() + NUMBER_OF_DEALS, deck.begin() + 2 * NUMBER_OF_DEALS);
    std::vector<Card> hand3(deck.begin() + 2 * NUMBER_OF_DEALS, deck.begin() + 3 * NUMBER_OF_DEALS);
    std::vector<Card> hand4(deck.begin() + 3 * NUMBER_OF_DEALS, deck.end());
    GamePlayer AI(hand1, hand2, hand3, hand4, "CLUBS") ;
    AI.StartProcessing();
    return 0;
}
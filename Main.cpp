#include "CurrentDeal.h"
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

    // GamePlayer AI(hand1, hand2, hand3, hand4, "CLUBS", 8) ;
    // AI.PrintHands();
    // AI.StartProcessing();.

    CurrentDeal deal({hand1, hand2, hand3, hand4}, "CLUBS");
    deal.Start();
    cout << "Result is " << deal.GetResult() << endl; 
    return 0;
}
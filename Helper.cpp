#include "Helper.h"
using namespace std;

Helper::Helper()
{
    color_trump = 69;
};

bool Helper::InitHelper(unsigned char color)
{
    color_trump = color;
    return true;
};


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
};

unsigned char Helper::findCard(vector<Card> hand, Card card)
{
    if(card.GetColor() == HEARTS || card.GetColor() == SPADES)
    {
        for(int i = hand.size() - 1; i >= 0 ; i--)
        {
            if(hand.at(i) == card)
                {
                    return i;
                }
        }
    }
    else
    {
        unsigned char size = hand.size();
        for(int i = 0; i < size ;i++)
        {
            if(hand.at(i) == card)
                {
                    return i;
                }
        }
    }

    return 69;
}

bool Helper::compare_cards_power(Card card1, Card card2 , string announce)
{
    if(announce.empty())
    {
        cerr << "NO ANNOUNCE" << endl;
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
        return card1.GetPower() > card2.GetColor();
    }
    return false;
}

bool Helper::search_by_color(const vector<Card>& hand, unsigned char color)
{
    int size = hand.size();
    for(int i = 0; i < size; i++)
    {
        if (hand.at(i).GetColor() == color)
        {
            return true;
        }
    }
    return false;
}

vector<Card>  Helper::playable_by_hand_and_played_cards
(const string& announce,const vector<Card>& hand, const vector<Card>& playedCards)
{
    if (announce.empty())
    {
        cerr << "NO ANNOUNCE" << endl;
    }

    if (playedCards.empty())
    {
        return hand;
    }

    vector<Card> possible_options;

    if (announce == "ALL_TRUMP")
    {
        if(!search_by_color(hand, playedCards.at(0).GetColor()))
        {
            return hand;
        }
        unsigned char hand_size = hand.size();
        for (int i = 0; i < hand_size; i++)
        {
            if(playedCards.at(0).GetColor() == hand.at(i).GetColor() && compare_cards_power(hand.at(i), playedCards.back(), announce))
            {
                possible_options.push_back(hand.at(i));
            }
        }

        if(possible_options.empty())
        {
            return hand;
        }
        return possible_options;
    }
    if (announce == "NO_TRUMP")
    {
        if(!search_by_color(hand, playedCards.at(0).GetColor()))
        {
            return hand;
        }
        unsigned char hand_size = hand.size();
        for (int i = 0; i < hand_size; i++)
        {
            if(playedCards.at(0).GetColor() == hand.at(i).GetColor())
            {
                possible_options.push_back(hand.at(i));
            }
        }

        if(possible_options.empty())
        {
            return hand;
        }
        return possible_options;
    }
    if (announce == "TRUMP")
    {
        if(color_trump == 69)
        {
            cerr << "NO TRUMP COLOR!" << endl;
        }

        if (!search_by_color(hand, playedCards.at(0).GetColor())) // if you dont have card with same color of first one
        {
            Card last_trump(0,69);
            unsigned char playedCardsSize = playedCards.size();
            for (int i = 0; i < playedCardsSize; i++)
            {
                if(playedCards.at(i).GetColor() == color_trump && playedCards.at(i).GetPower() >= last_trump.GetPower())
                {
                    last_trump = playedCards.at(i); // check if someone used trump
                }
            }

            bool teammate_trump = false;
            if(playedCards.size() > 2)
            {
                if(last_trump.GetPower() != playedCards.at(playedCardsSize - 2).GetPower())
                {
                    teammate_trump = true;
                }
            }
            if(last_trump.GetColor() != 69 && !teammate_trump) 
            {   
                unsigned char hand_size = hand.size(); // if someone used trump and u have highter u must use it unless it's your teammate
                for(int i = 0; i < hand_size; i++)
                {
                    if (hand.at(i).GetColor() == color_trump && hand.at(i).GetPower() > last_trump.GetPower())
                    {
                        possible_options.push_back(hand.at(i));
                    }
                }
            }
            else if(last_trump.GetColor() == 69)
            {
                unsigned char hand_size = hand.size();
                for(int i = 0; i < hand_size; i++)
                {
                    if (hand.at(i).GetColor() == color_trump)
                    {
                        possible_options.push_back(hand.at(i));
                    }
                }
            }
            if(!possible_options.empty())
            {
                return possible_options;
            }
            return hand;
        }
        else if(playedCards.at(0).GetColor() == color_trump) // if you have card with same color an play on trump all trump logic aplies
        {
            return playable_by_hand_and_played_cards("ALL_TRUMP", hand, playedCards);
        }
        else // if you have card with same color an play other than trump no trump logic aplies
        {
            return playable_by_hand_and_played_cards("NO_TRUMP", hand, playedCards);
        }
    }
    cerr << "INVALID ANNOUNCE: " << announce <<  endl;
    return possible_options;
}

unsigned char Helper::calculate_winner_of_deal(const vector<Card>& playedCards,const string& announce)
{
    Card current_winner = playedCards.at(0);
    if(announce == "TRUMP")
    {
        unsigned char playedCardsSize = playedCards.size();
        for (int i = 0; i < playedCardsSize; i++)
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
        unsigned char playedCardsSize = playedCards.size();
        for (int i = 0; i < playedCardsSize; i++)
        {
            if (compare_cards_power(playedCards.at(i), current_winner, announce))
            {
                current_winner = playedCards.at(i);
            }
        }
    }
    
    return distance(playedCards.begin(), playedCards.begin() + (int)findCard(playedCards, current_winner));
}


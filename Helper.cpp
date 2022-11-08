    #include "Helper.h"
    using namespace std;

    Helper::Helper()
    {
        color_trump = 69;
    };

    Helper::Helper(unsigned char color)
    {
        color_trump = color;
    };


    bool Helper::compare_cards(pair <unsigned char, unsigned char> card1, pair <unsigned char, unsigned char> card2)
    {
        if (card1.second > card2.second)
        {
            return true;
        }
        else if (card1.second == card2.second)
        {
            return card1.first > card2.first;
        }
        return false;
    };

    bool Helper::compare_cards_power(pair <unsigned char, unsigned char> card1, pair <unsigned char, unsigned char> card2 , string announce)
    {
        if(announce.empty())
        {
            cerr << "NO ANNOUNCE" << endl;
        }

        unsigned char points_first = ALL_TRUMP_POINTS[card1.first];
        unsigned char points_second = ALL_TRUMP_POINTS[card2.first]; // all trump by default 
        if (announce == "NO_TRUMP")
        {
            unsigned char points_first = NO_TRUMP_POINTS[card1.first];
            unsigned char points_second = NO_TRUMP_POINTS[card2.first];
        }

        if (points_first > points_second)
        {
            return true;
        }
        else if (points_first == points_second)
        {
            return card1.first > card2.second;
        }
        return false;
    }

    bool Helper::search_by_color(vector<pair <unsigned char, unsigned char>> hand, unsigned char color)
    {
        int size = hand.size();
        for(int i = 0; i < size; i++)
        {
            if (hand[i].second == color)
            {
                return true;
            }
        }
        return false;
    }

    vector<pair <unsigned char, unsigned char>>  Helper::playable_by_hand_and_played_cards
    (string announce,  vector<pair <unsigned char, unsigned char>> hand, vector<pair <unsigned char, unsigned char>> playedCards)
    {
        if (announce.empty())
        {
            cerr << "NO ANNOUNCE" << endl;
        }

        if (playedCards.empty())
        {
            return hand;
        }

        vector<pair <unsigned char, unsigned char>> possible_options;

        if (announce == "ALL_TRUMP")
        {
            if(!search_by_color(hand, playedCards[0].second))
            {
                return hand;
            }
            unsigned char hand_size = hand.size();
            for (int i; i < hand_size; i++)
            {
                if(playedCards[0].second == hand[i].second && compare_cards_power(hand[i], playedCards.back(), announce))
                {
                    possible_options.push_back(hand[i]);
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
            if(!search_by_color(hand, playedCards[0].second))
            {
                return hand;
            }
            unsigned char hand_size = hand.size();
            for (int i; i < hand_size; i++)
            {
                if(playedCards[0].second == hand[i].second)
                {
                    possible_options.push_back(hand[i]);
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

            if (!search_by_color(hand, playedCards[0].second)) // if you dont have card with same color of first one
            {
                pair <unsigned char, unsigned char> last_trump = {0, 69};
                unsigned char playedCardsSize = playedCards.size();
                for (int i = 0; i < playedCardsSize; i++)
                {
                    if(playedCards[i].second == color_trump && playedCards[i].first >= last_trump.first)
                    {
                        last_trump = playedCards[i]; // check if someone used trump
                    }
                }

                if(last_trump.second != 69 && last_trump.first != playedCards[playedCardsSize - 2].first) 
                {
                    unsigned char hand_size = hand.size(); // if someone used trump and u have highter u must use it unless it's your teammate
                    for(int i = 0; i < hand_size; i++)
                    {
                        if (hand[i].second == color_trump && hand[i].first > last_trump.first)
                        {
                            possible_options.push_back(hand[i]);
                        }
                    }
                }
                else if(last_trump.second == 69)
                {
                    unsigned char hand_size = hand.size();
                    for(int i = 0; i < hand_size; i++)
                    {
                        if (hand[i].second == color_trump)
                        {
                            possible_options.push_back(hand[i]);
                        }
                    }
                }

                if(!possible_options.empty())
                {
                    return possible_options;
                }
                return hand;
            }
            else if(playedCards[0].second == color_trump) // if you have card with same color an play on trump all trump logic aplies
            {
                return playable_by_hand_and_played_cards(to_string("ALL_TRUMP"), hand, playedCards);
            }
            else // if you have card with same color an play other than trump no trump logic aplies
            {
                return playable_by_hand_and_played_cards(to_string("NO_TRUMP"), hand, playedCards);
            }
        }
    }
    
    unsigned char Helper::calculate_winner_of_deal(vector<pair <unsigned char, unsigned char>> playedCards, string announce)
    {
        pair <unsigned char, unsigned char> current_winner = playedCards[0];
        if(announce == "TRUMP")
        {
            unsigned char playedCardsSize = playedCards.size();
            for (int i = 0; i < playedCardsSize; i++)
            {
                if (playedCards[i].second == color_trump && current_winner.second != color_trump)
                {
                    current_winner = playedCards[i];
                }
                else if (playedCards[i].second == color_trump && current_winner.second == color_trump)
                {
                    if(compare_cards_power(playedCards[i], current_winner, "ALL_TRUMP"))
                    {
                        current_winner = playedCards[i];
                    }
                }
                else if (playedCards[i].second != color_trump && current_winner.second != color_trump)
                {
                    if(compare_cards_power(playedCards[i], current_winner, "NO_TRUMP"))
                    {
                        current_winner = playedCards[i];
                    }
                }
            }
        }
        else
        {
            unsigned char playedCardsSize = playedCards.size();
            for (int i = 0; i < playedCardsSize; i++)
            {
                if (compare_cards_power(playedCards[i], current_winner, announce))
                {
                    current_winner = playedCards[i];
                }
            }
        }
        auto it = std::find(playedCards.begin(), playedCards.end(), current_winner);
        return distance(playedCards.begin(), it);
    }

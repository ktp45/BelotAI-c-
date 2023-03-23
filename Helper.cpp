#include "Helper.h"
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

unsigned char Helper::findCard(array<Card, HAND_SIZE> hand, Card card, unsigned char handSize, bool sortedHands)
{
    if(!sortedHands)
    {
        handSize = HAND_SIZE;
    }

    if(card.GetColor() == HEARTS || card.GetColor() == SPADES)
    {
        for(int i = handSize - 1; i >= 0 ; i--)
        {
            if((hand.at(i).GetColor() < card.GetColor()) && sortedHands)
            {
                return ERROR;
            }
            
            if(hand.at(i) == card)
            {
                return i;
            }
        }
    }
    else
    {
        for(int i = 0; i < handSize ;i++)
        {
            if((hand.at(i).GetColor() > card.GetColor()) && sortedHands)
            {
                return ERROR;
            }

            if(hand.at(i) == card)
            {
                return i;
            }
        }
    }

    return ERROR;
}

bool Helper::compare_cards_power(Card card1, Card card2 , string announce)
{
    if(announce.empty())
    {
        cerr << "NO ANNOUNCE" << endl;
    }
        // any card is bigger than the nullcard
    if(card1.GetColor() == NULLCARD.GetColor() || card1.GetPower() == NULLCARD.GetPower())
    {
        return false;
    }

    if(card2.GetColor() == NULLCARD.GetColor()|| card2.GetPower() == NULLCARD.GetPower())
    {           
        return true;
    }

    unsigned char points_first = ALL_TRUMP_POINTS.at(card1.GetPower());
    unsigned char points_second = ALL_TRUMP_POINTS.at(card2.GetPower()); // all trump by default 
    if (announce == "NO_TRUMP")
    {
        points_first = NO_TRUMP_POINTS.at(card1.GetPower());
        points_second = NO_TRUMP_POINTS.at(card2.GetPower());
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

        while (j >= 0 && compare_cards(hand.at(j), key))
        {
            hand.at(j + 1) = hand.at(j);
            j = j - 1;
        }
        hand.at(j + 1) = key;
    }

    return true;
}

bool Helper::sort_by_power(vector<Card>& hand, string announce)
{
    try
    {
        int j;
        for (int i = 1; i < HAND_SIZE; i++)
        {
            Card key = hand.at(i);
            j = i - 1;

            while (j >= 0 && compare_cards_power(hand.at(j) , key, announce))
            {
                hand.at(j + 1) = hand.at(j);
                j = j - 1;
            }
            hand.at(j + 1) = key;
        }
    }
    catch (const std::out_of_range &e)
    {
        for (int i = 0; i < HAND_SIZE; i++)
        {
            cout << hand.at(i) << " ";
        }
        cout << endl;
        cerr << "sort_by_power" << endl;
        throw __throw_out_of_range;
    }
    return true;
}

bool Helper::isNULLHAND(const array<Card , HAND_SIZE>& hand)
{
    for(int i = 0 ; i < HAND_SIZE; i++)
    {
        if((hand.at(i).GetColor() != NULLCARD.GetColor()) && (hand.at(i).GetPower() != NULLCARD.GetPower()))
        {
            return false;
        }  
    }

    return true;
}
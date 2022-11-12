#include "GamePlayer.h"

GamePlayer::GamePlayer()
{
    cerr << "Empty hands";
}


GamePlayer::GamePlayer(vector<Card> hand1, vector<Card> hand2, vector<Card> hand3, vector<Card> hand4, string announce)
{
    m_vCards.push_back(hand1);
    m_vCards.push_back(hand2);
    m_vCards.push_back(hand3);
    m_vCards.push_back(hand4);
    playedCombinations = 0;
    
    unsigned char colorTrump = 69;
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

    if (colorTrump != 69)
    {
        announce = "TRUMP";
    }
    m_helper.InitHelper(colorTrump);
    this->announce = announce;
}

void GamePlayer::sort_hands()
{

}
/* brute force play combinations */
bool GamePlayer::play_recursive(unsigned char turn_id, const vector<vector<Card>>& all_hands, vector<Card>& played_cards, 
                            vector<vector<Card>> current_deal, const unsigned char first_cards, 
                            const unsigned char second_cards, unsigned char current_player)
{
    if(played_cards.size() == NUMBER_OF_PLAYERS)
    {
        current_player = m_helper.calculate_winner_of_deal(played_cards, announce);
        //"""TODO: create 24 bit deal bitarray + save + choose how much CPU"""
        current_deal.push_back(played_cards);
        played_cards.clear();
    } 

    if(first_cards != 0)
    {
        if(turn_id == NUMBER_OF_PLAYERS * first_cards)
        {
            first_deals_combinations.push_back(current_deal);
            remaining_cards.push_back(all_hands);
            return_when_all_cards_played();
            return true;
        }
    }
    else if (second_cards != 0)
    {
        if(turn_id == NUMBER_OF_PLAYERS * second_cards)
        {
            return_when_all_cards_played();
            second_deal_combinations.push_back(current_deal);
            return true;
        }
    }
    else if (turn_id == NUMBER_OF_CARDS)
    {
        return_when_all_cards_played();
        return true;
    }
    current_player = current_player % 4;
    vector<Card> posibleOptions;
    posibleOptions = m_helper.playable_by_hand_and_played_cards(announce, all_hands.at(current_player), played_cards);
    unsigned char posibleOptionsCount = posibleOptions.size();
    for (int i = 0; i < posibleOptionsCount; i++ )
    {
        vector<vector<Card>> newHands = all_hands;
        newHands.at(current_player).erase(newHands.at(current_player).begin() + m_helper.findCard(newHands.at(current_player), posibleOptions.at(i)));
        remove(newHands.at(current_player).begin(),newHands.at(current_player).end(), posibleOptions.at(i));
        vector<Card> newPlayed_cards = played_cards;
        newPlayed_cards.push_back(posibleOptions.at(i));
        play_recursive((turn_id + 1), newHands, newPlayed_cards, current_deal, first_cards, second_cards, current_player + 1); // play the next card
    }
    return true;
}

/* manager of brute force */
void GamePlayer::play_deals_fast(unsigned char first_cards, unsigned char second_cards, long long deal_id)
{
    sort_hands();
    vector<vector<Card>> temphands = m_vCards;
    vector<Card> tempPlayedCards = {};
    vector<vector<Card>> tempCurrentDeal;
    if(second_cards != 0)
    {
        if(deal_id == -1) //if no deal id
        {
            for(int i = 0 ; i < NUMBER_OF_PLAYERS ; i++)
            {
                while(temphands.at(i).size() > second_cards)
                {
                    temphands.at(i).pop_back();
                }
            }
        }
        else
        {
            play_recursive(0, remaining_cards.at(deal_id), tempPlayedCards, tempCurrentDeal, first_cards, second_cards);
            return;
        }
    }
    play_recursive(0, temphands, tempPlayedCards, tempCurrentDeal, first_cards, second_cards);
}

/* separator between methods */
void GamePlayer::play_separated_to_x_then_y(unsigned char cardsNumber)
{
    auto start = chrono::steady_clock::now();
    play_deals_fast(cardsNumber);
    long long FirstDealsCount = GetFirstDealsCount();
    cout << "Played Deals with "<< (int)cardsNumber << " : " << FirstDealsCount << endl;

    for(long long deal_id = 0 ; deal_id < FirstDealsCount; deal_id++)
    {
        play_deals_fast(0, NUMBER_OF_DEALS - cardsNumber, -1);
        long long SecondDealsCount = GetSecondDealsCount();
        auto time_write = chrono::steady_clock::now();
        for(long long i = 0; i < SecondDealsCount; i++)
        {
            vector<vector<Card>> mergedDeal = first_deals_combinations.at(deal_id);
            mergedDeal.insert(mergedDeal.end(), second_deal_combinations.at(i).begin(), second_deal_combinations.at(i).end());
            played_deals.push_back(mergedDeal);
        }
        second_deal_combinations.clear();
        auto time_write_end = chrono::steady_clock::now();
        cout << "Time write" << chrono::duration_cast<chrono::seconds>(time_write_end - time_write).count() << endl;
        auto end = chrono::steady_clock::now();
        cout << "Current Played: " << played_deals.size() << " time:" << chrono::duration_cast<chrono::seconds>(end - start).count() << endl;
        FirstDealsCount = FirstDealsCount - 1;
        cout << "Remaining recursive calls : " << FirstDealsCount << endl;
    }

}


long long GamePlayer::GetFirstDealsCount()
{
    return first_deals_combinations.size();
}

long long GamePlayer::GetSecondDealsCount()
{
    return second_deal_combinations.size();
}

/* final results */
bool GamePlayer::return_when_all_cards_played()
{
    playedCombinations ++;
    if (playedCombinations % 5000000 == 0 )
    {
        cout << playedCombinations << endl;
    }
    return true;
}

bool GamePlayer::StartProcessing()
{
    play_separated_to_x_then_y(3);
    return true;
}
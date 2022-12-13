#include "GamePlayer.h"
#include <mutex>
#include <queue>

std::mutex mtx;

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
            return_when_all_cards_played();
            m_vSecond_deal_combinations.at(thread_id).at(m_vSecond_deals.at(thread_id)) = (current_deal);
            m_vSecond_deals.at(thread_id)++;
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
            play_recursive(0, const_cast<array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS>&>(m_vRemaining_cards.at(deal_id)), 
                            tempPlayedCards, tempCurrentDeal, first_cards, second_cards, 0, thread_id, current_announce);
            return;
        }
    }
    play_recursive(0, m_aCards, tempPlayedCards, tempCurrentDeal, first_cards, second_cards, 0, 0, current_announce);
}

/* separator between methods */
void GamePlayer::play_separated_to_x_then_y(unsigned char cardsNumber)
{
    auto start = chrono::steady_clock::now();
    int totalmaxed = 0 ;
    play_deals_fast(cardsNumber);
    long long FirstDealsCount = GetFirstDealsCount();
    cout << "Played Deals with "<< (int)cardsNumber << " : " << FirstDealsCount << endl;

    std::queue<int> RemainingDealIDs;
    for (int i = 0; i < FirstDealsCount; i++)
    {
        RemainingDealIDs.push(i);
    }

    vector<ThreadData> current_data;
    current_data.resize(m_ucThreadNumber);
    vector<thread> threads;
    threads.resize(m_ucThreadNumber);

    for(long long deal_id = 0 ; deal_id < FirstDealsCount; deal_id += m_ucThreadNumber)
    {
        int thread_id = 0;
        for (thread_id = 0; thread_id < m_ucThreadNumber; thread_id++)
        { 
        //    current_data.at(thread_id).deal_id = deal_id + thread_id;
           current_data.at(thread_id).second_cards = HAND_SIZE - cardsNumber;
           current_data.at(thread_id).first_cards = 0;
           current_data.at(thread_id).thread_id = thread_id;
           current_data.at(thread_id).announce = m_sAnnounce;

           std::thread tempThread([&]
           { /*TODO MAKE THREAD FUNCTION*/
                while(true)
                {
                    mtx.lock();
                    if(RemainingDealIDs.empty())
                    {
                        mtx.unlock();
                        break;
                    }
                    int currentDealID = RemainingDealIDs.front();
                    RemainingDealIDs.pop();
                    mtx.unlock();
                    play_deals_fast(0, current_data.at(thread_id).second_cards, current_data.at(thread_id).deal_id, 
                                    current_data.at(thread_id).thread_id, current_data.at(thread_id).announce);

                    long long SecondDealsCount = GetSecondDealsCount(thread_id);
                    for(long long i = 0; i < SecondDealsCount; i++)
                    {
                        array<array<Card, NUMBER_OF_PLAYERS>, HAND_SIZE> mergedDeal;
                        for(unsigned char j = 0 ; j < HAND_SIZE; j++)
                        {
                            if(i < cardsNumber)
                            {
                                mergedDeal.at(i) = m_vFirst_deals_combinations.at(deal_id + thread_id).at(j);
                            }
                            else 
                            {
                                mergedDeal.at(j) = m_vSecond_deal_combinations.at(thread_id).at(i).at(j);
                            }
                        }

                        mtx.lock();

                        if(m_ui64SavedDeals >= EXPECTED_SAVED_DEAL_COMBINATIONS)
                        {
                            totalmaxed ++;
                            m_ui64SavedDeals = 0;
                        }

                        
                        m_vPlayed_deals.at(m_ui64SavedDeals) = (mergedDeal);

                        m_ui64SavedDeals++;

                        mtx.unlock();                     
                    }
                    mtx.lock();
                    m_vSecond_deals.at(thread_id) = 0; 
                    mtx.unlock(); 
                    if(m_ui64PlayedCombinations % 10 == 1)
                    {
                        auto end = chrono::steady_clock::now();
                        cout << "Current Played: " << m_ui64SavedDeals << " time:" << chrono::duration_cast<chrono::seconds>(end - start).count() << " " << totalmaxed <<  endl;
                        
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
        cout << "Current Played: " << m_ui64SavedDeals << " time:" << chrono::duration_cast<chrono::seconds>(end - start).count() << " " << totalmaxed <<  endl;
        FirstDealsCount = FirstDealsCount - m_ucThreadNumber;
        cout << "Remaining recursive calls : " << FirstDealsCount - m_ucThreadNumber << endl;
    }

}


long long GamePlayer::GetFirstDealsCount()
{
    return m_ui64first_deals;
}

long long GamePlayer::GetSecondDealsCount(unsigned char thread_id)
{
    return m_vSecond_deals.at(thread_id);
}

/* final results */
bool GamePlayer::return_when_all_cards_played()
{
    mtx.lock();
    m_ui64PlayedCombinations ++;
    mtx.unlock();
    if (m_ui64PlayedCombinations % EXPECTED_FIRST_DEAL_COMBINATIONS == 0 )
    {
        cout << m_ui64PlayedCombinations << endl;
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

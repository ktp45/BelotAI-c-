#include "GamePlayer.h"
#include <mutex>
#include <queue>

std::mutex mtx_queue;
std::mutex mtx_write_deal;
std::mutex mtx_reset_deal;
std::mutex mtx_update_combinations;

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
    m_vCardCounters.resize(threadNumber);

    m_aCards.at(0) = (hand1);
    m_aCards.at(1) = (hand2);
    m_aCards.at(2) = (hand3);
    m_aCards.at(3) = (hand4);
    sort_hands();

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
        m_vCardCounters.at(i).InitCounter(colorTrump);
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

/* brute force play combinations */ 
bool GamePlayer::play_recursive(const array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS>& all_hands, 
                               array<Card, NUMBER_OF_PLAYERS>& played_cards, RecursiveData data)
{
    unsigned char playedCardsSize = m_helper.count_played_cards(played_cards);
    if(playedCardsSize == NUMBER_OF_PLAYERS)
    {
        data.current_player = m_helper.calculate_winner_of_deal(played_cards, data.announce);

        if(data.current_player == ERROR)
        {
            cerr << "NO WINNER OF DEAL" << endl; 
        }
        //"""TODO: create 24 bit deal bitarray + save + choose how much CPU"""
        data.current_deal.at((data.turn_id  + 1) / 4 - 1) = (played_cards); //turn id is counted from 0

        played_cards.fill(NULLCARD);
    } 

    if(data.first_cards != 0)
    {
        if(data.turn_id == NUMBER_OF_PLAYERS * data.first_cards)
        {
            m_vFirst_deals_combinations.at(m_ui64first_deals) = (data.current_deal);
            m_vRemaining_cards.at(m_ui64first_deals) = (all_hands);
            m_ui64first_deals++;

            return true;
        }
    }
    else if (data.second_cards != 0)
    {
        if(data.turn_id == NUMBER_OF_PLAYERS * data.second_cards)
        {
            mtx_update_combinations.lock();
            return_when_all_cards_played();
            m_vSecond_deal_combinations.at(data.thread_id).at(m_vSecond_deals.at(data.thread_id)) = (data.current_deal);
            m_vSecond_deals.at(data.thread_id)++;
            mtx_update_combinations.unlock();
            return true;
        }
    }
    else if (data.turn_id == NUMBER_OF_CARDS)
    {
        return_when_all_cards_played();
        return true;
    }

    data.current_player = data.current_player % 4;
    if(data.second_cards != 0 && data.turn_id < 12)
    {
        m_vCardCounters.at(data.thread_id).UpdateByHands(all_hands, data.turn_id + (HAND_SIZE - data.second_cards) * 4, false);
        if(m_vCardCounters.at(data.thread_id).IsHandClaimable(all_hands.at(data.current_player)))
        {
            array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS > newHands = all_hands;
            array<Card, NUMBER_OF_PLAYERS> newPlayed_cards = played_cards;
            newPlayed_cards.at(m_helper.count_played_cards(newPlayed_cards)) = (newHands.at(data.current_player).at(0));

            newHands.at(data.current_player).at(0) = NULLCARD;
            
            m_helper.sort_hand(newHands.at(data.current_player)); 
            data.turn_id++;
            data.current_player++;
            finishDeal(newHands, newPlayed_cards, data);
            return true;
        }
    }

    auto posibleOptions = m_helper.playable_by_hand_and_played_cards(data.announce, all_hands.at(data.current_player), played_cards);

    unsigned char posibleOptionsCount = m_helper.count_played_cards(posibleOptions);
    m_helper.sort_hand((posibleOptions));

    for (int i = 0; i < posibleOptionsCount; i++ )
    {
        array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS > newHands = all_hands;
        newHands.at(data.current_player).at(m_helper.findCard(newHands.at(data.current_player), posibleOptions.at(i), HAND_SIZE - data.turn_id / 4, true)) = NULLCARD;

        array<Card, NUMBER_OF_PLAYERS> newPlayed_cards = played_cards;

        newPlayed_cards.at(m_helper.count_played_cards(newPlayed_cards)) = (posibleOptions.at(i));
        m_helper.sort_hand(newHands.at(data.current_player)); 

        data.turn_id++;
        data.current_player++; 
        play_recursive(newHands, newPlayed_cards, data); // play the next card
        data.turn_id--; //avoid copying three times data
        data.current_player--;
    }
    return true;
}

/* manager of brute force */
void GamePlayer::play_deals_fast(ThreadData data)
{
    unsigned char first_cards = data.first_cards;
    unsigned char second_cards = data.second_cards;
    long long deal_id = data.deal_id;
    unsigned char thread_id = data.thread_id;
    string announce = data.announce;

    play_deals_fast(first_cards, second_cards, deal_id, thread_id, announce);
}

void GamePlayer::play_deals_fast(unsigned char first_cards, unsigned char second_cards, long long deal_id, unsigned char thread_id,
                                 string announce)
{
    RecursiveData data;
    data.turn_id = 0;

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
    data.current_deal = tempCurrentDeal;
    data.first_cards = first_cards;
    data.second_cards = second_cards;
    data.current_player = 0;
    data.announce = current_announce;
    data.thread_id = 0;

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
            data.thread_id = thread_id;
            play_recursive(m_vRemaining_cards.at(deal_id), tempPlayedCards, data);
            return;
        }
    }
    RecursiveData firstCardData;
    
    play_recursive(m_aCards, tempPlayedCards, data);
}

bool GamePlayer::finishDeal(const array<array<Card, NUMBER_OF_DEALS>, NUMBER_OF_PLAYERS>& all_hands, 
                            array<Card, NUMBER_OF_PLAYERS>& played_cards, RecursiveData data)
{
    unsigned char playedCardsSize = m_helper.count_played_cards(played_cards);
    if(playedCardsSize == NUMBER_OF_PLAYERS)
    {
        data.current_player = m_helper.calculate_winner_of_deal(played_cards, data.announce);

        if(data.current_player == ERROR)
        {
            cerr << "NO WINNER OF DEAL" << endl; 
        }
        //"""TODO: create 24 bit deal bitarray + save + choose how much CPU"""
        data.current_deal.at((data.turn_id  + 1 ) / 4 - 1) = (played_cards); //turn id is counted from 0
        played_cards.fill(NULLCARD);
        
    } 

    if(data.first_cards != 0)
    {
        if(data.turn_id == NUMBER_OF_PLAYERS * data.first_cards)
        {
            m_vFirst_deals_combinations.at(m_ui64first_deals) = (data.current_deal);
            m_vRemaining_cards.at(m_ui64first_deals) = (all_hands);
            m_ui64first_deals++;
            return true;
        }
    }
    else if (data.second_cards != 0)
    {
        if(data.turn_id == NUMBER_OF_PLAYERS * data.second_cards)
        {
            mtx_update_combinations.lock();
            return_when_all_cards_played();
            m_vSecond_deal_combinations.at(data.thread_id).at(m_vSecond_deals.at(data.thread_id)) = (data.current_deal);
            m_vSecond_deals.at(data.thread_id)++;
            mtx_update_combinations.unlock();
            return true;
        }
    }
    else if (data.turn_id == NUMBER_OF_CARDS)
    {
        return_when_all_cards_played();
        return true;
    }
    data.current_player = data.current_player % 4;

    auto posibleOptions = m_helper.playable_by_hand_and_played_cards(data.announce, all_hands.at( data.current_player), played_cards);

    m_helper.sort_hand((posibleOptions));

    // not needed to play all options since they all lead to the same exit
    array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS > newHands = all_hands;
    newHands.at(data.current_player).at(m_helper.findCard(newHands.at(data.current_player), posibleOptions.at(0), HAND_SIZE -  data.turn_id / 4, true)) = NULLCARD;

    array<Card, NUMBER_OF_PLAYERS> newPlayed_cards = played_cards;

    newPlayed_cards.at(m_helper.count_played_cards(newPlayed_cards)) = (posibleOptions.at(0));
    m_helper.sort_hand(newHands.at(data.current_player)); 
    data.turn_id++;
    data.current_player++; 
    finishDeal(newHands, newPlayed_cards, data); // play the next card

    return true;
}

/* separator between methods */
void GamePlayer::play_separated_to_x_then_y(unsigned char firstCardCount)
{
    auto start = chrono::steady_clock::now();
    int totalmaxed = 0 ;
    play_deals_fast(firstCardCount);
    long long FirstDealsCount = GetFirstDealsCount();
    cout << "Played Deals with "<< (int)firstCardCount << " : " << FirstDealsCount << endl;

    std::queue<int> RemainingDealIDs;
    for (int i = 0; i < FirstDealsCount; i++)
    {
        RemainingDealIDs.push(i);
    }

    vector<ThreadData> current_data;
    current_data.resize(m_ucThreadNumber);
    vector<thread> threads;
    threads.resize(m_ucThreadNumber);

    int thread_id = 0;
    for (thread_id = 0; thread_id < m_ucThreadNumber; thread_id++)
    { 
        current_data.at(thread_id).second_cards = HAND_SIZE - firstCardCount;
        current_data.at(thread_id).first_cards = 0;
        current_data.at(thread_id).thread_id = thread_id;
        current_data.at(thread_id).announce = m_sAnnounce;

        std::thread tempThread([&]
        { 
            while(true)
            {
                mtx_queue.lock();
                if(RemainingDealIDs.empty())
                {
                    mtx_queue.unlock();
                    break;
                }
                int currentDealID = RemainingDealIDs.front();
                RemainingDealIDs.pop();
                current_data.at(thread_id).deal_id = currentDealID;
                mtx_queue.unlock();
                
                play_deals_fast(current_data.at(thread_id));

                long long SecondDealsCount = GetSecondDealsCount(thread_id);
                for(long long i = 0; i < SecondDealsCount; i++)
                {
                    array<array<Card, NUMBER_OF_PLAYERS>, HAND_SIZE> mergedDeal;
                    for(unsigned char j = 0 ; j < HAND_SIZE; j++)
                    {
                        if(j < firstCardCount)
                        {
                            mergedDeal.at(j) = m_vFirst_deals_combinations.at(currentDealID).at(j);
                        }
                        else 
                        {
                            mergedDeal.at(j) = m_vSecond_deal_combinations.at(thread_id).at(i).at(j);
                        }
                    }

                    mtx_write_deal.lock();

                    if(GetSavedDealsCount() >= EXPECTED_SAVED_DEAL_COMBINATIONS)
                    {
                        totalmaxed ++;
                        m_ui64SavedDeals = 0;
                    }
                    
                    m_vPlayed_deals.at(GetSavedDealsCount()) = (mergedDeal);

                    m_ui64SavedDeals++;

                    mtx_write_deal.unlock();                     
                }
                mtx_reset_deal.lock();
                m_vSecond_deals.at(thread_id) = 0; 
                mtx_reset_deal.unlock(); 
                if(GetPlayedCombinationsCount() % 10 == 1)
                {
                    auto end = chrono::steady_clock::now();
                    cout << "Current Played: " << GetSavedDealsCount() << " time:" << chrono::duration_cast<chrono::seconds>(end - start).count() << " " << totalmaxed <<  endl;
                    
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
    cout << "Current Played: " << GetSavedDealsCount() << " time:" << chrono::duration_cast<chrono::seconds>(end - start).count() << " " << totalmaxed <<  endl;
    FirstDealsCount = FirstDealsCount - m_ucThreadNumber;
    cout << "Remaining recursive calls : " << FirstDealsCount - m_ucThreadNumber << endl;
}


long long GamePlayer::GetFirstDealsCount()
{
    return m_ui64first_deals;
}

long long GamePlayer::GetSecondDealsCount(unsigned char thread_id)
{
    return m_vSecond_deals.at(thread_id);
}

long long GamePlayer::GetSavedDealsCount()
{
    return m_ui64SavedDeals;
}

long long GamePlayer::GetPlayedCombinationsCount()
{
    return m_ui64PlayedCombinations;
}


/* final results */
bool GamePlayer::return_when_all_cards_played()
{
    m_ui64PlayedCombinations ++;

    if (GetPlayedCombinationsCount() % EXPECTED_FIRST_DEAL_COMBINATIONS == 0 )
    {
        cout << GetPlayedCombinationsCount() << endl;
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

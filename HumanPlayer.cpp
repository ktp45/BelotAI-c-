#include"HumanPlayer.h"
using namespace std;

HumanPlayer::HumanPlayer()
{
    cerr<<" HumanPlayer Called Default constructor " << endl;
}


HumanPlayer::HumanPlayer(array <Card, HAND_SIZE> hand, const string &announce)
{
    m_aHand = hand;
    m_helper.sort_hand(m_aHand);
    m_ucHandSize = 0;
    for (int i = 0; i < HAND_SIZE; i++)
    {
        m_ucHandSize++;
    }

    m_sAnnounce = announce;

    for (int i = 0; i < TRUMP_NAMES.size(); i++)
    {
        if(announce == TRUMP_NAMES.at(i))
        {
            m_helper.InitHelper(i);
            m_sAnnounce = "TRUMP";
            return;
        }
    }
}

HumanPlayer::HumanPlayer(const HumanPlayer &other)
{
    m_aHand = other.m_aHand;
    m_ucHandSize = other.m_ucHandSize;
    m_sAnnounce = other.m_sAnnounce;
    m_helper = other.m_helper;
}

void HumanPlayer::SetAnnounce(const string &announce)
{
    m_sAnnounce = announce;
    for (int i = 0; i < TRUMP_NAMES.size(); i++)
    {
        if(announce == TRUMP_NAMES.at(i))
        {
            m_helper.InitHelper(i);
            m_sAnnounce = "TRUMP";
            return;
        }
    }
}

array<Card, HAND_SIZE> HumanPlayer::GetPossibleOptions(const array<Card, NUMBER_OF_PLAYERS> &played_cards)
{
    array<Card, HAND_SIZE> options = m_helper.playable_by_hand_and_played_cards(m_sAnnounce, m_aHand, played_cards);
    m_helper.sort_hand(options);
    return options;
}

Card HumanPlayer::PlayCard(const array<Card, NUMBER_OF_PLAYERS> &played_cards)
{
    array<Card, HAND_SIZE> options = GetPossibleOptions(played_cards);

    cout << "Current player possible options: " << endl; 

    for(int i = 0 ; i < HAND_SIZE; i++)
    {
        if (options.at(i).GetColor() != NULLCARD.GetColor() || options.at(i).GetPower() != NULLCARD.GetPower())
        {
            cout << "Option :" << i << " - " << options.at(i) << endl;
        }
    }
    int playeroption;
    cout << "Write your choice" << endl;
    cin >> playeroption;
    m_aHand.at(m_helper.findCard(m_aHand, options.at(playeroption), m_ucHandSize, true)) = NULLCARD;

    m_helper.sort_hand(m_aHand);

    return options.at(playeroption);
}

Card HumanPlayer::PlayCard(const array<Card, NUMBER_OF_PLAYERS> &played_cards, unsigned char option)
{
    array<Card, HAND_SIZE> options = GetPossibleOptions(played_cards);

    m_aHand.at(m_helper.findCard(m_aHand, options.at(option), m_ucHandSize, true)) = NULLCARD;

    m_helper.sort_hand(m_aHand);

    return options.at(option);
}

array<Card, HAND_SIZE> HumanPlayer::GetHand()
{
    return m_aHand;
}

unsigned char HumanPlayer::GetHandSize()
{
    return m_ucHandSize; 
}


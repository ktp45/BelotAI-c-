#include"Analyzer.h"
using namespace std;

Analyzer::Analyzer()
{
    cerr<<" Analyzer Called Default constructor " << endl;
}

Analyzer::Analyzer(const Analyzer &other)
{
    color_trump = other.color_trump;
}

Analyzer::Analyzer(const Helper &other)
{
    color_trump = other.GetColorTrump();
}

short Analyzer::AnalyzeHand(array<Card, NUMBER_OF_TICKS> hand)
{
    return 0;
}

float Analyzer::AnalyzeDeal(array<array<Card, NUMBER_OF_PLAYERS>, NUMBER_OF_TICKS> deal, string announce)
{
    short team1_score = 0 , team2_score = 0, turn_id = 0; 
    for(int i = 0; i < NUMBER_OF_TICKS; i++)
    {
        short tickScore = 0;
        for(int j = 0; j < NUMBER_OF_PLAYERS; j++)
        {
            if (announce == "NO_TRUMP")
            {
                tickScore += NO_TRUMP_POINTS.at(deal.at(i).at(j).GetPower());
            }
            else if (announce == "ALL_TRUMP")
            {
                tickScore += ALL_TRUMP_POINTS.at(deal.at(i).at(j).GetPower());
            }
            else if (announce == "TRUMP")
            {
                if (color_trump == ERROR)
                {
                    cerr << "No color trump" << endl;
                }
                if (color_trump == deal.at(i).at(j).GetColor())
                {
                    tickScore += ALL_TRUMP_POINTS.at(deal.at(i).at(j).GetPower());
                }
                else
                {
                    tickScore += NO_TRUMP_POINTS.at(deal.at(i).at(j).GetPower());
                }
            }
        }

        if (i == NUMBER_OF_TICKS - 1)
        {
            tickScore += 10; // posledno 10
        }

        bool winner = calculate_winner_of_tick(deal.at(i), announce) % 2;

        if (winner)
        {
            team1_score += tickScore;
        }
        else
        {
            team2_score += tickScore;
        }

    }

    float result = static_cast<float>(team1_score) + static_cast<float>(team2_score) / 1000;
    return result;
}
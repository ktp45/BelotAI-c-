#include "Helper.h"
using namespace std;
class Analyzer: public Helper
{
    public:
    Analyzer();

    Analyzer(const Analyzer &other);

    Analyzer(const Helper &other);

    short AnalyzeHand(array<Card, NUMBER_OF_TICKS> hand);

    float AnalyzeDeal(array<array<Card, NUMBER_OF_PLAYERS>, NUMBER_OF_TICKS> deal, string announce);
};

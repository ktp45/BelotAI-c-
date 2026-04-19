#include "CurrentDeal.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// Convert string to Card
Card strToCard(const string& cardStr) {
    if (cardStr.empty() || cardStr == "NULL") {
        return NULLCARD;
    }

    // Format: "(RANK)(SUIT)" e.g., "(7)(CLUBS)"
    string rank, suit;

    size_t rankStart = cardStr.find('(');
    size_t rankEnd = cardStr.find(')');
    if (rankStart != string::npos && rankEnd != string::npos) {
        rank = cardStr.substr(rankStart + 1, rankEnd - rankStart - 1);
    }

    size_t suitStart = cardStr.find('(', rankEnd);
    size_t suitEnd = cardStr.find(')', suitStart);
    if (suitStart != string::npos && suitEnd != string::npos) {
        suit = cardStr.substr(suitStart + 1, suitEnd - suitStart - 1);
    }

    // Map rank string to enum
    int rankVal = 0;
    if (rank == "7") rankVal = 0;
    else if (rank == "8") rankVal = 1;
    else if (rank == "9") rankVal = 2;
    else if (rank == "10") rankVal = 3;
    else if (rank == "J") rankVal = 4;
    else if (rank == "Q") rankVal = 5;
    else if (rank == "K") rankVal = 6;
    else if (rank == "A") rankVal = 7;

    // Map suit string to enum
    int suitVal = 0;
    if (suit == "CLUBS") suitVal = 0;
    else if (suit == "DIAMONDS") suitVal = 1;
    else if (suit == "HEARTS") suitVal = 2;
    else if (suit == "SPADES") suitVal = 3;

    return Card(rankVal, suitVal);
}

// Convert Card to string
string cardToStr(const Card& card) {
    if (card.GetColor() == NULLCARD.GetColor() && card.GetPower() == NULLCARD.GetPower()) {
        return "NULL";
    }

    static const char* ranks[] = {"7", "8", "9", "10", "J", "Q", "K", "A"};
    static const char* suits[] = {"CLUBS", "DIAMONDS", "HEARTS", "SPADES"};

    string result = "(";
    result += ranks[card.GetPower()];
    result += ")(";
    result += suits[card.GetColor()];
    result += ")";
    return result;
}

// Get announce type based on trump
string getAnnounce(const string& trumpSuit) {
    if (trumpSuit == "CLUBS") return "TRUMP";
    if (trumpSuit == "DIAMONDS") return "TRUMP";
    if (trumpSuit == "HEARTS") return "TRUMP";
    if (trumpSuit == "SPADES") return "TRUMP";
    return "NO_TRUMP";
}

// Parse command
// Commands: SORT_HAND, PLAYABLE, WINNER, DEAL
int main() {
    string command;
    while (cin >> command) {
        if (command == "SORT_HAND") {
            // Read hand size and cards
            int handSize;
            cin >> handSize;

            array<Card, HAND_SIZE> hand;
            for (int i = 0; i < HAND_SIZE; i++) {
                string cardStr;
                cin >> cardStr;
                hand.at(i) = strToCard(cardStr);
            }

            Helper h;
            h.InitHelper(0); // Initialize with any suit
            h.sort_hand(hand);

            cout << "SORTED ";
            for (int i = 0; i < HAND_SIZE; i++) {
                cout << cardToStr(hand.at(i)) << " ";
            }
            cout << endl;
        }
        else if (command == "PLAYABLE") {
            // Read trump suit
            string trumpSuit;
            cin >> trumpSuit;

            // Read hand
            array<Card, HAND_SIZE> hand;
            for (int i = 0; i < HAND_SIZE; i++) {
                string cardStr;
                cin >> cardStr;
                hand.at(i) = strToCard(cardStr);
            }

            // Read played cards
            array<Card, NUMBER_OF_PLAYERS> playedCards;
            for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
                string cardStr;
                cin >> cardStr;
                playedCards.at(i) = strToCard(cardStr);
            }

            Helper h;
            string announce = getAnnounce(trumpSuit);
            h.InitHelper(0);

            // Initialize helper with correct trump
            if (trumpSuit == "CLUBS") h.InitHelper(0);
            else if (trumpSuit == "DIAMONDS") h.InitHelper(1);
            else if (trumpSuit == "HEARTS") h.InitHelper(2);
            else if (trumpSuit == "SPADES") h.InitHelper(3);

            auto options = h.playable_by_hand_and_played_cards(announce, hand, playedCards);

            cout << "PLAYABLE ";
            for (int i = 0; i < HAND_SIZE; i++) {
                if (options.at(i).GetColor() != NULLCARD.GetColor() ||
                    options.at(i).GetPower() != NULLCARD.GetPower()) {
                    cout << cardToStr(options.at(i)) << " ";
                }
            }
            cout << endl;
        }
        else if (command == "WINNER") {
            // Read trump suit
            string trumpSuit;
            cin >> trumpSuit;

            // Read played cards
            array<Card, NUMBER_OF_PLAYERS> playedCards;
            for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
                string cardStr;
                cin >> cardStr;
                playedCards.at(i) = strToCard(cardStr);
            }

            Helper h;
            string announce = getAnnounce(trumpSuit);

            // Initialize helper with correct trump
            if (trumpSuit == "CLUBS") h.InitHelper(0);
            else if (trumpSuit == "DIAMONDS") h.InitHelper(1);
            else if (trumpSuit == "HEARTS") h.InitHelper(2);
            else if (trumpSuit == "SPADES") h.InitHelper(3);

            unsigned char winner = h.calculate_winner_of_tick(playedCards, announce);

            cout << "WINNER " << (int)winner << endl;
        }
        else if (command == "ANNOUNCE") {
            // ANNOUNCE <trump_suit> <hand1_cards...> <hand2_cards...> <hand3_cards...> <hand4_cards...>
            string trumpSuit;
            cin >> trumpSuit;

            string announce = getAnnounce(trumpSuit);

            // Read 4 hands of 8 cards each
            array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS> hands;

            for (int p = 0; p < NUMBER_OF_PLAYERS; p++) {
                for (int i = 0; i < HAND_SIZE; i++) {
                    string cardStr;
                    cin >> cardStr;
                    hands.at(p).at(i) = strToCard(cardStr);
                }
            }

            // Create AnnounceTracker and check for announces
            AnnounceTracker tracker(hands, announce);

            // Output announces for each player
            cout << "ANNOUNCES ";
            for (int p = 0; p < NUMBER_OF_PLAYERS; p++) {
                bool hasAnnounce = tracker.IsThereTurnIAnnounce(p);
                unsigned char beloteCount = tracker.GetBeloteCountPerPlayer(p);

                string announceStr = "";
                if (hasAnnounce) {
                    // Check for cards in row and same cards
                    array<Card, HAND_SIZE> hand = hands.at(p);
                    // We'll output the info and let Python handle the detailed analysis
                    unsigned char cardsInRow = 0;
                    unsigned char sameCards = 0;

                    // Simple check - count consecutive same color
                    for (int i = 1; i < HAND_SIZE; i++) {
                        if (hand.at(i-1).GetColor() == hand.at(i).GetColor() &&
                            hand.at(i-1).GetPower() == (hand.at(i).GetPower() - 1)) {
                            cardsInRow++;
                        }
                    }

                    // Check for four of a kind
                    int powerCount[8] = {0};
                    for (int i = 0; i < HAND_SIZE; i++) {
                        powerCount[hand.at(i).GetPower()]++;
                    }
                    for (int i = 0; i < 8; i++) {
                        if (powerCount[i] >= 4) sameCards = 4;
                    }

                    if (cardsInRow >= 2) {
                        announceStr += "TERCA+";
                    }
                    if (sameCards >= 4) {
                        if (!announceStr.empty()) announceStr += ",";
                        announceStr += "KAPE";
                    }
                }
                if (beloteCount > 0) {
                    if (!announceStr.empty()) announceStr += ",";
                    announceStr += "BELOTE";
                }

                cout << "P" << (p+1) << ":" << announceStr << " ";
            }
            cout << endl;
        }
        else if (command == "SCORE") {
            // SCORE <trump_suit> <played_cards_history>
            // Format: SCORE <trump_suit> <tick1_p1> <tick1_p2> <tick1_p3> <tick1_p4> <tick2_p1> ...
            string trumpSuit;
            cin >> trumpSuit;

            string announce = getAnnounce(trumpSuit);

            // Read 8 ticks of 4 cards each = 32 cards
            array<array<Card, NUMBER_OF_PLAYERS>, NUMBER_OF_TICKS> deal;
            for (int tick = 0; tick < NUMBER_OF_TICKS; tick++) {
                for (int p = 0; p < NUMBER_OF_PLAYERS; p++) {
                    string cardStr;
                    cin >> cardStr;
                    deal.at(tick).at(p) = strToCard(cardStr);
                }
            }

            // Initialize helper for the announce type
            Helper h;
            if (trumpSuit == "CLUBS") h.InitHelper(0);
            else if (trumpSuit == "DIAMONDS") h.InitHelper(1);
            else if (trumpSuit == "HEARTS") h.InitHelper(2);
            else if (trumpSuit == "SPADES") h.InitHelper(3);

            // Calculate scores using Analyzer
            Analyzer ResultCalc(h);
            float result = ResultCalc.AnalyzeDeal(deal, announce);

            // Get announce scores
            array<array<Card, HAND_SIZE>, NUMBER_OF_PLAYERS> hands;
            // We need to get initial hands - but we don't have them in this command
            // Just output the card points result
            short team0Score = static_cast<short>(result);
            short team1Score = static_cast<short>((result - team0Score) * 1000);

            // Output scores in format: TEAM0 TEAM1
            cout << "SCORES " << team0Score << " " << team1Score << endl;
        }
        else if (command == "DEAL") {
            // Generate and deal cards, sort hands
            vector<Card> deck;
            for (int i = 0; i <= SPADES; i++) {
                for (int j = 0; j <= Ace; j++) {
                    deck.push_back(Card(j, i));
                }
            }
            srand(time(0));
            random_shuffle(deck.begin(), deck.end());

            array<Card, HAND_SIZE> hand1, hand2, hand3, hand4;

            for(int i = 0; i < HAND_SIZE / 2; i++) {
                hand1.at(i) = deck.at(HAND_SIZE * 0 + i);
                hand2.at(i) = deck.at(HAND_SIZE * 1 + i);
                hand3.at(i) = deck.at(HAND_SIZE * 2 + i);
                hand4.at(i) = deck.at(HAND_SIZE * 3 + i);
            }

            for(int i = HAND_SIZE / 2; i < HAND_SIZE; i++) {
                hand1.at(i) = deck.at(HAND_SIZE * (i-4) + 4);
                hand2.at(i) = deck.at(HAND_SIZE * (i-4) + 5);
                hand3.at(i) = deck.at(HAND_SIZE * (i-4) + 6);
                hand4.at(i) = deck.at(HAND_SIZE * (i-4) + 7);
            }

            Helper h;
            h.InitHelper(0);
            h.sort_hand(hand1);
            h.sort_hand(hand2);
            h.sort_hand(hand3);
            h.sort_hand(hand4);

            cout << "DEALT ";
            for (int i = 0; i < HAND_SIZE; i++) {
                cout << cardToStr(hand1.at(i)) << " ";
            }
            cout << "| ";
            for (int i = 0; i < HAND_SIZE; i++) {
                cout << cardToStr(hand2.at(i)) << " ";
            }
            cout << "| ";
            for (int i = 0; i < HAND_SIZE; i++) {
                cout << cardToStr(hand3.at(i)) << " ";
            }
            cout << "| ";
            for (int i = 0; i < HAND_SIZE; i++) {
                cout << cardToStr(hand4.at(i)) << " ";
            }
            cout << endl;
        }
    }

    return 0;
}
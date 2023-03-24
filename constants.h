// import bitarray
#include <iostream>
#include <vector>
#include <utility>
#include <array>
#include "Card.h"
#include <thread>
#include <string>
#include <cstdlib>
#include <unistd.h>

// """0 equals to 0 and 1 to 1 =>  00101 is 8 of DIAMONDS and 11100 is Ace of CLUBS"""
// """First 3 digits represent the value and the last 2 represent the color"""

// enum TEAMS
// {
//     TEAM_1 = 0b00,
//     TEAM_2 = 0b01,
//     TEAM_1 = 0b10,
//     TEAM_2 = 0b11
// };

enum COLORS_TO_BYTES{
    CLUBS = 0b00,
    DIAMONDS = 0b01,
    HEARTS = 0b10,
    SPADES = 0b11
};


enum CARD_VALUE_TO_BYTES{
    seven = 0b000,
    eight = 0b001,
    nine = 0b010,
    ten = 0b011,
    Jack = 0b100,
    Queen = 0b101,
    King = 0b110,
    Ace = 0b111
};



const size_t ERROR = 69; 
const size_t HAND_SIZE = 8;
const size_t NUMBER_OF_TICKS = 8;
const size_t NUMBER_OF_PLAYERS = 4;
const size_t NUMBER_OF_CARDS = 32;
const unsigned char CLUBS_ANNOUNCE = 0;
const unsigned char DIAMONDS_ANNOUNCE = 1;
const unsigned char HEARTS_ANNOUNCE = 2;
const unsigned char SPADES_ANNOUNCE = 3;
const unsigned char NO_TRUMP_ANNOUNCE = 4;
const unsigned char ALL_TRUMP_ANNOUNCE = 5;

static const Card NULLCARD(ERROR, ERROR);
static const std::array<Card, HAND_SIZE> NULLHAND{NULLCARD, NULLCARD, NULLCARD, NULLCARD, NULLCARD, NULLCARD, NULLCARD, NULLCARD};

const size_t EXPECTED_FIRST_DEAL_COMBINATIONS     = 5000000;
const size_t EXPECTED_SECOND_DEAL_COMBINATIONS    = 10000000;
const size_t EXPECTED_SAVED_DEAL_COMBINATIONS    = 25000000;

static const std::vector <std::string> TRUMP_NAMES =
{
    "CLUBS",
    "DIAMONDS",
    "HEARTS",
    "SPADES"
};

static const std::vector <unsigned char> ALL_TRUMP_POINTS =
{
    0,  // seven 
    0, // eight
    14, // nine
    10, // ten
    20, // Jack 
    3, // Queen  
    4, // King 
    11  // Ace
};

static const std::vector <unsigned char> NO_TRUMP_POINTS =
{
    0,  // seven 
    0, // eight
    0, // nine
    10, // ten
    2, // Jack 
    3, // Queen  
    4, // King 
    11  // Ace
};



// BYTES_TO_COLORS = {tpl: color for color, tpl in COLORS_TO_BYTES.items()}
// BYTES_TO_CARD_VALUE = {tpl: value for value, tpl in CARD_VALUE_TO_BYTES.items()}


// """Generate card names dynamically based on dictionary keys"""
// CARD_TUPLES = [(value, color) for value in CARD_VALUE_TO_BYTES.keys() for color in COLORS_TO_BYTES.keys()]

// """Generate card bites representation dynamically based on value on color"""
// CARD_TO_BYTES = {card: bitarray.bitarray(CARD_VALUE_TO_BYTES[card[0]] + COLORS_TO_BYTES[card[1]]) for card in
//                  CARD_TUPLES}
// BYTES_TO_CARD = {tuple(bitarray.bitarray(CARD_VALUE_TO_BYTES[card[0]] + COLORS_TO_BYTES[card[1]])): card for card in
//                  CARD_TUPLES}

// if __name__ == '__main__':
//     import pprint

//     pprint.pprint(CARD_TO_BYTES)
//     pprint.pprint(BYTES_TO_CARD)

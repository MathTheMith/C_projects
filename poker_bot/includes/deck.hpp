#ifndef DECK_HPP
#define DECK_HPP

#include <vector>
#include <algorithm>
#include <random>
#include "card.hpp"


class Deck {
public:
    std::vector<Card> cards;
    
    Deck() {
        // Initialiser le deck avec 52 cartes
        for (int s = 0; s < 4; ++s) {
            for (int v = 2; v <= 14; ++v) {
                cards.push_back(Card(v, s));
            }
        }
    }
    
    void shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    }
};

#endif
#ifndef HAND_EVALUATOR_HPP
#define HAND_EVALUATOR_HPP

#include "card.hpp"
#include <vector>

// Vérifie s'il y a une flush
bool checkFlush(const std::vector<Card>& cards);

// Vérifie s'il y a une straight
bool checkStraight(const std::vector<Card>& cards);

// Retourne les cartes d'une couleur spécifique
std::vector<Card> getCardsOfSuit(const std::vector<Card>& cards, int suit);

// Trouve la couleur dominante (au moins 5 cartes d'une même couleur)
int findDominantSuit(const std::vector<Card>& cards);

#endif

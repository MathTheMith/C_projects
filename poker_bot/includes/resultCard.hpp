#ifndef HAND_EVALUATOR_HPP
#define HAND_EVALUATOR_HPP

#include <vector>
#include "card.hpp"

int calculateHandValue(const Card& card1, const Card& card2, const std::vector<Card>& board);
int comparePlayers(const Card& p1_card1, const Card& p1_card2,
                   const Card& p2_card1, const Card& p2_card2,
                   const std::vector<Card>& board);
void displayHandType(int score);
bool checkFlush(const std::vector<Card>& cards);
bool checkStraight(const std::vector<Card>& cards);
int getStraightHighCard(const std::vector<Card>& cards);
std::vector<Card> getCardsOfSuit(const std::vector<Card>& cards, int suit);
int findDominantSuit(const std::vector<Card>& cards);

#endif

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include "includes/resultCard.hpp"
#include "includes/GameLogic.hpp"
#include "includes/card.hpp"

using namespace std;

void displayHandType(int score) {
    switch(score) {
        case 9: std::cout << "Quinte flush royale"; break;
        case 8: std::cout << "Quinte flush"; break;
        case 7: std::cout << "Carré"; break;
        case 6: std::cout << "Full"; break;
        case 5: std::cout << "Couleur"; break;
        case 4: std::cout << "Suite"; break;
        case 3: std::cout << "Brelan"; break;
        case 2: std::cout << "Double paire"; break;
        case 1: std::cout << "Paire"; break;
        case 0: std::cout << "Carte haute"; break;
        default: std::cout << "Main inconnue"; break;
    }
}

int calculateHandValue(const Card& card1, const Card& card2, const vector<Card>& board) {
    vector<Card> allCards = board;
    allCards.push_back(card1);
    allCards.push_back(card2);

    if (allCards.size() < 5) {
        std::cout << "[DEBUG] Pas assez de cartes pour calculer la main." << std::endl;
        return 0;
    }

    bool isFlush = checkFlush(allCards);
    bool isStraight = checkStraight(allCards);

    if (isFlush && isStraight) {
        int suit = findDominantSuit(allCards);
        vector<Card> flushCards = getCardsOfSuit(allCards, suit);
        if (!flushCards.empty() && checkStraight(flushCards)) {
            bool hasAce = false, hasTen = false, hasJack = false, hasQueen = false, hasKing = false;
            for (const auto& card : flushCards) {
                if (card.value == 14) hasAce = true;
                if (card.value == 10) hasTen = true;
                if (card.value == 11) hasJack = true;
                if (card.value == 12) hasQueen = true;
                if (card.value == 13) hasKing = true;
            }
            if (hasAce && hasTen && hasJack && hasQueen && hasKing)
                return 9;
            return 8;
        }
    }

    map<int, int> valueCount;
    for (const auto& card : allCards)
        valueCount[card.value]++;

    bool hasFour = false, hasThree = false;
    int pairs = 0;

    for (const auto& entry : valueCount) {
        if (entry.second == 4) hasFour = true;
        else if (entry.second == 3) hasThree = true;
        else if (entry.second == 2) pairs++;
    }

    if (hasFour) return 7;
    if (hasThree && pairs > 0) return 6;
    if (isFlush) return 5;
    if (isStraight) return 4;
    if (hasThree) return 3;
    if (pairs >= 2) return 2;
    if (pairs == 1) return 1;
    return 0;
}

int comparePlayers(const Card& p1_card1, const Card& p1_card2, const Card& p2_card1, const Card& p2_card2, const std::vector<Card>& board) {
    if (board.size() < 3) {
        std::cout << "[DEBUG] Pas assez de cartes sur le board pour comparer." << std::endl;
        return 0;
    }

    int score1 = calculateHandValue(p1_card1, p1_card2, board);
    int score2 = calculateHandValue(p2_card1, p2_card2, board);

    std::cout << "\nJoueur 1 : ";
    displayHandType(score1);
    std::cout << "\nJoueur 2 : ";
    displayHandType(score2);

    if (score1 > score2) {
        std::cout << "\nLe Joueur 1 gagne le pot de " << pot << " jetons!" << std::endl;
        player1_chips += pot;
        pot = 0;
        return 1;
    } else if (score2 > score1) {
        std::cout << "\nLe Joueur 2 gagne le pot de " << pot << " jetons!" << std::endl;
        player2_chips += pot;
        pot = 0;
        return 2;
    } else {
        std::cout << "\nEgalité - le pot est partagé." << std::endl;
        player1_chips += pot / 2;
        player2_chips += pot / 2;
        pot = 0;
        return 0;
    }
}
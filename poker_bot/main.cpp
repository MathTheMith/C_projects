#include <iostream>
#include <vector>
#include "includes/deck.hpp"
#include "includes/card.hpp"
#include "includes/GameLogic.hpp"
#include "includes/preflop.hpp"
#include "resultCard.hpp"

using namespace std;

void afficherTable(Card p1_card1, Card p1_card2, Card p2_card1, Card p2_card2, const vector<Card>& board) {
    cout << "\033[2J\033[1;1H";
    cout << "================ TABLE DE POKER ================\n";
    cout << "Joueur 1 : " << p1_card1.to_string() << " - " << p1_card2.to_string() << endl;
    cout << "Joueur 2 : " << p2_card1.to_string() << " - " << p2_card2.to_string() << endl;
    cout << "\nBoard : ";
    for (size_t i = 0; i < board.size(); ++i)
        cout << board[i].to_string() << " ";
    cout << "\n-----------------------------------------------\n";
    messagePoints();
}

int main() {
    while (true) {
        Deck deck;
        deck.shuffle();

        Card p1_card1 = deck.cards.back(); deck.cards.pop_back();
        Card p1_card2 = deck.cards.back(); deck.cards.pop_back();
        Card p2_card1 = deck.cards.back(); deck.cards.pop_back();
        Card p2_card2 = deck.cards.back(); deck.cards.pop_back();
        Card flop1 = deck.cards.back(); deck.cards.pop_back();
        Card flop2 = deck.cards.back(); deck.cards.pop_back();
        Card flop3 = deck.cards.back(); deck.cards.pop_back();
        Card turn = deck.cards.back(); deck.cards.pop_back();
        Card river = deck.cards.back(); deck.cards.pop_back();

        vector<Card> board;

        Bind();
        if (player1_chips < 0 || player2_chips < 0) return 0;
        afficherTable(p1_card1, p1_card2, p2_card1, p2_card2, board);
        
        if (!turnFirstPlayer()) main();
        if (player1_chips < 0 || player2_chips < 0) return 0;
        afficherTable(p1_card1, p1_card2, p2_card1, p2_card2, board);
        if (!turnPlayer()) main();
        if (player1_chips < 0 || player2_chips < 0) return 0;
        board = { flop1, flop2, flop3 };
        afficherTable(p1_card1, p1_card2, p2_card1, p2_card2, board);
        if (!turnPlayer()) main();
        if (player1_chips < 0 || player2_chips < 0) return 0;

        board.push_back(turn);
        afficherTable(p1_card1, p1_card2, p2_card1, p2_card2, board);
        if (!turnPlayer()) main();
        if (player1_chips < 0 || player2_chips < 0) return 0;

        board.push_back(river);
        afficherTable(p1_card1, p1_card2, p2_card1, p2_card2, board);
        if (!turnPlayer()) main();
        if (player1_chips < 0 || player2_chips < 0) return 0;

        afficherTable(p1_card1, p1_card2, p2_card1, p2_card2, board);
        comparePlayers(p1_card1, p1_card2, p2_card1, p2_card2, board);

        cout << "\nAppuyez sur ENTREE pour continuer...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();

        if (player1_chips <= 0 || player2_chips <= 0) return 0;
        cout << "\033[2J\033[1;1H";
    }

    return 0;
}
#ifndef DECK_H
#define DECK_H

#include "Card.h"
#include "CircularLinkedList.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

/**
 * @brief Manages the draw pile: builds, shuffles, and deals cards.
 * @author Tam
 */
class Deck {
private:
    static const int NUM_COLORS = 4;
    static const int MAX_NUMBER = 9;
    static const int ACTION_COPIES = 2;

    CircularLinkedList<Card> cards;

public:
    Deck() {}

    /** Build a standard UNO-Lite deck (76 number + 24 action = 100 cards). */
    void build() {
        CardColor colors[] = { RED, BLUE, GREEN, YELLOW };

        for (int c = 0; c < NUM_COLORS; c++) {
            cards.insertBack(Card(colors[c], 0, NUMBER));

            for (int v = 1; v <= MAX_NUMBER; v++) {
                cards.insertBack(Card(colors[c], v, NUMBER));
                cards.insertBack(Card(colors[c], v, NUMBER));
            }

            for (int i = 0; i < ACTION_COPIES; i++) {
                cards.insertBack(Card(colors[c], -1, SKIP));
                cards.insertBack(Card(colors[c], -1, REVERSE));
                cards.insertBack(Card(colors[c], -1, DRAW_TWO));
            }
        }
    }

    /** Shuffle using Fisher-Yates algorithm with rand(). */
    void shuffle() {
        int n = cards.size();
        std::vector<Card> temp;
        for (int i = 0; i < n; i++) {
            temp.push_back(cards.get(i));
        }

        while (!cards.isEmpty()) {
            cards.removeFront();
        }

        for (int i = n - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            std::swap(temp[i], temp[j]);
        }

        for (int i = 0; i < n; i++) {
            cards.insertBack(temp[i]);
        }
    }

    void addCard(Card card) {
        cards.insertBack(card);
    }

    Card drawFromDeck() {
        if (cards.isEmpty()) {
            std::cerr << "drawFromDeck: deck is empty" << std::endl;
            return Card();
        }
        Card top = cards.get(0);
        cards.removeFront();
        return top;
    }

    bool isEmpty() const { return cards.isEmpty(); }
    int size() const { return cards.size(); }
};

#endif // DECK_H

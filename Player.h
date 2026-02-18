#ifndef PLAYER_H
#define PLAYER_H

#include "Card.h"
#include "CircularLinkedList.h"
#include <iostream>
#include <string>

/**
 * @brief Represents a player with a name and a hand of cards.
 * @author Tam
 */
class Player {
public:
    std::string name;
    CircularLinkedList<Card> hand;

    Player() : name("Unknown") {}
    explicit Player(const std::string& name) : name(name) {}

    void drawCard(Card card) {
        hand.insertBack(card);
    }

    Card playCard(int index) {
        Card card = hand.get(index);
        hand.removeAt(index);
        return card;
    }

    bool hasPlayableCard(const Card& topCard) const {
        for (int i = 0; i < hand.size(); i++) {
            if (hand.get(i).isPlayable(topCard)) return true;
        }
        return false;
    }

    void showHand() const {
        for (int i = 0; i < hand.size(); i++) {
            std::cout << "  " << i << ": " << hand.get(i) << std::endl;
        }
    }

    int handSize() const {
        return hand.size();
    }
};

#endif // PLAYER_H

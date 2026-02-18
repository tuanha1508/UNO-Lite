#ifndef CARD_H
#define CARD_H

#include <iostream>
#include <string>

/**
 * @brief Card color and type enumerations for UNO-Lite.
 * @author Tam
 */
enum CardColor { RED, BLUE, GREEN, YELLOW };
enum CardType { NUMBER, SKIP, REVERSE, DRAW_TWO };

/**
 * @brief Represents a single UNO card with color, value, and type.
 * @author Tam
 */
class Card {
public:
    CardColor color;
    int value;
    CardType type;

    Card() : color(RED), value(0), type(NUMBER) {}

    Card(CardColor color, int value, CardType type)
        : color(color), value(value), type(type) {}

    bool operator==(const Card& other) const {
        return color == other.color && value == other.value && type == other.type;
    }

    /** Check if this card can be played on the given top card. */
    bool isPlayable(const Card& topCard) const {
        if (color == topCard.color) return true;
        if (type == NUMBER && topCard.type == NUMBER && value == topCard.value) return true;
        if (type != NUMBER && type == topCard.type) return true;
        return false;
    }

    /** Check if this card can be stacked with another (same number or same action type). */
    bool matchesForStacking(const Card& other) const {
        if (type == NUMBER && other.type == NUMBER) return value == other.value;
        return type != NUMBER && type == other.type;
    }

    std::string colorToString() const {
        switch (color) {
            case RED:    return "Red";
            case BLUE:   return "Blue";
            case GREEN:  return "Green";
            case YELLOW: return "Yellow";
        }
        return "Unknown";
    }

    std::string typeToString() const {
        switch (type) {
            case SKIP:     return "Skip";
            case REVERSE:  return "Reverse";
            case DRAW_TWO: return "Draw Two";
            default:       return std::to_string(value);
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Card& card) {
        os << "[" << card.colorToString() << " " << card.typeToString() << "]";
        return os;
    }
};

#endif // CARD_H

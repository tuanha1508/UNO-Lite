#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Deck.h"
#include "CircularLinkedList.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

/**
 * @brief Game engine that orchestrates the UNO-Lite game loop.
 *
 * Manages player turns via CircularLinkedList<Player*>, handles
 * card effects (Skip, Reverse, Draw Two), and supports card
 * stacking (playing multiple same-number/type cards in one turn).
 *
 * @author Tuan
 */
class Game {
private:
    static const int INITIAL_HAND_SIZE = 7;
    static const int MIN_PLAYERS = 2;
    static const int MAX_PLAYERS = 10;
    static const int DRAW_TWO_PENALTY = 2;

    CircularLinkedList<Player*> players;
    std::vector<Player*> allPlayers;
    Deck deck;
    Card currentTopCard;
    int numPlayers;
    bool gameOver;

    // --- Setup helpers ---

    void dealCards() {
        for (int i = 0; i < numPlayers; i++) {
            for (int j = 0; j < INITIAL_HAND_SIZE; j++) {
                if (!deck.isEmpty()) {
                    allPlayers[i]->drawCard(deck.drawFromDeck());
                }
            }
        }
    }

    void flipFirstCard() {
        currentTopCard = deck.drawFromDeck();
        // If first card is an action card, put it back and reshuffle
        while (currentTopCard.type != NUMBER) {
            deck.addCard(currentTopCard);
            deck.shuffle();
            currentTopCard = deck.drawFromDeck();
        }
        std::cout << "\nFirst card flipped: " << currentTopCard << std::endl;
    }

    // --- Core game logic helpers ---

    bool checkWinner(Player* player) {
        if (player->handSize() == 0) {
            std::cout << "\n========================================" << std::endl;
            std::cout << "  " << player->name << " wins! Congratulations!" << std::endl;
            std::cout << "========================================" << std::endl;
            gameOver = true;
            return true;
        }
        return false;
    }

    void announceUno(Player* player) {
        if (player->handSize() == 1) {
            std::cout << ">> " << player->name << " has UNO!" << std::endl;
        }
    }

    bool drawFromDeckIfPossible(Player* player) {
        if (deck.isEmpty()) {
            std::cout << "Deck is empty! Skipping turn." << std::endl;
            return false;
        }
        Card drawn = deck.drawFromDeck();
        std::cout << "Drew: " << drawn << std::endl;
        player->drawCard(drawn);
        return true;
    }

    // Apply stacked card effects. Accounts for gameLoop's advance() after the turn:
    //   SKIP:     advance N times -> gameLoop advance skips past N players
    //   REVERSE:  odd count flips direction, even cancels out
    //   DRAW_TWO: next player draws 2*N and loses their turn
    void applyStackedEffects(CardType type, int count) {
        switch (type) {
            case SKIP:
                if (count == 1) {
                    std::cout << ">> SKIP! Next player loses their turn." << std::endl;
                } else {
                    std::cout << ">> SKIP x" << count
                              << "! Next " << count << " players lose their turn." << std::endl;
                }
                for (int i = 0; i < count; i++) {
                    players.advance();
                }
                break;

            case REVERSE:
                if (count % 2 == 1) {
                    std::cout << ">> REVERSE! Turn order reversed." << std::endl;
                    players.reverse();
                    if (numPlayers == 2) {
                        players.advance();
                    }
                } else {
                    std::cout << ">> REVERSE x" << count
                              << "! Direction unchanged (cancels out)." << std::endl;
                }
                break;

            case DRAW_TWO: {
                int totalDraw = DRAW_TWO_PENALTY * count;
                players.advance();
                Player* victim = players.getCurrent();
                if (count == 1) {
                    std::cout << ">> DRAW TWO! " << victim->name
                              << " draws 2 cards and loses their turn." << std::endl;
                } else {
                    std::cout << ">> DRAW TWO x" << count << "! " << victim->name
                              << " draws " << totalDraw
                              << " cards and loses their turn." << std::endl;
                }
                for (int i = 0; i < totalDraw; i++) {
                    if (!deck.isEmpty()) {
                        victim->drawCard(deck.drawFromDeck());
                    }
                }
                break;
            }

            case NUMBER:
                break;
        }
    }

    // --- Input helpers ---

    /** Parse comma-separated indices from input, e.g. "3,4" or "3, 4" or "3". */
    std::vector<int> parseIndices(const std::string& input) {
        std::vector<int> indices;
        std::string token;
        for (size_t i = 0; i <= input.size(); i++) {
            char c = (i < input.size()) ? input[i] : ',';
            if (c == ',' || c == ' ') {
                if (!token.empty()) {
                    try {
                        indices.push_back(std::stoi(token));
                    } catch (...) {
                        return {};
                    }
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        return indices;
    }

    void handleForcedDraw(Player* player) {
        std::cout << "\nNo playable cards! Drawing from deck..." << std::endl;
        if (!drawFromDeckIfPossible(player)) return;

        Card drawn = player->hand.get(player->handSize() - 1);
        if (!drawn.isPlayable(currentTopCard)) return;

        std::cout << "You can play the drawn card! Play it? (y/n): ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty() || (input[0] != 'y' && input[0] != 'Y')) return;

        player->playCard(player->handSize() - 1);
        currentTopCard = drawn;
        std::cout << player->name << " plays " << drawn << std::endl;

        announceUno(player);
        if (!checkWinner(player)) {
            applyStackedEffects(drawn.type, 1);
        }
    }

    /** Prompt for card indices. Returns validated indices, or empty vector for draw. */
    std::vector<int> promptCardSelection(Player* player) {
        while (true) {
            std::cout << "\nPlay card(s) (e.g. 0 or 0,2) or -1 to draw: ";
            std::string line;
            std::getline(std::cin, line);

            std::vector<int> indices = parseIndices(line);
            if (indices.empty()) {
                std::cout << "Invalid input. Try again." << std::endl;
                continue;
            }

            if (indices.size() == 1 && indices[0] == -1) return {};

            // Validate all indices are in range
            bool valid = true;
            for (int idx : indices) {
                if (idx < 0 || idx >= player->handSize()) {
                    std::cout << "Invalid index: " << idx << ". Try again." << std::endl;
                    valid = false;
                    break;
                }
            }
            if (!valid) continue;

            // Check for duplicate indices
            std::vector<int> sorted = indices;
            std::sort(sorted.begin(), sorted.end());
            for (size_t i = 1; i < sorted.size(); i++) {
                if (sorted[i] == sorted[i - 1]) {
                    std::cout << "Duplicate index: " << sorted[i] << ". Try again." << std::endl;
                    valid = false;
                    break;
                }
            }
            if (!valid) continue;

            // First card must be playable on the top card
            Card first = player->hand.get(indices[0]);
            if (!first.isPlayable(currentTopCard)) {
                std::cout << first << " cannot be played on "
                          << currentTopCard << ". Try again." << std::endl;
                continue;
            }

            // Remaining cards must match the first for stacking
            for (size_t i = 1; i < indices.size(); i++) {
                Card card = player->hand.get(indices[i]);
                if (!card.matchesForStacking(first)) {
                    std::cout << card << " does not match "
                              << first << " for stacking. Try again." << std::endl;
                    valid = false;
                    break;
                }
            }
            if (!valid) continue;

            return indices;
        }
    }

public:
    Game() : numPlayers(0), gameOver(false) {}

    ~Game() {
        for (Player* p : allPlayers) {
            delete p;
        }
    }

    void setupGame() {
        std::cout << "========================================" << std::endl;
        std::cout << "         Welcome to UNO-Lite!           " << std::endl;
        std::cout << "========================================" << std::endl;

        std::string input;
        do {
            std::cout << "\nEnter number of players ("
                      << MIN_PLAYERS << "-" << MAX_PLAYERS << "): ";
            std::getline(std::cin, input);
            try {
                numPlayers = std::stoi(input);
            } catch (...) {
                numPlayers = 0;
            }
            if (numPlayers < MIN_PLAYERS || numPlayers > MAX_PLAYERS) {
                std::cout << "Please enter a number between "
                          << MIN_PLAYERS << " and " << MAX_PLAYERS << "." << std::endl;
            }
        } while (numPlayers < MIN_PLAYERS || numPlayers > MAX_PLAYERS);

        for (int i = 0; i < numPlayers; i++) {
            std::string name;
            std::cout << "Enter name for Player " << (i + 1) << ": ";
            std::getline(std::cin, name);
            Player* p = new Player(name);
            allPlayers.push_back(p);
            players.insertBack(p);
        }

        deck.build();
        deck.shuffle();
        dealCards();
        flipFirstCard();

        std::cout << "\nGame is ready! Each player has "
                  << INITIAL_HAND_SIZE << " cards.\n" << std::endl;
    }

    void displayGameState() {
        Player* current = players.getCurrent();
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Top card: " << currentTopCard << std::endl;
        std::cout << "Current player: " << current->name
                  << " (" << current->handSize() << " cards)" << std::endl;

        std::cout << "Players: ";
        for (int i = 0; i < static_cast<int>(allPlayers.size()); i++) {
            std::cout << allPlayers[i]->name
                      << "(" << allPlayers[i]->handSize() << ")";
            if (i < static_cast<int>(allPlayers.size()) - 1) std::cout << "  ";
        }
        std::cout << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }

    void playTurn() {
        Player* current = players.getCurrent();

        displayGameState();

        std::cout << "\n" << current->name << "'s hand:" << std::endl;
        current->showHand();

        if (!current->hasPlayableCard(currentTopCard)) {
            handleForcedDraw(current);
            return;
        }

        std::vector<int> indices = promptCardSelection(current);
        if (indices.empty()) {
            drawFromDeckIfPossible(current);
            return;
        }

        // Collect cards before removing (indices shift on removal)
        std::vector<Card> cards;
        for (int idx : indices) {
            cards.push_back(current->hand.get(idx));
        }

        // Remove from highest index first to keep lower indices valid
        std::vector<int> sortedDesc = indices;
        std::sort(sortedDesc.rbegin(), sortedDesc.rend());
        for (int idx : sortedDesc) {
            current->playCard(idx);
        }

        // Last card's color becomes the new top card
        currentTopCard = cards.back();

        // Print what was played
        std::cout << current->name << " plays ";
        for (size_t i = 0; i < cards.size(); i++) {
            std::cout << cards[i];
            if (i < cards.size() - 1) std::cout << " + ";
        }
        std::cout << std::endl;

        announceUno(current);
        if (!checkWinner(current)) {
            applyStackedEffects(cards[0].type, static_cast<int>(cards.size()));
        }
    }

    void gameLoop() {
        gameOver = false;
        while (!gameOver) {
            playTurn();
            if (gameOver) break;
            players.advance();
        }
    }
};

#endif // GAME_H

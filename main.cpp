// ============================================================
// UNO-Lite — A simplified UNO card game in C++
// Single-file architecture: all classes in main.cpp
// ============================================================

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <vector>

// ============================================================
// Section 1: Core Data Structure (Khang)
// Node<T> and CircularLinkedList<T>
// ============================================================

template <typename T>
struct Node {
    T data;
    Node<T>* next;

    Node(T data) : data(data), next(nullptr) {}
};

template <typename T>
class CircularLinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    Node<T>* current;
    int count;
    bool forward;

public:
    CircularLinkedList() : head(nullptr), tail(nullptr), current(nullptr), count(0), forward(true) {}

    ~CircularLinkedList() {
        if (head == nullptr) return;

        // Break circular link, then delete linearly
        tail->next = nullptr;
        Node<T>* curr = head;
        while (curr != nullptr) {
            Node<T>* next = curr->next;
            delete curr;
            curr = next;
        }
        head = nullptr;
        tail = nullptr;
        current = nullptr;
        count = 0;
    }

    // Prevent shallow copies
    CircularLinkedList(const CircularLinkedList&) = delete;
    CircularLinkedList& operator=(const CircularLinkedList&) = delete;

    // --- Insertion ---

    void insertBack(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
            newNode->next = head;
            current = head;
        } else {
            newNode->next = head;
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    void insertFront(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
            newNode->next = head;
            current = head;
        } else {
            newNode->next = head;
            head = newNode;
            tail->next = head;
        }
        count++;
    }

    void insertAt(int index, T value) {
        if (index < 0 || index > count) {
            std::cerr << "insertAt: index " << index << " out of range" << std::endl;
            return;
        }
        if (index == 0) {
            insertFront(value);
            return;
        }
        if (index == count) {
            insertBack(value);
            return;
        }

        Node<T>* prev = head;
        for (int i = 0; i < index - 1; i++) {
            prev = prev->next;
        }
        Node<T>* newNode = new Node<T>(value);
        newNode->next = prev->next;
        prev->next = newNode;
        count++;
    }

    // --- Removal ---

    void removeFront() {
        if (head == nullptr) return;

        if (count == 1) {
            if (current == head) current = nullptr;
            delete head;
            head = nullptr;
            tail = nullptr;
            count = 0;
            return;
        }

        Node<T>* toDelete = head;
        head = head->next;
        tail->next = head;
        if (current == toDelete) current = head;
        delete toDelete;
        count--;
    }

    void removeAt(int index) {
        if (index < 0 || index >= count) {
            std::cerr << "removeAt: index " << index << " out of range" << std::endl;
            return;
        }
        if (index == 0) {
            removeFront();
            return;
        }

        Node<T>* prev = head;
        for (int i = 0; i < index - 1; i++) {
            prev = prev->next;
        }
        Node<T>* toDelete = prev->next;
        prev->next = toDelete->next;

        if (toDelete == tail) {
            tail = prev;
            tail->next = head;
        }
        if (toDelete == current) {
            current = prev->next;
        }

        delete toDelete;
        count--;

        if (count == 0) {
            head = nullptr;
            tail = nullptr;
            current = nullptr;
        }
    }

    void removeByValue(T value) {
        if (head == nullptr) return;

        if (head->data == value) {
            removeFront();
            return;
        }

        Node<T>* prev = head;
        Node<T>* curr = head->next;
        for (int i = 1; i < count; i++) {
            if (curr->data == value) {
                prev->next = curr->next;

                if (curr == tail) {
                    tail = prev;
                    tail->next = head;
                }
                if (curr == current) {
                    current = prev->next;
                }

                delete curr;
                count--;
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    // --- Access ---

    T get(int index) const {
        if (index < 0 || index >= count) {
            std::cerr << "get: index " << index << " out of range" << std::endl;
            return T();
        }
        Node<T>* temp = head;
        for (int i = 0; i < index; i++) {
            temp = temp->next;
        }
        return temp->data;
    }

    int size() const {
        return count;
    }

    bool isEmpty() const {
        return count == 0;
    }

    void display() const {
        if (head == nullptr) {
            std::cout << "(empty)" << std::endl;
            return;
        }
        Node<T>* temp = head;
        for (int i = 0; i < count; i++) {
            std::cout << temp->data;
            if (i < count - 1) std::cout << ", ";
            temp = temp->next;
        }
        std::cout << std::endl;
    }

    // --- Traversal / UNO-specific ---

    T getCurrent() const {
        if (current == nullptr) {
            std::cerr << "getCurrent: list is empty" << std::endl;
            return T();
        }
        return current->data;
    }

    void advance() {
        if (current == nullptr || count <= 1) return;

        if (forward) {
            current = current->next;
        } else {
            // Walk the circle to find predecessor of current
            Node<T>* prev = current;
            while (prev->next != current) {
                prev = prev->next;
            }
            current = prev;
        }
    }

    void reverse() {
        forward = !forward;
    }

    void skipNext() {
        advance();
        advance();
    }
};

// ============================================================
// Section 2: Game Objects (Tam)
// Card, Player, Deck
// ============================================================

enum CardColor { RED, BLUE, GREEN, YELLOW };
enum CardType { NUMBER, SKIP, REVERSE, DRAW_TWO };

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

    bool isPlayable(const Card& topCard) const {
        if (color == topCard.color) return true;
        if (type == NUMBER && topCard.type == NUMBER && value == topCard.value) return true;
        if (type != NUMBER && type == topCard.type) return true;
        return false;
    }

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

class Player {
public:
    std::string name;
    CircularLinkedList<Card> hand;

    Player() : name("Unknown") {}
    Player(const std::string& name) : name(name) {}

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

class Deck {
private:
    CircularLinkedList<Card> cards;

public:
    Deck() {}

    void build() {
        CardColor colors[] = { RED, BLUE, GREEN, YELLOW };

        for (int c = 0; c < 4; c++) {
            // One 0 card per color
            cards.insertBack(Card(colors[c], 0, NUMBER));
            // Two of each 1-9 per color
            for (int v = 1; v <= 9; v++) {
                cards.insertBack(Card(colors[c], v, NUMBER));
                cards.insertBack(Card(colors[c], v, NUMBER));
            }
            // Two Skip, two Reverse, two Draw Two per color
            for (int i = 0; i < 2; i++) {
                cards.insertBack(Card(colors[c], -1, SKIP));
                cards.insertBack(Card(colors[c], -1, REVERSE));
                cards.insertBack(Card(colors[c], -1, DRAW_TWO));
            }
        }
    }

    void shuffle() {
        int n = cards.size();
        std::vector<Card> temp;
        for (int i = 0; i < n; i++) {
            temp.push_back(cards.get(i));
        }
        // Clear the list
        while (!cards.isEmpty()) {
            cards.removeFront();
        }
        // Fisher-Yates shuffle
        for (int i = n - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            std::swap(temp[i], temp[j]);
        }
        // Re-insert shuffled cards
        for (int i = 0; i < n; i++) {
            cards.insertBack(temp[i]);
        }
    }

    Card drawFromDeck() {
        Card top = cards.get(0);
        cards.removeFront();
        return top;
    }

    bool isEmpty() const {
        return cards.isEmpty();
    }

    int size() const {
        return cards.size();
    }
};

// ============================================================
// Section 3: Game Engine (Tuan)
// Game class + main()
// ============================================================

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
        while (currentTopCard.type != NUMBER) {
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
                std::cout << ">> SKIP x" << count
                          << "! Next " << count << " player(s) lose their turn." << std::endl;
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
                std::cout << ">> DRAW TWO x" << count << "! " << victim->name
                          << " draws " << totalDraw
                          << " cards and loses their turn." << std::endl;
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

    // Parse comma-separated indices from input string, e.g. "3,4" or "3, 4" or "3"
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

    // Handle the case where the player has no playable cards.
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

    // Prompt for card indices. Returns validated indices, or empty vector for draw.
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

// ============================================================
// Entry Point
// ============================================================

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    Game game;
    game.setupGame();
    game.gameLoop();

    return 0;
}

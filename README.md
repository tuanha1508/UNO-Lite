# UNO-Lite

A simplified UNO card game built in C++ using a **Circularly Linked List** template data structure. Uses only C++ standard libraries. All interaction is via terminal input/output.

## Architecture Overview

The project is split into **header files per team member**, with a single `main.cpp` entry point:

```
┌─────────────────────────────────────────┐
│  Node.h  /  CircularLinkedList.h        │  ← Khang
│         (Template Data Structure)       │
├──────────────┬──────────────────────────┤
│  Card.h      │  Player.h  /  Deck.h    │  ← Tam
│  (Game Data) │  (Game Objects)          │
├──────────────┴──────────────────────────┤
│  Game.h  /  main.cpp                    │  ← Tuan
│         (Game Engine + Entry Point)     │
└─────────────────────────────────────────┘
```

### File Dependency Graph

```
main.cpp
  └── Game.h
        ├── Player.h
        │     ├── Card.h
        │     └── CircularLinkedList.h
        │           └── Node.h
        └── Deck.h
              ├── Card.h
              └── CircularLinkedList.h
                    └── Node.h
```

**Standard libraries used:** `<iostream>`, `<string>`, `<cstdlib>`, `<ctime>`, `<algorithm>`, `<vector>`

---

## Task Division

### Khang — Core Data Structure (~33.33%)

**Files:** `Node.h`, `CircularLinkedList.h`

**`Node<T>`**
- Template struct holding `data` and `next` pointer

**`CircularLinkedList<T>`** — all templated, reusable for any type

| Method | Purpose |
|---|---|
| `insertBack(T)` | Add to end of list |
| `insertFront(T)` | Add to front of list |
| `insertAt(index, T)` | Add at specific position |
| `removeFront()` | Remove first node |
| `removeAt(index)` | Remove node at index |
| `removeByValue(T)` | Remove a specific element |
| `get(index)` | Get element at index |
| `size()` | Return number of elements |
| `isEmpty()` | Check if empty |
| `display()` | Print all elements |
| `getCurrent()` | Get current node's data |
| `advance()` | Move current pointer forward |
| `reverse()` | Reverse traversal direction (for UNO reverse card) |
| `skipNext()` | Advance by 2 (for UNO skip card) |
| Destructor | Clean up all nodes |

---

### Tam — Game Objects (~33.33%)

**Files:** `Card.h`, `Player.h`, `Deck.h`

**`Card`** (`Card.h`)
- Members: `color` (Red/Blue/Green/Yellow), `value` (0–9), `type` (Number/Skip/Reverse/DrawTwo)
- `operator==` for comparison (needed by `removeByValue`)
- `operator<<` for printing (e.g., `[Red 7]`, `[Blue Skip]`)
- `isPlayable(Card topCard)` — checks if this card can be played on top
- `matchesForStacking(Card other)` — checks if two cards can be stacked (same number or same action type)

**`Player`** (`Player.h`)
- Members: `name`, `CircularLinkedList<Card> hand`
- `drawCard(Card)` — add card to hand
- `playCard(index)` — remove and return card from hand
- `hasPlayableCard(Card topCard)` — check if player can play
- `showHand()` — display all cards in hand
- `handSize()` — number of cards

**`Deck`** (`Deck.h`)
- Builds a full UNO-Lite deck (76 number cards + 24 action cards = 100 total)
- `shuffle()` — randomize the deck using Fisher-Yates with `rand()`
- `drawFromDeck()` — pop top card
- `isEmpty()` — check if deck is exhausted
- Uses `CircularLinkedList<Card>` internally

---

### Tuan — Game Engine (~33.33%)

**Files:** `Game.h`, `main.cpp`

**`Game`** (`Game.h`) — orchestrates everything
- `CircularLinkedList<Player*> players` — the player turn order (key circular list usage)
- Members: `Deck`, `currentTopCard`, `numPlayers`, `gameOver`

| Method | Purpose |
|---|---|
| `setupGame()` | Prompt for player names, create players, deal 7 cards each |
| `playTurn()` | Show hand, prompt for card indices (supports stacking), or draw |
| `applyStackedEffects()` | Handle Skip, Reverse, Draw Two with stacking multiplier |
| `checkWinner()` | Player with 0 cards wins |
| `gameLoop()` | Main loop: cycle turns until someone wins |
| `displayGameState()` | Print top card, current player, card counts |
| `promptCardSelection()` | Parse comma-separated input for multi-card plays |

**`main()`** (`main.cpp`)
- Seeds RNG, creates a `Game` instance, calls `setupGame()` then `gameLoop()`

---

## How Circular Linked List is Used

| UNO Feature | Circular List Operation |
|---|---|
| Players take turns in a loop | `advance()` on `CircularLinkedList<Player*>` |
| Reverse card | `reverse()` changes traversal direction |
| Skip card | `advance()` skips the next player |
| Player's hand of cards | `CircularLinkedList<Card>` per player |
| Player wins (0 cards) | `removeByValue()` removes them from turn order |

---

## Card Stacking (House Rule)

Players can play multiple cards of the **same number** or **same action type** in one turn using comma-separated input:

```
Play card(s) (e.g. 0 or 0,2) or -1 to draw: 0,3,5
Alice plays [Red 5] + [Blue 5] + [Green 5]
```

Stacked effects compound:

| Stack | Effect |
|---|---|
| 2x Skip | Skip 2 players |
| 2x Draw Two | Next player draws 4 |
| 2x Reverse | Cancels out (no direction change) |

---

## Build & Run

```bash
g++ -std=c++17 -o uno main.cpp
./uno
```

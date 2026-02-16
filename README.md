# UNO-Lite

A simplified UNO card game built in C++ using a **Circularly Linked List** template data structure.

## Architecture Overview

```
┌─────────────────────────────────────┐
│            main.cpp                 │  ← Tuan
│         (Game Engine + UI)          │
├──────────────┬──────────────────────┤
│  Player      │   Deck              │  ← Tam
│  Card        │   Hand management   │
├──────────────┴──────────────────────┤
│  CircularLinkedList<T>              │  ← Khang
│  Node<T>                           │
└─────────────────────────────────────┘
```

---

## Task Division

### Khang — Core Data Structure (~35%)

**Files:** `Node.h`, `CircularLinkedList.h`

This is the **foundation** everyone else depends on, so it should be started **first**.

**`Node<T>`**
- Template struct/class holding `data` and `next` pointer

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

### Tam — Game Objects (~30%)

**Files:** `Card.h`, `Card.cpp`, `Player.h`, `Player.cpp`, `Deck.h`, `Deck.cpp`

**`Card`**
- Members: `color` (Red/Blue/Green/Yellow), `value` (0–9), `type` (Number/Skip/Reverse/DrawTwo)
- `operator==` for comparison (needed by `removeByValue`)
- `operator<<` for printing (e.g., `[Red 7]`, `[Blue Skip]`)
- `isPlayable(Card topCard)` — checks if this card can be played on top

**`Player`**
- Members: `name`, `CircularLinkedList<Card> hand`
- `drawCard(Card)` — add card to hand
- `playCard(index)` — remove and return card from hand
- `hasPlayableCard(Card topCard)` — check if player can play
- `showHand()` — display all cards in hand
- `handSize()` — number of cards

**`Deck`**
- Builds a full UNO-lite deck (e.g., 76 number cards + 24 action cards)
- `shuffle()` — randomize the deck
- `drawFromDeck()` — pop top card
- `isEmpty()` — check if deck is exhausted
- Uses `CircularLinkedList<Card>` internally

---

### Tuan — Game Engine & UI (~35%)

**Files:** `Game.h`, `Game.cpp`, `main.cpp`

**`Game`** — orchestrates everything
- `CircularLinkedList<Player*> players` — the player turn order (this is the **key** circular list usage)
- Members: `Deck`, `discardPile`, `currentTopCard`, `direction`

| Method | Purpose |
|---|---|
| `setupGame()` | Ask for player names, create players, deal 7 cards each |
| `playTurn()` | Show current player's hand, prompt for card or draw |
| `applyCardEffect()` | Handle Skip, Reverse, Draw Two |
| `checkWinner()` | Player with 0 cards wins |
| `gameLoop()` | Main loop: cycle turns until someone wins |
| `displayGameState()` | Show top card, current player, card counts |

**`main.cpp`**
- Welcome menu: Start Game / Rules / Quit
- Calls `Game.setupGame()` then `Game.gameLoop()`

---

## How Circular Linked List is Used

| UNO Feature | Circular List Operation |
|---|---|
| Players take turns in a loop | `advance()` on `CircularLinkedList<Player*>` |
| Reverse card | `reverse()` changes traversal direction |
| Skip card | `skipNext()` skips the next player |
| Player's hand of cards | `CircularLinkedList<Card>` per player |
| Player wins (0 cards) | `removeByValue()` removes them from turn order |

---

## Build & Run

```bash
g++ -std=c++17 -o uno main.cpp Card.cpp Player.cpp Deck.cpp Game.cpp
./uno
```

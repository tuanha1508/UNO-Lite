# UNO-Lite

A simplified UNO card game built in C++ using a **Circularly Linked List** template data structure. The entire program is contained in a single `main.cpp` file and uses only C++ standard libraries. All interaction is via terminal input/output.

## Architecture Overview

Everything lives in **one file** (`main.cpp`), organized top-to-bottom in this order:

```
┌─────────────────────────────────────┐
│  Node<T>  /  CircularLinkedList<T>  │  ← Khang
│         (Template Data Structure)   │
├──────────────┬──────────────────────┤
│  Card        │   Player  /  Deck   │  ← Tam
│  (Game Data) │   (Game Objects)    │
├──────────────┴──────────────────────┤
│  Game  /  main()                   │  ← Tuan
│         (Game Engine + Entry)      │
└─────────────────────────────────────┘
```

**Standard libraries used:** `<iostream>`, `<string>`, `<cstdlib>`, `<ctime>`, `<algorithm>`, `<vector>` (for shuffle only)

---

## Task Division

### Khang — Core Data Structure (~33.33%)

**Section:** Top of `main.cpp` (template classes)

This is the **foundation** everything else depends on, so it should be written **first**.

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

**Section:** Middle of `main.cpp` (Card, Player, Deck classes)

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
- `shuffle()` — randomize the deck using Fisher-Yates with `rand()`
- `drawFromDeck()` — pop top card
- `isEmpty()` — check if deck is exhausted
- Uses `CircularLinkedList<Card>` internally

---

### Tuan — Game Engine (~33.33%)

**Section:** Bottom of `main.cpp` (Game class + `main()`)

**`Game`** — orchestrates everything
- `CircularLinkedList<Player*> players` — the player turn order (this is the **key** circular list usage)
- Members: `Deck`, `discardPile`, `currentTopCard`, `direction`

| Method | Purpose |
|---|---|
| `setupGame()` | Prompt for player names, create players, deal 7 cards each |
| `playTurn()` | Print current player's hand, prompt for card index or draw |
| `applyCardEffect()` | Handle Skip, Reverse, Draw Two |
| `checkWinner()` | Player with 0 cards wins |
| `gameLoop()` | Main loop: cycle turns until someone wins |
| `displayGameState()` | Print top card, current player, card counts |

**`main()`**
- Creates a `Game` instance, calls `setupGame()` then `gameLoop()`
- All interaction via terminal input/output (`cin`/`cout`)

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
g++ -std=c++17 -o uno main.cpp
./uno
```

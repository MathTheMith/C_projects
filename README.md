# 🧪 C_projects

> A collection of standalone C/C++ projects built outside the 42 curriculum — games, solvers and small tools written for practice.

![Language](https://img.shields.io/badge/language-C%20%2F%20C%2B%2B-blue)
![Status](https://img.shields.io/badge/status-in%20progress-yellow)

---

## Description

Each folder is an independent project with its own `Makefile`. They range from terminal utilities
to graphical games, written to practice specific things: game loops and rendering, backtracking
algorithms, and small game-logic engines.

| Project | Language | What it is |
| --- | --- | --- |
| [`chess_game`](chess_game) | C | A playable chess game with a bitboard-based board representation and Raylib rendering |
| [`poker_bot`](poker_bot) | C++ | A two-player Texas Hold'em simulator: deck/card model, hand evaluator, preflop logic |
| [`sudoku_solver`](sudoku_solver) | C | A backtracking sudoku solver, plus a grid generator |
| [`tetris`](tetris) | C | A terminal Tetris — piece rotation, line clearing, rendering |
| [`wordle`](wordle) | C | A terminal Wordle clone, picking a random word from a word list and colouring guesses |

## Build

Each project builds independently:

```sh
cd chess_game && make
```

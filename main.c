#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <getopt.h>
#include <assert.h>

#define HAND_SIZE 5
#define NUM_PLAYERS 2
#define MAX_TURNS 10


// Represent's both of a player's hands
typedef struct _player {
    uint8_t left: 4;
    uint8_t right: 4;
} player;

// Returns true if the player has zero for both hands
bool isOut(player p) {
    return !p.left && !p.right;
}

// Counts how many branches have been terminated and therefore computed
long count = 0;

// Keeps track of the number of turns in the shortest game
int shortestGame = MAX_TURNS;

// Keeps track of the hands
player shortestGameHistory[NUM_PLAYERS][MAX_TURNS+1];


// Recursive function, computes all branch moves possible for A on B
void advance(player h[NUM_PLAYERS][MAX_TURNS+1], player a, player b, int turns) {

    // Variables for storing
	assert(NUM_PLAYERS == 2);
    int A = (turns % NUM_PLAYERS) ? (1) : (0);
    int B = (turns % NUM_PLAYERS) ? (0) : (1);

    // Cut turns off if it reaches max
    if (turns >= MAX_TURNS) {
        //printf("Found branch with maximum turns (%d)\n", MAX_TURNS);
        count++;
        return;
    }

    // Handle Roll-overs
    a.left %= HAND_SIZE;
    a.right %= HAND_SIZE;
    b.left %= HAND_SIZE;
    b.right %= HAND_SIZE;

    // Check previous hands for a match
    for (int i = 0; i < turns; i++) {
        if (
            (h[0][i].left == h[A][turns].left) &&
            (h[0][i].right == h[A][turns].right) &&
            (h[1][i].left == h[B][turns].left) &&
            (h[1][i].right == h[B][turns].right)) {
                // we have a match
            printf("Found circular game, ending early at turn %d\n", turns);
            count++;
            return;
        }
    }

    // If game is over, return and print number of turns
    if (isOut(a) || isOut(b)) {
        printf("Game is over! (%d turns)\n", turns);
        if (isOut(a)) {
            printf("B wins\n");
        } else {
            printf("A wins\n");
        }

        // Print history
        for (int i = 0; i <= turns; i++) {
            // Print a turn
            printf("Turn %d: (%d, %d) - (%d, %d)\n", i, h[0][i].left, h[0][i].right, h[1][i].left, h[1][i].right);
        }
        count++;

        if (turns < shortestGame) {
            shortestGame = turns;
            memcpy(shortestGameHistory, h, sizeof(player) * 2 * (MAX_TURNS + 1));
        }
        return;
    }

    // A takes a turn
    turns++;

    // Try Left-tap-Left
    if (a.left && b.left) {
        b.left += a.left;    // Modify
        // Store history
        h[A][turns] = a;
        h[B][turns] = b;
        advance(h, b, a, turns); // Recurse
        b.left -= a.left;    // Restore
    }

    // Try Left-tap-Right
    if (a.left && b.right) {
        b.right += a.left;   // Modify
        // Store history
        h[A][turns] = a;
        h[B][turns] = b;
        advance(h, b, a, turns); // Recurse
        b.right -= a.left;   // Restore
    }

    // Try Right-tap-Left
    if (a.right && b.left) {
        b.left += a.right;    // Modify
        // Store history
        h[A][turns] = a;
        h[B][turns] = b;
        advance(h, b, a, turns); // Recurse
        b.left -= a.right;    // Restore
    }

    // Try Right-tap-Right
    if (a.right && b.right) {
        b.right += a.right;   // Modify
        // Store history
        h[A][turns] = a;
        h[B][turns] = b;
        advance(h, b, a, turns); // Recurse
        b.right -= a.right;   // Restore
    }

    // Try all possible right shift (move fingers from left to right)
    if (a.left) {
        //for (int i = 1; i <= a.left; i++) {
        for (int i = 1; i < a.left; i++) {
            if ((a.left == (a.right + i)) && (a.right == (a.left - i))) {
                continue;   // Skip pure swaps
            }
            if ((a.right + i) < HAND_SIZE) {
                // Execute shift
                a.left -= i;    // Modify
                a.right += i;
                // Store history
                h[A][turns] = a;
                h[B][turns] = b;
                advance(h, b, a, turns);   // Recurse
                a.left += i;    // Restore
                a.right -= i;
            }
        }
    }

    // Try all possible left shift (move fingers from right to left)
    if (a.right) {
        //for (int i = 1; i <= a.right; i++) {
        for (int i = 1; i < a.right; i++) {
            if ((a.right == (a.left + i)) && (a.left == (a.right - i))) {
                continue;   // Skip pure swaps
            }
            if ((a.left + i) < HAND_SIZE) {
                // Execute shift
                a.right -= i;    // Modify
                a.left += i;
                // Store history
                h[A][turns] = a;
                h[B][turns] = b;
                advance(h, b, a, turns);   // Recurse
                a.right += i;    // Restore
                a.left -= i;
            }
        }
    }


}

int main() {

    // 0 is first turn, 1-10 is ith turn so MAX + 1 items
    player history[NUM_PLAYERS][MAX_TURNS+1];

	// Start off both players at (1,1)
    player human;
    player cpu;
    human.left = 1;
    human.right = 1;
    cpu.left = 1;
    cpu.right = 1;

    // Store starting value as turn 0
    history[0][0] = human;
    history[1][0] = cpu;

    advance(history, human, cpu, 0);

    printf("\n\nTotal branches: %ld\n\n", count);

    printf("Shortest game: %d moves\n", shortestGame);
    // Print history
    for (int i = 0; i <= shortestGame; i++) {
        // Print a turn
        printf("Turn %d: (%d, %d) - (%d, %d)\n", i, shortestGameHistory[0][i].left, shortestGameHistory[0][i].right, shortestGameHistory[1][i].left, shortestGameHistory[1][i].right);
    }
}

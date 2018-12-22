#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define HAND_SIZE 5
#define MAX_TURNS 10



typedef struct _player {
    uint8_t left;
    uint8_t right;
} player;

bool isLeftHandOut(player p) {
    return !p.left;
}

bool isRightHandOut(player p) {
    return !p.right;
}

bool isOut(player p) {
    return isLeftHandOut(p) && isRightHandOut(p);
}

long count = 0;
int shortestGame = MAX_TURNS;
player shortestGameHistory[2][MAX_TURNS+1];

void advance(player h[2][MAX_TURNS+1], player a, player b, int turns) {

    // Variables for storing
    int A = (turns % 2) ? (1) : (0);
    int B = (turns % 2) ? (0) : (1);

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

    // Try L-L
    if (a.left && b.left) {
        b.left += a.left;    // Modify
        // Store history
        h[A][turns] = a;
        h[B][turns] = b;
        advance(h, b, a, turns); // Recurse
        b.left -= a.left;    // Restore
    }

    // Try L-R
    if (a.left && b.right) {
        b.right += a.left;   // Modify
        // Store history
        h[A][turns] = a;
        h[B][turns] = b;
        advance(h, b, a, turns); // Recurse
        b.right -= a.left;   // Restore
    }

    // Try R-L
    if (a.right && b.left) {
        b.left += a.right;    // Modify
        // Store history
        h[A][turns] = a;
        h[B][turns] = b;
        advance(h, b, a, turns); // Recurse
        b.left -= a.right;    // Restore
    }

    // Try R-R
    if (a.right && b.right) {
        b.right += a.right;   // Modify
        // Store history
        h[A][turns] = a;
        h[B][turns] = b;
        advance(h, b, a, turns); // Recurse
        b.right -= a.right;   // Restore
    }

    // Try all possible right shift (L->R)
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

    // Try all possible left shift (R->L)
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
    // Create history
    player history[2][MAX_TURNS+1];

    player human;
    player cpu;
    human.left = 1;
    human.right = 1;
    cpu.left = 1;
    cpu.right = 1;

    // Store history
    history[0][0] = human;
    history[1][0] = cpu;

    advance(history, human, cpu, 0);

    printf("\n\nTotal branches: %d\n\n", count);

    printf("Shortest game: %d moves\n", shortestGame);
    // Print history
        for (int i = 0; i <= shortestGame; i++) {
            // Print a turn
            printf("Turn %d: (%d, %d) - (%d, %d)\n", i, shortestGameHistory[0][i].left, shortestGameHistory[0][i].right, shortestGameHistory[1][i].left, shortestGameHistory[1][i].right);
        }
}
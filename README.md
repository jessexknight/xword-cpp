# C++ Crossword Generator

From a list of words, generate a crossword (solved) and display it.

### Details

The crossword is represented as a 2D array of characters and BLANKs.

The user-supplied list of words are added in a random order to the array so that
they intersect (matching letters) with words already added. Due to the
randomness, some word orders may leave no valid intersections near the end,
so sometimes not all words will be added. The building process is repeated
iMaxiter times (randomly) to hopefully avoid this, and also to minimize the
size required for the array.

The crossword (solved) is displayed using CIL.
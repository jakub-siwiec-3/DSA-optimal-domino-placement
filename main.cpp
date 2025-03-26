#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

/*For a given rectangular board, the program finds the optimal arrangement of domino tiles such that the sum of the
 * values on the covered fields is maximized. It utilizes bit masking and dynamic programming.
 */


// Recursively generate vertical block masks that do not conflict with the given baseMask.
// baseMask: the bit mask representing horizontal placements already used
// currentMask: the current vertical block mask being constructed
// results: vector in which valid vertical block masks will be stored
// index: current row index we are considering
// numRows: total number of rows on the board
void generateVerticalBlocks(int baseMask, int currentMask, vector<int>& results, int index, int numRows) {
    // When we reach the last row (or beyond), add the current vertical mask to results.
    if (index >= numRows - 1) {
        results.push_back(currentMask);
        return;
    }

    // Check if the current and next rows (positions) are free (i.e., not used in the baseMask).
    bool currentFree = !(baseMask & (1 << (numRows - index - 1)));
    bool nextFree = !(baseMask & (1 << (numRows - index - 2)));

    if (currentFree && nextFree) {
        // Option 1: do not place a vertical block at this position.
        generateVerticalBlocks(baseMask, currentMask, results, index + 1, numRows);

        // Option 2: place a vertical block covering current and next row.
        int newMask = currentMask;
        newMask |= (1 << (numRows - index - 1));
        newMask |= (1 << (numRows - index - 2));
        generateVerticalBlocks(baseMask, newMask, results, index + 2, numRows);
    } else {
        generateVerticalBlocks(baseMask, currentMask, results, index + 1, numRows);
    }
}

// Reads the board from standard input.
vector<vector<int>> readBoard(int numRows, int numColumns) {
    vector<vector<int>> board(numRows, vector<int>(numColumns, 0));
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numColumns; j++) {
            cin >> board[i][j];
        }
    }
    return board;
}

// Precompute the horizontal block sums for each possible mask and starting column.
// For each mask (representing positions where a horizontal block can be placed),
// horizontalBlockSum[mask][column] equals the sum of board values for a horizontal block
// covering positions (row, column) and (row, column+1) for rows where the mask bit is set.
vector<vector<int>> computeHorizontalBlockSum(const vector<vector<int>>& board, int numRows, int numColumns) {
    int totalMasks = 1 << numRows;
    vector<vector<int>> horizontalBlockSum(totalMasks, vector<int>(numColumns - 1, 0));

    // Loop over each starting column for horizontal blocks.
    for (int col = 0; col < numColumns - 1; col++) {
        // For each row, add contribution to all masks that have that row set.
        for (int row = 0; row < numRows; row++) {
            int maskBit = 1 << row;
            // Note: the board rows are considered from bottom to top in the original code.
            // Here we mimic that by accessing board[numRows - row - 1][col] and board[numRows - row - 1][col+1].
            for (int mask = 0; mask < totalMasks; mask++) {
                if (mask & maskBit) {
                    horizontalBlockSum[mask][col] += board[numRows - row - 1][col] + board[numRows - row - 1][col + 1];
                }
            }
        }
    }
    return horizontalBlockSum;
}

// Generates all valid horizontal block placements (masks) for each base mask.
// For a given base mask, a valid horizontal mask is any mask that can be placed in the remaining free positions.
vector<vector<int>> generateMatchingHorizontalMasks(int numRows) {
    int totalMasks = 1 << numRows;
    vector<vector<int>> matchingHorizontalMasks(totalMasks);

    for (int baseMask = 0; baseMask < totalMasks; baseMask++) {
        vector<int> positionsFree;
        // Collect positions where baseMask has a 0 bit (free position).
        for (int row = 0; row < numRows; row++) {
            if ((baseMask & (1 << row)) == 0) {
                positionsFree.push_back(row);
            }
        }
        int freeCount = positionsFree.size();
        // For each possible assignment on free positions generate a mask.
        for (int assign = 0; assign < (1 << freeCount); assign++) {
            int newMask = 0;
            for (int bit = 0; bit < freeCount; bit++) {
                if (assign & (1 << bit)) {
                    newMask |= (1 << positionsFree[bit]);
                }
            }
            matchingHorizontalMasks[baseMask].push_back(newMask);
        }
    }
    return matchingHorizontalMasks;
}

// Generates matching vertical masks for every possible horizontal mask.
vector<vector<int>> generateMatchingVerticalMasks(int numRows) {
    int totalMasks = 1 << numRows;
    vector<vector<int>> matchingVerticalMasks(totalMasks);
    // For each horizontal mask, generate all vertical masks that do not conflict.
    for (int baseMask = 0; baseMask < totalMasks; baseMask++) {
        generateVerticalBlocks(baseMask, 0, matchingVerticalMasks[baseMask], 0, numRows);
    }
    return matchingVerticalMasks;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // Read dimensions: number of columns and number of rows.
    int numColumns, numRows;
    cin >> numColumns >> numRows;

    // Read the board values.
    vector<vector<int>> board = readBoard(numRows, numColumns);

    int totalMasks = 1 << numRows;

    // Precompute horizontal block sums.
    vector<vector<int>> horizontalBlockSum = computeHorizontalBlockSum(board, numRows, numColumns);

    // Precompute matching vertical masks for each horizontal mask.
    vector<vector<int>> matchingVerticalMasks = generateMatchingVerticalMasks(numRows);

    // Precompute matching horizontal masks.
    vector<vector<int>> matchingHorizontalMasks = generateMatchingHorizontalMasks(numRows);

    // DP table: dp[mask][col] represents the maximum sum achievable up to column col ending with configuration "mask"
    vector<vector<long long>> dp(totalMasks, vector<long long>(numColumns, 0));
    long long overallMax = 0;

    // Main DP loop over columns.
    for (int col = 0; col < numColumns; col++) {
        // Process vertical block placements for the current column.
        for (int horizontalMask = 0; horizontalMask < totalMasks; horizontalMask++) {
            // Try all vertical masks that match with the current horizontal configuration.
            for (int verticalMask : matchingVerticalMasks[horizontalMask]) {
                int verticalBlockSum = 0;
                // Sum values for vertical blocks placed in this column.
                for (int row = 0; row < numRows; row++) {
                    if (verticalMask & (1 << row)) {
                        // Mimic the bottom-to-top order as in original code.
                        verticalBlockSum += board[numRows - row - 1][col];
                    }
                }
                // Combine horizontal mask and vertical block mask.
                int combinedMask = horizontalMask | verticalMask;
                dp[combinedMask][col] = max(dp[combinedMask][col], dp[horizontalMask][col] + verticalBlockSum);
            }
            // For the first column we do not consider horizontal placements.
            if (col == 0) break;
        }
        // Process horizontal block placements, which span the current and next column.
        if (col < numColumns - 1) {
            for (int horizontalMask = 0; horizontalMask < totalMasks; horizontalMask++) {
                // Try all valid horizontal placements from the current horizontal configuration.
                for (int newMask : matchingHorizontalMasks[horizontalMask]) {
                    // Only add horizontal block sum if it is non-negative.
                    int addValue = (horizontalBlockSum[newMask][col] >= 0) ? horizontalBlockSum[newMask][col] : 0;
                    dp[newMask][col + 1] = max(dp[newMask][col + 1], dp[horizontalMask][col] + addValue);
                }
            }
        }
    }

    // Find the maximum sum achieved in the last column.
    for (int mask = 0; mask < totalMasks; mask++) {
        overallMax = max(overallMax, dp[mask][numColumns - 1]);
    }

    cout << overallMax;
    return 0;
}

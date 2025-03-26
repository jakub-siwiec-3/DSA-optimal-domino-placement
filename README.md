# Optimal Domino Placement using Bitmasking and Dynamic Programming

## Overview  
This program finds the optimal arrangement of domino tiles on a given rectangular board to maximize the sum of covered cell values. It efficiently explores valid tile placements using **bitmasking** and solves the problem using **dynamic programming (DP)**.  

## Algorithmic Techniques  

### 1. Bitmasking for State Representation  
- Each row configuration is represented as a bitmask (an integer where each bit indicates whether a cell is occupied).  
- This allows fast set operations (AND/OR) to check valid placements efficiently.  

### 2. Dynamic Programming (DP) Approach  
- **State Definition:** `dp[mask][col]` stores the maximum sum achievable up to column `col` with row configuration `mask`.  
- **State Transition:**  
  - **Vertical dominoes:** Recursively generate all valid placements and update `dp`.  
  - **Horizontal dominoes:** Precompute possible transitions between row configurations to avoid redundant calculations.  

### 3. Precomputing Valid Transitions  
- To speed up the DP transitions, valid placements of **vertical** and **horizontal** dominoes are **precomputed** before running the main DP loop.  
- This significantly reduces redundant calculations and improves efficiency.  

## Complexity Analysis  
- **State Space:** `O(2^k * n)`, where `k` is the number of rows and `n` is the number of columns.  
- **Preprocessing:** `O(2^k * k)`, for generating valid placements.  
- **DP Transitions:** `O(n * 2^k * k)`, since for each column, every mask checks possible transitions.  
- The approach is feasible for small-to-moderate values of `k` (typically `k ≤ 10`).  

## Conclusion  
This program efficiently solves the **maximum weighted domino tiling** problem using **bitmasking + DP**, avoiding brute-force approaches that would be computationally infeasible for large grids.

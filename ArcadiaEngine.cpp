// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

class ConcretePlayerTable : public PlayerTable {
private:

    static const int tableSize = 101;
    struct Entry {
        int playerId;
        string playerName;
        bool isBusy= false;
        bool isDeleted =false;

    };

    Entry hashTable [tableSize];

    int hash1(int key){
        double A = 0.6180339887;
        double m= (key*A) - floor(key*A);
        return floor(tableSize* m);
    }

    int hash2(int key){
        return 7 -(key % 7);
    }

public:
    ConcretePlayerTable() {
        for(int i=0;i<tableSize;i++){
            hashTable[i].playerId= INT_MIN;
            hashTable[i].playerName="";
            hashTable[i].isBusy=false;
            hashTable[i].isDeleted=false;
        }

    }

    void insert(int playerID, string name) override {

        int j=0;
        int key;
        while(j < tableSize){
            key = (hash1(playerID) + j*hash2(playerID)) %tableSize;

            if(!hashTable[key].isBusy || hashTable[key].isDeleted){
                hashTable[key].playerId = playerID;
                hashTable[key].playerName= name;
                hashTable[key].isBusy=true;
                hashTable[key].isDeleted=false;
                return;
            }

            if(hashTable[key].playerId == playerID){
                hashTable[key].playerName=name;
                return;
            }
            j++;
        }
        cout <<"Sorry: Table is full"<<endl;

    }

    string search(int playerID) override {
        int j=0;int key;
        while(j < tableSize){
            key = (hash1(playerID) + j*hash2(playerID))%tableSize;
            if(hashTable[key].playerId == playerID && !hashTable[key].isDeleted){
                return hashTable[key].playerName;
            }
            if(!hashTable[key].isBusy){
                return "";
            }
            j++;

        }
        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard {
private:
    struct SNode {
        int playID;
        int score;
        SNode** forward;
        int nodeL;

        SNode(int lev, int playerID, int score) {
            this->playID = playerID;
            this->score = score;
            this->nodeL = lev;
            forward = new SNode*[lev + 1];
            for (int j = 0; j <= lev; ++j) {
                forward[j] = nullptr;
            }
        }

        ~SNode() {
            delete[] forward;
        }
    };

    static const int MAX_LEVEL = 16;
    static constexpr double P = 0.5;

    SNode* head;
    int lev;

    int randomLevel() {
        int lvl = 0;
        while ((rand() / (double)RAND_MAX) < P && lvl < MAX_LEVEL) {
            ++lvl;
        }
        return lvl;
    }

public:
    ConcreteLeaderboard() {
        srand((unsigned)time(nullptr));
        lev = 0;
        head = new SNode(MAX_LEVEL, -1, INT_MAX); // dummy head
    }

    void addScore(int playerID, int sco) override {
        SNode* upd[MAX_LEVEL + 1];
        SNode* curr = head;

        for (int j = lev; j >= 0; --j) {
            while (curr->forward[j] != nullptr &&
                   (curr->forward[j]->score > sco ||
                    (curr->forward[j]->score == sco &&
                     curr->forward[j]->playID < playerID))) {
                curr = curr->forward[j];
            }
            upd[j] = curr;
        }

        int newLevel = randomLevel();
        if (newLevel > lev) {
            for (int j = lev + 1; j <= newLevel; ++j) upd[j] = head;
            lev = newLevel;
        }

        SNode* newNode = new SNode(newLevel, playerID, sco);
        for (int j = 0; j <= newLevel; ++j) {
            newNode->forward[j] = upd[j]->forward[j];
            upd[j]->forward[j] = newNode;
        }
    }

    void removePlayer(int playerID) override {
        SNode* upd[MAX_LEVEL + 1];
        SNode* curr = head;

        for (int j = lev; j >= 0; --j) {
            while (curr->forward[j] != nullptr &&
                   curr->forward[j]->playID < playerID) {
                curr = curr->forward[j];
            }
            upd[j] = curr;
        }

        curr = curr->forward[0];
        if (curr == nullptr || curr->playID != playerID) return;

        for (int j = 0; j <= lev; ++j) {
            if (upd[j]->forward[j] != curr) break;
            upd[j]->forward[j] = curr->forward[j];
        }
        delete curr;

        while (lev > 0 && head->forward[lev] == nullptr) {
            lev--;
        }
    }

    vector<int> getTopN(int n) override {
        vector<int> res;
        SNode* curr = head->forward[0];
        int count = 0;
        while (curr != nullptr && count < n) {
            res.push_back(curr->playID);
            curr = curr->forward[0];
            count++;
        }
        return res;
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree {
private:
    // TODO: Define your Red-Black Tree node structure
    // Hint: Each node needs: id, price, color, left, right, parent pointers

public:
    ConcreteAuctionTree() {
        // TODO: Initialize your Red-Black Tree
    }

    void insertItem(int itemID, int price) override {
        // TODO: Implement Red-Black Tree insertion
        // Remember to maintain RB-Tree properties with rotations and recoloring
    }

    void deleteItem(int itemID) override {
        // TODO: Implement Red-Black Tree deletion
        // This is complex - handle all cases carefully
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    // TODO: Implement partition problem using DP
    // Goal: Minimize |sum(subset1) - sum(subset2)|
    // Hint: Use subset sum DP to find closest sum to total/2

    int coins_sum = 0;
    for (int coin : coins) {
        coins_sum += coin;
    }
    int half_sum = coins_sum / 2;

    vector<bool> possible_sum(half_sum + 1, false);
    possible_sum[0] = true;

    for (int i = 0; i < n; i++) {
        for (int j = half_sum; j >= coins[i]; j--) {
            if (possible_sum[j - coins[i]]) {
                possible_sum[j] = true;
            }
        }
    }
    int subset_sum = 0;
    for (int i = half_sum; i >= 0; i--) {
        if (possible_sum[i]) {
            subset_sum = i;
            break;
        }
    }
    int difference = coins_sum - 2 * subset_sum;
    return difference;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    // TODO: Implement 0/1 Knapsack using DP
    // items = {weight, value} pairs
    // Return maximum value achievable within capacity

    int n = items.size();
    vector<int> max_value (capacity + 1, 0);

    for (int i = 0 ; i < n ; i++) {
        int weight = items[i].first;
        int value = items[i].second;

        for (int curr_cap = capacity; curr_cap >= weight; curr_cap --) {
            max_value[curr_cap] = max(max_value[curr_cap], max_value[curr_cap - weight] + value);
        }
    }
    return max_value [capacity];
}

long long InventorySystem::countStringPossibilities(string s) {
    // TODO: Implement string decoding DP
    // Rules: "uu" can be decoded as "w" or "uu"
    //        "nn" can be decoded as "m" or "nn"
    // Count total possible decodings

    const long long mod = 1000000007;
    int text_length = s.length();
    if (text_length == 0) return 1;

    vector<long long> ways_num(text_length + 1, 0);
    ways_num[0] = 1;

    for (int i = 1; i <= text_length; i++) {
        ways_num[i] = ways_num[i - 1];

        if (i >= 2) {
            string char_pairs = s.substr(i - 2, 2);
            if (char_pairs == "uu" || char_pairs == "nn") {
                ways_num[i] = (ways_num[i] + ways_num[i - 2]) % mod;
            }
        }
        ways_num[i] %= mod ;
    }
    return ways_num[text_length];
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    // TODO: Implement path existence check using BFS or DFS
    // edges are bidirectional
    return false;
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>>& roadData) {
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected
    return -1;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    return "0";
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char>& tasks, int n) {
    // TODO: Implement task scheduler with cooling time
    // Same task must wait 'n' intervals before running again
    // Return minimum total intervals needed (including idle time)
    // Hint: Use greedy approach with frequency counting
    return 0;
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C" {
    PlayerTable* createPlayerTable() {
        return new ConcretePlayerTable();
    }

    Leaderboard* createLeaderboard() {
        return new ConcreteLeaderboard();
    }

    AuctionTree* createAuctionTree() {
        return new ConcreteAuctionTree();
    }
}


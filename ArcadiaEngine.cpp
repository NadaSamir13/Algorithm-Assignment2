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
        return key%tableSize;
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
        cout <<"Table is full"<<endl;

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
    enum Node_color {red,black};
    struct Node{
        int id;
        int price;
        Node_color color;
        Node* left;
        Node* right;
        Node* p;

        Node(int itemID,int itemPR):id(itemID),price(itemPR),color(red),left(nullptr),
        right(nullptr),p(nullptr){}
    };
    Node* root;
    Node* NIL;
    //helper functions
    void rotate_left(Node* A){
        Node* B = A->right;
        A->right = B->left;
        //make A the parent of B left
        if (B->left != NIL) B->left->p = A;
        //give A's parent to B
        B->p = A->p;
        if(A->p == NIL){
            root = B;
        }else if (A == A->p->left){
            A->p->left = B;
        }else{
            A->p->right = B;
        }
        B->left = A;
        A->p =B;
    }
    void rotate_right(Node* A){
        Node* B = A->left;
        A->left = B->right;
        //make A the parent of B right
        if (B->right != NIL) B->right->p = A;
        //give A's parent to B
        B->p = A->p;
        if(A->p == NIL){
            root = B;
        }else if (A == A->p->right){
            A->p->right = B;
        }else{
            A->p->left = B;
        }
        B->right = A;
        A->p =B;
    }
    Node* searchById(int id){
        Node* current = root;
        while(id != current->id && current != NIL){
            if (id < current->id){
                current= current->left;
            }else{
                current=current->right;
            }
        }
        return current;
    }
    void deleteTree(Node* node){
        if (node != NIL && node != nullptr){
            deleteTree(node->right);
            deleteTree(node->left);
            delete NIL;
        }
    }
    void insertRecolor(Node* A){
        while (A->p->color == red){
            if (A->p == A->p->p->left){
                Node* B = A->p->p->right; //uncle

                // Case 1: Uncle is RED
                if (B->color == red) {
                    A->p->color = black;
                    B->color = black;
                    A->p->p->color = red;
                    A = A->p->p;
                } else {
                    // Case 2: A is right child
                    if (A == A->p->right) {
                        A = A->p;
                        rotate_left(A);
                    }
                    // Case 3: A is left child
                    A->p->color = black;
                    A->p->p->color = red;
                    rotate_right(A->p->p);
                }

            } else{
                // parent is right child
                Node* B = A->p->p->left;  // Uncle

                // Case 1: Uncle is RED
                if (B->color == red) {
                    A->p->color = black;
                    B->color = black;
                    A->p->p->color = red;
                    A = A->p->p;
                } else {
                    // Case 2: A is left child
                    if (A == A->p->left) {
                        A = A->p;
                        rotate_right(A);
                    }
                    // Case 3: A is right child
                    A->p->color = black;
                    A->p->p->color = red;
                    rotate_left(A->p->p);
                }
            }
        }
        root->color = black;
    }
    void move(Node* a, Node* b){
        if (a->p == NIL) {
            root = b;
        } else if (a == a->p->left) {
            a->p->left = b;
        } else {
            a->p->right = b;
        }
        b->p = a->p;
    }
    Node* minimum(Node* node){
        while (node->left != NIL) {
            node = node->left;
        }
        return node;
    }
    void deleteRecolor(Node* A){
        while (A != root && A->color == black) {
            if (A == A->p->left) {
                Node* B = A->p->right;  // Sibling

                // Case 1: Sibling is RED
                if (B->color == red) {
                    B->color = black;
                    A->p->color = red;
                    rotate_left(A->p);
                    B = A->p->right;
                }

                // Case 2: Both of sibling's children are BLACK
                if (B->left->color == black && B->right->color == black) {
                    B->color = red;
                    A = A->p;
                } else {
                    // Case 3: Sibling's right child is BLACK, left child is RED
                    if (B->right->color == black) {
                        B->left->color = black;
                        B->color = red;
                        rotate_right(B);
                        B = A->p->right;
                    }

                    // Case 4: Sibling's right child is RED
                    B->color = A->p->color;
                    A->p->color = black;
                    B->right->color = black;
                    rotate_left(A->p);
                    A = root;  // Terminate the loop
                }
            } else {
                // A is right child)
                Node* B = A->p->left;  // Sibling

                // Case 1: Sibling is RED
                if (B->color == red) {
                    B->color = black;
                    A->p->color = red;
                    rotate_right(A->p);
                    B = A->p->left;
                }

                // Case 2: Both of sibling's children are BLACK
                if (B->right->color == black && B->left->color == black) {
                    B->color = red;
                    A = A->p;
                } else {
                    // Case 3: Sibling's left child is BLACK, right child is RED
                    if (B->left->color == black) {
                        B->right->color = black;
                        B->color = red;
                        rotate_left(B);
                        B = A->p->left;
                    }

                    // Case 4: Sibling's left child is RED
                    B->color = A->p->color;
                    A->p->color = black;
                    B->left->color = black;
                    rotate_right(A->p);
                    A = root;  // Terminate the loop
                }
            }
        }

        // Ensure A is black
        A->color = black;
    }
    
public:
    ConcreteAuctionTree() {
        // TODO: Initialize your Red-Black Tree
        NIL = new Node(0,0);
        NIL->color = black;
        NIL->left = nullptr;
        NIL->right = nullptr;
        NIL->p = nullptr;
        root = NIL;

    }

    void insertItem(int itemID, int price) override {

        Node* C = new Node(itemID,price);
        C->left = NIL;
        C->right = NIL;
        C->p = NIL;

        Node* B = NIL;
        Node* A = root;

        while (A != NIL){
            B = A;
            if (C->id < A->id){
                A = A->left;
            }else{
                A = A->right;
            }
        }

        C->p = B;
        if (B == NIL){
            root = C;
        }else if(C->id < B->id){
            B->left = C;
        }else{
            B->right = C;
        }
        C->color = red;
        insertRecolor(C);
    }

    void deleteItem(int itemID) override {

        Node* A = searchById(itemID);
        if (A == NIL) {
            return;  // Item not found
        }

        Node* B = A;
        Node* C;
        Node_color yOriginalColor = B->color;

        // Case 1: A has no left child
        if (A->left == NIL) {
            C = A->right;
            move(A, A->right);
        }
            // Case 2: A has no right child
        else if (A->right == NIL) {
            C = A->left;
            move(A, A->left);
        }
            // Case 3: A has both children
        else {
            B = minimum(A->right);  // Successor
            yOriginalColor = B->color;
            C = B->right;

            if (B->p == A) {
                C->p = B;  // In case C is NIL
            } else {
                move(B, B->right);
                B->right = A->right;
                B->right->p = B;
            }

            move(A, B);
            B->left = A->left;
            B->left->p = B;
            B->color = A->color;
        }

        delete A;  // Free the deleted node

        // If the removed node was black, fix the tree
        if (yOriginalColor == black) {
            deleteRecolor(C);
        }

    }

    ~ConcreteAuctionTree(){
        deleteTree(root);
        delete NIL;
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

bool WorldNavigator::pathExists(int n, vector<vector<int>> &edges, int source, int dest)
{
    // TODO: Implement path existence check using BFS or DFS
    // edges are bidirectional

    if (source == dest)
        return true;
    vector<vector<int>> adj(n);
    for (auto &edge : edges)
    {
        int u = edge[0];
        int v = edge[1];
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    vector<bool> visited(n, false);
    queue<int> q;
    q.push(source);
    visited[source] = true;

    while (!q.empty())
    {
        int current = q.front();
        q.pop();
        for (int neighbor : adj[current])
        {
            if (!visited[neighbor])
            {
                if (neighbor == dest)
                    return true;

                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }

    return false;
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>> &roadData)
{
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected

    struct Edge
    {
        int u, v;
        long long cost;
    };
    vector<Edge> edges;
    for (int i = 0; i < m; ++i)
    {
        long long total = (long long)roadData[i][2] * goldRate + (long long)roadData[i][3] * silverRate;
        edges.push_back({roadData[i][0], roadData[i][1], total});
    }

    sort(edges.begin(), edges.end(), [](const Edge &a, const Edge &b)
         { return a.cost < b.cost; });

    vector<int> parent(n);
    for (int i = 0; i < n; ++i)
        parent[i] = i;

    auto find = [&](auto self, int i) -> int
    {
        return (parent[i] == i) ? i : (parent[i] = self(self, parent[i]));
    };

    long long minTotalCost = 0;
    int edgesIncluded = 0;

    for (const auto &edge : edges)
    {
        int rootU = find(find, edge.u);
        int rootV = find(find, edge.v);

        if (rootU != rootV)
        {
            parent[rootU] = rootV;
            minTotalCost += edge.cost;
            edgesIncluded++;
        }
    }

    if (n > 1 && edgesIncluded != n - 1)
        return -1;
    if (n <= 1)
        return 0;

    return minTotalCost;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>> &roads)
{
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully

    const long long INF = 1e15;

    vector<vector<long long>> dist(n, vector<long long>(n, INF));

    for (int i = 0; i < n; i++)
    {
        dist[i][i] = 0;
    }

    for (const auto &road : roads)
    {
        int u = road[0];
        int v = road[1];
        long long w = road[2];

        if (w < dist[u][v])
        {
            dist[u][v] = dist[v][u] = w;
        }
    }

    for (int k = 0; k < n; k++)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (dist[i][k] != INF && dist[k][j] != INF)
                {
                    if (dist[i][k] + dist[k][j] < dist[i][j])
                    {
                        dist[i][j] = dist[i][k] + dist[k][j];
                    }
                }
            }
        }
    }

    long long totalSum = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (dist[i][j] != INF)
            {
                totalSum += dist[i][j];
            }
        }
    }

    if (totalSum == 0)
        return "0";

    string binary = "";
    while (totalSum > 0)
    {
        binary += (totalSum % 2 == 0 ? '0' : '1');
        totalSum /= 2;
    }

    reverse(binary.begin(), binary.end());

    return binary;
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char> &tasks, int n)
{
    // TODO: Implement task scheduler with cooling time
    // Same task must wait 'n' intervals before running again
    // Return minimum total intervals needed (including idle time)
    // Hint: Use greedy approach with frequency counting

    vector<int> freq(26, 0);
    for (char t : tasks)
    {
        freq[t - 'A']++;
    }

    sort(freq.rbegin(), freq.rend());

    int maxFreq = freq[0];

    int countMaxFreq = 0;
    for (int f : freq)
    {
        if (f == maxFreq)
            countMaxFreq++;
        else
            break;
    }

    int minTime = (maxFreq - 1) * (n + 1) + countMaxFreq;

    return max(minTime, (int)tasks.size());
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


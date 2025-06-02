#ifndef BTREE_H
#define BTREE_H

#include <stdbool.h>

typedef struct BtreeNode {
    int num_keys;
    int keys[4];
    struct BtreeNode *children[5];
    char values[4][100];
    bool is_leaf;
} BtreeNode;

typedef struct {
    BtreeNode* data[1000];
    int front;
    int rear;
} Queue;

// Creation and utility
BtreeNode* create_node(bool is_leaf);
void print(BtreeNode* root);

// Core operations
void insert(BtreeNode** root, int key, char* value);
char* search(BtreeNode* root, int key);
void deletion(BtreeNode** root, int key);

#endif

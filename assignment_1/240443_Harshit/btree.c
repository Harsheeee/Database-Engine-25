#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "btree.h"

// typedef struct BtreeNode {
//     int num_keys;
//     int keys[4];
//     struct BtreeNode *children[5];
//     char values[4][100];
//     bool is_leaf;
// } BtreeNode;

BtreeNode* create_node(bool is_leaf) {
    BtreeNode* new_node = (BtreeNode*)malloc(sizeof(BtreeNode));
    new_node->num_keys = 0;
    new_node->is_leaf = is_leaf;
    for (int i = 0; i < 5; i++) {
        new_node->children[i] = NULL;
    }
    for (int i = 0; i < 4; i++) {
        new_node->values[i][0] = '\0';
    }
    return new_node;
}

char* search(BtreeNode* root, int k){
    if(root==NULL)return NULL;
    int i = 0;
    while(i< root->num_keys && k > root->keys[i]) {
        i++;
    }
    if(i < root->num_keys && k == root->keys[i]) {
        return root->values[i];
    }
    if(root->is_leaf) {
        return NULL;
    }
    return search(root->children[i], k);
}

//INSERTION

void splitChild(BtreeNode* parent, BtreeNode* child, int index){
    BtreeNode* newChild = create_node(child->is_leaf);
    newChild->num_keys=1;   //left biased therefore new node has 1 key
    newChild->keys[0] = child->keys[3]; //move the last key of child to newChild
    strcpy(newChild->values[0], child->values[3]);

    if(!child->is_leaf) {   //if child is not a leaf, move the last two children to newChild
        newChild->children[0] = child->children[3];
        newChild->children[1] = child->children[4];
    }
    parent->keys[index] = child->keys[2];   //move the mid key,value of child to parent
    strcpy(parent->values[index], child->values[2]);
    child->num_keys = 2;    //reduce keys in child to 2

    for(int i = parent->num_keys; i >= index+1; i--) { //shift children in parent 
        parent->children[i + 1] = parent->children[i];
    }
    parent->children[index + 1] = newChild; //add newChild in parent

    for(int i = parent->num_keys - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i]; //shift keys, values in parent
        strcpy(parent->values[i + 1], parent->values[i]);
    }

    parent->num_keys++;
}

void insertNonFull(BtreeNode* node, int key, char* value){
    int i=node->num_keys - 1;
    if(node->is_leaf) {
        while(i >= 0 && key < node->keys[i]) {  //shift keys
            node->keys[i + 1] = node->keys[i];
            strcpy(node->values[i + 1], node->values[i]);
            i--;
        }
        node->keys[i + 1] = key;    //insert
        strcpy(node->values[i + 1], value);
        node->num_keys++;
    } else {
        while(i >= 0 && key < node->keys[i]) {  //find correct child
            i--;
        }
        i++;
        if(node->children[i]->num_keys == 4) { //if child is full, split it
            splitChild(node, node->children[i], i);
            if(key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key, value);
    }
}

void insert(BtreeNode** root, int key, char* value){
    if(*root == NULL) {
        *root = create_node(true); //create a new root if tree is empty
        (*root)->keys[0] = key;
        strcpy((*root)->values[0], value);
        (*root)->num_keys = 1;
        return;
    }
    if(search(*root, key) != NULL) {
        printf("Key %d already exists.\n", key);
        return;
    }

    BtreeNode* r = *root;
    if(r->num_keys == 4) { //if root is full, create a new root
        BtreeNode* newRoot = create_node(false);
        newRoot->children[0] = r;
        splitChild(newRoot, r, 0);
        *root = newRoot;
        insertNonFull(newRoot, key, value);
    } else {
        insertNonFull(r, key, value);
    }
}

//DELETION

int find_key(BtreeNode* node, int key) {
    int idx = 0;
    while (idx < node->num_keys && node->keys[idx] < key)
        ++idx;
    return idx;
}

void remove_from_leaf(BtreeNode* node, int idx) {
    for (int i = idx + 1; i < node->num_keys; ++i) {
        node->keys[i - 1] = node->keys[i];
        strcpy(node->values[i - 1], node->values[i]);
    }
    node->num_keys--;
}

void remove_from_non_leaf(BtreeNode* node, int idx) {
    int key = node->keys[idx];

    BtreeNode* pred = node->children[idx];
    if (pred->num_keys >= 3) {  // If pred has enough keys
        while (!pred->is_leaf)  
            pred = pred->children[pred->num_keys];
        node->keys[idx] = pred->keys[pred->num_keys - 1];   
        strcpy(node->values[idx], pred->values[pred->num_keys - 1]);    
        deletion(&(node->children[idx]), pred->keys[pred->num_keys - 1]);   // Remove the predecessor key
    } else {
        BtreeNode* succ = node->children[idx + 1];
        if (succ->num_keys >= 3) {  // If succ has enough keys
            while (!succ->is_leaf)
                succ = succ->children[0];
            node->keys[idx] = succ->keys[0];
            strcpy(node->values[idx], succ->values[0]);
            deletion(&(node->children[idx + 1]), succ->keys[0]);
        } else {    //else merge
            BtreeNode* child = node->children[idx];
            BtreeNode* sibling = node->children[idx + 1];

            child->keys[child->num_keys++] = key;
            strcpy(child->values[child->num_keys - 1], node->values[idx]);

            for (int i = 0; i < sibling->num_keys; ++i) {
                child->keys[child->num_keys] = sibling->keys[i];
                strcpy(child->values[child->num_keys], sibling->values[i]);
                child->num_keys++;
            }
            if (!child->is_leaf) {
                for (int i = 0; i <= sibling->num_keys; ++i)
                    child->children[child->num_keys + i] = sibling->children[i];
            }
            for (int i = idx + 1; i < node->num_keys; ++i) {
                node->keys[i - 1] = node->keys[i];
                strcpy(node->values[i - 1], node->values[i]);
                node->children[i] = node->children[i + 1];
            }
            node->num_keys--;
            free(sibling);
            deletion(&(node->children[idx]), key);  //delete from merged child
        }
    }
}

void borrow_from_prev(BtreeNode* node, int idx) {
    BtreeNode* child = node->children[idx];     //child
    BtreeNode* sibling = node->children[idx - 1];   //prev child

    for (int i = child->num_keys - 1; i >= 0; --i) {    
        child->keys[i + 1] = child->keys[i];    //shift keys
        strcpy(child->values[i + 1], child->values[i]);
    }

    if (!child->is_leaf)
        for (int i = child->num_keys; i >= 0; --i)
            child->children[i + 1] = child->children[i];

    child->keys[0] = node->keys[idx - 1];   //move key from parent to child
    strcpy(child->values[0], node->values[idx - 1]);

    if (!child->is_leaf)
        child->children[0] = sibling->children[sibling->num_keys];

    node->keys[idx - 1] = sibling->keys[sibling->num_keys - 1]; //move last key from sibling to parent
    strcpy(node->values[idx - 1], sibling->values[sibling->num_keys - 1]);

    child->num_keys += 1;
    sibling->num_keys -= 1;
}

void borrow_from_next(BtreeNode* node, int idx) {
    BtreeNode* child = node->children[idx];
    BtreeNode* sibling = node->children[idx + 1];   //next child

    child->keys[child->num_keys] = node->keys[idx]; //move key from parent to child
    strcpy(child->values[child->num_keys], node->values[idx]);

    if (!child->is_leaf)
        child->children[child->num_keys + 1] = sibling->children[0];

    node->keys[idx] = sibling->keys[0]; //move first key from sibling to parent
    strcpy(node->values[idx], sibling->values[0]);

    for (int i = 1; i < sibling->num_keys; ++i) {   
        sibling->keys[i - 1] = sibling->keys[i];    // shift keys
        strcpy(sibling->values[i - 1], sibling->values[i]);
    }
    if (!sibling->is_leaf)
        for (int i = 1; i <= sibling->num_keys; ++i)
            sibling->children[i - 1] = sibling->children[i];

    child->num_keys += 1;
    sibling->num_keys -= 1;
}

void merge(BtreeNode* node, int idx) {
    BtreeNode* child = node->children[idx];
    BtreeNode* sibling = node->children[idx + 1];

    child->keys[child->num_keys] = node->keys[idx];
    strcpy(child->values[child->num_keys], node->values[idx]);
    child->num_keys++;

    for (int i = 0; i < sibling->num_keys; ++i) {
        child->keys[child->num_keys] = sibling->keys[i];
        strcpy(child->values[child->num_keys], sibling->values[i]);
        child->num_keys++;
    }
    if (!child->is_leaf) {
        for (int i = 0; i <= sibling->num_keys; ++i)
            child->children[child->num_keys + i] = sibling->children[i];
    }
    for (int i = idx + 1; i < node->num_keys; ++i) {
        node->keys[i - 1] = node->keys[i];
        strcpy(node->values[i - 1], node->values[i]);
        node->children[i] = node->children[i + 1];
    }
    node->num_keys--;
    free(sibling);
}

void fill(BtreeNode* node, int idx) {
    if (idx != 0 && node->children[idx - 1]->num_keys >= 3)
        borrow_from_prev(node, idx);
    else if (idx != node->num_keys && node->children[idx + 1]->num_keys >= 3)
        borrow_from_next(node, idx);
    else {
        if (idx != node->num_keys)
            merge(node, idx);
        else
            merge(node, idx - 1);
    }
}

void deletion(BtreeNode** root_ref, int key) {
    BtreeNode* root = *root_ref;
    int idx = find_key(root, key);  //find idx of the key greater than or equal to key

    if (idx < root->num_keys && root->keys[idx] == key) {   //if key found
        if (root->is_leaf)
            remove_from_leaf(root, idx);
        else
            remove_from_non_leaf(root, idx);
    } else {
        if (root->is_leaf) {
            printf("Key %d not found.\n", key);
            return;
        }

        int flag = (idx == root->num_keys); //whether going in the last child

        if (root->children[idx]->num_keys < 3)
            fill(root, idx);

        if (flag && idx > root->num_keys)   //if merge took place during fill then the key is in idx-1
            deletion(&root->children[idx - 1], key);
        else
            deletion(&root->children[idx], key);
    }
    if (root->num_keys == 0) { //if all keys went to children
        BtreeNode* tmp = root;
        if (root->is_leaf)
            *root_ref = NULL;   //no chiildren then tree is empty
        else
            *root_ref = root->children[0];  //else first child is the root
        free(tmp);
    }
}

//PRINT

int isQueueEmpty(Queue* q) {
    return q->front == q->rear;
}
void enqueue(Queue* q, BtreeNode* node) {
    if (q->rear < 1000) {
        q->data[q->rear++] = node;
    }
}
BtreeNode* dequeue(Queue* q) {
    if (isQueueEmpty(q)) return NULL;
    return q->data[q->front++];
}

void print(BtreeNode* root) {
    if (root == NULL) return;
    Queue q;
    q.front = 0, q.rear = 0;
    enqueue(&q, root);

    while (!isQueueEmpty(&q)) {
        int levelSize = q.rear - q.front;
        for (int i = 0; i < levelSize; i++) {
            BtreeNode* node = dequeue(&q);
            printf("[");
            for (int j = 0; j < node->num_keys; j++) {
                printf("(%d, %s) ", node->keys[j], node->values[j]);
            }
            printf("] ");
            if (!node->is_leaf) {
                for (int j = 0; j <= node->num_keys; j++) {
                    if (node->children[j] != NULL) {
                        enqueue(&q, node->children[j]);
                    }
                }
            }
        }
        printf("\n");
    }
}
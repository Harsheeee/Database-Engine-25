#include <stdio.h>
#include "btree.h"

int main() {
    BtreeNode* root = NULL;

   char buffer[20];
    for (int i = 1; i <= 11; i++) {
        sprintf(buffer, "Value %d", i);
        insert(&root, i, buffer);
}
    printf("B-Tree after insertions:\n");
    print(root);
    search(root, 5);
    printf("Searchfor key 5: %s\n", search(root, 5) ? search(root, 5) : "Not found");
    deletion(&root, 12);
    deletion(&root, 9);
    printf("B-Tree after deleting 9\n");
    print(root);
    deletion(&root, 5);
    printf("B-Tree after deleting 5:\n");
    print(root);
    insert(&root, 12, "Value 12");
    printf("B-Tree after inserting 12:\n");
    print(root);
    return 0;
}

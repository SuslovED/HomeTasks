#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct TreeNode {
    int key;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

/* Функция для создания нового узла */
TreeNode* create_node(int key) {
    TreeNode *new_node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!new_node) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(1);
    }
    new_node->key = key;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

/* Функция для добавления элемента в дерево */
TreeNode* insert(TreeNode *root, int key) {
    if (root == NULL) {
        return create_node(key);
    }
    
    if (key < root->key) {
        root->left = insert(root->left, key);
    } else if (key > root->key) {
        root->right = insert(root->right, key);
    }
    
    return root;
}

/* Функция для поиска минимального элемента в дереве */
TreeNode* find_min(TreeNode *root) {
    while (root && root->left != NULL) {
        root = root->left;
    }
    return root;
}

/* Функция для удаления элемента из дерева */
TreeNode* delete(TreeNode *root, int key) {
    if (root == NULL) {
        return NULL;
    }
    
    if (key < root->key) {
        root->left = delete(root->left, key);
    } else if (key > root->key) {
        root->right = delete(root->right, key);
    } else {
        if (root->left == NULL) {
            TreeNode *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            TreeNode *temp = root->left;
            free(root);
            return temp;
        }
        
        TreeNode *temp = find_min(root->right);
        
        root->key = temp->key;
        
        root->right = delete(root->right, temp->key);
    }
    
    return root;
}

/* Функция для поиска элемента в дереве */
int search(TreeNode *root, int key) {
    if (root == NULL) {
        return 0;
    }
    
    if (key == root->key) {
        return 1;
    } else if (key < root->key) {
        return search(root->left, key);
    } else {
        return search(root->right, key);
    }
}

/* Функция для освобождения памяти дерева */
void free_tree(TreeNode *root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

/* Функция для обработки ввода */
void process_input() {
    TreeNode *root = NULL;
    char line[100];
    char operation;
    int number;
    
    while (fgets(line, sizeof(line), stdin) != NULL) {
        if (line[0] == '\n') continue;
        
        if (sscanf(line, " %c%d", &operation, &number) == 2 && number >= 0) {
            switch (operation) {
                case '+':
                    root = insert(root, number);
                    break;
                    
                case '-':
                    root = delete(root, number);
                    break;
                    
                case '?':
                    printf("%d %s\n", number, search(root, number) ? "yes" : "no");
                    break;
                    
                default:
                    fprintf(stderr, "Недопустимая операция\n");
                    break;
            }
        }
    }

    free_tree(root);
}

int main() {
    process_input();
    return 0;
}

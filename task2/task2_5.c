#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Node {
    char *word;
    struct Node *next;
} Node;

/* Функция для создания нового узла */
Node* create_node(const char *word) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Ошибка выделения памяти для узла\n");
        exit(1);
    }
    
    new_node->word = (char*)malloc(strlen(word) + 1);
    if (!new_node->word) {
        fprintf(stderr, "Ошибка выделения памяти для слова\n");
        free(new_node);
        exit(1);
    }
    
    strcpy(new_node->word, word);
    new_node->next = NULL;
    return new_node;
}

/* Функция для добавления слова в конец списка */
void append_word(Node **head, const char *word) {
    Node *new_node = create_node(word);
    
    if (*head == NULL) {
        *head = new_node;
    } else {
        Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

/* Функция для получения последнего слова в списке */
char* get_last_word(Node *head) {
    if (head == NULL) return NULL;
    
    Node *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    return current->word;
}

/* Функция для удаления всех вхождений слова (кроме последнего) */
void remove_all_occurrences(Node **head, const char *target_word) {
    if (*head == NULL || target_word == NULL) return;
    
    Node *current = *head;
    Node *prev = NULL;
    Node *to_delete = NULL;
    
    Node *last = *head;
    while (last->next != NULL) {
        last = last->next;
    }
    
    while (current != NULL) {
        if (strcmp(current->word, target_word) == 0 && current != last) {
            to_delete = current;
            
            if (prev == NULL) {
                *head = current->next;
                current = *head;
            } else {
                prev->next = current->next;
                current = current->next;
            }
            
            free(to_delete->word);
            free(to_delete);
        } else {
            prev = current;
            current = current->next;
        }
    }
}

/* Функция для освобождения памяти списка */
void free_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        Node *next = current->next;
        free(current->word);
        free(current);
        current = next;
    }
}

/* Функция для печати списка */
void print_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%s", current->word);
        if (current->next != NULL) {
            printf(" ");
        }
        current = current->next;
    }
    printf("\n");
}

/* Функция для разбиения строки на слова и создания списка */
Node* parse_string_to_list(const char *input) {
    Node *head = NULL;
    const char *start = NULL;
    const char *p = input;
    
    while (*p != '\0') {
        while (*p != '\0' && isspace(*p)) {
            p++;
        }
        
        if (*p == '\0') break;
        
        start = p;
        
        while (*p != '\0' && !isspace(*p)) {
            p++;
        }
        
        int word_length = p - start;
        char *word = (char*)malloc(word_length + 1);
        if (!word) {
            fprintf(stderr, "Ошибка выделения памяти для слова\n");
            free_list(head);
            exit(1);
        }
        
        strncpy(word, start, word_length);
        word[word_length] = '\0';
        
        append_word(&head, word);
        free(word);
    }
    
    return head;
}

int main() {
    char input[1024];
    Node *word_list = NULL;
    
    printf("Введите строку: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fprintf(stderr, "Ошибка чтения ввода\n");
        return 1;
    }
    
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }
    
    word_list = parse_string_to_list(input);
    
    char *last_word = get_last_word(word_list);
    
    if (last_word != NULL) {
        remove_all_occurrences(&word_list, last_word);
    }
    
    print_list(word_list);
    
    free_list(word_list);
    
    return 0;
}

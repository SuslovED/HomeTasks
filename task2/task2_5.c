#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Node {
    char *word;
    struct Node *next;
} Node;


/* Функция увеличения буфера */
void expand_buffer(char **buf, int *size) {
int new_size;
    if (*size == 0) {
        new_size = 2;
    } else {
        new_size = *size * 2;
    }
    
    char *tmp = realloc(*buf, new_size);
    if (!tmp) {
        fprintf(stderr, "Ошибка выделения памяти при увеличении буфера\n");
        free(*buf);
        exit(1);
    }
    *buf = tmp;
}

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

/* Чтение строки с удвоением буфера */
char* read_line_dynamic(void) {
    char *input = NULL;
    int size = 32, len = 0;
    int c;

    input = (char*)malloc(size);
    if (!input) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(1);
    }

    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= size) {
            expand_buffer(&input, &size);
        }
        input[len++] = (char)c;
    }

    if (input) {
        if (len >= size) {
            expand_buffer(&input, &size);
        }
        input[len] = '\0';
    }

    return input;
}

/* Функция для разбиения строки на слова и создания списка */
Node* parse_string_to_list(const char *input) {
    Node *head = NULL;
    char *word = NULL;
    const char *p = input;
    int size = 0, len = 0;
    
    while (*p != '\0') {
        while (*p != '\0' && isspace(*p)) {
            p++;
        }
        
        if (*p == '\0') break;
        
        size = 16;
        len = 0;
        word = (char*)malloc(size);
         if (!word) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            exit(1);
        }
        
        while (*p != '\0' && !isspace(*p)) {
            if (len + 1 >= size)
                expand_buffer(&word, &size);
            word[len++] = *p;
            p++;
        }
        
        if (word) {
            if (len + 1 >= size)
                expand_buffer(&word, &size);
            word[len] = '\0';
            append_word(&head, word);
            free(word);
            word = NULL;
        }
    }
    
    return head;
}

int main() {
    printf("Введите строку: ");
    char *input = read_line_dynamic();

    Node *word_list = parse_string_to_list(input);

    char *last_word = get_last_word(word_list);
    if (last_word)
        remove_all_occurrences(&word_list, last_word);

    print_list(word_list);

    free_list(word_list);
    free(input);

    return 0;
}

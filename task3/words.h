#ifndef WORDS_H
#define WORDS_H

/* Шаг увеличения памяти под массив списка строк */
#define LIST_BLOCK_SIZE 16

/* Структура списка строк:
 * data     - массив указателей на строки
 * size     - текущее количество слов
 * capacity - размер массива data (включая место под завершающий NULL)
 */
typedef struct {
    char **data;
    int size;
    int capacity;
} StringList;

/* initList
 * Инициализирует пустой список.
 */
void initList(StringList *list);

/* clearList
 * Освобождает всю память, занятую списком (строки + массив),
 * и делает список пустым.
 */
void clearList(StringList *list);

/* addWord
 * Добавляет слово в конец списка.
 * Параметр word — указатель на '\0'-терминированную строку в динамической памяти.
 * Функция забирает владение word.
 * Возвращает 0 при успехе, -1 при ошибке (word при ошибке освобождается).
 */
int addWord(StringList *list, char *word);

/* finalizeList
 * Гарантирует наличие завершающего NULL в конце списка.
 * По возможности подрезает capacity до size+1.
 * Если список пуст и data == NULL, создаёт массив из одного NULL.
 */
void finalizeList(StringList *list);

/* printList
 * Печатает:
 *   первая строка: длина списка (size)
 *   далее: по одному слову в строке.
 * Если список пуст или data == NULL — печатает "0" и перевод строки.
 */
void printList(const StringList *list);

/* sortList
 * Сортирует список лексикографически по strcmp.
 * Используется сортировка вставками.
 */
void sortList(StringList *list);

void doubleWords(StringList *list);

#endif /* WORDS_H */

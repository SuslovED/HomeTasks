#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <time.h>

int sem_id; /* ID семафора */
int shm_id; /* ID разделяемой памяти */
int *bowl_ptr; /* Указатель на миску */

/* Функция для очистки ресурсов при завершении */
void cleanup() {
    printf("\nУдаление IPC ресурсов...\n");
    /* Отсоединяем разделяемую память */
    shmdt(bowl_ptr);
    /* Удаляем разделяемую память */
    shmctl(shm_id, IPC_RMID, NULL);
    /* Удаляем семафор */
    semctl(sem_id, 0, IPC_RMID);
    printf("Ресурсы удалены.\n");
}

void handle_sigint(int sig) {
    exit(0);
}

/* Функция для ожидания семафора */
void semaphore_wait() {
    struct sembuf op = {0, -1, 0}; /* {номер семафора, операция, флаги} */
    semop(sem_id, &op, 1);
}

/* Функция для освобождения семафора */
void semaphore_post() {
    struct sembuf op = {0, +1, 0};
    semop(sem_id, &op, 1);
}

int main() {
    atexit(cleanup);
    signal(SIGINT, handle_sigint);

    /* Генерируем уникальный ключ для IPC ресурсов */
    key_t ipc_key = ftok(".", 'R');
    if (ipc_key == -1) {
        perror("ftok");
        exit(1);
    }

    /* Создаем семафор (дверь в клетку) */
    sem_id = semget(ipc_key, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    semctl(sem_id, 0, SETVAL, 1);

    /* Создаем разделяемую память (миску с мясом) */
    shm_id = shmget(ipc_key, sizeof(int), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }
    /* Подключаем разделяемую память к нашему процессу */
    bowl_ptr = (int*)shmat(shm_id, NULL, 0);
    if (bowl_ptr == (int*)-1) {
        perror("shmat");
        exit(1);
    }
    *bowl_ptr = 0;

    srand(time(NULL));

    printf("Программа 'Тигр и Смотритель' запущена.\n");
    printf("---------------------------------------\n");

    /* Создаем дочерний процесс */
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        /* ТИГР (дочерний процесс) */
        signal(SIGINT, SIG_DFL);

        while (1) {
            sleep(rand() % 5 + 2); /* Тигр спит от 2 до 6 сек */

            semaphore_wait(); 

            if (*bowl_ptr > 0) {
                int to_eat = rand() % (*bowl_ptr) + 1; /* Съедает от 1 до всего, что есть */
                *bowl_ptr -= to_eat;
                printf("[ТИГР] Съел %d кг мяса. В миске осталось: %d кг.\n", to_eat, *bowl_ptr);
            } else {
                printf("[ТИГР] Миска пуста! :( \n");
            }

            semaphore_post();
        }
    } else {
        /* СМОТРИТЕЛЬ (родительский процесс) */
        while (1) {
            sleep(rand() % 8 + 4); /* Смотритель отдыхает от 4 до 11 сек */

            semaphore_wait();

            if (*bowl_ptr == 0) {
                int to_add = rand() % 6 + 5; /* Кладет от 5 до 10 кг мяса */
                *bowl_ptr = to_add;
                printf("[СМОТРИТЕЛЬ] Миска была пуста. Наполнил ее %d кг мяса.\n", to_add);
            } else {
                printf("[СМОТРИТЕЛЬ] Миска еще не пуста (%d кг). Жду.\n", *bowl_ptr);
            }

            semaphore_post();
        }
    }

    return 0;
}
#include "../common.h"

const int READER_TURNS = 10;
const int WRITER_TURNS = 10;
const int READERS_COUNT = 5;
const int WRITERS_COUNT = 5;

const char *READER_SEM = "reader_sem";
const char *WRITER_SEM = "writer_sem";
const char *FILE_NAME = "file.txt";

sem_t *writer_sem, *reader_sem;
int readers_count = 0;

pthread_mutex_t mask_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mask_full = PTHREAD_COND_INITIALIZER;
int readers_mask = 0;
int full_mask = 0;

int critic(void *data)
{

    printf("Critic started, %d %d\n", readers_mask, full_mask);

    FILE *file = fopen(FILE_NAME, "a");

    if (pthread_mutex_lock(&mask_mutex) != 0)
        error("Error occured during locking the mask mutex.\n");
    while (readers_mask != full_mask)
        pthread_cond_wait(&mask_full, &mask_mutex);
    readers_mask = 0;
    if (pthread_mutex_unlock(&mask_mutex) != 0)
        error("Error occured during unlocking the mask mutex.\n");

    if (sem_wait(writer_sem) != 0)
        error("Error occured during locking the writer semaphore.\n");

    // Write
    printf("(C) critic started writing...");
    fflush(stdout);
    fseek(file, 0, SEEK_END);
    fprintf(file, "C writes \n");
    fflush(file);
    usleep(get_random_time(800));
    printf("(C) critic finished\n");

    // Release ownership of the mutex object.
    if (sem_post(writer_sem) != 0)
        error("Error occured during unlocking the writer semaphore.\n");
    // Think, think, think, think
    usleep(get_random_time(1000));

    fclose(file);

    return 0;
}

// writer thread function
int writer(void *data)
{
    int threadId = *(int *)data;
    printf("Writer %d started\n", threadId);

    FILE *file = fopen(FILE_NAME, "a");

    for (int i = 0; i < WRITER_TURNS; i++)
    {
        if (sem_wait(writer_sem) != 0)
            error("Error occured during locking the writer semaphore.\n");

        // Write
        printf("(W) writer %d started writing...", threadId);
        fflush(stdout);
        fseek(file, 0, SEEK_END);
        fprintf(file, "W %03d %03d\n", threadId, i);
        fflush(file);
        usleep(get_random_time(800));
        printf("(W) writer %d finished\n", threadId);

        if (pthread_mutex_lock(&mask_mutex) != 0)
            error("Error occured during locking the mask mutex.\n");
        readers_mask |= 1 << threadId;
        if (readers_mask == full_mask)
            pthread_cond_broadcast(&mask_full);
        if (pthread_mutex_unlock(&mask_mutex) != 0)
            error("Error occured during unlocking the mask mutex.\n");

        // Release ownership of the mutex object.
        if (sem_post(writer_sem) != 0)
            error("Error occured during unlocking the writer semaphore.\n");
        // Think, think, think, think
        usleep(get_random_time(1000));
    }

    free((void *)data);
    fclose(file);

    return 0;
}

// reader thread function
int reader(void *data)
{
    int i;
    int threadId = *(int *)data;
    printf("Reader %d started\n", threadId);

    FILE *file = fopen(FILE_NAME, "r");
    char *read_buffer =
        (char *)calloc(10, sizeof(char));

    for (i = 0; i < READER_TURNS; i++)
    {
        if (sem_wait(reader_sem) != 0)
            error("Error occured during locking the reader semaphore.\n");

        readers_count++;
        if (readers_count == 1)
            if (sem_wait(writer_sem) != 0)
                error("Error occured during locking the writer semaphore.\n");
        if (sem_post(reader_sem) != 0)
            error("Error occured during unlocking the reader semaphore.\n");

        printf("(R) reader %d started reading...", threadId);
        fflush(stdout);
        fread(read_buffer, sizeof(char), 10, file);
        printf("(R) reader %d read \"%s\"", threadId, read_buffer);
        usleep(get_random_time(200));
        printf("(R) reader %d finished\n", threadId);

        if (sem_wait(reader_sem) != 0)
            error("Error occured during locking the reader semaphore.\n");

        readers_count--;
        if (readers_count == 0)
            if (sem_post(writer_sem) != 0)
                error("Error occured during unlocking the writer semaphore.\n");
        if (sem_post(reader_sem) != 0)
            error("Error occured during unlocking the reader semaphore.\n");

        usleep(get_random_time(800));
    }

    free((void *)data);
    free((void *)read_buffer);
    fclose(file);

    return 0;
}

int main(int argc, char *argv[])
{
    srand(100005);

    pthread_t critic_thread;
    pthread_t writer_threads[WRITERS_COUNT];
    pthread_t reader_threads[READERS_COUNT];

    int i, rc;

    for (i = 0; i < WRITERS_COUNT; i++)
        full_mask |= 1 << i;

    unlink(FILE_NAME);

    if ((writer_sem = sem_open(WRITER_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        error("create writer sem");

    if ((reader_sem = sem_open(READER_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        error("create writer sem");

    // Create the writer thread
    rc = pthread_create(
        &critic_thread, // thread identifier
        NULL,           // thread attributes
        (void *)critic, // thread function
        (void *)NULL);  // thread function argument

    if (rc != 0)
        error("Couldn't create the buffer writer threads");

    for (i = 0; i < WRITERS_COUNT; i++)
    {
        // writer initialization - takes random amount of time
        usleep(get_random_time(1000));
        int *threadId = malloc(sizeof(int));
        *threadId = i;
        rc = pthread_create(
            &writer_threads[i], // thread identifier
            NULL,               // thread attributes
            (void *)writer,     // thread function
            (void *)threadId);  // thread function argument

        if (rc != 0)
            error("Couldn't create the buffer writer threads");
    }

    // Create the reader threads
    for (i = 0; i < READERS_COUNT; i++)
    {
        // reader initialization - takes random amount of time
        usleep(get_random_time(1000));
        int *threadId = malloc(sizeof(int));
        *threadId = i;
        rc = pthread_create(
            &reader_threads[i], // thread identifier
            NULL,               // thread attributes
            (void *)reader,     // thread function
            (void *)threadId);  // thread function argument

        if (rc != 0)
            error("Couldn't create the reader threads");
    }

    // At this point, the readers and writers should perform their operations

    // Wait for the readers
    for (i = 0; i < READERS_COUNT; i++)
        pthread_join(reader_threads[i], NULL);

    // Wait for the writers
    for (i = 0; i < READERS_COUNT; i++)
        pthread_join(writer_threads[i], NULL);

    // Wait for the writer
    pthread_join(critic_thread, NULL);

    if (sem_unlink(WRITER_SEM) < 0)
        error("writer sem unlink");

    if (sem_unlink(READER_SEM) < 0)
        error("reader sem unlink");

    return (0);
}

#include "../common.h"

const int READER_TURNS = 10;
const int WRITER_TURNS = 10;
const int READERS_COUNT = 5;
const int WRITERS_COUNT = 5;

const int BUFFER_SIZE = 100;

const char *READER_SEM = "reader_sem";
const char *WRITER_SEM = "writer_sem";
const char *FILE_NAME = "file.txt";

sem_t *writer_sem, *reader_sem;
pthread_mutex_t buffer_empty_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_empty_cond = PTHREAD_COND_INITIALIZER;
int buffer_empty = 0;
pthread_mutex_t buffer_full_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_full_cond = PTHREAD_COND_INITIALIZER;
int buffer_full = 1;
int readers_count = 0;

char *buffer;
int buffer_index = 0;

// writer function
int writer(void *data)
{
    int threadId = *(int *)data;

    char *message =
        (char *)calloc(10, sizeof(char));
    for (int i = 0; i < WRITER_TURNS; i++)
    {
        sprintf(message, "W %03d %03d\n", threadId, i);

        if (pthread_mutex_lock(&buffer_empty_mutex) != 0)
            error("Error occured during locking the buffer mutex.\n");
        while (buffer_empty)
            pthread_cond_wait(&buffer_empty_cond, &buffer_empty_mutex);

        if (sem_wait(writer_sem) != 0)
            error("Error occured during locking the writer semaphore.\n");

        printf("(W) writer %d started writing...", threadId);

        strcpy(&buffer[buffer_index], message);

        buffer_index += 10;
        if (buffer_index >= BUFFER_SIZE)
        {
            buffer_full = 0;
            buffer_empty = 1;
            pthread_cond_broadcast(&buffer_full_cond);
        }
        usleep(get_random_time(400));
        printf("(W) writer %d finished\n", threadId);

        if (sem_post(writer_sem) != 0)
            error("Error occured during unlocking the writer semaphore.\n");

        if (pthread_mutex_unlock(&buffer_empty_mutex) != 0)
            error("Error occured during unlocking the buffer mutex.\n");

        usleep(get_random_time(500));
    }

    free((void *)data);

    return 0;
}

// reader thread function
int reader(void *data)
{
    int threadId = *(int *)data;

    char *read_buffer =
        (char *)calloc(10, sizeof(char));

    for (int i = 0; i < READER_TURNS; i++)
    {

        if (pthread_mutex_lock(&buffer_full_mutex) != 0)
            error("Error occured during locking the buffer mutex.\n");

        while (buffer_full)
            pthread_cond_wait(&buffer_full_cond, &buffer_full_mutex);

        if (sem_wait(reader_sem) != 0)
            error("Error occured during locking the reader semaphore.\n");
        readers_count++;
        if (readers_count == 1)
            if (sem_wait(writer_sem) != 0)
                error("Error occured during locking the writer semaphore.\n");
        if (sem_post(reader_sem) != 0)
            error("Error occured during unlocking the reader semaphore.\n");

        // Read
        printf("(R) reader %d started reading...", threadId);
        fflush(stdout);
        buffer_index -= 10;
        if (buffer_index == 0)
        {
            buffer_full = 1;
            buffer_empty = 0;
            pthread_cond_broadcast(&buffer_empty_cond);
        }
        strncpy(read_buffer, &buffer[buffer_index], 10);
        printf("(R) reader %d read \"%s\"", threadId, read_buffer);
        usleep(get_random_time(200));
        printf("(R) reader %d finished, buffer index %d\n", threadId, buffer_index);

        if (sem_wait(reader_sem) != 0)
            error("Error occured during locking the reader semaphore.\n");

        readers_count--;
        if (readers_count == 0)
            if (sem_post(writer_sem) != 0)
                error("Error occured during unlocking the writer semaphore.\n");
        if (sem_post(reader_sem) != 0)
            error("Error occured during unlocking the reader semaphore.\n");

        if (pthread_mutex_unlock(&buffer_full_mutex) != 0)
            error("Error occured during unlocking the buffer mutex.\n");

        usleep(get_random_time(800));
    }

    free((void *)data);
    free((void *)read_buffer);
    return 0;
}

int main(int argc, char *argv[])
{
    srand(100005);

    buffer =
        (char *)calloc(BUFFER_SIZE, sizeof(char));

    pthread_t writer_threads[WRITERS_COUNT];
    pthread_t reader_threads[READERS_COUNT];

    int i, rc;
    sem_unlink(WRITER_SEM);
    if ((writer_sem = sem_open(WRITER_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        error("create writer sem");
    sem_unlink(READER_SEM);
    if ((reader_sem = sem_open(READER_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        error("create writer sem");

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

    if (sem_unlink(WRITER_SEM) < 0)
        error("writer sem unlink");

    if (sem_unlink(READER_SEM) < 0)
        error("reader sem unlink");

    free((void *)buffer);

    return (0);
}

#include "../common.h"

const int READER_TURNS = 10;
const int WRITER_TURNS = 10;
const int READERS_COUNT = 5;
const int WRITERS_COUNT = 5;

const int FILES_NUMBER = 3;

const char *READER_SEM = "reader_sem";
const char *WRITER_SEM = "writer_sem";
const char *FILE_NAME = "file";

pthread_mutex_t *write_mutexes;
pthread_mutex_t *read_mutexes;

// writer thread function
int writer(void *data)
{

    FILE *file = fopen(FILE_NAME, "w");

    for (int i = 0; i < WRITER_TURNS; i++)
    {
        if (sem_wait(writer_sem) != 0)
            error("Error occured during locking the writer semaphore.\n");

        // write
        printf("(W) writer started writing...");
        fflush(stdout);
        fprintf(file, "%02d\t", i);
        fflush(file);
        usleep(get_random_time(800));
        printf("(W) finished\n");

        // Release ownership of the semapthore object.
        if (sem_post(writer_sem) != 0)
            error("Error occured during unlocking the writer semaphore.\n");
        // _think, think, think, think
        usleep(get_random_time(1000));
    }

    fclose(file);

    return 0;
}

// reader thread function
int reader(void *data)
{
    int threadId = *(int *)data;

    FILE *file = fopen(FILE_NAME, "r");
    char *read_buffer =
        (char *)calloc(10, sizeof(char));

    for (int i = 0; i < READER_TURNS; i++)
    {
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

    buffer =
        (char *)calloc(BUFFER_SIZE, sizeof(char));

    pthread_t writer_thread;
    pthread_t buffer_writer_threads[WRITERS_COUNT];
    pthread_t reader_threads[READERS_COUNT];

    int i, rc;

    if ((writer_sem = sem_open(WRITER_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        error("create writer sem");

    if ((reader_sem = sem_open(READER_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        error("create writer sem");

    // Create the writer thread
    rc = pthread_create(
        &writer_thread, // thread identifier
        NULL,           // thread attributes
        (void *)writer, // thread function
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
            &buffer_writer_threads[i], // thread identifier
            NULL,                      // thread attributes
            (void *)buffer_writer,     // thread function
            (void *)threadId);         // thread function argument

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

    // Wait for the writer
    pthread_join(writer_thread, NULL);

    if (sem_unlink(WRITER_SEM) < 0)
        error("writer sem unlink");

    if (sem_unlink(READER_SEM) < 0)
        error("reader sem unlink");

    free((void *)buffer);

    return (0);
}

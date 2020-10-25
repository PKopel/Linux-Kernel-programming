#include "../common.h"

const int READER_TURNS = 10;
const int WRITER_TURNS = 10;
const int READERS_COUNT = 5;

const char *READER_SEM = "reader_sem";
const char *WRITER_SEM = "writer_sem";
const char *FILE_NAME = "file.txt";

sem_t *writer_sem, *reader_sem;
int readers_count = 0;

// Writer thread function
int Writer(void *data)
{

    FILE *file = fopen(FILE_NAME, "w");

    for (int i = 0; i < WRITER_TURNS; i++)
    {
        if (sem_wait(writer_sem) != 0)
            error("Error occured during locking the writer semaphore.\n");
        else
        {

            // Write
            printf("(W) Writer started writing...");
            fflush(stdout);
            fprintf(file, "%02d\t", i);
            fflush(file);
            usleep(get_random_time(800));
            printf("(W) finished\n");

            // Release ownership of the mutex object.
            if (sem_post(writer_sem) != 0)
                error("Error occured during unlocking the writer semaphore.\n");
            // Think, think, think, think
            usleep(get_random_time(1000));
        }
    }

    fclose(file);

    return 0;
}

// Reader thread function
int Reader(void *data)
{
    int i;
    int threadId = *(int *)data;

    FILE *file = fopen(FILE_NAME, "r");
    char *read_buffer =
        (char *)calloc(10, sizeof(char));

    for (i = 0; i < READER_TURNS; i++)
    {
        if (sem_wait(reader_sem) != 0)
            error("Error occured during locking the reader semaphore.\n");
        else
        {
            readers_count++;
            if (readers_count == 1)
                if (sem_wait(writer_sem) != 0)
                    error("Error occured during locking the writer semaphore.\n");
            if (sem_post(reader_sem) != 0)
                error("Error occured during unlocking the reader semaphore.\n");

            // Read
            printf("(R) Reader %d started reading...", threadId);
            fflush(stdout);
            fread(read_buffer, sizeof(char), 10, file);
            printf("(R) Reader %d read \"%s\"", threadId, read_buffer);
            usleep(get_random_time(200));
            printf("(R) Reader %d finished\n", threadId);

            if (sem_wait(reader_sem) != 0)
                error("Error occured during locking the reader semaphore.\n");
            else
            {
                readers_count--;
                if (readers_count == 0)
                    if (sem_post(writer_sem) != 0)
                        error("Error occured during unlocking the writer semaphore.\n");
                if (sem_post(reader_sem) != 0)
                    error("Error occured during unlocking the reader semaphore.\n");
            }

            usleep(get_random_time(800));
        }
    }

    free((void *)data);
    free((void *)read_buffer);
    fclose(file);

    return 0;
}

int main(int argc, char *argv[])
{
    srand(100005);

    pthread_t writerThread;
    pthread_t readerThreads[READERS_COUNT];

    int i, rc;

    if ((writer_sem = sem_open(WRITER_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        error("create writer sem");

    if ((reader_sem = sem_open(READER_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        error("create writer sem");

    // Create the Writer thread
    rc = pthread_create(
        &writerThread,  // thread identifier
        NULL,           // thread attributes
        (void *)Writer, // thread function
        (void *)NULL);  // thread function argument

    if (rc != 0)
    {
        fprintf(stderr, "Couldn't create the writer thread");
        exit(-1);
    }

    // Create the Reader threads
    for (i = 0; i < READERS_COUNT; i++)
    {
        // Reader initialization - takes random amount of time
        usleep(get_random_time(1000));
        int *threadId = malloc(sizeof(int));
        *threadId = i;
        rc = pthread_create(
            &readerThreads[i], // thread identifier
            NULL,              // thread attributes
            (void *)Reader,    // thread function
            (void *)threadId); // thread function argument

        if (rc != 0)
        {
            fprintf(stderr, "Couldn't create the reader threads");
            exit(-1);
        }
    }

    // At this point, the readers and writers should perform their operations

    // Wait for the Readers
    for (i = 0; i < READERS_COUNT; i++)
        pthread_join(readerThreads[i], NULL);

    // Wait for the Writer
    pthread_join(writerThread, NULL);

    if (sem_unlink(WRITER_SEM) < 0)
        error("writer sem unlink");

    if (sem_unlink(READER_SEM) < 0)
        error("reader sem unlink");
    return (0);
}

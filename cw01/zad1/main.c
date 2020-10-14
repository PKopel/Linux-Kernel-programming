#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

const int READER_TURNS = 10;
const int WRITER_TURNS = 10;
const int READERS_COUNT = 5;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Writer thread function
int Writer(void *data)
{
    int i;

    for (i = 0; i < WRITER_TURNS; i++)
    {
        int result = pthread_mutex_lock(
            &mutex);
        if (result != 0)
        {
            fprintf(stderr, "Error occured during locking the mutex.\n");
            exit(-1);
        }
        else
        {
            // Write
            printf("(W) Writer started writing...");
            fflush(stdout);
            usleep(GetRandomTime(800));
            printf("finished\n");

            // Release ownership of the mutex object.
            result = pthread_mutex_unlock(&mutex);
            if (result != 0)
            {
                fprintf(stderr, "Error occured during unlocking the mutex.\n");
                exit(-1);
            }
            // Think, think, think, think
            usleep(GetRandomTime(1000));
        }
    }

    return 0;
}

// Reader thread function
int Reader(void *data)
{
    int i;
    int threadId = *(int *)data;

    for (i = 0; i < READER_TURNS; i++)
    {
        int result = pthread_mutex_lock(
            &mutex);
        if (result != 0)
        {
            fprintf(stderr, "Error occured during locking the mutex.\n");
            exit(-1);
        }
        else
        {
            // Read
            printf("(R) Reader %d started reading...", threadId);
            fflush(stdout);
            // Read, read, read
            usleep(GetRandomTime(200));
            printf("finished\n");

            // Release ownership of the mutex object.
            result = pthread_mutex_unlock(&mutex);
            if (result != 0)
            {
                fprintf(stderr, "Error occured during unlocking the mutex.\n");
                exit(-1);
            }

            usleep(GetRandomTime(800));
        }
    }

    free(data);

    return 0;
}

int main(int argc, char *argv[])
{
    srand(100005);

    pthread_t writerThread;
    pthread_t readerThreads[READERS_COUNT];

    int i, rc;

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
        usleep(GetRandomTime(1000));
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

    return (0);
}
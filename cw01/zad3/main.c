#include "../common.h"

const int READER_TURNS = 10;
const int WRITER_TURNS = 10;
const int READERS_COUNT = 5;
const int WRITERS_COUNT = 5;

const int FILES_NUMBER = 3;

const char *READER_SEM = "reader_sem";
const char *WRITER_SEM = "writer_sem";
const char *FILE_NAMES[] = {"file1", "file2", "file3"};

sem_t *write_sems;
sem_t *read_sems;
int *readers_counts;

// writer thread function
int writer(void *data)
{
    int threadId = *(int *)data;

    int i, index;

    FILE **files =
        (FILE **)calloc(FILES_NUMBER, sizeof(FILE *));

    for (i = 0; i < FILES_NUMBER; i++)
        files[i] = fopen(FILE_NAMES[i], "a");

    for (i = 0; i < WRITER_TURNS; i++)
    {
    try_file:
        index = get_random_index(FILES_NUMBER);

        if (sem_trywait(&write_sems[index]) != 0)
        {
            if (errno == EAGAIN)
            {
                usleep(get_random_time(10));
                goto try_file;
            }
            else
                error("(W) Error occured during locking the writer semaphore.\n");
        }

        // write
        printf("(W) writer %d started writing to file %d...", threadId, index);
        fflush(stdout);
        fseek(files[index], 0, SEEK_END);
        fprintf(files[index], "W %03d %03d\n", threadId, i);
        fflush(files[index]);
        usleep(get_random_time(800));
        printf("(W) writer %d finished\n", threadId);

        // Release ownership of the semapthore object.
        if (sem_post(&write_sems[index]) != 0)
            error("(W) Error occured during unlocking the writer semaphore.\n");
        // _think, think, think, think
        usleep(get_random_time(1000));
    }

    free((void *)data);
    for (i = 0; i < FILES_NUMBER; i++)
        fclose(files[i]);

    return 0;
}

// reader thread function
int reader(void *data)
{
    int i, index, threadId = *(int *)data;

    FILE **files =
        (FILE **)calloc(FILES_NUMBER, sizeof(FILE *));

    for (i = 0; i < FILES_NUMBER; i++)
        files[i] = fopen(FILE_NAMES[i], "r");

    char *read_buffer =
        (char *)calloc(10, sizeof(char));

    for (i = 0; i < READER_TURNS; i++)
    {
    try_again:
        index = get_random_index(FILES_NUMBER);

        if (sem_trywait(&read_sems[index]) != 0)
        {
            if (errno == EAGAIN)
            {
                usleep(get_random_time(10));
                goto try_again;
            }
            else
                error("(R) Error occured during locking the reader semaphore (I).\n");
        }
        readers_counts[index]++;
        if (readers_counts[index] == 1)
            if (sem_wait(&write_sems[index]) != 0)
                error("(R) Error occured during locking the writer semaphore.\n");
        if (sem_post(&read_sems[index]) != 0)
            error("(R) Error occured during unlocking the reader semaphore (I).\n");

        // Read
        printf("(R) reader %d started reading from file %d...", threadId, index);
        fflush(stdout);
        fread(read_buffer, sizeof(char), 10, files[index]);
        printf("(R) reader %d read \"%s\"", threadId, read_buffer);
        usleep(get_random_time(200));
        printf("(R) reader %d finished\n", threadId);

        if (sem_wait(&read_sems[index]) != 0)
            error("(R) Error occured during locking the reader semaphore (D).\n");

        readers_counts[index]--;
        if (readers_counts[index] == 0)
            if (sem_post(&write_sems[index]) != 0)
                error("(R) Error occured during unlocking the writer semaphore.\n");
        if (sem_post(&read_sems[index]) != 0)
            error("(R) Error occured during unlocking the reader semaphore (D).\n");

        usleep(get_random_time(800));
    }

    free((void *)data);
    free((void *)read_buffer);
    for (i = 0; i < FILES_NUMBER; i++)
        fclose(files[i]);

    return 0;
}

int main(int argc, char *argv[])
{
    srand(100005);

    write_sems =
        (sem_t *)calloc(FILES_NUMBER, sizeof(sem_t));

    read_sems =
        (sem_t *)calloc(FILES_NUMBER, sizeof(sem_t));

    readers_counts =
        (int *)calloc(FILES_NUMBER, sizeof(int));

    pthread_t writer_threads[WRITERS_COUNT];
    pthread_t reader_threads[READERS_COUNT];

    int i, rc;

    for (i = 0; i < FILES_NUMBER; i++)
    {
        sem_init(&write_sems[i], 0, 1);
        sem_init(&read_sems[i], 0, 1);
        readers_counts[i] = 0;
    }

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

    free((void *)write_sems);
    free((void *)read_sems);
    free((void *)readers_counts);

    return (0);
}

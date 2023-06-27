#include "zc_io.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>

// The zc_file struct is analogous to the FILE struct that you get from fopen.
struct zc_file
{
    // Insert the fields you need here.
    int reader_count;
    char *mapped_memory;
    off_t offset;
    size_t file_size;
    int file_descriptor;
    pthread_mutex_t memory_mutex;
    sem_t read_write_sem;
    pthread_mutex_t *page_mutexes;
    size_t page_size;


    /* Some suggested fields : 
        - pointer to the virtual memory space
        - offset from the start of the virtual memory
        - total size of the file
        - file descriptor to the opened file
        - mutex for access to the memory space and number of readers
    */
};

/**************
 * Exercise 1 *
 **************/

zc_file *zc_open(const char *path)
{
    // To implement
    printf("Opening file");
    if (path == NULL)
    {
        return NULL;
    }

    // int new_file = 0;
    zc_file *file = (zc_file *)malloc(sizeof(zc_file));

    // Open for reading/writing, and create the file if it doesn't exist.
    int fd = open(path, O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        // if (errno == ENOENT)
        // {
        //     new_file = 1;
        // }
        return NULL;
    }

    // if (new_file)
    // {
    //     file->reader_count = 0;
    //     file->mapped_memory = NULL;
    //     file->offset = 0;
    //     file->file_size = 0;
    //     file->file_descriptor = fd;
    //     pthread_mutex_init(&file->memory_mutex, NULL);
    //     return file;
    // }

    // Read fstat into st
    struct stat st;
    int fstat_result = fstat(fd, &st);
    if (fstat_result == -1)
    {
        close(fd);
        return NULL;
    }

    if (st.st_size == 0)
    {
        file->reader_count = 0;
        file->mapped_memory = NULL;
        file->offset = 0;
        file->file_size = 0;
        file->file_descriptor = fd;
        pthread_mutex_init(&file->memory_mutex, NULL);
        sem_init(&file->read_write_sem, 0, 1);
        file->page_size = sysconf(_SC_PAGE_SIZE);
        
        if (file->page_size > 0)
        {
            size_t num_pages = (st.st_size + file->page_size - 1) / file->page_size;
            file->page_mutexes = (pthread_mutex_t *)malloc(num_pages * sizeof(pthread_mutex_t));
            for (size_t i = 0; i < num_pages; i++)
            {
                pthread_mutex_init(&file->page_mutexes[i], NULL);

            }
        }

        // Allocate and initialize per-page mutexes
        
        return file;
    }

    char *mem = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED_VALIDATE, fd, 0);
    if (mem == MAP_FAILED)
    {
        munmap(mem, st.st_size);
        close(fd);
        return NULL;
    }

    file->reader_count = 0;
    file->mapped_memory = mem;
    file->offset = 0;
    file->file_size = st.st_size;
    file->file_descriptor = fd;
    pthread_mutex_init(&file->memory_mutex, NULL);
    sem_init(&file->read_write_sem, 0, 1);

    return file;
}

int zc_close(zc_file *file)
{
    // To implement
    if (file == NULL)
    {
        return -1;
    }

    // p_thread_mutex_lock(&file->memory_mutex);
    int msync_result = msync(file->mapped_memory, file->file_size, MS_SYNC);
    if (msync_result == -1)
    {
        return -1;
    }

    int munmap_result = munmap(file->mapped_memory, file->file_size);
    if (munmap_result == -1)
    {
        return -1;
    }

    int close_result = close(file->file_descriptor);
    if (close_result == -1)
    {
        return -1;
    }

    pthread_mutex_destroy(&file->memory_mutex);
    sem_destroy(&file->read_write_sem);

    free(file);
    return 0;
}

const char *zc_read_start(zc_file *file, size_t *size)
{
    // To implement
    // if (file == NULL || size == NULL)
    // {
    //     return NULL;
    // }

    pthread_mutex_lock(&file->memory_mutex);
    file->reader_count++;
    if (file->reader_count == 1)
    {
        sem_wait(&file->read_write_sem);
    }


    size_t remaining_size = file->file_size - file->offset;
    if (remaining_size < *size)
    {
        *size = remaining_size;
    }

    char *start = file->mapped_memory + file->offset;
    file->offset += *size;

    pthread_mutex_unlock(&file->memory_mutex);

    return start;
}

void zc_read_end(zc_file *file)
{
    // To implement
    pthread_mutex_lock(&file->memory_mutex);
    file->reader_count--;
    if (file->reader_count == 0)
    {
        sem_post(&file->read_write_sem);
    }
    pthread_mutex_unlock(&file->memory_mutex);
    
}

char *zc_write_start(zc_file *file, size_t size)
{
    // To implement
    // if (file == NULL || size == 0)
    // {
    //     return NULL;
    // }

    // pthread_mutex_lock(&file->memory_mutex);
    sem_wait(&file->read_write_sem);

    size_t remaining_size = file->file_size - file->offset;

    size_t new_size = size > remaining_size 
                ? file->offset + size 
                : file->file_size;

    // ftruncate(file->file_descriptor, new_size);
    // mremap(file->mapped_memory, file->file_size, new_size, MREMAP_MAYMOVE);
    // file->file_size = new_size;


    ftruncate(file->file_descriptor, new_size);

    file->mapped_memory = file->file_size == 0
                    ? mmap(NULL, new_size, PROT_READ | PROT_WRITE, MAP_SHARED_VALIDATE, file->file_descriptor, 0)
                    : mremap(file->mapped_memory, file->file_size, new_size, MREMAP_MAYMOVE);


    file->file_size = new_size;
    char *start = file->mapped_memory + file->offset;
    file->offset += size;

    return start;
}

void zc_write_end(zc_file *file)
{
    msync(file->mapped_memory, file->file_size, MS_SYNC);
    // pthread_mutex_unlock(&file->memory_mutex);
    sem_post(&file->read_write_sem);
}

/**************
 * Exercise 2 *
 **************/

off_t zc_lseek(zc_file *file, long offset, int whence)
{
    
    // pthread_mutex_lock(&file->memory_mutex);
    sem_wait(&file->read_write_sem);
    off_t new_offset = -1;

    if (whence == SEEK_SET)
    {
        new_offset = offset;
        file->offset = new_offset;
    }
    else if (whence == SEEK_CUR)
    {
        new_offset = file->offset + offset;
        file->offset = new_offset;
    }
    else if (whence == SEEK_END)
    {
        new_offset = file->file_size + offset;
        file->offset = new_offset;
    }

    // pthread_mutex_unlock(&file->memory_mutex);
    sem_post(&file->read_write_sem);
    return new_offset;
}

/**************
 * Exercise 3 *
 **************/

int zc_copyfile(const char *source, const char *dest)
{
    // To implement
    zc_file *source_file = zc_open(source);
    zc_file *dest_file = zc_open(dest);

    if (source_file == NULL || dest_file == NULL)
    {
        zc_close(source_file);
        zc_close(dest_file);
        return -1;
    }

    size_t size = source_file->file_size;

    char *start = zc_read_start(source_file, &size);
    char *dest_start = zc_write_start(dest_file, size);

    if (start == NULL || dest_start == NULL)
    {
        zc_read_end(source_file);
        zc_write_end(dest_file);
        zc_close(source_file);
        zc_close(dest_file);
        return -1;
    }

    memcpy(dest_start, start, size);

    zc_read_end(source_file);
    zc_write_end(dest_file);

    int close_source = zc_close(source_file);
    int close_dest = zc_close(dest_file);

    if (close_source == -1 || close_dest == -1)
    {
        return -1;
    }

    return 0;
}

/**************
 * Bonus Exercise *
 **************/

const char *zc_read_offset(zc_file *file, size_t *size, long offset)
{
    if (file == NULL || size == NULL || offset < 0) {
            return NULL;
    }

    // Calculate the starting page and offset within the page
    long page_size = sysconf(_SC_PAGESIZE);
    long page_num = offset / page_size;
    long page_offset = offset % page_size;

    // Lock the page's mutex
    pthread_mutex_t *page_mutex = &file->page_mutexes[page_num];
    pthread_mutex_lock(page_mutex);

    pthread_mutex_lock(&file->memory_mutex);
    file->reader_count++;
    if (file->reader_count == 1)
    {
        sem_wait(&file->read_write_sem);
    }


    size_t remaining_size = file->file_size - offset;
    if (remaining_size < *size)
    {
        *size = remaining_size;
    }

    char *start = file->mapped_memory + offset;
    // file->offset += *size;

    pthread_mutex_unlock(&file->memory_mutex);

    // Unlock the page's mutex
    pthread_mutex_unlock(page_mutex);

    return start;
}

char *zc_write_offset(zc_file *file, size_t size, long offset)
{
    if (file == NULL || size == 0 || offset < 0) {
        return NULL;
    }

    // Calculate the starting page and offset within the page
    long page_size = sysconf(_SC_PAGESIZE);
    long page_num = offset / page_size;
    long page_offset = offset % page_size;

    // Lock the page's mutex
    pthread_mutex_t *page_mutex = &file->page_mutexes[page_num];
    pthread_mutex_lock(page_mutex);

    sem_wait(&file->read_write_sem);

    size_t remaining_size = file->file_size - offset;

    size_t new_size = size > remaining_size 
                ? offset + size 
                : file->file_size;

    ftruncate(file->file_descriptor, new_size);

    file->mapped_memory = file->file_size == 0
                    ? mmap(NULL, new_size, PROT_READ | PROT_WRITE, MAP_SHARED_VALIDATE, file->file_descriptor, 0)
                    : mremap(file->mapped_memory, file->file_size, new_size, MREMAP_MAYMOVE);


    file->file_size = new_size;
    char *start = file->mapped_memory + offset;


    // Unlock the page's mutex
    pthread_mutex_unlock(page_mutex);

    return start;
}
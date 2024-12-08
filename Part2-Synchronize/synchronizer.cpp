#include <windows.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "buffer.cpp"

using namespace std;

bool threads_running = false;

DWORD WINAPI producer(LPVOID b) {
    /* initialize buffer and item */
    Buffer *buffer = (Buffer*)b;
    buffer_item item;

    while (threads_running) {
        /* sleep for a random period of time */
        sleep(rand() % 20);

        /* wait */
        WaitForSingleObject(buffer->empty, INFINITE);
        WaitForSingleObject(buffer->mutex, INFINITE);

        /*** critical section ***/
        item = rand();
        buffer->insert_item(item);
        cout << "\titem inserted: " << item << endl << "\t";
        buffer->display();
        cout << endl;

        /* signal */
        ReleaseMutex(buffer->mutex);
        ReleaseSemaphore(buffer->full, 1, NULL);
    }

    return 0;
}

DWORD WINAPI consumer(LPVOID b) {
    /* initialize buffer and item */
    Buffer *buffer = (Buffer*)b;
    buffer_item item;

    while (threads_running) {
        /* sleep for a random period of time */
        sleep(rand() % 20);

        /* wait */
        WaitForSingleObject(buffer->full, INFINITE);
        WaitForSingleObject(buffer->mutex, INFINITE);
        
        /*** critical section ***/
        buffer->remove_item(item);
        cout << "\titem removed: " << item << endl << "\t";
        buffer->display();
        cout << endl;

        /* signal */
        ReleaseMutex(buffer->mutex);
        ReleaseSemaphore(buffer->empty, 1, NULL);
    }

    return 0;
}

int synchronizer(int num_producers, int num_consumers, int sleep_time) {
    cout << "> begin test: num_producers = " << num_producers << ", num_consumers = " << num_consumers << ", sleep_time = " << sleep_time << endl << endl;
    
    /* initialize buffer */
    Buffer buffer;
    HANDLE p_threads[num_producers];
    HANDLE c_threads[num_consumers];
    threads_running = true;

    /* create producer threads */
    for (int i = 0; i < num_producers; i++) {
        DWORD thread_id;
        HANDLE thread_handle = CreateThread(NULL, 0, producer, &buffer, 0, &thread_id);
        p_threads[i] = thread_handle;
    }

    /* create consumer threads */
    for (int i = 0; i < num_consumers; i++) {
        DWORD thread_id;
        HANDLE thread_handle = CreateThread(NULL, 0, consumer, &buffer, 0, &thread_id);
        c_threads[i] = thread_handle;
    }

    /* sleep */
    sleep(sleep_time);

    /* close threads */  
    cout << "> begin shutdown" << endl << endl;
    threads_running = false;

    WaitForMultipleObjects(num_producers, p_threads, TRUE, INFINITE);
    WaitForMultipleObjects(num_consumers, c_threads, TRUE, INFINITE);
    
    cout << "> threads closed: end test" << endl << endl;

    /* exit*/
    return 0;
}

int main() {
    int list_producers[12] = {1, 4, 16, 1, 4, 16, 1, 4, 16, 1, 4, 16};
    int list_consumers[12] = {1, 1, 1, 2, 2, 2, 4, 4, 4, 16, 16, 16};
    int list_sleep_times[12] = {10, 20, 30, 40, 50, 60, 10, 20, 30, 40, 50, 60};

    for (int i = 0; i < sizeof(list_producers); i++) {
        synchronizer(list_producers[i], list_consumers[i], list_sleep_times[i]);
    }
}
#include <windows.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "buffer.cpp"

using namespace std;

DWORD WINAPI producer(LPVOID b) {
    /* initialize buffer and item */
    Buffer *buffer = (Buffer*)b;
    buffer_item item;

    while (true) {
        /* sleep for a random period of time */
        sleep(rand() % 20);

        /* generate random number */
        item = rand();

        /* wait */
        WaitForSingleObject(buffer->empty, INFINITE);
        WaitForSingleObject(buffer->mutex, INFINITE);

        /*** critical section ***/
        buffer->insert_item(item);
        cout << "item inserted: " << item << endl;
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

    while (true) {
        /* sleep for a random period of time */
        sleep(rand() % 20);

        /* wait */
        WaitForSingleObject(buffer->full, INFINITE);
        WaitForSingleObject(buffer->mutex, INFINITE);
        
        /*** critical section ***/
        buffer->remove_item(item);
        cout << "item removed: " << item << endl;
        buffer->display();
        cout << endl;

        /* signal */
        ReleaseMutex(buffer->mutex);
        ReleaseSemaphore(buffer->empty, 1, NULL);
    }

    return 0;
}

int main() {
    /* get params */
    int sleep_time = 120;
    int num_producers = 1;
    int num_consumers = 1;
    
    /* initialize buffer */
    Buffer buffer;

    /* create producer threads */
    for (int i = 0; i < num_producers; i++) {
        DWORD thread_id;
        HANDLE thread_handle = CreateThread(NULL, 0, producer, &buffer, 0, &thread_id);
    }

    /* create consumer threads */
    for (int i = 0; i < num_consumers; i++) {
        DWORD thread_id;
        HANDLE thread_handle = CreateThread(NULL, 0, consumer, &buffer, 0, &thread_id);
    }

    /* sleep */
    sleep(sleep_time);

    /* exit*/
    return 0;
}
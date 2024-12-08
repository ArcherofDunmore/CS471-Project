#include <windows.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "buffer.cpp"
#include <synchapi.h>
#include <vector>
#include <chrono>
#include <cmath>
#include <fstream>

using namespace std;

bool threads_running = false;
vector<double> p_turnarounds;
vector<double> c_turnarounds;

DWORD WINAPI producer(LPVOID b) {
    /* initialize buffer and item */
    Buffer *buffer = (Buffer*)b;
    buffer_item item;

    while (threads_running) {
        /* sleep for a random period of time */
        sleep(rand() % 15);

        chrono::system_clock::time_point start_time = chrono::system_clock::now();
        
        /* wait */
        WaitForSingleObject(buffer->empty, 16000);
        WaitForSingleObject(buffer->mutex, 16000);

        if (threads_running) {
            /*** critical section ***/
            item = rand();
            buffer->insert_item(item);
            cout << "\titem inserted: " << item << endl << "\t";
            buffer->display();
            cout << endl;

            chrono::system_clock::time_point end_time = chrono::system_clock::now();
            std::chrono::duration<double> elapsed = end_time - start_time;
            p_turnarounds.push_back(elapsed.count());
        }

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
        sleep(rand() % 15);

        chrono::system_clock::time_point start_time = chrono::system_clock::now();

        /* wait */
        WaitForSingleObject(buffer->full, 16000);
        WaitForSingleObject(buffer->mutex, 16000);

        if (threads_running) {          
            /*** critical section ***/
            buffer->remove_item(item);
            cout << "\titem removed: " << item << endl << "\t";
            buffer->display();
            cout << endl;

            chrono::system_clock::time_point end_time = chrono::system_clock::now();
            std::chrono::duration<double> elapsed = end_time - start_time;
            c_turnarounds.push_back(elapsed.count());
        }

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

int print_data(string status, vector<vector<double>> x, int p[], int c[], int s) {
    ofstream fout;

    if (status == "producer") {
        fout.open("producer.txt");
    }
    else {
        fout.open("consumer.txt");
    }

    double all_sum = 0.0;
    int all_total = 0;

    for (int i = 0; i < x.size(); i++) {
        double sum = 0.0;
        int total = 0;

        fout << "test " << i+1 << ": " << "num_producers = " << p[i] << ", num_consumers = " << c[i] << ", sleep_time = " << s << endl
             << "\tall turnaround times: [ ";

        for (int j = 0; j < x[i].size(); j++) {
            fout << x[i][j] << " ";

            sum += x[i][j];
            all_sum += x[i][j];

            total++;
            all_total++;
        }
        fout << "]" << endl;
        fout << "\taverage turaround time: " << (sum/total) << endl << endl;
    }

    fout << "overall average turnaround time: " << (all_sum/all_total);

    fout.close();

    return 0;
}

int main(int argc, char **argv) {
    vector<vector<double>> pt_collect;
    vector<vector<double>> ct_collect;
    int list_producers[12] = {1, 4, 16, 1, 4, 16, 1, 4, 16, 1, 4, 16};
    int list_consumers[12] = {1, 1, 1, 2, 2, 2, 4, 4, 4, 16, 16, 16};
    int sleep_time = atoi(argv[1]);

    for (int j = 0; j < 12; j++) {
        p_turnarounds.clear();
        c_turnarounds.clear();
        synchronizer(list_producers[j], list_consumers[j], sleep_time);
        pt_collect.push_back(p_turnarounds);
        ct_collect.push_back(c_turnarounds);
    }

    print_data("producer", pt_collect, list_producers, list_consumers, sleep_time);
    print_data("consumer", ct_collect, list_producers, list_consumers, sleep_time);

    return 0;
}
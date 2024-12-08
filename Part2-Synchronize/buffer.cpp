#include <windows.h>
#include <iostream>
#include <vector>
#include <stdlib.h>

typedef int buffer_item;

class Buffer {
    private:
        #define BUFFER_SIZE 5
        std::vector<buffer_item> buffer;
        int buffer_fill = 0;

    public:
        HANDLE mutex;
        HANDLE empty;
        HANDLE full;

        Buffer() {
            mutex = CreateMutex(NULL, FALSE, NULL);
            empty = CreateSemaphore(NULL, 5, BUFFER_SIZE, NULL);
            full = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);
        }

        int insert_item(buffer_item item) {
            if (buffer.size() < BUFFER_SIZE) {
                buffer.push_back(item);
                return 0;
            }
            else {
                return -1;
            }
            // return 0 if successful
            // return -1 if error

        }

        int remove_item(buffer_item &item) {
            if (buffer.size() > 0) {
                item = buffer.back();
                buffer.pop_back();
                return 0;
            }
            else {
                item = -1;
                return -1;
            }
            // remove item from buffer
            // return 0 if successful
            // return -1 if error
        }

        void display() {
            std::cout << "[ ";
            for (int i = 0; i < buffer.size(); i++) {
                std::cout << buffer[i] << " ";
            }
            std::cout << "]" << std::endl;
        }
};
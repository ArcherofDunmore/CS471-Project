#include <windows.h>
#include <iostream>

typedef int buffer_item;

class Buffer {
    private:
        #define BUFFER_SIZE 5
        buffer_item buffer[BUFFER_SIZE];
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
            buffer[buffer_fill] = item;
            buffer_fill++;
            return 0;
            // return 0 if successful
            // return -1 if error

        }

        int remove_item(buffer_item &item) {
            item = buffer[buffer_fill-1];
            buffer_fill--;
            return 0;
            // remove item from buffer
            // return 0 if successful
            // return -1 if error
        }

        void display() {
            if (buffer_fill == 0) {
                std::cout << "[]" << std::endl;
            } else if (buffer_fill == 1) {
                std::cout << "[" << buffer[0] << "]" << std::endl;
            } else {
                std::cout << "[";
                for (int i = 0; i < buffer_fill-1; i++) {
                    std::cout << buffer[i] << ", ";
                }
                std::cout << buffer[buffer_fill-1] << "]" << std::endl;
            }
        }
};
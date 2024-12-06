#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>

using namespace std;

// Struct to hold process info
struct Process
{
    int arrival_time;
    int burst_length;
    int priority;
    int start_time = -1;
    int finish_time = 0;
    int remaining_time;
    Process(int at, int bl, int p)
        : arrival_time(at), burst_length(bl), priority(p), remaining_time(bl) {}
};

// Comparator for FIFO (sort by arrival time)
struct FIFOComparator
{
    bool operator()(const Process &a, const Process &b)
    {
        return a.arrival_time > b.arrival_time;
    }
};

// Comparator for Priority (sort by priority, breaks ties with arrival time)
struct PriorityComparator
{
    bool operator()(const Process &a, const Process &b)
    {
        if (a.priority == b.priority)
        {
            return a.arrival_time > b.arrival_time;
        }
        return a.priority > b.priority;
    }
};

// Function to simulate CPU scheduling
void simulateScheduling(const string &filename, int scheduling_type)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Unable to open file: " << filename << endl;
        return;
    }
    else
    {
        cout << "File opened successfully: " << filename << endl;
    }

    vector<Process> processes;
    int arrival_time, burst_length, priority;

    string line;
    getline(file, line); // Skip the first line (header)

    // Read processes from the file
    while (file >> arrival_time >> burst_length >> priority)
    {
        // Adds new process at the end of the list, retaining the same order
        processes.emplace_back(arrival_time, burst_length, priority);

        // Process arrival debugging
        /* cout << "Read process: Arrival=" << arrival_time
             << ", Burst=" << burst_length
             << ", Priority=" << priority << endl; */
    }
    file.close();

    // Ready queue and finished processes
    priority_queue<Process, vector<Process>, FIFOComparator> fifoQueue;
    priority_queue<Process, vector<Process>, PriorityComparator> priorityQueue;
    vector<Process> completedProcesses;

    int current_time = 0;
    int num_completed = 0;
    int cpu_utilization = 0;

    // Scheduling loop
    while (num_completed < 500 && !processes.empty())
    {
        // Adds a process to the queue whenever current_time comes to equal that process's arrival_time
        while (!processes.empty() && processes.front().arrival_time <= current_time)
        {
            if (scheduling_type == 1)
            { // FIFO
                fifoQueue.push(processes.front());

                // Add to queue debugging
                /* cout << "Added to FIFO Queue: Arrival=" << processes.front().arrival_time
                     << ", Burst=" << processes.front().burst_length
                     << ", Priority=" << processes.front().priority << endl; */
            }
            else if (scheduling_type == 2)
            { // Priority
                priorityQueue.push(processes.front());

                // Add to queue debugging
                /* cout << "Added to Priority Queue: Arrival=" << processes.front().arrival_time
                     << ", Burst=" << processes.front().burst_length
                     << ", Priority=" << processes.front().priority << endl; */
            }
            // Erase the process that was just added to the queue
            processes.erase(processes.begin());
        }

        // Create process placeholder object
        Process currentProcess(0, 0, 0);

        // Copy top of stack onto placeholder, then delete that item from stack
        if (scheduling_type == 1 && !fifoQueue.empty())
        {
            currentProcess = fifoQueue.top();
            fifoQueue.pop();
        }
        else if (scheduling_type == 2 && !priorityQueue.empty())
        {
            currentProcess = priorityQueue.top();
            priorityQueue.pop();
        }
        else // If no processes here to work with, simply advance current_time and restart
        {
            current_time++;
            continue;
        }

        if (currentProcess.start_time == -1)
        {
            currentProcess.start_time = current_time;
        }

        // Perform CPU burst to complete current process
        current_time += currentProcess.burst_length;
        // Allocate finish_time to process that was just finished
        currentProcess.finish_time = current_time;
        // Add burst length to cpu_utilization
        cpu_utilization += currentProcess.burst_length;

                completedProcesses.push_back(currentProcess);
        // Increment number of processes completed
        num_completed++;

        // Process completed debugging
        /* cout << "Process completed: Arrival=" << currentProcess.arrival_time
             << ", Start=" << currentProcess.start_time
             << ", Finish=" << currentProcess.finish_time
             << ", Turnaround=" << currentProcess.finish_time - currentProcess.arrival_time
             << ", Waiting=" << (currentProcess.finish_time - currentProcess.arrival_time) - currentProcess.burst_length
             << endl; */
    }

    double total_turnaround_time = 0, total_waiting_time = 0, total_response_time = 0;

    for (const auto &process : completedProcesses)
    {
        int turnaround_time = process.finish_time - process.arrival_time;
        int waiting_time = turnaround_time - process.burst_length;
        int response_time = process.start_time - process.arrival_time;

        total_turnaround_time += turnaround_time;
        total_waiting_time += waiting_time;
        total_response_time += response_time;
    }

    cout << "Statistics for the Run:" << endl;
    cout << "Number of processes: " << num_completed << endl;
    cout << "Total elapsed time (in CPU burst units): " << current_time << endl;
    cout << "Throughput: " << static_cast<double>(num_completed) / current_time << " processes per CPU burst unit" << endl;
    cout << "CPU utilization: " << static_cast<double>(cpu_utilization) / current_time * 100 << "%" << endl;
    cout << "Average waiting time: " << total_waiting_time / num_completed << " CPU burst units" << endl;
    cout << "Average turnaround time: " << total_turnaround_time / num_completed << " CPU burst units" << endl;
    cout << "Average response time: " << total_response_time / num_completed << " CPU burst units" << endl;
}

int main()
{
    string filename = "Datafile1-txt.txt";
    int scheduling_type;

    while (!(scheduling_type == 1 || scheduling_type == 2))
    {
        cout << "Enter scheduling type (1 for FIFO, 2 for Priority with preemption): ";
        cin >> scheduling_type;

        if (scheduling_type != 1 && scheduling_type != 2)
        {
            cerr << "Invalid selection." << endl;
        }
    }

    simulateScheduling(filename, scheduling_type);
    return 0;
}

// To run this program, in the terminal run the command -> g++ -o cpu_scheduling scheduler.cpp
// Then enter -> .\cpu_scheduling.exe
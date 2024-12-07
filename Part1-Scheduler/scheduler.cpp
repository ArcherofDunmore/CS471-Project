#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <functional>

using namespace std;

struct Process
{
    int pid;            // Process ID
    int arrivalTime;    // Arrival time
    int priority;       // Priority
    int cpuBurst;       // CPU burst units
    int remainingBurst; // Remaining burst time for preemption
    int waitingTime;    // Waiting time
    int turnaroundTime; // Turnaround time
    int responseTime;   // Response time
    int startTime;      // Start time

    Process(int id, int arrival, int pri, int burst)
        : pid(id), arrivalTime(arrival), priority(pri), cpuBurst(burst),
          remainingBurst(burst), waitingTime(0), turnaroundTime(0),
          responseTime(-1), startTime(-1) {}
};

class Scheduler
{
private:
    int time;

    queue<Process> fifoQueue;                                                                 // FIFO queue
    priority_queue<Process, vector<Process>, function<bool(Process, Process)>> priorityQueue; // Priority queue needs the comparator for priority

public:
    Scheduler()
        : time(0),
          priorityQueue([](const Process &a, const Process &b)
                        { return a.priority > b.priority; }) {}

    void runFIFO(vector<Process> &processes)
    {
        // Push all processes into the fifoQueue
        for (auto &p : processes)
        {
            fifoQueue.push(p);
        }

        // While the fifoQueue still has processes in it...
        while (!fifoQueue.empty())
        {
            // Copy the top of the stack into the placeholder object, and delete that item from the stack
            Process current = fifoQueue.front();
            fifoQueue.pop();

            // Advance time to the process's arrival time (if necessary)
            time = max(time, current.arrivalTime);

            // Set start time if current start time is still the default value
            if (current.startTime == -1)
            {
                current.startTime = time;
                current.responseTime = current.startTime - current.arrivalTime; // Set response time
            }

            // Calculate waiting time
            current.waitingTime = time - current.arrivalTime;

            // Execute the process
            time += current.cpuBurst;

            // Calculate turnaround time
            current.turnaroundTime = time - current.arrivalTime;

            // Update the process in the vector
            // Create pointer to processes, then update the item in 'processes' by applying the new updated 'current' values
            auto &processRef = processes[current.pid];
            processRef = current;
        }
    }

    void runPriority(vector<Process> &processes)
    {
        // Boolean comparator so it can know what processes are higher priority
        priority_queue<Process, vector<Process>, function<bool(Process, Process)>> priorityQueue(
            [](const Process &a, const Process &b)
            {
                if (a.priority == b.priority)
                    return a.arrivalTime > b.arrivalTime; // Break ties by arrival time
                return a.priority > b.priority;
            });

        size_t currentIndex = 0; // To track processes in the `processes` vector
        while (!priorityQueue.empty() || currentIndex < processes.size())
        {
            // Add newly arriving processes to the priority queue
            while (currentIndex < processes.size() && processes[currentIndex].arrivalTime <= time)
            {
                priorityQueue.push(processes[currentIndex]);
                currentIndex++;
            }

            // If no process is ready to run, increment time and continue
            if (priorityQueue.empty())
            {
                time++;
                continue;
            }

            // Otherwise, fetch the process from the queue into a placeholder object 'current'
            Process current = priorityQueue.top();
            priorityQueue.pop();

            // Set the start time if this is the first time the process is being executed
            if (current.startTime == -1)
            {
                current.startTime = time;
                current.responseTime = time - current.arrivalTime; // Calculate response time
            }

            // Execute the process one tick at a time
            while (current.remainingBurst > 0)
            {
                // Simulate one unit of execution
                current.remainingBurst--;
                time++;

                // Add newly arriving processes to the priority queue
                while (currentIndex < processes.size() && processes[currentIndex].arrivalTime <= time)
                {
                    priorityQueue.push(processes[currentIndex]);
                    currentIndex++;
                }

                // Check if the newly arrived process is higher priority
                if (!priorityQueue.empty() && priorityQueue.top().priority < current.priority)
                {
                    // If it is, preempt the current process and add it back to the priority queue
                    priorityQueue.push(current);
                    break; // Stop executing the current process
                }
            }

            // If the process finishes, calculate its turnaround and waiting times
            if (current.remainingBurst == 0)
            {
                current.turnaroundTime = time - current.arrivalTime;             // Calculate turnaround time
                current.waitingTime = current.turnaroundTime - current.cpuBurst; // Calculate waiting time

                // Update the original process in the `processes` vector
                processes[current.pid] = current;

                // Debug: Log completed processes
                // cout << "Completed PID: " << current.pid
                //     << " at time: " << time
                //     << " turnaround: " << current.turnaroundTime
                //     << " waiting: " << current.waitingTime << endl;
            }
        }
    }

    void printStatistics(const vector<Process> &processes, int scheduling_type) const
    {
        int totalWaitingTime = 0, totalTurnaroundTime = 0, totalResponseTime = 0, totalBurstTime = 0;

        // Calculate the total of the following variables from the 'processes' object, which contains a copy of all processes from the input file
        for (const auto &p : processes)
        {
            totalWaitingTime += p.waitingTime;
            totalTurnaroundTime += p.turnaroundTime;
            totalResponseTime += p.responseTime;
            totalBurstTime += p.cpuBurst;
        }

        int numProcesses = processes.size();
        string outputFilename = "";

        // Set the .txt file name
        if (scheduling_type == 1)
        {
            outputFilename = "Scheduler_Statistics_FIFO.txt";
        }
        else
        {
            outputFilename = "Scheduler_Statistics_Priority.txt";
        }

        // Open file
        ofstream outputFile(outputFilename);
        if (!outputFile.is_open())
        {
            cerr << "Error: Unable to open file for writing statistics." << endl;
            return;
        }

        // Write to terminal
        cout << "Statistics for the Run:" << endl;
        cout << "Number of processes: " << numProcesses << endl;
        cout << "Total elapsed time: " << time << endl;
        cout << "Throughput: " << (double)numProcesses / time << " processes/unit time" << endl;
        cout << fixed << setprecision(2);
        cout << "CPU utilization (Total Burst Time / Total Elapsed Time): " << (double)totalBurstTime / time * 100 << "%" << endl;
        cout << "Average waiting time: " << (double)totalWaitingTime / numProcesses << endl;
        cout << "Average turnaround time: " << (double)totalTurnaroundTime / numProcesses << endl;
        cout << "Average response time: " << (double)totalResponseTime / numProcesses << endl;

        // Write to file
        outputFile << "Statistics for the Run:" << endl;
        outputFile << "Number of processes: " << numProcesses << endl;
        outputFile << "Total elapsed time: " << time << endl;
        outputFile << "Throughput: " << (double)numProcesses / time << " processes/unit time" << endl;
        outputFile << fixed << setprecision(2);
        outputFile << "CPU utilization (Total Burst Time / Total Elapsed Time): " << (double)totalBurstTime / time * 100 << "%" << endl;
        outputFile << "Average waiting time: " << (double)totalWaitingTime / numProcesses << endl;
        outputFile << "Average turnaround time: " << (double)totalTurnaroundTime / numProcesses << endl;
        outputFile << "Average response time: " << (double)totalResponseTime / numProcesses << endl;

        outputFile.close();
    }
};

int main()
{
    // Create the object 'processes'
    vector<Process> processes;

    // Open file
    ifstream file("Datafile1-txt.txt");
    if (!file.is_open())
    {
        cerr << "Error: Unable to open file Datafile1-txt.txt" << endl;
        return 1;
    }

    string line;
    // Skip the first line (header)
    getline(file, line);

    // Read the first 500 processes into the object 'processes'
    int pid = 0; // Assign process IDs incrementally
    int arrivalTime, cpuBurst, priority;
    while (pid < 500 && file >> arrivalTime >> cpuBurst >> priority)
    {
        processes.emplace_back(pid++, arrivalTime, priority, cpuBurst);
    }

    // Clode file
    file.close();

    // Old debugging
    /* if (processes.size() < 500)
    {
        cerr << "Error: Less than 500 processes available in the file." << endl;
        return 1;
    } */

    // Create scheduler object
    Scheduler scheduler;

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

    // Run specified type of scheduling based on user input
    if (scheduling_type == 1)
    {
        scheduler.runFIFO(processes);
    }
    else
    {
        scheduler.runPriority(processes);
    }

    // Print completed output
    scheduler.printStatistics(processes, scheduling_type);

    return 0;
}

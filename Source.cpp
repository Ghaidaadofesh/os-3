
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int finishTime;
    int waitingTime;
    int turnaroundTime;
    int startTime;
    vector<pair<int, int>> executionTimes; // {startTime, endTime}
};

void FCFS(vector<Process> processes);
void SRT(vector<Process> processes);
void RoundRobin(vector<Process> processes, int quantum);
void displayResults(const vector<Process>& processes, const string& schedulerName);
void displayGanttChart(const vector<Process>& processes);

int main() {
    ifstream file("gh.txt");
    if (!file.is_open()) {
        cerr << "Error: Could not open the file.\n";
        return 1;
    }

    int numProcesses, quantum;
    file >> numProcesses >> quantum;

    vector<Process> processes(numProcesses);
    for (int i = 0; i < numProcesses; i++) {
        processes[i].id = i + 1;
        file >> processes[i].arrivalTime >> processes[i].burstTime;
        processes[i].remainingTime = processes[i].burstTime;
    }
    file.close();

    FCFS(processes);
    SRT(processes);
    RoundRobin(processes, quantum);

    return 0;
}

// First-Come First-Served (FCFS) scheduling
void FCFS(vector<Process> processes) {
    int currentTime = 0;
    for (auto& p : processes) {
        p.startTime = max(currentTime, p.arrivalTime);
        p.finishTime = p.startTime + p.burstTime;
        p.turnaroundTime = p.finishTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        p.executionTimes.push_back({ p.startTime, p.finishTime });
        currentTime = p.finishTime;
    }
    displayResults(processes, "FCFS");
    displayGanttChart(processes);
}

// Shortest Remaining Time (SRT) scheduling
void SRT(vector<Process> processes) {
    int currentTime = 0, completed = 0;
    Process* currentProcess = nullptr;

    while (completed < processes.size()) {
        for (auto& p : processes) {
            if (p.arrivalTime <= currentTime && p.remainingTime > 0 &&
                (!currentProcess || p.remainingTime < currentProcess->remainingTime)) {
                currentProcess = &p;
            }
        }

        if (currentProcess) {
            if (currentProcess->remainingTime == currentProcess->burstTime) {
                currentProcess->executionTimes.push_back({ currentTime, currentTime + 1 });
            }
            else {
                currentProcess->executionTimes.back().second = currentTime + 1;
            }

            currentProcess->remainingTime--;
            if (currentProcess->remainingTime == 0) {
                currentProcess->finishTime = currentTime + 1;
                currentProcess->turnaroundTime = currentProcess->finishTime - currentProcess->arrivalTime;
                currentProcess->waitingTime = currentProcess->turnaroundTime - currentProcess->burstTime;
                completed++;
                currentProcess = nullptr;
            }
            currentTime++;
        }
        else {
            currentTime++;
        }
    }
    displayResults(processes, "SRT (Preemptive SJF)");
    displayGanttChart(processes);
}

// Round-Robin (RR) scheduling
void RoundRobin(vector<Process> processes, int quantum) {
    int currentTime = 0;
    int completed = 0;
    int n = processes.size();
    queue<int> readyQueue;
    vector<bool> inQueue(n, false);

    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= currentTime && !inQueue[i] && processes[i].remainingTime > 0) {
                readyQueue.push(i);
                inQueue[i] = true;
            }
        }

        if (!readyQueue.empty()) {
            int idx = readyQueue.front();
            readyQueue.pop();

            int timeToRun = min(processes[idx].remainingTime, quantum);
            processes[idx].executionTimes.push_back({ currentTime, currentTime + timeToRun });
            processes[idx].remainingTime -= timeToRun;
            currentTime += timeToRun;

            if (processes[idx].remainingTime == 0) {
                processes[idx].finishTime = currentTime;
                processes[idx].turnaroundTime = processes[idx].finishTime - processes[idx].arrivalTime;
                processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
                completed++;
            }
            else {
                readyQueue.push(idx);
            }
        }
        else {
            currentTime++;
        }
    }

    displayResults(processes, "Round Robin");
    displayGanttChart(processes);
}

// Display results
void displayResults(const vector<Process>& processes, const string& schedulerName) {
    double totalWaitingTime = 0, totalTurnaroundTime = 0;
    cout << "\n" << schedulerName << " Scheduling Results\n";
    cout << "------------------------------------------------------\n";
    cout << "Process\tArrival\tBurst\tFinish\tWaiting\tTurnaround\n";
    for (const auto& p : processes) {
        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
        cout << "P" << p.id << "\t"
            << p.arrivalTime << "\t"
            << p.burstTime << "\t"
            << p.finishTime << "\t"
            << p.waitingTime << "\t"
            << p.turnaroundTime << "\n";
    }
    cout << "\nAverage Waiting Time: " << totalWaitingTime / processes.size();
    cout << "\nAverage Turnaround Time: " << totalTurnaroundTime / processes.size() << "\n";
    double cpuUtilization = ((processes.back().finishTime) / totalTurnaroundTime) * 100;
    cout << "CPU Utilization: " << fixed << setprecision(2) << cpuUtilization << "%\n";
}

// Display Gantt Chart
void displayGanttChart(const vector<Process>& processes) {
    cout << "\nGantt Chart:\n";
    cout << "------------------------------------------------------\n";

    // Print process execution sequence
    for (const auto& p : processes) {
        for (const auto& exec : p.executionTimes) {
            cout << "|  P" << p.id << "  ";
        }
    }
    cout << "|\n";

    // Print timeline
    for (const auto& p : processes) {
        for (const auto& exec : p.executionTimes) {
            cout << exec.first << "       ";
        }
    }
    cout << processes.back().finishTime << "\n";
    cout << "------------------------------------------------------\n";
}
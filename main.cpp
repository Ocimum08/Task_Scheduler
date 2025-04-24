#include <iostream>
#include <unordered_map>
#include <queue>
#include <stack>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
using namespace std;

struct Task
{
    int id;
    string name;
    string description;
    string deadline;
    int priority;
    bool completed;

    Task() {}
    Task(int i, string n, string d, string dl, int p)
        : id(i), name(n), description(d), deadline(dl), priority(p), completed(false) {}
};

// Global storage
unordered_map<int, Task> taskMap;
stack<int> undoStack;
int currentId = 1;

// Comparator for priority queue
struct Compare
{
    bool operator()(Task a, Task b)
    {
        return a.priority < b.priority;
    }
};

priority_queue<Task, vector<Task>, Compare> taskQueue;

void addTask()
{
    string name, desc, deadline;
    int priority;

    cin.ignore();
    cout << "Enter Task Name: ";
    getline(cin, name);
    cout << "Enter Description: ";
    getline(cin, desc);
    cout << "Enter Deadline: ";
    getline(cin, deadline);
    cout << "Enter Priority (higher = more important): ";
    cin >> priority;

    Task t(currentId, name, desc, deadline, priority);
    taskMap[currentId] = t;
    taskQueue.push(t);
    undoStack.push(currentId);
    cout << "Task added with ID #" << currentId << "\n";
    currentId++;
}

void removeTask()
{
    int id;
    cout << "Enter Task ID to remove: ";
    cin >> id;

    if (taskMap.count(id))
    {
        taskMap.erase(id);
        cout << "Task #" << id << " removed.\n";
    }
    else
    {
        cout << "Task not found.\n";
    }
}

void markCompleted()
{
    int id;
    cout << "Enter Task ID to mark as completed: ";
    cin >> id;

    if (taskMap.count(id))
    {
        taskMap[id].completed = true;
        cout << "Task marked as completed.\n";
    }
    else
    {
        cout << "Task not found.\n";
    }
}

void showTopPriority()
{
    priority_queue<Task, vector<Task>, Compare> temp = taskQueue;
    while (!temp.empty())
    {
        Task t = temp.top();
        temp.pop();
        if (taskMap.count(t.id) && !taskMap[t.id].completed)
        {
            cout << "Top Priority Task:\n";
            cout << "ID: " << t.id << "\nName: " << t.name
                 << "\nPriority: " << t.priority << "\nDeadline" << t.deadline << "\n";
            return;
        }
    }
    cout << "No pending tasks found.\n";
}

void listAllTasks()
{
    if (taskMap.empty())
    {
        cout << "No tasks found.\n";
        return;
    }
    cout << "\nAll Tasks:\n";
    for (auto &[id, t] : taskMap)
    {
        cout << "----------------------------\n";
        cout << "ID: " << id << "\nName: " << t.name
             << "\nDescription: " << t.description
             << "\nDeadline: " << t.deadline
             << "\nPriority: " << t.priority
             << "\nStatus: " << (t.completed ? "Completed" : "Pending") << "\n";
    }
}

void sortByDeadline()
{
    vector<Task> tasks;
    for (auto &[id, t] : taskMap)
        tasks.push_back(t);
    sort(tasks.begin(), tasks.end(), [](Task &a, Task &b)
         { return a.deadline < b.deadline; });

    cout << "\nTasks Sorted by Deadline:\n";
    for (Task &t : tasks)
    {
        cout << "----------------------------\n";
        cout << "ID: " << t.id << "\nName: " << t.name
             << "\nDeadline: " << t.deadline
             << "\nPriority: " << t.priority
             << "\nStatus: " << (t.completed ? "Completed" : "Pending") << "\n";
    }
}

void undoLastAdd()
{
    if (undoStack.empty())
    {
        cout << "Nothing to undo.\n";
        return;
    }
    int id = undoStack.top();
    undoStack.pop();
    if (taskMap.count(id))
    {
        taskMap.erase(id);
        cout << "Last added task (ID #" << id << ") has been undone.\n";
    }
    else
    {
        cout << "Task already removed.\n";
    }
}

void saveToFile()
{
    ofstream fout("tasks.txt");
    for (auto &[id, t] : taskMap)
    {
        fout << t.id << '\n'
             << t.name << '\n'
             << t.description << '\n'
             << t.deadline << '\n'
             << t.priority << '\n'
             << t.completed << '\n';
    }
    fout.close();

    ofstream uout("undo_stack.txt");
    stack<int> temp = undoStack;
    while (!temp.empty())
    {
        uout << temp.top() << '\n';
        temp.pop();
    }
    uout.close();
}

void loadFromFile()
{
    ifstream fin("tasks.txt");
    if (!fin)
        return;

    string name, desc, deadline;
    int id, priority;
    bool completed;

    while (fin >> id)
    {
        fin.ignore();
        getline(fin, name);
        getline(fin, desc);
        getline(fin, deadline);
        fin >> priority >> completed;
        Task t(id, name, desc, deadline, priority);
        t.completed = completed;
        taskMap[id] = t;
        taskQueue.push(t);
        currentId = max(currentId, id + 1);
    }
    fin.close();

    ifstream uin("undo_stack.txt");
    if (uin)
    {
        int val;
        while (uin >> val)
            undoStack.push(val);
        uin.close();
    }
}

void menu()
{
    int choice;
    do
    {
        cout << "\n====== Task Scheduler ======\n";
        cout << "1. Add Task\n2. Remove Task\n3. Mark as Completed\n4. Show Top Priority\n";
        cout << "5. List All Tasks\n6. Sort by Deadline\n7. Undo Last Add\n8. Exit\n";
        cout << "Choose option: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            addTask();
            break;
        case 2:
            removeTask();
            break;
        case 3:
            markCompleted();
            break;
        case 4:
            showTopPriority();
            break;
        case 5:
            listAllTasks();
            break;
        case 6:
            sortByDeadline();
            break;
        case 7:
            undoLastAdd();
            break;
        case 8:
            cout << "Saving and exiting...\n";
            break;
        default:
            cout << "Invalid option.\n";
        }
    } while (choice != 8);
}

int main()
{
    loadFromFile();
    menu();
    saveToFile();
    return 0;
}
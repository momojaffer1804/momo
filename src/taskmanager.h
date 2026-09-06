#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define MAX_TASKS 10

struct Task {
    int id;
    String text;
    bool completed;
};

class TaskManager {

public:

    TaskManager();

    void begin(const String& ip);

    bool fetchTasks();

    bool toggleTask(int taskId);

    int getTaskCount();

    Task getTask(int index);

    int getSelectedIndex();

    void selectNext();

    void selectPrevious();

private:

    String laptopIP;

    Task tasks[MAX_TASKS];

    int taskCount;

    int selectedIndex;
};

#endif
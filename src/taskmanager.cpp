#include "taskmanager.h"

TaskManager::TaskManager() {
    laptopIP = "";
    taskCount = 0;
    selectedIndex = 0;
}

void TaskManager::begin(const String& ip) {
    laptopIP = ip;
    taskCount = 0;
    selectedIndex = 0;
}

bool TaskManager::fetchTasks() {

    if (laptopIP.length() == 0)
        return false;

    HTTPClient http;

    String url =
        "http://" + laptopIP + ":5000/api/data";

    http.begin(url);
    http.setTimeout(3000);

    int httpCode = http.GET();

    if (httpCode != 200) {

        Serial.print("Task fetch failed: ");
        Serial.println(httpCode);

        http.end();
        return false;
    }

    String payload = http.getString();

    JsonDocument doc;

    DeserializationError error =
        deserializeJson(doc, payload);

    if (error) {

        Serial.print("JSON error: ");
        Serial.println(error.c_str());

        http.end();
        return false;
    }

    JsonArray taskArray =
        doc["tasks"].as<JsonArray>();

    taskCount = 0;

    for (JsonObject taskObject : taskArray) {

        if (taskCount >= MAX_TASKS)
            break;

        tasks[taskCount].id =
            taskObject["id"] | 0;

        tasks[taskCount].text =
            taskObject["text"] | "";

        tasks[taskCount].completed =
            taskObject["completed"] | false;

        taskCount++;
    }

    if (taskCount == 0) {
        selectedIndex = 0;
    }
    else if (selectedIndex >= taskCount) {
        selectedIndex = taskCount - 1;
    }

    Serial.print("Tasks loaded: ");
    Serial.println(taskCount);

    http.end();

    return true;
}

bool TaskManager::toggleTask(int taskId) {

    if (laptopIP.length() == 0)
        return false;

    HTTPClient http;

    String url =
        "http://" +
        laptopIP +
        ":5000/api/tasks/" +
        String(taskId) +
        "/toggle";

    http.begin(url);
    http.setTimeout(3000);

    int httpCode = http.POST("");

    if (httpCode != 200) {

        Serial.print("Toggle failed: ");
        Serial.println(httpCode);

        http.end();
        return false;
    }

    http.end();

    return true;
}

int TaskManager::getTaskCount() {
    return taskCount;
}

Task TaskManager::getTask(int index) {

    if (index < 0 || index >= taskCount)
        return Task{0, "", false};

    return tasks[index];
}

int TaskManager::getSelectedIndex() {
    return selectedIndex;
}

void TaskManager::selectNext() {

    if (taskCount == 0)
        return;

    selectedIndex++;

    if (selectedIndex >= taskCount)
        selectedIndex = 0;
}

void TaskManager::selectPrevious() {

    if (taskCount == 0)
        return;

    selectedIndex--;

    if (selectedIndex < 0)
        selectedIndex = taskCount - 1;
}
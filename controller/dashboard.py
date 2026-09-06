from flask import Flask, request, jsonify, render_template_string
import json
import os
from datetime import date

# =====================================================
# MoMo Task Dashboard
# =====================================================

app = Flask(__name__)

DATA_FILE = "tasks.json"


# =====================================================
# Load Tasks
# =====================================================

def load_data():

    if not os.path.exists(DATA_FILE):

        return {
            "tasks": [],
            "weekly_goals": []
        }

    try:

        with open(DATA_FILE, "r", encoding="utf-8") as file:
            return json.load(file)

    except (json.JSONDecodeError, OSError):

        return {
            "tasks": [],
            "weekly_goals": []
        }


# =====================================================
# Save Tasks
# =====================================================

def save_data(data):

    with open(DATA_FILE, "w", encoding="utf-8") as file:

        json.dump(
            data,
            file,
            indent=4,
            ensure_ascii=False
        )


# =====================================================
# Dashboard HTML
# =====================================================

HTML = """
<!DOCTYPE html>

<html>

<head>

    <meta charset="UTF-8">

    <meta name="viewport"
          content="width=device-width, initial-scale=1.0">

    <title>MoMo Dashboard</title>

    <style>

        * {
            box-sizing: border-box;
        }

        body {

            margin: 0;

            font-family:
                Arial,
                Helvetica,
                sans-serif;

            background: #111;

            color: white;

            padding: 30px;
        }

        .container {

            max-width: 900px;

            margin: auto;
        }

        h1 {

            text-align: center;

            margin-bottom: 5px;
        }

        .subtitle {

            text-align: center;

            color: #aaa;

            margin-bottom: 30px;
        }

        .card {

            background: #1c1c1c;

            border-radius: 15px;

            padding: 25px;

            margin-bottom: 25px;

            box-shadow:
                0 5px 20px rgba(0,0,0,0.3);
        }

        .section-title {

            font-size: 22px;

            margin-bottom: 20px;
        }

        .add-row {

            display: flex;

            gap: 10px;

            margin-bottom: 20px;
        }

        input {

            flex: 1;

            padding: 12px;

            border-radius: 8px;

            border: 1px solid #444;

            background: #292929;

            color: white;

            font-size: 16px;
        }

        button {

            border: none;

            border-radius: 8px;

            padding: 10px 16px;

            cursor: pointer;

            font-size: 14px;
        }

        .add {

            background: #4caf50;

            color: white;
        }

        .delete {

            background: #d9534f;

            color: white;
        }

        .task {

            display: flex;

            align-items: center;

            gap: 12px;

            padding: 14px;

            margin-bottom: 10px;

            background: #292929;

            border-radius: 10px;
        }

        .task-text {

            flex: 1;

            font-size: 17px;
        }

        .completed {

            text-decoration: line-through;

            color: #777;
        }

        .checkbox {

            width: 22px;

            height: 22px;

            cursor: pointer;
        }

        .empty {

            color: #777;

            text-align: center;

            padding: 20px;
        }

        .stats {

            display: flex;

            justify-content: space-between;

            color: #aaa;

            margin-top: 15px;
        }

        .goal {

            display: flex;

            align-items: center;

            gap: 12px;

            padding: 12px;

            background: #292929;

            border-radius: 10px;

            margin-bottom: 10px;
        }

        @media (max-width: 600px) {

            body {
                padding: 15px;
            }

            .add-row {
                flex-direction: column;
            }

        }

    </style>

</head>


<body>

<div class="container">

    <h1>🤖 MoMo Dashboard</h1>

    <div class="subtitle">
        Productivity & Task Manager
    </div>


    <!-- ================= TODAY ================= -->

    <div class="card">

        <div class="section-title">
            📋 Today's Tasks
        </div>

        <div class="add-row">

            <input
                id="taskInput"
                type="text"
                placeholder="Enter a task..."
                onkeydown="handleTaskEnter(event)"
            >

            <button
                class="add"
                onclick="addTask()"
            >
                + Add Task
            </button>

        </div>


        <div id="tasks"></div>

        <div class="stats">

            <span id="taskCount">
                0 tasks
            </span>

            <span id="progress">
                0% complete
            </span>

        </div>

    </div>


    <!-- ================= WEEKLY GOALS ================= -->

    <div class="card">

        <div class="section-title">
            🎯 Weekly Goals
        </div>

        <div class="add-row">

            <input
                id="goalInput"
                type="text"
                placeholder="Enter a weekly goal..."
                onkeydown="handleGoalEnter(event)"
            >

            <button
                class="add"
                onclick="addGoal()"
            >
                + Add Goal
            </button>

        </div>


        <div id="goals"></div>

    </div>

</div>


<script>


// =====================================================
// Load Dashboard
// =====================================================

async function loadDashboard() {

    const response = await fetch("/api/data");

    const data = await response.json();

    renderTasks(data.tasks);

    renderGoals(data.weekly_goals);
}


// =====================================================
// Render Tasks
// =====================================================

function renderTasks(tasks) {

    const container =
        document.getElementById("tasks");

    container.innerHTML = "";


    if (tasks.length === 0) {

        container.innerHTML =
            '<div class="empty">No tasks yet 👀</div>';

    }


    tasks.forEach(task => {

        const row =
            document.createElement("div");

        row.className = "task";


        const checkbox =
            document.createElement("input");

        checkbox.type = "checkbox";

        checkbox.className = "checkbox";

        checkbox.checked = task.completed;


        checkbox.onchange = function() {

            toggleTask(task.id);

        };


        const text =
            document.createElement("div");

        text.className = "task-text";


        if (task.completed) {

            text.classList.add("completed");

        }


        text.textContent = task.text;


        const deleteButton =
            document.createElement("button");

        deleteButton.className = "delete";

        deleteButton.textContent = "Delete";


        deleteButton.onclick = function() {

            deleteTask(task.id);

        };


        row.appendChild(checkbox);

        row.appendChild(text);

        row.appendChild(deleteButton);

        container.appendChild(row);

    });


    const total = tasks.length;

    const completed =
        tasks.filter(task => task.completed).length;


    document.getElementById("taskCount")
        .textContent =
        `${completed}/${total} tasks completed`;


    let percentage = 0;


    if (total > 0) {

        percentage =
            Math.round(
                (completed / total) * 100
            );

    }


    document.getElementById("progress")
        .textContent =
        `${percentage}% complete`;

}


// =====================================================
// Render Goals
// =====================================================

function renderGoals(goals) {

    const container =
        document.getElementById("goals");

    container.innerHTML = "";


    if (goals.length === 0) {

        container.innerHTML =
            '<div class="empty">No weekly goals yet 🎯</div>';

    }


    goals.forEach(goal => {

        const row =
            document.createElement("div");

        row.className = "goal";


        const checkbox =
            document.createElement("input");

        checkbox.type = "checkbox";

        checkbox.className = "checkbox";

        checkbox.checked = goal.completed;


        checkbox.onchange = function() {

            toggleGoal(goal.id);

        };


        const text =
            document.createElement("div");

        text.className = "task-text";

        text.textContent = goal.text;


        if (goal.completed) {

            text.classList.add("completed");

        }


        const deleteButton =
            document.createElement("button");

        deleteButton.className = "delete";

        deleteButton.textContent = "Delete";


        deleteButton.onclick = function() {

            deleteGoal(goal.id);

        };


        row.appendChild(checkbox);

        row.appendChild(text);

        row.appendChild(deleteButton);

        container.appendChild(row);

    });

}


// =====================================================
// Add Task
// =====================================================

async function addTask() {

    const input =
        document.getElementById("taskInput");

    const text =
        input.value.trim();


    if (!text) {

        return;

    }


    await fetch(
        "/api/tasks",
        {

            method: "POST",

            headers: {
                "Content-Type":
                    "application/json"
            },

            body: JSON.stringify({
                text: text
            })

        }
    );


    input.value = "";

    loadDashboard();

}


// =====================================================
// Toggle Task
// =====================================================

async function toggleTask(id) {

    await fetch(
        `/api/tasks/${id}/toggle`,
        {
            method: "POST"
        }
    );

    loadDashboard();

}


// =====================================================
// Delete Task
// =====================================================

async function deleteTask(id) {

    await fetch(
        `/api/tasks/${id}`,
        {
            method: "DELETE"
        }
    );

    loadDashboard();

}


// =====================================================
// Add Goal
// =====================================================

async function addGoal() {

    const input =
        document.getElementById("goalInput");

    const text =
        input.value.trim();


    if (!text) {

        return;

    }


    await fetch(
        "/api/goals",
        {

            method: "POST",

            headers: {
                "Content-Type":
                    "application/json"
            },

            body: JSON.stringify({
                text: text
            })

        }
    );


    input.value = "";

    loadDashboard();

}


// =====================================================
// Toggle Goal
// =====================================================

async function toggleGoal(id) {

    await fetch(
        `/api/goals/${id}/toggle`,
        {
            method: "POST"
        }
    );

    loadDashboard();

}


// =====================================================
// Delete Goal
// =====================================================

async function deleteGoal(id) {

    await fetch(
        `/api/goals/${id}`,
        {
            method: "DELETE"
        }
    );

    loadDashboard();

}


// =====================================================
// Enter Key
// =====================================================

function handleTaskEnter(event) {

    if (event.key === "Enter") {

        addTask();

    }

}


function handleGoalEnter(event) {

    if (event.key === "Enter") {

        addGoal();

    }

}


// =====================================================
// Start
// =====================================================

loadDashboard();


</script>


</body>

</html>
"""


# =====================================================
# Dashboard Route
# =====================================================

@app.route("/")
def dashboard():

    return render_template_string(HTML)


# =====================================================
# Get All Data
# =====================================================

@app.route("/api/data", methods=["GET"])
def get_data():

    data = load_data()

    today = str(date.today())


    # Only show today's tasks

    today_tasks = [

        task
        for task in data["tasks"]
        if task["date"] == today

    ]


    return jsonify({

        "tasks": today_tasks,

        "weekly_goals":
            data["weekly_goals"]

    })


# =====================================================
# Add Task
# =====================================================

@app.route("/api/tasks", methods=["POST"])
def add_task():

    data = load_data()

    body = request.get_json()

    text = body.get("text", "").strip()


    if not text:

        return jsonify({
            "error": "Task cannot be empty"
        }), 400


    new_id = 1


    if data["tasks"]:

        new_id = max(
            task["id"]
            for task in data["tasks"]
        ) + 1


    task = {

        "id": new_id,

        "text": text,

        "completed": False,

        "date": str(date.today())

    }


    data["tasks"].append(task)

    save_data(data)


    return jsonify(task)


# =====================================================
# Toggle Task
# =====================================================

@app.route("/api/tasks/<int:task_id>/toggle", methods=["POST"])
def toggle_task(task_id):

    data = load_data()


    for task in data["tasks"]:

        if task["id"] == task_id:

            task["completed"] = \
                not task["completed"]

            save_data(data)

            return jsonify(task)


    return jsonify({
        "error": "Task not found"
    }), 404


# =====================================================
# Delete Task
# =====================================================

@app.route("/api/tasks/<int:task_id>", methods=["DELETE"])
def delete_task(task_id):

    data = load_data()


    data["tasks"] = [

        task
        for task in data["tasks"]
        if task["id"] != task_id

    ]


    save_data(data)


    return jsonify({
        "success": True
    })


# =====================================================
# Add Weekly Goal
# =====================================================

@app.route("/api/goals", methods=["POST"])
def add_goal():

    data = load_data()

    body = request.get_json()

    text = body.get("text", "").strip()


    if not text:

        return jsonify({
            "error": "Goal cannot be empty"
        }), 400


    new_id = 1


    if data["weekly_goals"]:

        new_id = max(
            goal["id"]
            for goal in data["weekly_goals"]
        ) + 1


    goal = {

        "id": new_id,

        "text": text,

        "completed": False

    }


    data["weekly_goals"].append(goal)

    save_data(data)


    return jsonify(goal)


# =====================================================
# Toggle Goal
# =====================================================

@app.route("/api/goals/<int:goal_id>/toggle", methods=["POST"])
def toggle_goal(goal_id):

    data = load_data()


    for goal in data["weekly_goals"]:

        if goal["id"] == goal_id:

            goal["completed"] = \
                not goal["completed"]

            save_data(data)

            return jsonify(goal)


    return jsonify({
        "error": "Goal not found"
    }), 404


# =====================================================
# Delete Goal
# =====================================================

@app.route("/api/goals/<int:goal_id>", methods=["DELETE"])
def delete_goal(goal_id):

    data = load_data()


    data["weekly_goals"] = [

        goal
        for goal in data["weekly_goals"]
        if goal["id"] != goal_id

    ]


    save_data(data)


    return jsonify({
        "success": True
    })


# =====================================================
# Run Dashboard
# =====================================================

if __name__ == "__main__":

    print()
    print("====================================")
    print("       🤖 MoMo Task Dashboard")
    print("====================================")
    print()
    print("Dashboard:")
    print("http://127.0.0.1:5000")
    print()
    print("CTRL+C → Stop")
    print()

    app.run(
    host="0.0.0.0",
    port=5000,
    debug=False
)
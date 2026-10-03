#include "../headers/todolistapp.h"

#include <QDate>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QPixmap>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>

namespace {

QStringList parseTags(const QString &text) {
    QStringList result;

    for (const QString &part : text.split(',', Qt::SkipEmptyParts)) {
        const QString tag = part.trimmed();
        if (!tag.isEmpty()) {
            result.append(tag);
        }
    }

    return result;
}

QString taskText(const Task &task) {
    QString text = QString("[%1] %2")
                       .arg(priorityToString(task.getPriority()),
                            task.getDescription());

    if (!task.getDeadline().isEmpty()) {
        text += " | due: " + task.getDeadline();
    }

    if (!task.getTags().isEmpty()) {
        text += " | tags: " + task.getTags().join(", ");
    }

    return text;
}

}

ToDoListApp::ToDoListApp(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle("To-Do List App");
    resize(620, 650);

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    taskInput = new QLineEdit;
    taskInput->setPlaceholderText("Task description");

    priorityBox = new QComboBox;
    priorityBox->addItems({"Low", "Medium", "High"});
    priorityBox->setCurrentText("Medium");

    deadlineInput = new QLineEdit;
    deadlineInput->setPlaceholderText("Deadline: YYYY-MM-DD");

    tagsInput = new QLineEdit;
    tagsInput->setPlaceholderText("Tags: home, work, study");

    addButton = new QPushButton("Add Task");

    searchInput = new QLineEdit;
    searchInput->setPlaceholderText("Search");

    taskList = new QListWidget;
    deleteButton = new QPushButton("Delete Selected Task");

    saveButton = new QPushButton("Save Tasks");
    loadButton = new QPushButton("Load Tasks");
    addImageButton = new QPushButton("Add Image");
    imageLabel = new QLabel;
    imageLabel->setMinimumHeight(100);

    layout->addWidget(taskInput);
    layout->addWidget(priorityBox);
    layout->addWidget(deadlineInput);
    layout->addWidget(tagsInput);
    layout->addWidget(addButton);
    layout->addWidget(searchInput);
    layout->addWidget(taskList);
    layout->addWidget(deleteButton);
    layout->addWidget(saveButton);
    layout->addWidget(loadButton);
    layout->addWidget(addImageButton);
    layout->addWidget(imageLabel);

    setCentralWidget(central);

    connect(addButton, &QPushButton::clicked,
            this, &ToDoListApp::addTask);

    connect(taskList, &QListWidget::itemDoubleClicked,
            this, &ToDoListApp::toggleTaskComplete);

    connect(deleteButton, &QPushButton::clicked,
            this, &ToDoListApp::deleteSelectedTask);

    connect(saveButton, &QPushButton::clicked,
            this, &ToDoListApp::saveTasks);

    connect(loadButton, &QPushButton::clicked,
            this, &ToDoListApp::loadTasks);

    connect(addImageButton, &QPushButton::clicked,
            this, &ToDoListApp::addImageToTask);

    connect(searchInput, &QLineEdit::textChanged,
            this, [this]() {
                updateTaskList();
            });

    cacheFilePath = "cached_tasks.json";
    cacheTasksFromCacheFile();
}

void ToDoListApp::addTask() {
    const QString description = taskInput->text().trimmed();

    if (description.isEmpty()) {
        QMessageBox::warning(this, "Invalid task",
                             "Task description cannot be empty.");
        return;
    }

    const QString deadline = deadlineInput->text().trimmed();

    if (!deadline.isEmpty()
        && !QDate::fromString(deadline, Qt::ISODate).isValid()) {
        QMessageBox::warning(this, "Invalid deadline",
                             "Use deadline format YYYY-MM-DD.");
        return;
    }

    Task task(description);
    task.setPriority(priorityFromString(priorityBox->currentText()));
    task.setDeadline(deadline);
    task.setTags(parseTags(tagsInput->text()));

    tasks.append(task);

    taskInput->clear();
    deadlineInput->clear();
    tagsInput->clear();
    priorityBox->setCurrentText("Medium");

    updateTaskList();
    cacheTasksToFile();
}

int ToDoListApp::selectedTaskIndex() const {
    QListWidgetItem *item = taskList->currentItem();

    if (item == nullptr) {
        return -1;
    }

    return item->data(Qt::UserRole).toInt();
}

void ToDoListApp::toggleTaskComplete(QListWidgetItem *item) {
    if (item == nullptr) {
        return;
    }

    const int index = item->data(Qt::UserRole).toInt();

    if (index < 0 || index >= tasks.size()) {
        return;
    }

    tasks[index].toggleComplete();
    updateTaskList();
    cacheTasksToFile();
}

void ToDoListApp::deleteSelectedTask() {
    const int index = selectedTaskIndex();

    if (index < 0 || index >= tasks.size()) {
        QMessageBox::information(this, "Delete task",
                                 "Select a task first.");
        return;
    }

    tasks.removeAt(index);
    imageLabel->clear();

    updateTaskList();
    cacheTasksToFile();
}

void ToDoListApp::addImageToTask() {
    const int index = selectedTaskIndex();

    if (index < 0 || index >= tasks.size()) {
        QMessageBox::information(this, "Add image",
                                 "Select a task first.");
        return;
    }

    const QString imagePath = QFileDialog::getOpenFileName(
        this,
        "Select Image",
        "",
        "Images (*.png *.jpg *.jpeg)"
    );

    if (imagePath.isEmpty()) {
        return;
    }

    tasks[index].setImagePath(imagePath);

    QPixmap image(imagePath);
    imageLabel->setPixmap(
        image.scaledToHeight(100, Qt::SmoothTransformation)
    );

    cacheTasksToFile();
}

void ToDoListApp::updateTaskList() {
    taskList->clear();

    const QString query = searchInput->text().trimmed();

    for (int i = 0; i < tasks.size(); ++i) {
        const Task &task = tasks[i];

        const QString searchable =
            task.getDescription() + " "
            + priorityToString(task.getPriority()) + " "
            + task.getDeadline() + " "
            + task.getTags().join(" ");

        if (!query.isEmpty()
            && !searchable.contains(query, Qt::CaseInsensitive)) {
            continue;
        }

        QListWidgetItem *item =
            new QListWidgetItem(taskText(task));

        item->setData(Qt::UserRole, i);

        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(
            task.isCompleted() ? Qt::Checked : Qt::Unchecked
        );

        taskList->addItem(item);
    }
}

void ToDoListApp::saveTasks() {
    QFile file("tasks.txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error",
                             "Could not save tasks to file.");
        return;
    }

    QTextStream stream(&file);

    for (const Task &task : tasks) {
        stream << task.getDescription() << "\t"
               << (task.isCompleted() ? "1" : "0") << "\t"
               << task.getImagePath() << "\t"
               << priorityToString(task.getPriority()) << "\t"
               << task.getDeadline() << "\t"
               << task.getTags().join(",") << "\n";
    }

    file.close();

    QMessageBox::information(this, "Tasks Saved",
                             "Tasks saved to tasks.txt");

    cacheTasksToFile();
}

void ToDoListApp::loadTasks() {
    QFile file("tasks.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error",
                             "Could not load tasks from file.");
        return;
    }

    tasks.clear();
    QTextStream stream(&file);

    while (!stream.atEnd()) {
        const QString line = stream.readLine();
        const QStringList parts = line.split('\t');

        if (parts.size() < 3) {
            continue;
        }

        Task task(parts[0], parts[1] == "1");
        task.setImagePath(parts[2]);

        if (parts.size() >= 4) {
            task.setPriority(priorityFromString(parts[3]));
        }

        if (parts.size() >= 5) {
            task.setDeadline(parts[4]);
        }

        if (parts.size() >= 6) {
            task.setTags(parseTags(parts[5]));
        }

        tasks.append(task);
    }

    file.close();

    updateTaskList();

    QMessageBox::information(this, "Tasks Loaded",
                             "Tasks loaded from tasks.txt");

    cacheTasksToFile();
}

void ToDoListApp::cacheTasksToFile() {
    QFile cacheFile(cacheFilePath);

    if (!cacheFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QJsonArray tasksArray;

    for (const Task &task : tasks) {
        QJsonObject object;

        object["description"] = task.getDescription();
        object["completed"] = task.isCompleted();
        object["imagePath"] = task.getImagePath();
        object["priority"] = priorityToString(task.getPriority());
        object["deadline"] = task.getDeadline();

        QJsonArray tagsArray;
        for (const QString &tag : task.getTags()) {
            tagsArray.append(tag);
        }

        object["tags"] = tagsArray;
        tasksArray.append(object);
    }

    const QJsonDocument document(tasksArray);
    cacheFile.write(document.toJson(QJsonDocument::Indented));
    cacheFile.close();
}

void ToDoListApp::cacheTasksFromCacheFile() {
    QFile cacheFile(cacheFilePath);

    if (!cacheFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        updateTaskList();
        return;
    }

    const QByteArray data = cacheFile.readAll();
    cacheFile.close();

    const QJsonDocument document = QJsonDocument::fromJson(data);

    if (!document.isArray()) {
        updateTaskList();
        return;
    }

    tasks.clear();

    for (const QJsonValue &value : document.array()) {
        if (!value.isObject()) {
            continue;
        }

        const QJsonObject object = value.toObject();

        Task task(
            object["description"].toString(),
            object["completed"].toBool()
        );

        task.setImagePath(object["imagePath"].toString());
        task.setPriority(
            priorityFromString(object["priority"].toString("Medium"))
        );
        task.setDeadline(object["deadline"].toString());

        QStringList tags;
        for (const QJsonValue &tagValue : object["tags"].toArray()) {
            tags.append(tagValue.toString());
        }
        task.setTags(tags);

        tasks.append(task);
    }

    updateTaskList();
}

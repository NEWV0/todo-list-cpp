#pragma once

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <QVector>

#include "task.h"

class ToDoListApp : public QMainWindow {
    Q_OBJECT

public:
    explicit ToDoListApp(QWidget *parent = nullptr);

private slots:
    void addTask();
    void toggleTaskComplete(QListWidgetItem *item);
    void saveTasks();
    void loadTasks();
    void addImageToTask();
    void deleteSelectedTask();

private:
    QLineEdit *taskInput = nullptr;
    QComboBox *priorityBox = nullptr;
    QLineEdit *deadlineInput = nullptr;
    QLineEdit *tagsInput = nullptr;
    QLineEdit *searchInput = nullptr;

    QPushButton *addButton = nullptr;
    QListWidget *taskList = nullptr;
    QPushButton *deleteButton = nullptr;
    QPushButton *saveButton = nullptr;
    QPushButton *loadButton = nullptr;
    QPushButton *addImageButton = nullptr;
    QLabel *imageLabel = nullptr;

    QVector<Task> tasks;
    QString cacheFilePath;

    void updateTaskList();
    void cacheTasksToFile();
    void cacheTasksFromCacheFile();
    int selectedTaskIndex() const;
};

#include "../headers/task.h"

QString priorityToString(Priority priority) {
    switch (priority) {
        case Priority::Low:
            return "Low";
        case Priority::High:
            return "High";
        case Priority::Medium:
        default:
            return "Medium";
    }
}

Priority priorityFromString(const QString &value) {
    const QString normalized = value.trimmed().toLower();

    if (normalized == "low") {
        return Priority::Low;
    }

    if (normalized == "high") {
        return Priority::High;
    }

    return Priority::Medium;
}

Task::Task(const QString &description, bool completed)
    : description(description),
      completed(completed) {
}

QString Task::getDescription() const {
    return description;
}

bool Task::isCompleted() const {
    return completed;
}

void Task::toggleComplete() {
    completed = !completed;
}

QString Task::getImagePath() const {
    return imagePath;
}

void Task::setImagePath(const QString &path) {
    imagePath = path;
}

Priority Task::getPriority() const {
    return priority;
}

void Task::setPriority(Priority value) {
    priority = value;
}

QString Task::getDeadline() const {
    return deadline;
}

void Task::setDeadline(const QString &value) {
    deadline = value;
}

QStringList Task::getTags() const {
    return tags;
}

void Task::setTags(const QStringList &value) {
    tags = value;
}

#pragma once

#include <QString>
#include <QStringList>

enum class Priority {
    Low,
    Medium,
    High
};

QString priorityToString(Priority priority);
Priority priorityFromString(const QString &value);

class Task {
public:
    explicit Task(const QString &description, bool completed = false);

    QString getDescription() const;
    bool isCompleted() const;
    void toggleComplete();

    QString getImagePath() const;
    void setImagePath(const QString &path);

    Priority getPriority() const;
    void setPriority(Priority priority);

    QString getDeadline() const;
    void setDeadline(const QString &deadline);

    QStringList getTags() const;
    void setTags(const QStringList &tags);

private:
    QString description;
    bool completed;
    QString imagePath;

    Priority priority = Priority::Medium;
    QString deadline;
    QStringList tags;
};

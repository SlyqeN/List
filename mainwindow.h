#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QTableWidgetItem>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QCheckBox>
#include <QWidgetAction>
#include <QMenu>
#include <QComboBox>
#include <QStringList>
#include <QSet>
#include <QRegularExpression>

// Класс для сортировки числовых значений (курс, стипендия)
class NumericTableWidgetItem : public QTableWidgetItem {
public:
    bool operator<(const QTableWidgetItem &other) const override {
        return this->data(Qt::UserRole).toInt() < other.data(Qt::UserRole).toInt();
    }
};

// Класс для сортировки групп (число + текст)
class GroupTableWidgetItem : public QTableWidgetItem {
public:
    bool operator<(const QTableWidgetItem &other) const override {
        QString thisText = text().toLower();
        QString otherText = other.text().toLower();

        QRegularExpression re("^(\\d+)([а-яa-z]*)$");
        auto thisMatch = re.match(thisText);
        auto otherMatch = re.match(otherText);

        if (thisMatch.hasMatch() && otherMatch.hasMatch()) {
            int thisNum = thisMatch.captured(1).toInt();
            int otherNum = otherMatch.captured(1).toInt();

            if (thisNum != otherNum) {
                return thisNum < otherNum;
            }
            return thisMatch.captured(2) < otherMatch.captured(2);
        }
        return QTableWidgetItem::operator<(other);
    }
};

class AddStudentDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddStudentDialog(QWidget *parent = nullptr);
    QStringList getData() const;

private slots:
    void onDebtChanged(int index);

private:
    QLineEdit *editGroup;
    QComboBox *comboCourse;
    QLineEdit *editSurname;
    QLineEdit *editName;
    QLineEdit *editPatronymic;
    QLineEdit *editScholarship;
    QLineEdit *editMonitor;
    QLineEdit *editCurator;
    QComboBox *comboDebt;
    QLineEdit *editFaculty;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCourseHeaderClicked(int logicalIndex);
    void updateCourseFilter();
    void onLoadClicked();
    void onAddClicked();
    void onDeleteClicked();
    void onSaveClicked();
    void onHeaderClicked(int logicalIndex);
    void onQueriesClicked();
    void onChartsClicked();

private:
    Ui::MainWindow *ui;
    QMenu *courseMenu;
    QList<QCheckBox*> courseCheckboxes;
    QSet<int> selectedCourses;
    QString currentFile;
    Qt::SortOrder sortOrderGroup = Qt::AscendingOrder;
    Qt::SortOrder sortOrderSurname = Qt::AscendingOrder;
    Qt::SortOrder sortOrderDebt = Qt::AscendingOrder;
    Qt::SortOrder sortOrderFaculty = Qt::AscendingOrder;

    void setupCourseFilter();
    void filterTable();
    void loadData(const QString &filename);
    void saveData(const QString &filename);
    void addStudent(const QStringList &data);
    bool validateData(const QStringList &data);
    void updateHeaderArrow(int column, Qt::SortOrder order);
};

#endif // MAINWINDOW_H

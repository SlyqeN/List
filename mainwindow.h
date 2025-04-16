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


// Класс для сортировки числовых значений в таблице
class NumericTableWidgetItem : public QTableWidgetItem {
public:
    bool operator<(const QTableWidgetItem &other) const override {
        return text().toInt() < other.text().toInt();
    }
};

// Класс для сортировки групп (число + текст)
class GroupTableWidgetItem : public QTableWidgetItem {
public:
    bool operator<(const QTableWidgetItem &other) const override {
        QString thisText = text().toLower();
        QString otherText = other.text().toLower();
        QRegularExpression re("(\\d*)(\\D*)");
        auto thisMatch = re.match(thisText);
        auto otherMatch = re.match(otherText);
        int thisNum = thisMatch.captured(1).toInt();
        int otherNum = otherMatch.captured(1).toInt();
        if (thisNum != otherNum) {
            return thisNum < otherNum;
        }
        return thisMatch.captured(2) < otherMatch.captured(2);
    }
};

// Диалоговое окно для добавления студента
class AddStudentDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddStudentDialog(QWidget *parent = nullptr);
    QStringList getData() const;

private:
    QLineEdit *editGroup, *editSurname, *editName, *editPatronymic, *editScholarship, *editMonitor, *editCurator, *editDebt, *editFaculty;
    QComboBox *comboCourse;
};

// Главное окно приложения
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCourseHeaderClicked(int logicalIndex); // Обработка кликов по заголовкам таблицы
    void updateCourseFilter(); // Фильтрация по курсам
    void onLoadClicked(); // Загрузка данных из файла
    void onAddClicked(); // Добавление студента
    void onDeleteClicked(); // Удаление студента
    void onSaveClicked(); // Сохранение данных в файл
    void onHeaderClicked(int logicalIndex); // Сортировка таблицы
    void onQueriesClicked(); // Открытие окна запросов
    void onChartsClicked(); // Открытие окна диаграмм

private:
    Ui::MainWindow *ui; // UI-форма
    QMenu *courseMenu; // Меню фильтрации по курсам
    QList<QCheckBox*> courseCheckboxes; // Чекбоксы для фильтрации
    QSet<int> selectedCourses; // Выбранные курсы
    QString currentFile; // Текущий файл данных
    Qt::SortOrder sortOrderGroup = Qt::AscendingOrder; // Порядок сортировки групп
    Qt::SortOrder sortOrderSurname = Qt::AscendingOrder; // Порядок сортировки фамилий
    Qt::SortOrder sortOrderDebt = Qt::AscendingOrder; // Порядок сортировки задолженностей
    Qt::SortOrder sortOrderFaculty = Qt::AscendingOrder; // Порядок сортировки факультетов

    void setupCourseFilter(); // Настройка фильтрации по курсам
    void filterTable(); // Применение фильтрации
    void loadData(const QString &filename); // Загрузка данных
    void saveData(const QString &filename); // Сохранение данных
    void addStudent(const QStringList &data); // Добавление студента в таблицу
    bool validateData(const QStringList &data); // Валидация данных
    void updateHeaderArrow(int column, Qt::SortOrder order); // Обновление стрелок сортировки
};

#endif // MAINWINDOW_H

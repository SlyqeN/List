#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QTextStream>
#include "queryresults.h" // Подключаем реализацию запросов
#include "chartwindow.h"  // Подключаем реализацию диаграмм

// Конструктор диалога добавления студента
AddStudentDialog::AddStudentDialog(QWidget *parent) : QDialog(parent) {
    QFormLayout *layout = new QFormLayout(this);

    editGroup = new QLineEdit(this);
    comboCourse = new QComboBox(this);
    for (int i = 1; i <= 6; ++i) {  // Курсы 1-6
        comboCourse->addItem(QString::number(i));
    }
    editSurname = new QLineEdit(this);
    editName = new QLineEdit(this);
    editPatronymic = new QLineEdit(this);
    editScholarship = new QLineEdit(this);
    editMonitor = new QLineEdit(this);
    editCurator = new QLineEdit(this);
    editDebt = new QLineEdit(this);
    editFaculty = new QLineEdit(this);

    layout->addRow("Группа:", editGroup);
    layout->addRow("Курс:", comboCourse);
    layout->addRow("Фамилия:", editSurname);
    layout->addRow("Имя:", editName);
    layout->addRow("Отчество:", editPatronymic);
    layout->addRow("Стипендия:", editScholarship);
    layout->addRow("Староста:", editMonitor);
    layout->addRow("Куратор:", editCurator);
    layout->addRow("Задолженность:", editDebt);
    layout->addRow("Факультет:", editFaculty);

    QPushButton *btnOk = new QPushButton("Добавить", this);
    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    layout->addRow(btnOk);
}

// Получение данных из диалога добавления студента
QStringList AddStudentDialog::getData() const {
    return {
        editGroup->text(),
        comboCourse->currentText(),
        editSurname->text(),
        editName->text(),
        editPatronymic->text(),
        editScholarship->text(),
        editMonitor->text(),
        editCurator->text(),
        editDebt->text(),
        editFaculty->text()
    };
}

// Конструктор главного окна
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Настройка таблицы
    QStringList headers = {"Группа", "Курс •", "Фамилия", "Имя", "Отчество",
                           "Стипендия", "Староста", "Куратор", "Задолженность", "Факультет"};
    ui->tableWidget->setColumnCount(headers.size());
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSortingEnabled(true);

    // Подключение сигналов
    connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MainWindow::onHeaderClicked);
    connect(ui->btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(ui->btnSave, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(ui->btnQueries, &QPushButton::clicked, this, &MainWindow::onQueriesClicked);
    connect(ui->btnCharts, &QPushButton::clicked, this, &MainWindow::onChartsClicked);

    setupCourseFilter();
}

// Обработка кликов по заголовкам таблицы
void MainWindow::onHeaderClicked(int logicalIndex) {
    ui->tableWidget->setSortingEnabled(false);
    if (logicalIndex == 0) {
        sortOrderGroup = (sortOrderGroup == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
        ui->tableWidget->sortByColumn(0, sortOrderGroup);
        updateHeaderArrow(0, sortOrderGroup);
    } else if (logicalIndex == 2) {
        sortOrderSurname = (sortOrderSurname == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
        ui->tableWidget->sortByColumn(2, sortOrderSurname);
        updateHeaderArrow(2, sortOrderSurname);
    } else if (logicalIndex == 8) { // Сортировка по задолженностям
        sortOrderDebt = (sortOrderDebt == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
        ui->tableWidget->sortByColumn(8, sortOrderDebt);
        updateHeaderArrow(8, sortOrderDebt);
    } else if (logicalIndex == 9) { // Сортировка по факультету
        sortOrderFaculty = (sortOrderFaculty == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
        ui->tableWidget->sortByColumn(9, sortOrderFaculty);
        updateHeaderArrow(9, sortOrderFaculty);
    } else {
        ui->tableWidget->setSortingEnabled(false);
    }
}

// Обновление стрелок сортировки
void MainWindow::updateHeaderArrow(int column, Qt::SortOrder order) {
    QString arrow = (order == Qt::AscendingOrder) ? "↑" : "↓";
    QString headerText = ui->tableWidget->horizontalHeaderItem(column)->text().split(" ")[0];
    ui->tableWidget->horizontalHeaderItem(column)->setText(headerText + " " + arrow);
}

// Настройка фильтрации по курсам
void MainWindow::setupCourseFilter() {
    courseMenu = new QMenu(this);
    for (int i = 1; i <= 6; ++i) {
        QCheckBox *checkBox = new QCheckBox(QString("Курс %1").arg(i), courseMenu);
        checkBox->setChecked(true);
        connect(checkBox, &QCheckBox::stateChanged, this, &MainWindow::updateCourseFilter);
        QWidgetAction *action = new QWidgetAction(courseMenu);
        action->setDefaultWidget(checkBox);
        courseMenu->addAction(action);
        courseCheckboxes.append(checkBox);
        selectedCourses.insert(i);
    }
    connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MainWindow::onCourseHeaderClicked);
}

// Обработка кликов по заголовку "Курс"
void MainWindow::onCourseHeaderClicked(int logicalIndex) {
    if (logicalIndex == 1) {
        QPoint pos = ui->tableWidget->horizontalHeader()->mapToGlobal(
            QPoint(ui->tableWidget->horizontalHeader()->sectionViewportPosition(1), 0));
        courseMenu->exec(pos);
    }
}

// Обновление фильтрации по курсам
void MainWindow::updateCourseFilter() {
    selectedCourses.clear();
    for (QCheckBox *cb : courseCheckboxes) {
        if (cb->isChecked()) selectedCourses.insert(cb->text().split(" ").last().toInt());
    }
    filterTable();
}

// Применение фильтрации
void MainWindow::filterTable() {
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        bool visible = selectedCourses.contains(ui->tableWidget->item(row, 1)->text().toInt());
        ui->tableWidget->setRowHidden(row, !visible);
    }
}

// Загрузка данных из файла
void MainWindow::onLoadClicked() {
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Текстовые файлы (*.txt)");
    if (!filename.isEmpty()) loadData(filename);
}

// Загрузка данных
void MainWindow::loadData(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл");
        return;
    }
    ui->tableWidget->setRowCount(0);
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList data = line.split(" ");
        if (data.size() == 10 && validateData(data)) addStudent(data);
    }
    file.close();
    currentFile = filename;
}

// Добавление студента через диалог
void MainWindow::onAddClicked() {
    AddStudentDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList data = dialog.getData();
        if (validateData(data)) {
            addStudent(data);
        } else {
            QMessageBox::critical(this, "Ошибка", "Некорректные данные!");
        }
    }
}

// Валидация данных
bool MainWindow::validateData(const QStringList &data) {
    bool ok;
    data[1].toInt(&ok); // Курс
    if (!ok || data[1].toInt() < 1 || data[1].toInt() > 6) return false;
    data[5].toInt(&ok); // Стипендия
    if (!ok) return false;
    return !data[9].isEmpty(); // Факультет не должен быть пустым
}

// Добавление студента в таблицу
void MainWindow::addStudent(const QStringList &data) {
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    for (int i = 0; i < data.size(); ++i) {
        QTableWidgetItem *item;
        if (i == 0) { // Группа
            item = new GroupTableWidgetItem();
            item->setText(data[i]);
        } else if (i == 8 || i == 9) { // Задолженность или Факультет
            item = new QTableWidgetItem(data[i]);
            item->setTextAlignment(Qt::AlignCenter);
        } else {
            item = new QTableWidgetItem(data[i]);
        }
        if (i == 1 || i == 5 || i == 8 || i == 9) { // Редактируемые столбцы
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        } else {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
        ui->tableWidget->setItem(row, i, item);
    }
}

// Удаление студента
void MainWindow::onDeleteClicked() {
    int row = ui->tableWidget->currentRow();
    if (row >= 0) ui->tableWidget->removeRow(row);
}

// Сохранение данных в файл
void MainWindow::onSaveClicked() {
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Сохранить файл",
        "",
        "Текстовые файлы (*.txt)"
        );
    if (!filename.isEmpty()) saveData(filename);
}

// Сохранение данных
void MainWindow::saveData(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл");
        return;
    }
    QTextStream out(&file);
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QStringList data;
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            data << ui->tableWidget->item(row, col)->text();
        }
        out << data.join(" ") << "\n";
    }
    file.close();
}

// Открытие окна запросов
void MainWindow::onQueriesClicked() {
    QueryResults queryWindow(this);
    queryWindow.setData(ui->tableWidget);
    queryWindow.exec();
}

// Открытие окна диаграмм
void MainWindow::onChartsClicked() {
    ChartWindow chartWindow(this);
    chartWindow.setData(ui->tableWidget);
    chartWindow.exec();
}

// Деструктор
MainWindow::~MainWindow() {
    delete ui;
}

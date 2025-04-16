#include "chartwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMap>
#include <QStringList>

using namespace QtCharts;

ChartWindow::ChartWindow(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Диаграммы");
    dataTable = nullptr;

    // Создание 3 отдельных QChartView
    chartViewPieCourse = new QChartView(this);
    chartViewPieCourse->setRenderHint(QPainter::Antialiasing);

    chartViewPieThird = new QChartView(this);
    chartViewPieThird->setRenderHint(QPainter::Antialiasing);

    chartViewBarScholarship = new QChartView(this);
    chartViewBarScholarship->setRenderHint(QPainter::Antialiasing);

    // Layout для размещения диаграмм рядом
    QHBoxLayout *chartsLayout = new QHBoxLayout();
    chartsLayout->addWidget(chartViewPieCourse);
    chartsLayout->addWidget(chartViewPieThird);
    chartsLayout->addWidget(chartViewBarScholarship);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(chartsLayout);
    setLayout(mainLayout);
}

void ChartWindow::setData(QTableWidget *table) {
    dataTable = table;
    createPieChartCourseDistribution();
    createPieChartThirdCoursePercentage();
    createBarChartScholarship();
}

void ChartWindow::createPieChartCourseDistribution() {
    QMap<int, int> courseCounts;
    int totalStudents = 0;

    for (int row = 0; row < dataTable->rowCount(); ++row) {
        int course = dataTable->item(row, 1)->text().toInt();
        courseCounts[course]++;
        totalStudents++;
    }

    auto *series = new QPieSeries();
    for (auto it = courseCounts.begin(); it != courseCounts.end(); ++it) {
        double percent = double(it.value()) / totalStudents * 100;
        auto slice = series->append(QString("Курс %1 (%2%)").arg(it.key()).arg(percent, 0, 'f', 2), it.value());
        slice->setLabelVisible(true);
        slice->setLabelPosition(QPieSlice::LabelOutside);

        // Устанавливаем цвет для каждого сектора
        switch (it.key()) {
        case 1:
            slice->setBrush(QColor(255, 99, 71));  // Цвет для 1 курса
            break;
        case 2:
            slice->setBrush(QColor(100, 149, 237));  // Цвет для 2 курса
            break;
        case 3:
            slice->setBrush(QColor(34, 139, 34));  // Цвет для 3 курса
            break;
        // Добавьте другие курсы по необходимости
        default:
            slice->setBrush(QColor(211, 211, 211));  // Цвет для остальных
            break;
        }
    }

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Распределение студентов по курсам");
    chart->legend()->setVisible(false); // Легенду можно убрать, так как всё видно на выносных подписях

    chartViewPieCourse->setChart(chart);
}

void ChartWindow::createPieChartThirdCoursePercentage() {
    int thirdCourseCount = 0;
    int totalStudents = 0;

    for (int row = 0; row < dataTable->rowCount(); ++row) {
        int course = dataTable->item(row, 1)->text().toInt();
        if (course == 3)
            thirdCourseCount++;
        totalStudents++;
    }

    double percent = (double(thirdCourseCount) / totalStudents) * 100;

    auto *series = new QPieSeries();
    auto slice1 = series->append(QString("3 курс (%1%)").arg(percent, 0, 'f', 2), thirdCourseCount);
    auto slice2 = series->append(QString("Остальные (%1%)").arg(100 - percent, 0, 'f', 2), totalStudents - thirdCourseCount);

    // Устанавливаем цвет для секторов
    slice1->setBrush(QColor(34, 139, 34));  // Цвет для 3 курса
    slice2->setBrush(QColor(211, 211, 211));  // Цвет для остальных студентов

    for (auto slice : series->slices()) {
        slice->setLabelVisible(true);
        slice->setLabelPosition(QPieSlice::LabelOutside);
    }

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Процент студентов 3 курса");
    chart->legend()->setVisible(false);

    chartViewPieThird->setChart(chart);
}

void ChartWindow::createBarChartScholarship() {
    QMap<int, double> courseScholarships;

    for (int row = 0; row < dataTable->rowCount(); ++row) {
        int course = dataTable->item(row, 1)->text().toInt();
        bool ok;
        double scholarship = dataTable->item(row, 5)->text().toDouble(&ok);
        if (ok) courseScholarships[course] += scholarship;
    }

    auto *set = new QBarSet("Стипендия");
    QStringList categories;
    for (auto it = courseScholarships.begin(); it != courseScholarships.end(); ++it) {
        categories << QString("Курс %1").arg(it.key());
        *set << it.value();
    }

    auto *series = new QBarSeries();
    series->append(set);

    // Устанавливаем цвет для столбцов
    set->setColor(QColor(255, 99, 71));  // Цвет для столбцов (сумма стипендий)

    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);

    auto *axisY = new QValueAxis();
    axisY->setTitleText("Сумма стипендии");

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    chart->setTitle("Суммарные стипендии по курсам");
    chart->legend()->setAlignment(Qt::AlignBottom);

    chartViewBarScholarship->setChart(chart);
}

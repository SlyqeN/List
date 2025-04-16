#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts>

using namespace QtCharts;


class ChartWindow : public QDialog {
    Q_OBJECT
public:
    explicit ChartWindow(QWidget *parent = nullptr);
    void setData(QTableWidget *table);

private:
    QTableWidget *dataTable;

    QChartView *chartViewPieCourse;
    QChartView *chartViewPieThird;
    QChartView *chartViewBarScholarship;

    void createPieChartCourseDistribution();
    void createPieChartThirdCoursePercentage();
    void createBarChartScholarship();
};

#endif // CHARTWINDOW_H

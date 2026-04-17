#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "graph.h"
#include "graphwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadFileClicked();
    void onAddEdgeClicked();
    void onClearClicked();
    void onFindPathClicked();
    void onSaveSolutionClicked();
    void onResetViewClicked();

private:
    Ui::MainWindow *ui;
    Graph graph;
    GraphWidget* graphWidget;
    QVector<int> lastFoundPath;
    int lastPathCost;

    void applyPinkStyle();
    void updateGraphDisplay();
    bool validateInputs();
    QString generateSolutionReport();
    void updateNodeCountLabel();
};

#endif // MAINWINDOW_H

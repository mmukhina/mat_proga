#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSplitter>
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

    void setupUI();
    void applyPinkStyle();
    void updateGraphDisplay();
    bool validateInputs();
    QString generateSolutionReport();
    void updateNodeCountLabel();

    QTableWidget* edgesTable;
    QLineEdit* fromEdit;
    QLineEdit* toEdit;
    QLineEdit* weightEdit;
    QLineEdit* startNodeEdit;
    QLineEdit* endNodeEdit;
    QPushButton* addEdgeBtn;
    QPushButton* clearBtn;
    QPushButton* loadFileBtn;
    QPushButton* findPathBtn;
    QPushButton* saveSolutionBtn;
    QPushButton* resetViewBtn;
    QLabel* resultLabel;
    QLabel* nodeCountLabel;
};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , graphWidget(nullptr)
    , lastPathCost(-1)
{
    ui->setupUi(this);

    // Создаём виджет графа и вставляем в контейнер
    graphWidget = new GraphWidget(this);
    QVBoxLayout* containerLayout = qobject_cast<QVBoxLayout*>(ui->graphContainer->layout());
    if (!containerLayout) {
        containerLayout = new QVBoxLayout(ui->graphContainer);
        ui->graphContainer->setLayout(containerLayout);
    }
    containerLayout->addWidget(graphWidget);

    // Подключаем сигналы из UI
    connect(ui->loadFileBtn, &QPushButton::clicked, this, &MainWindow::onLoadFileClicked);
    connect(ui->addEdgeBtn, &QPushButton::clicked, this, &MainWindow::onAddEdgeClicked);
    connect(ui->clearBtn, &QPushButton::clicked, this, &MainWindow::onClearClicked);
    connect(ui->findPathBtn, &QPushButton::clicked, this, &MainWindow::onFindPathClicked);
    connect(ui->saveSolutionBtn, &QPushButton::clicked, this, &MainWindow::onSaveSolutionClicked);
    connect(ui->resetViewBtn, &QPushButton::clicked, this, &MainWindow::onResetViewClicked);
    connect(ui->deleteNodeBtn, &QPushButton::clicked, this, &MainWindow::onDeleteNodeClicked);

    // Настройка таблицы
    ui->edgesTable->setColumnCount(3);
    ui->edgesTable->setHorizontalHeaderLabels({"От", "До", "Вес"});
    ui->edgesTable->horizontalHeader()->setStretchLastSection(true);
    ui->edgesTable->setAlternatingRowColors(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLoadFileClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл с графом", "", "Text files (*.txt)");
    if (filename.isEmpty()) return;

    if (graph.loadFromFile(filename)) {
        updateGraphDisplay();
        graphWidget->setGraph(&graph);
        graphWidget->clearHighlightedPath();
        updateNodeCountLabel();
        QMessageBox::information(this, "Успех", "Граф успешно загружен из файла");
    } else {
        QMessageBox::critical(this, "Ошибка", graph.getLastError());
    }
}

void MainWindow::onAddEdgeClicked()
{
    bool ok1, ok2, ok3;
    int from = ui->fromEdit->text().toInt(&ok1);
    int to = ui->toEdit->text().toInt(&ok2);
    int weight = ui->weightEdit->text().toInt(&ok3);

    if (!ok1 || !ok2 || !ok3) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, введите корректные числа");
        return;
    }

    if (from <= 0 || to <= 0) {
        QMessageBox::warning(this, "Ошибка", "Номера узлов должны быть положительными");
        return;
    }

    if (from == to) {
        QMessageBox::warning(this, "Ошибка", "Нельзя создать ребро из узла в самого себя (петля)");
        return;
    }

    if (weight < 1) {
        QMessageBox::warning(this, "Ошибка", "Вес ребра должен быть >= 1");
        return;
    }

    // Проверка на существование такого же ребра
    const auto& edges = graph.getEdges(from);
    for (const auto& edge : edges) {
        if (edge.first == to) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Ребро %1 -> %2 уже существует. Удалите его перед добавлением.").arg(from).arg(to));
            return;
        }
    }

    if (graph.addEdge(from, to, weight)) {
        updateGraphDisplay();
        graphWidget->setGraph(&graph);
        graphWidget->clearHighlightedPath();
        updateNodeCountLabel();

        ui->fromEdit->clear();
        ui->toEdit->clear();
        ui->weightEdit->clear();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось добавить ребро");
    }
}

void MainWindow::onClearClicked()
{
    graph.clear();
    ui->edgesTable->setRowCount(0);
    ui->resultLabel->setText("Ожидание поиска...");
    graphWidget->setGraph(nullptr);
    graphWidget->clearHighlightedPath();
    updateNodeCountLabel();
    lastFoundPath.clear();
    lastPathCost = -1;
    QMessageBox::information(this, "Очистка", "Граф успешно очищен");
}

void MainWindow::onFindPathClicked()
{
    if (!validateInputs()) return;

    int start = ui->startNodeEdit->text().toInt();
    int end = ui->endNodeEdit->text().toInt();

    auto result = graph.findShortestPath(start, end);
    lastFoundPath = result.first;
    lastPathCost = result.second;

    if (lastPathCost == -1) {
        ui->resultLabel->setText("Ошибка: " + graph.getLastError());
        graphWidget->clearHighlightedPath();
        QMessageBox::warning(this, "Ошибка", graph.getLastError());
    } else {
        QString pathStr;
        for (int i = 0; i < lastFoundPath.size(); ++i) {
            if (i > 0) pathStr += " -> ";
            pathStr += QString::number(lastFoundPath[i]);
        }

        QString resultText = QString("КРАТЧАЙШИЙ ПУТЬ НАЙДЕН!\n\n"
                                     "Путь: %1\n\n"
                                     "Общая стоимость: %2\n\n"
                                     "Количество шагов: %3")
                                 .arg(pathStr)
                                 .arg(lastPathCost)
                                 .arg(lastFoundPath.size() - 1);
        ui->resultLabel->setText(resultText);
        graphWidget->setHighlightedPath(lastFoundPath);

        QMessageBox::information(this, "Успех",
                                 QString("Кратчайший путь найден!\nСтоимость: %1").arg(lastPathCost));
    }
}

void MainWindow::onSaveSolutionClicked()
{
    if (lastFoundPath.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Сначала найдите путь!");
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Сохранить решение", "", "Text files (*.txt)");
    if (filename.isEmpty()) return;

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << generateSolutionReport();
        file.close();
        QMessageBox::information(this, "Успех", "Решение сохранено в файл");
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл");
    }
}

void MainWindow::onResetViewClicked()
{
    graphWidget->resetNodePositions();
    if (!lastFoundPath.isEmpty()) {
        graphWidget->setHighlightedPath(lastFoundPath);
    }
}

void MainWindow::onDeleteNodeClicked()
{
    bool ok;
    int nodeToDelete = ui->deleteNodeEdit->text().toInt(&ok);

    if (!ok || nodeToDelete <= 0) {
        QMessageBox::warning(this, "Ошибка", "Введите корректный номер узла для удаления");
        return;
    }

    if (graph.getNodeCount() == 0) {
        QMessageBox::warning(this, "Ошибка", "Граф пуст");
        return;
    }

    if (nodeToDelete > graph.getNodeCount()) {
        QMessageBox::warning(this, "Ошибка",
                             QString("Узел %1 не существует. Максимальный узел: %2")
                                 .arg(nodeToDelete).arg(graph.getNodeCount()));
        return;
    }

    // Проверка, не удаляем ли мы узел, который используется в найденном пути
    if (!lastFoundPath.isEmpty() && lastFoundPath.contains(nodeToDelete)) {
        lastFoundPath.clear();
        lastPathCost = -1;
        ui->resultLabel->setText("Ожидание поиска...");
        graphWidget->clearHighlightedPath();
    }

    if (graph.deleteNode(nodeToDelete)) {
        updateGraphDisplay();
        graphWidget->setGraph(&graph);
        graphWidget->clearHighlightedPath();
        updateNodeCountLabel();

        // Очищаем поле ввода
        ui->deleteNodeEdit->clear();

        QMessageBox::information(this, "Успех",
                                 QString("Узел %1 и все связанные с ним рёбра удалены").arg(nodeToDelete));
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось удалить узел");
    }
}

void MainWindow::updateGraphDisplay()
{
    QMap<QPair<int, int>, int> edges;

    for (int i = 1; i <= graph.getNodeCount(); ++i) {
        for (const auto& edge : graph.getEdges(i)) {
            edges[qMakePair(i, edge.first)] = edge.second;
        }
    }

    ui->edgesTable->setRowCount(edges.size());
    int row = 0;
    for (auto it = edges.begin(); it != edges.end(); ++it) {
        ui->edgesTable->setItem(row, 0, new QTableWidgetItem(QString::number(it.key().first)));
        ui->edgesTable->setItem(row, 1, new QTableWidgetItem(QString::number(it.key().second)));
        ui->edgesTable->setItem(row, 2, new QTableWidgetItem(QString::number(it.value())));
        ++row;
    }

    ui->edgesTable->sortItems(0);
}

bool MainWindow::validateInputs()
{
    bool ok1, ok2;
    int start = ui->startNodeEdit->text().toInt(&ok1);
    int end = ui->endNodeEdit->text().toInt(&ok2);

    if (!ok1 || !ok2) {
        QMessageBox::warning(this, "Ошибка", "Некорректные номера узлов");
        return false;
    }

    if (graph.getNodeCount() == 0) {
        QMessageBox::warning(this, "Ошибка", "Граф пуст. Загрузите или добавьте ребра");
        return false;
    }

    if (start < 1 || start > graph.getNodeCount() ||
        end < 1 || end > graph.getNodeCount()) {
        QMessageBox::warning(this, "Ошибка",
                             QString("Узлы должны быть в диапазоне 1-%1").arg(graph.getNodeCount()));
        return false;
    }

    return true;
}

QString MainWindow::generateSolutionReport()
{
    QString report;
    report += "========================================\n";
    report += "ОТЧЕТ О РЕШЕНИИ ЗАДАЧИ\n";
    report += "========================================\n\n";

    report += "Метод решения: Динамическое программирование\n";
    report += "Алгоритм: Релаксация (аналог Bellman-Ford)\n\n";

    report += "Параметры графа:\n";
    report += "- Количество узлов: " + QString::number(graph.getNodeCount()) + "\n";
    report += "- Количество ребер: " + QString::number(ui->edgesTable->rowCount()) + "\n\n";

    report += "Задача:\n";
    report += "- Начальный узел: " + ui->startNodeEdit->text() + "\n";
    report += "- Конечный узел: " + ui->endNodeEdit->text() + "\n\n";

    report += "Результат:\n";
    report += ui->resultLabel->text() + "\n\n";

    report += "========================================\n";
    report += "Конец отчета\n";

    return report;
}

void MainWindow::updateNodeCountLabel()
{
    ui->nodeCountLabel->setText(QString("Узлов: %1").arg(graph.getNodeCount()));
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QAbstractItemView>
#include <algorithm>
#include <limits>

namespace {
const int INF = std::numeric_limits<int>::max() / 4;

struct ReportEdge {
    int from;
    int to;
    int weight;
};

struct DpSnapshot {
    int iteration;
    QVector<int> dist;
    QVector<int> prev;
    QStringList updates;
};

QString numberOrInfinity(int value)
{
    return value >= INF / 2 ? QString::fromUtf8("∞") : QString::number(value);
}

QString predecessorText(int value)
{
    return value > 0 ? QString::number(value) : "-";
}

QString makeTextTable(const QStringList& headers, const QVector<QStringList>& rows)
{
    QVector<int> widths(headers.size(), 0);

    for (int i = 0; i < headers.size(); ++i) {
        widths[i] = headers[i].length();
    }

    for (const QStringList& row : rows) {
        for (int i = 0; i < row.size() && i < widths.size(); ++i) {
            widths[i] = qMax(widths[i], row[i].length());
        }
    }

    auto makeSeparator = [&]() {
        QString line = "+";
        for (int width : widths) {
            line += QString("-").repeated(width + 2) + "+";
        }
        return line + "\n";
    };

    auto makeRow = [&](const QStringList& cells) {
        QString line = "|";
        for (int i = 0; i < widths.size(); ++i) {
            const QString cell = i < cells.size() ? cells[i] : "";
            line += " " + cell + QString(" ").repeated(widths[i] - cell.length()) + " |";
        }
        return line + "\n";
    };

    QString table;
    table += makeSeparator();
    table += makeRow(headers);
    table += makeSeparator();
    for (const QStringList& row : rows) {
        table += makeRow(row);
    }
    table += makeSeparator();

    return table;
}

QVector<ReportEdge> collectEdges(const Graph& graph)
{
    QVector<ReportEdge> edges;
    for (int from = 1; from <= graph.getNodeCount(); ++from) {
        for (const auto& edge : graph.getEdges(from)) {
            edges.append({from, edge.first, edge.second});
        }
    }
    return edges;
}

int edgeWeight(const QVector<ReportEdge>& edges, int from, int to)
{
    for (const ReportEdge& edge : edges) {
        if (edge.from == from && edge.to == to) {
            return edge.weight;
        }
    }
    return 0;
}

QString pathToString(const QVector<int>& path)
{
    QStringList nodes;
    for (int node : path) {
        nodes << QString::number(node);
    }
    return nodes.join(" -> ");
}

QVector<DpSnapshot> buildDpSnapshots(const Graph& graph, int start)
{
    QVector<DpSnapshot> snapshots;
    const QVector<ReportEdge> edges = collectEdges(graph);
    QVector<int> dist(graph.getNodeCount() + 1, INF);
    QVector<int> prev(graph.getNodeCount() + 1, -1);

    dist[start] = 0;
    snapshots.append({0, dist, prev, {QString("Инициализация: D(%1) = 0, остальные D(j) = ∞").arg(start)}});

    for (int iteration = 1; iteration <= graph.getNodeCount() - 1; ++iteration) {
        bool changed = false;
        QStringList updates;

        for (const ReportEdge& edge : edges) {
            if (dist[edge.from] >= INF / 2) {
                continue;
            }

            const int candidate = dist[edge.from] + edge.weight;
            if (candidate < dist[edge.to]) {
                const QString oldValue = numberOrInfinity(dist[edge.to]);
                dist[edge.to] = candidate;
                prev[edge.to] = edge.from;
                changed = true;

                updates << QString("D(%1) = min(%2, D(%3)+%4 = %5) = %5; P(%1) = %3")
                               .arg(edge.to)
                               .arg(oldValue)
                               .arg(edge.from)
                               .arg(edge.weight)
                               .arg(candidate);
            }
        }

        if (!changed) {
            updates << "Изменений нет, дальнейшие итерации не улучшают решение";
        }

        snapshots.append({iteration, dist, prev, updates});
        if (!changed) {
            break;
        }
    }

    return snapshots;
}

QTableWidgetItem* readOnlyItem(const QString& text)
{
    auto* item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}
}

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
    ui->edgesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->edgesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
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
        lastFoundPath.clear();
        lastPathCost = -1;
        ui->resultLabel->setText("Ожидание поиска...");
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
        lastFoundPath.clear();
        lastPathCost = -1;
        ui->resultLabel->setText("Ожидание поиска...");

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

    if (graph.deleteNode(nodeToDelete)) {
        updateGraphDisplay();
        graphWidget->setGraph(&graph);
        graphWidget->clearHighlightedPath();
        updateNodeCountLabel();
        lastFoundPath.clear();
        lastPathCost = -1;
        ui->resultLabel->setText("Ожидание поиска...");

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
        ui->edgesTable->setItem(row, 0, readOnlyItem(QString::number(it.key().first)));
        ui->edgesTable->setItem(row, 1, readOnlyItem(QString::number(it.key().second)));
        ui->edgesTable->setItem(row, 2, readOnlyItem(QString::number(it.value())));
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
    const int start = ui->startNodeEdit->text().toInt();
    const int end = ui->endNodeEdit->text().toInt();
    const QVector<ReportEdge> edges = collectEdges(graph);
    const QVector<DpSnapshot> snapshots = buildDpSnapshots(graph, start);

    QString report;
    report += "========================================\n";
    report += "ОТЧЕТ О РЕШЕНИИ ЗАДАЧИ\n";
    report += "========================================\n\n";

    report += "Метод решения: Динамическое программирование\n";
    report += "Рекуррентное соотношение: D(j) = min(D(j), D(i) + c(i,j)) для каждого ребра i -> j\n\n";

    report += "Параметры графа:\n";
    report += "- Количество узлов: " + QString::number(graph.getNodeCount()) + "\n";
    report += "- Количество ребер: " + QString::number(edges.size()) + "\n\n";

    report += "Задача:\n";
    report += "- Найти кратчайший путь из узла " + QString::number(start) + " в узел " + QString::number(end) + "\n\n";

    QVector<QStringList> edgeRows;
    int edgeNumber = 1;
    for (const ReportEdge& edge : edges) {
        edgeRows.append({QString::number(edgeNumber++),
                         QString::number(edge.from),
                         QString::number(edge.to),
                         QString::number(edge.weight)});
    }
    report += "Таблица 1. Список ребер\n";
    report += makeTextTable({"№", "От", "До", "Стоимость"}, edgeRows) + "\n";

    QVector<QStringList> matrixRows;
    QStringList matrixHeaders = {"i\\j"};
    for (int j = 1; j <= graph.getNodeCount(); ++j) {
        matrixHeaders << QString::number(j);
    }

    for (int i = 1; i <= graph.getNodeCount(); ++i) {
        QStringList row = {QString::number(i)};
        for (int j = 1; j <= graph.getNodeCount(); ++j) {
            if (i == j) {
                row << "0";
            } else {
                const int weight = edgeWeight(edges, i, j);
                row << (weight > 0 ? QString::number(weight) : QString::fromUtf8("∞"));
            }
        }
        matrixRows.append(row);
    }
    report += "Таблица 2. Матрица стоимостей c(i,j)\n";
    report += makeTextTable(matrixHeaders, matrixRows) + "\n";

    QStringList distanceHeaders = {"Итерация"};
    for (int node = 1; node <= graph.getNodeCount(); ++node) {
        distanceHeaders << QString("D(%1)").arg(node);
    }

    QVector<QStringList> distanceRows;
    for (const DpSnapshot& snapshot : snapshots) {
        QStringList row = {QString::number(snapshot.iteration)};
        for (int node = 1; node <= graph.getNodeCount(); ++node) {
            row << numberOrInfinity(snapshot.dist[node]);
        }
        distanceRows.append(row);
    }
    report += "Таблица 3. Значения D(j) по итерациям\n";
    report += makeTextTable(distanceHeaders, distanceRows) + "\n";

    QStringList predecessorHeaders = {"Итерация"};
    for (int node = 1; node <= graph.getNodeCount(); ++node) {
        predecessorHeaders << QString("P(%1)").arg(node);
    }

    QVector<QStringList> predecessorRows;
    for (const DpSnapshot& snapshot : snapshots) {
        QStringList row = {QString::number(snapshot.iteration)};
        for (int node = 1; node <= graph.getNodeCount(); ++node) {
            row << predecessorText(snapshot.prev[node]);
        }
        predecessorRows.append(row);
    }
    report += "Таблица 4. Предшественники P(j) по итерациям\n";
    report += makeTextTable(predecessorHeaders, predecessorRows) + "\n";

    report += "Журнал пересчетов:\n";
    for (const DpSnapshot& snapshot : snapshots) {
        report += "Итерация " + QString::number(snapshot.iteration) + ":\n";
        for (const QString& update : snapshot.updates) {
            report += "  - " + update + "\n";
        }
    }
    report += "\n";

    QVector<QStringList> pathRows;
    int cumulativeCost = 0;
    for (int i = 0; i < lastFoundPath.size(); ++i) {
        const int node = lastFoundPath[i];
        QString edgeCost = "-";
        if (i > 0) {
            const int weight = edgeWeight(edges, lastFoundPath[i - 1], node);
            cumulativeCost += weight;
            edgeCost = QString::number(weight);
        }

        pathRows.append({QString::number(i),
                         QString::number(node),
                         i == 0 ? "-" : QString::number(lastFoundPath[i - 1]),
                         edgeCost,
                         QString::number(cumulativeCost)});
    }

    report += "Таблица 5. Восстановление кратчайшего пути\n";
    report += makeTextTable({"Шаг", "Узел", "Предыдущий", "Стоимость ребра", "Накопленная стоимость"}, pathRows) + "\n";

    report += "Результат:\n";
    report += "- Кратчайший путь: " + pathToString(lastFoundPath) + "\n";
    report += "- Общая стоимость: " + QString::number(lastPathCost) + "\n";
    report += "- Количество шагов: " + QString::number(lastFoundPath.size() - 1) + "\n\n";

    report += "========================================\n";
    report += "Конец отчета\n";

    return report;
}

void MainWindow::updateNodeCountLabel()
{
    ui->nodeCountLabel->setText(QString("Узлов: %1").arg(graph.getNodeCount()));
}

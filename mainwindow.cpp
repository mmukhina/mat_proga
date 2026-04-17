#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QSplitter>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , lastPathCost(-1)
{
    setupUI();
    applyPinkStyle();
}

MainWindow::~MainWindow() {}

void MainWindow::applyPinkStyle()
{
    QString pinkStyle = R"(
        QMainWindow {
            background-color: #FFF0F5;
        }
        QWidget {
            background-color: #FFF0F5;
        }
        QScrollArea {
            background-color: #FFF0F5;
            border: none;
        }
        QScrollBar:vertical {
            border: none;
            background-color: #FFF0F5;
            width: 10px;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical {
            background-color: #FF69B4;
            border-radius: 5px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #FF1493;
        }
        QScrollBar:horizontal {
            border: none;
            background-color: #FFF0F5;
            height: 10px;
            border-radius: 5px;
        }
        QScrollBar::handle:horizontal {
            background-color: #FF69B4;
            border-radius: 5px;
            min-width: 20px;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid #FF69B4;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
            background-color: #FFF5F8;
            color: #8B0045;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: #FF1493;
        }
        QPushButton {
            background-color: #FF69B4;
            border: none;
            border-radius: 6px;
            padding: 8px;
            font-weight: bold;
            color: white;
        }
        QPushButton:hover {
            background-color: #FF1493;
        }
        QPushButton:pressed {
            background-color: #C71585;
        }
        QLineEdit {
            border: 2px solid #FFB6C1;
            border-radius: 5px;
            padding: 5px;
            background-color: white;
            color: #8B0045;
        }
        QLineEdit:focus {
            border-color: #FF69B4;
        }
        QTableWidget {
            border: 2px solid #FFB6C1;
            border-radius: 5px;
            background-color: white;
            alternate-background-color: #FFF0F5;
            gridline-color: #FFB6C1;
        }
        QTableWidget::item {
            padding: 5px;
        }
        QHeaderView::section {
            background-color: #FF69B4;
            color: white;
            padding: 5px;
            border: none;
            font-weight: bold;
        }
        QLabel {
            color: #8B0045;
        }
        QLabel#resultLabel {
            border: 2px solid #FF69B4;
            border-radius: 8px;
            padding: 10px;
            background-color: #FFF0F5;
            color: #8B0045;
        }
        QSplitter::handle {
            background-color: #FFB6C1;
        }
    )";

    this->setStyleSheet(pinkStyle);
    resultLabel->setObjectName("resultLabel");
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Левая панель с прокруткой
    QScrollArea* leftScrollArea = new QScrollArea();
    leftScrollArea->setWidgetResizable(true);
    leftScrollArea->setMaximumWidth(450);
    leftScrollArea->setMinimumWidth(350);
    leftScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    leftScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget* leftPanel = new QWidget();
    leftPanel->setObjectName("leftPanel");
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(10);
    leftLayout->setContentsMargins(10, 10, 10, 10);

    // Заголовок
    QLabel* titleLabel = new QLabel("Поиск кратчайшего пути через сеть");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px; color: #FF1493;");
    titleLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(titleLabel);

    // Группа ввода графа
    QGroupBox* inputGroup = new QGroupBox("Ввод данных");
    QVBoxLayout* inputLayout = new QVBoxLayout(inputGroup);

    QHBoxLayout* fileLayout = new QHBoxLayout();
    loadFileBtn = new QPushButton("Загрузить из файла");
    connect(loadFileBtn, &QPushButton::clicked, this, &MainWindow::onLoadFileClicked);
    fileLayout->addWidget(loadFileBtn);
    fileLayout->addStretch();
    inputLayout->addLayout(fileLayout);

    QGridLayout* manualLayout = new QGridLayout();
    QLabel* fromLabel = new QLabel("От:");
    fromLabel->setStyleSheet("color: #8B0045; font-weight: bold;");
    fromEdit = new QLineEdit();
    fromEdit->setPlaceholderText("1");
    QLabel* toLabel = new QLabel("До:");
    toLabel->setStyleSheet("color: #8B0045; font-weight: bold;");
    toEdit = new QLineEdit();
    toEdit->setPlaceholderText("2");
    QLabel* weightLabel = new QLabel("Вес:");
    weightLabel->setStyleSheet("color: #8B0045; font-weight: bold;");
    weightEdit = new QLineEdit();
    weightEdit->setPlaceholderText("5");
    addEdgeBtn = new QPushButton("Добавить ребро");
    connect(addEdgeBtn, &QPushButton::clicked, this, &MainWindow::onAddEdgeClicked);

    manualLayout->addWidget(fromLabel, 0, 0);
    manualLayout->addWidget(fromEdit, 0, 1);
    manualLayout->addWidget(toLabel, 0, 2);
    manualLayout->addWidget(toEdit, 0, 3);
    manualLayout->addWidget(weightLabel, 0, 4);
    manualLayout->addWidget(weightEdit, 0, 5);
    manualLayout->addWidget(addEdgeBtn, 1, 0, 1, 6);

    inputLayout->addLayout(manualLayout);

    clearBtn = new QPushButton("Очистить граф");
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearClicked);
    inputLayout->addWidget(clearBtn);

    nodeCountLabel = new QLabel("Узлов: 0");
    nodeCountLabel->setStyleSheet("color: #FF1493; font-weight: bold; font-size: 12px;");
    inputLayout->addWidget(nodeCountLabel);

    leftLayout->addWidget(inputGroup);

    // Таблица ребер
    QGroupBox* edgesGroup = new QGroupBox("Список ребер");
    QVBoxLayout* edgesLayout = new QVBoxLayout(edgesGroup);
    edgesTable = new QTableWidget();
    edgesTable->setColumnCount(3);
    edgesTable->setHorizontalHeaderLabels({"От", "До", "Вес"});
    edgesTable->horizontalHeader()->setStretchLastSection(true);
    edgesTable->setMinimumHeight(150);
    edgesTable->setMaximumHeight(250);
    edgesTable->setAlternatingRowColors(true);
    edgesLayout->addWidget(edgesTable);
    leftLayout->addWidget(edgesGroup);

    // Поиск пути
    QGroupBox* searchGroup = new QGroupBox("Поиск пути");
    QHBoxLayout* searchLayout = new QHBoxLayout(searchGroup);
    QLabel* startLabel = new QLabel("Старт:");
    startLabel->setStyleSheet("color: #8B0045; font-weight: bold;");
    startNodeEdit = new QLineEdit();
    startNodeEdit->setText("1");
    startNodeEdit->setFixedWidth(60);
    QLabel* endLabel = new QLabel("Финиш:");
    endLabel->setStyleSheet("color: #8B0045; font-weight: bold;");
    endNodeEdit = new QLineEdit();
    endNodeEdit->setFixedWidth(60);
    findPathBtn = new QPushButton("Найти путь");
    connect(findPathBtn, &QPushButton::clicked, this, &MainWindow::onFindPathClicked);

    searchLayout->addWidget(startLabel);
    searchLayout->addWidget(startNodeEdit);
    searchLayout->addWidget(endLabel);
    searchLayout->addWidget(endNodeEdit);
    searchLayout->addWidget(findPathBtn);
    leftLayout->addWidget(searchGroup);

    // Результат
    QGroupBox* resultGroup = new QGroupBox("Результат");
    QVBoxLayout* resultLayout = new QVBoxLayout(resultGroup);
    resultLabel = new QLabel("Ожидание поиска...");
    resultLabel->setWordWrap(true);
    resultLabel->setAlignment(Qt::AlignTop);
    resultLabel->setMinimumHeight(120);
    resultLayout->addWidget(resultLabel);
    leftLayout->addWidget(resultGroup);

    // Кнопки управления
    QGroupBox* viewGroup = new QGroupBox("Управление");
    QHBoxLayout* viewLayout = new QHBoxLayout(viewGroup);
    resetViewBtn = new QPushButton("⟳ Сброс позиций");
    connect(resetViewBtn, &QPushButton::clicked, this, &MainWindow::onResetViewClicked);
    viewLayout->addWidget(resetViewBtn);
    leftLayout->addWidget(viewGroup);

    // Сохранение решения
    saveSolutionBtn = new QPushButton("Сохранить решение");
    connect(saveSolutionBtn, &QPushButton::clicked, this, &MainWindow::onSaveSolutionClicked);
    leftLayout->addWidget(saveSolutionBtn);

    leftLayout->addStretch();

    leftScrollArea->setWidget(leftPanel);

    // Правая панель - граф
    QWidget* rightPanel = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    graphWidget = new GraphWidget();
    graphWidget->setMinimumHeight(500);
    rightLayout->addWidget(graphWidget);

    // Разделитель
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(leftScrollArea);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 3);

    mainLayout->addWidget(splitter);

    setWindowTitle("Визуализация кратчайшего пути");
    resize(1400, 900);
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
    int from = fromEdit->text().toInt(&ok1);
    int to = toEdit->text().toInt(&ok2);
    int weight = weightEdit->text().toInt(&ok3);

    if (!ok1 || !ok2 || !ok3) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, введите корректные числа");
        return;
    }

    if (from <= 0 || to <= 0) {
        QMessageBox::warning(this, "Ошибка", "Номера узлов должны быть положительными");
        return;
    }

    // НОВОЕ ОГРАНИЧЕНИЕ: запрет петли
    if (from == to) {
        QMessageBox::warning(this, "Ошибка", "Нельзя создать ребро из узла в самого себя (петля)");
        return;
    }

    // НОВОЕ ОГРАНИЧЕНИЕ: вес не может быть меньше 1
    if (weight < 1) {
        QMessageBox::warning(this, "Ошибка", "Вес ребра должен быть >= 1");
        return;
    }

    // ОПЦИОНАЛЬНО: проверка на существование такого же ребра
    const auto& edges = graph.getEdges(from);
    for (const auto& edge : edges) {
        if (edge.first == to) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Ребро %1 → %2 уже существует. Удалите его перед добавлением.").arg(from).arg(to));
            return;
        }
    }

    if (graph.addEdge(from, to, weight)) {
        updateGraphDisplay();
        graphWidget->setGraph(&graph);
        graphWidget->clearHighlightedPath();
        updateNodeCountLabel();

        fromEdit->clear();
        toEdit->clear();
        weightEdit->clear();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось добавить ребро");
    }
}

void MainWindow::onClearClicked()
{
    graph.clear();
    edgesTable->setRowCount(0);
    resultLabel->setText("Ожидание поиска...");
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

    int start = startNodeEdit->text().toInt();
    int end = endNodeEdit->text().toInt();

    auto result = graph.findShortestPath(start, end);
    lastFoundPath = result.first;
    lastPathCost = result.second;

    if (lastPathCost == -1) {
        resultLabel->setText("Ошибка: " + graph.getLastError());
        graphWidget->clearHighlightedPath();
        QMessageBox::warning(this, "Ошибка", graph.getLastError());
    } else {
        QString pathStr;
        for (int i = 0; i < lastFoundPath.size(); i++) {
            if (i > 0) pathStr += " → ";
            pathStr += QString::number(lastFoundPath[i]);
        }

        QString resultText = QString("КРАТЧАЙШИЙ ПУТЬ НАЙДЕН!\n\n"
                                     "Путь: %1\n\n"
                                     "Общая стоимость: %2\n\n"
                                     "Количество шагов: %3")
                                 .arg(pathStr)
                                 .arg(lastPathCost)
                                 .arg(lastFoundPath.size() - 1);
        resultLabel->setText(resultText);
        graphWidget->setHighlightedPath(lastFoundPath);

        QMessageBox::information(this, "Успех",
                                 QString("Кратчайший путь найден!\nСтоимость: %1 ").arg(lastPathCost));
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

void MainWindow::updateGraphDisplay()
{
    QMap<QPair<int, int>, int> edges;

    for (int i = 1; i <= graph.getNodeCount(); i++) {
        for (const auto& edge : graph.getEdges(i)) {
            edges[qMakePair(i, edge.first)] = edge.second;
        }
    }

    edgesTable->setRowCount(edges.size());
    int row = 0;
    for (auto it = edges.begin(); it != edges.end(); ++it) {
        edgesTable->setItem(row, 0, new QTableWidgetItem(QString::number(it.key().first)));
        edgesTable->setItem(row, 1, new QTableWidgetItem(QString::number(it.key().second)));
        edgesTable->setItem(row, 2, new QTableWidgetItem(QString::number(it.value())));
        row++;
    }

    edgesTable->sortItems(0);
}

bool MainWindow::validateInputs()
{
    bool ok1, ok2;
    int start = startNodeEdit->text().toInt(&ok1);
    int end = endNodeEdit->text().toInt(&ok2);

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
    report += "  Решение задачи  \n";
    report += "========================================\n\n";


    report += "Алгоритм: Релаксация (аналог Bellman-Ford)\n\n";

    report += "Параметры графа:\n";
    report += "- Количество узлов: " + QString::number(graph.getNodeCount()) + "\n";
    report += "- Количество ребер: " + QString::number(edgesTable->rowCount()) + "\n\n";

    report += "Задача:\n";
    report += "- Начальный узел: " + startNodeEdit->text() + "\n";
    report += "- Конечный узел: " + endNodeEdit->text() + "\n\n";

    report += "Результат:\n";
    report += resultLabel->text() + "\n\n";



    return report;
}

void MainWindow::updateNodeCountLabel()
{
    nodeCountLabel->setText(QString("Узлов: %1").arg(graph.getNodeCount()));
}

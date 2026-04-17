// graph.cpp
#include "graph.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QStack>

Graph::Graph() : nodeCount(0) {}

bool Graph::loadFromFile(const QString& filename)
{
    clear();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        lastError = "Не удалось открыть файл: " + filename;
        return false;
    }

    QTextStream in(&file);
    QString line;
    int lineNum = 0;
    QSet<int> nodes;

    // Читаем количество узлов
    if (!in.atEnd()) {
        line = in.readLine().trimmed();
        bool ok;
        nodeCount = line.toInt(&ok);
        if (!ok || nodeCount <= 0) {
            lastError = "Некорректное количество узлов";
            return false;
        }
        lineNum++;
    } else {
        lastError = "Пустой файл";
        return false;
    }

    // Читаем ребра
    while (!in.atEnd()) {
        line = in.readLine().trimmed();
        lineNum++;

        if (line.isEmpty()) continue;

        // Парсим строку: from to weight
        QStringList parts = line.split(QRegularExpression("\\s+"));
        if (parts.size() < 3) {
            lastError = QString("Ошибка в строке %1: недостаточно данных").arg(lineNum);
            return false;
        }

        bool ok1, ok2, ok3;
        int from = parts[0].toInt(&ok1);
        int to = parts[1].toInt(&ok2);
        int weight = parts[2].toInt(&ok3);

        if (!ok1 || !ok2 || !ok3) {
            lastError = QString("Ошибка в строке %1: некорректные числа").arg(lineNum);
            return false;
        }

        if (from < 1 || from > nodeCount || to < 1 || to > nodeCount) {
            lastError = QString("Ошибка в строке %1: узел вне диапазона").arg(lineNum);
            return false;
        }

        addEdge(from, to, weight);
        nodes.insert(from);
        nodes.insert(to);
    }

    file.close();

    if (!validateGraph()) {
        return false;
    }

    return true;
}

bool Graph::addEdge(int from, int to, int weight)
{
    if (from < 1 || to < 1) return false;

    adjList[from].append(qMakePair(to, weight));
    nodeCount = qMax(nodeCount, qMax(from, to));
    return true;
}

void Graph::clear()
{
    nodeCount = 0;
    adjList.clear();
    lastError.clear();
}

QPair<QVector<int>, int> Graph::findShortestPath(int start, int end)
{
    QVector<int> path;
    int totalCost = -1;

    if (start < 1 || end < 1 || start > nodeCount || end > nodeCount) {
        lastError = "Некорректные начальный или конечный узлы";
        return qMakePair(path, -1);
    }

    if (!isReachable(start, end)) {
        lastError = "Путь из узла " + QString::number(start) +
                    " в узел " + QString::number(end) + " не существует";
        return qMakePair(path, -1);
    }

    // Динамическое программирование
    // dp[i] = минимальная стоимость достижения узла i
    QVector<int> dp(nodeCount + 1, INT_MAX);
    QVector<int> prev(nodeCount + 1, -1); // для восстановления пути

    dp[start] = 0;

    // Так как граф может быть не топологически отсортирован,
    // используем итеративный подход с релаксацией (Bellman-Ford подобный)
    bool changed;
    for (int i = 0; i < nodeCount - 1; i++) {
        changed = false;
        for (int u = 1; u <= nodeCount; u++) {
            if (dp[u] == INT_MAX) continue;

            for (const auto& edge : adjList[u]) {
                int v = edge.first;
                int weight = edge.second;

                if (dp[u] + weight < dp[v]) {
                    dp[v] = dp[u] + weight;
                    prev[v] = u;
                    changed = true;
                }
            }
        }
        if (!changed) break;
    }

    // Проверка на отрицательные циклы
    for (int u = 1; u <= nodeCount; u++) {
        if (dp[u] == INT_MAX) continue;
        for (const auto& edge : adjList[u]) {
            int v = edge.first;
            int weight = edge.second;
            if (dp[u] + weight < dp[v]) {
                lastError = "Обнаружен отрицательный цикл!";
                return qMakePair(path, -1);
            }
        }
    }

    if (dp[end] == INT_MAX) {
        lastError = "Путь не найден";
        return qMakePair(path, -1);
    }

    // Восстанавливаем путь
    totalCost = dp[end];
    for (int v = end; v != -1; v = prev[v]) {
        path.prepend(v);
    }

    return qMakePair(path, totalCost);
}

bool Graph::hasNegativeWeights() const
{
    for (auto it = adjList.begin(); it != adjList.end(); ++it) {
        for (const auto& edge : it.value()) {
            if (edge.second < 0) {
                return true;
            }
        }
    }
    return false;
}

bool Graph::hasDeadEnds() const
{
    for (int i = 1; i <= nodeCount; i++) {
        if (!adjList.contains(i) || adjList[i].isEmpty()) {
            // Узел i не имеет исходящих ребер
            if (i != nodeCount) { // Если это не конечный узел
                return true;
            }
        }
    }
    return false;
}

bool Graph::hasCycles() const
{
    QVector<int> visited(nodeCount + 1, 0); // 0=unvisited, 1=visiting, 2=visited

    std::function<bool(int)> dfs = [&](int u) -> bool {
        visited[u] = 1;

        for (const auto& edge : adjList[u]) {
            int v = edge.first;
            if (visited[v] == 1) {
                return true; // цикл найден
            }
            if (visited[v] == 0 && dfs(v)) {
                return true;
            }
        }

        visited[u] = 2;
        return false;
    };

    for (int i = 1; i <= nodeCount; i++) {
        if (visited[i] == 0 && adjList.contains(i)) {
            if (dfs(i)) return true;
        }
    }
    return false;
}

bool Graph::isReachable(int start, int end) const
{
    QVector<bool> visited(nodeCount + 1, false);
    findReachableNodes(start, visited);
    return visited[end];
}

bool Graph::validateGraph()
{
    if (nodeCount <= 0) {
        lastError = "Граф не содержит узлов";
        return false;
    }

    // Проверка на отрицательные веса (выводим предупреждение)
    if (hasNegativeWeights()) {
        qDebug() << "Предупреждение: Граф содержит отрицательные веса";
    }

    // Проверка на тупиковые узлы
    if (hasDeadEnds()) {
        qDebug() << "Предупреждение: Граф содержит тупиковые узлы";
    }

    return true;
}

void Graph::findReachableNodes(int start, QVector<bool>& visited) const
{
    QStack<int> stack;
    stack.push(start);
    visited[start] = true;

    while (!stack.isEmpty()) {
        int u = stack.pop();

        if (adjList.contains(u)) {
            for (const auto& edge : adjList[u]) {
                int v = edge.first;
                if (!visited[v]) {
                    visited[v] = true;
                    stack.push(v);
                }
            }
        }
    }
}

void Graph::printGraph() const
{
    qDebug() << "Graph with" << nodeCount << "nodes:";
    for (int i = 1; i <= nodeCount; i++) {
        if (adjList.contains(i)) {
            QString edges;
            for (const auto& edge : adjList[i]) {
                edges += QString("(%1,%2) ").arg(edge.first).arg(edge.second);
            }
            qDebug() << QString("Node %1 -> %2").arg(i).arg(edges);
        } else {
            qDebug() << QString("Node %1 -> (no outgoing edges)").arg(i);
        }
    }
}

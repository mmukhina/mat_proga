// graph.cpp
#include "graph.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QStack>
#include <QSet>
#include <algorithm>
#include <climits>
#include <functional>

namespace {
QString removeComment(const QString& line)
{
    int commentPos = line.indexOf('#');
    const int slashCommentPos = line.indexOf("//");
    if (slashCommentPos >= 0 && (commentPos < 0 || slashCommentPos < commentPos)) {
        commentPos = slashCommentPos;
    }

    return commentPos >= 0 ? line.left(commentPos).trimmed() : line.trimmed();
}

bool parseIntegerRow(const QString& line, QVector<int>& values)
{
    values.clear();
    const QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    for (const QString& part : parts) {
        bool ok = false;
        int value = part.toInt(&ok);
        if (!ok) {
            return false;
        }
        values.append(value);
    }

    return !values.isEmpty();
}

struct FileRow {
    int lineNumber;
    QVector<int> values;
};

struct FileEdge {
    int lineNumber;
    int from;
    int to;
    int weight;
};
}

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
    QVector<FileRow> rows;
    int lineNum = 0;

    while (!in.atEnd()) {
        ++lineNum;
        const QString line = removeComment(in.readLine());
        if (line.isEmpty()) {
            continue;
        }

        QVector<int> values;
        if (!parseIntegerRow(line, values)) {
            lastError = QString("Ошибка в строке %1: ожидались целые числа").arg(lineNum);
            return false;
        }

        rows.append({lineNum, values});
    }

    file.close();

    if (rows.isEmpty()) {
        lastError = "Пустой файл";
        return false;
    }

    QVector<FileEdge> parsedEdges;
    QSet<QString> usedEdges;
    int maxNode = 0;

    for (int i = 0; i < rows.size(); ++i) {
        const FileRow& row = rows[i];
        if (row.values.size() != 3) {
            lastError = QString("Ошибка в строке %1: ребро задается тремя числами: от до вес")
                            .arg(row.lineNumber);
            return false;
        }

        const int from = row.values[0];
        const int to = row.values[1];
        const int weight = row.values[2];

        if (from <= 0 || to <= 0) {
            lastError = QString("Ошибка в строке %1: номера узлов должны быть положительными")
                            .arg(row.lineNumber);
            return false;
        }

        if (from == to) {
            lastError = QString("Ошибка в строке %1: петли не допускаются").arg(row.lineNumber);
            return false;
        }

        if (weight <= 0) {
            lastError = QString("Ошибка в строке %1: вес ребра должен быть положительным")
                            .arg(row.lineNumber);
            return false;
        }

        const QString edgeKey = QString("%1:%2").arg(from).arg(to);
        if (usedEdges.contains(edgeKey)) {
            lastError = QString("Ошибка в строке %1: ребро %2 -> %3 уже задано")
                            .arg(row.lineNumber)
                            .arg(from)
                            .arg(to);
            return false;
        }

        usedEdges.insert(edgeKey);
        maxNode = qMax(maxNode, qMax(from, to));
        parsedEdges.append({row.lineNumber, from, to, weight});
    }

    if (parsedEdges.isEmpty()) {
        lastError = "В файле не найдено ни одного ребра. Формат каждой строки: от до вес.";
        return false;
    }

    nodeCount = maxNode;

    for (const FileEdge& edge : parsedEdges) {
        adjList[edge.from].append(qMakePair(edge.to, edge.weight));
    }

    if (!validateGraph()) {
        clear();
        return false;
    }

    return true;
}

bool Graph::addEdge(int from, int to, int weight)
{
    if (from < 1 || to < 1 || from == to || weight <= 0) return false;

    for (const auto& edge : adjList.value(from)) {
        if (edge.first == to) {
            return false;
        }
    }

    adjList[from].append(qMakePair(to, weight));
    nodeCount = qMax(nodeCount, qMax(from, to));
    return true;
}

bool Graph::deleteNode(int nodeId)
{
    if (nodeId < 1 || nodeId > nodeCount) {
        lastError = "Узел не существует";
        return false;
    }

    // Удаляем все рёбра, исходящие из узла
    adjList.remove(nodeId);

    // Удаляем все рёбра, входящие в узел
    for (auto it = adjList.begin(); it != adjList.end(); ++it) {
        QVector<QPair<int, int>>& edges = it.value();
        edges.erase(std::remove_if(edges.begin(), edges.end(),
                                   [nodeId](const QPair<int, int>& edge) {
                                       return edge.first == nodeId;
                                   }), edges.end());
    }

    // Перенумеровываем узлы для компактности
    renumberNodes();

    return true;
}

void Graph::renumberNodes()
{
    if (adjList.isEmpty()) {
        nodeCount = 0;
        return;
    }

    // Собираем все существующие узлы
    QSet<int> existingNodes;
    for (auto it = adjList.begin(); it != adjList.end(); ++it) {
        existingNodes.insert(it.key());
        for (const auto& edge : it.value()) {
            existingNodes.insert(edge.first);
        }
    }

    if (existingNodes.isEmpty()) {
        nodeCount = 0;
        adjList.clear();
        return;
    }

    // Создаём отображение старых ID на новые
    QList<int> sortedNodes = existingNodes.values();
    std::sort(sortedNodes.begin(), sortedNodes.end());

    QMap<int, int> oldToNew;
    for (int i = 0; i < sortedNodes.size(); ++i) {
        oldToNew[sortedNodes[i]] = i + 1;
    }

    // Создаём новый список смежности с новыми ID
    QMap<int, QVector<QPair<int, int>>> newAdjList;
    for (auto it = adjList.begin(); it != adjList.end(); ++it) {
        int newFrom = oldToNew[it.key()];
        for (const auto& edge : it.value()) {
            int newTo = oldToNew[edge.first];
            newAdjList[newFrom].append(qMakePair(newTo, edge.second));
        }
    }

    adjList = newAdjList;
    nodeCount = sortedNodes.size();
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

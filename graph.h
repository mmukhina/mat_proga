// graph.h
#ifndef GRAPH_H
#define GRAPH_H

#include <QVector>
#include <QMap>
#include <QPair>
#include <QDebug>

class Graph
{
public:
    Graph();

    bool loadFromFile(const QString& filename);
    bool addEdge(int from, int to, int weight);
    bool deleteNode(int nodeId);  // Новый метод
    void clear();

    // Метод динамического программирования для поиска кратчайшего пути
    QPair<QVector<int>, int> findShortestPath(int start, int end);

    // Проверки корректности
    bool hasNegativeWeights() const;
    bool hasDeadEnds() const;
    bool hasCycles() const;
    bool isReachable(int start, int end) const;

    // Геттеры
    int getNodeCount() const { return nodeCount; }
    QVector<QPair<int, int>> getEdges(int node) const { return adjList.value(node); }
    QString getLastError() const { return lastError; }

    // Для отладки
    void printGraph() const;

private:
    int nodeCount;
    QMap<int, QVector<QPair<int, int>>> adjList; // adjacency list: from -> [(to, weight)]
    QString lastError;

    bool validateGraph();
    void findReachableNodes(int start, QVector<bool>& visited) const;
    void renumberNodes();  // Новый метод для перенумерации узлов
};

#endif // GRAPH_H

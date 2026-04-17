#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMap>
#include <QVector>
#include <QPoint>
#include <QMouseEvent>
#include "graph.h"

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

    void setGraph(const Graph* graph);
    void setHighlightedPath(const QVector<int>& path);
    void clearHighlightedPath();
    void resetNodePositions();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    const Graph* currentGraph;
    QVector<int> highlightedPath;

    QMap<int, QPointF> nodePositions;
    QMap<int, QPointF> defaultPositions;

    int draggedNode;
    QPointF dragOffset;

    void calculateDefaultPositions();
    void drawNode(QPainter* painter, int nodeId, const QPointF& pos);
    void drawEdge(QPainter* painter, int from, int to, int weight, bool isHighlighted);
    void drawPathArrow(QPainter* painter, const QPointF& from, const QPointF& to, bool isHighlighted);

    QPointF getNodePosition(int nodeId) const;
    int getNodeAtPosition(const QPointF& pos) const;
};

#endif // GRAPH_WIDGET_H

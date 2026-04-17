#include "graphwidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QtMath>

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent)
    , currentGraph(nullptr)
    , draggedNode(-1)
{
    setMinimumSize(400, 400);
    setMouseTracking(true);
    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(255, 240, 245));
    setPalette(pal);
}

void GraphWidget::setGraph(const Graph* graph)
{
    currentGraph = graph;
    calculateDefaultPositions();
    if (nodePositions.isEmpty()) {
        nodePositions = defaultPositions;
    }
    update();
}

void GraphWidget::resetNodePositions()
{
    nodePositions = defaultPositions;
    update();
}

void GraphWidget::setHighlightedPath(const QVector<int>& path)
{
    highlightedPath = path;
    update();
}

void GraphWidget::clearHighlightedPath()
{
    highlightedPath.clear();
    update();
}

QSize GraphWidget::minimumSizeHint() const
{
    return QSize(400, 400);
}

QSize GraphWidget::sizeHint() const
{
    return QSize(800, 700);
}

void GraphWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    if (!currentGraph || currentGraph->getNodeCount() == 0) {
        QPainter painter(this);
        painter.setPen(QColor(255, 105, 180));
        painter.drawText(rect(), Qt::AlignCenter, "Нет данных для отображения\nЗагрузите граф или добавьте ребра.");
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Рисуем рёбра
    for (int i = 1; i <= currentGraph->getNodeCount(); i++) {
        for (const auto& edge : currentGraph->getEdges(i)) {
            int to = edge.first;
            int weight = edge.second;

            bool isHighlighted = false;
            for (int j = 0; j < highlightedPath.size() - 1; j++) {
                if (highlightedPath[j] == i && highlightedPath[j + 1] == to) {
                    isHighlighted = true;
                    break;
                }
            }

            drawEdge(&painter, i, to, weight, isHighlighted);
        }
    }

    // Рисуем узлы
    for (int i = 1; i <= currentGraph->getNodeCount(); i++) {
        QPointF pos = getNodePosition(i);
        drawNode(&painter, i, pos);
    }
}

void GraphWidget::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    calculateDefaultPositions();
    for (auto it = defaultPositions.begin(); it != defaultPositions.end(); ++it) {
        if (!nodePositions.contains(it.key())) {
            nodePositions[it.key()] = it.value();
        }
    }
    update();
}

void GraphWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPointF clickPos = event->pos();
        draggedNode = getNodeAtPosition(clickPos);

        if (draggedNode != -1) {
            dragOffset = getNodePosition(draggedNode) - clickPos;
            setCursor(Qt::ClosedHandCursor);
        }
    }
    QWidget::mousePressEvent(event);
}

void GraphWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (draggedNode != -1) {
        QPointF newPos = event->pos() + dragOffset;
        newPos.setX(qBound(30.0, newPos.x(), (double)width() - 30));
        newPos.setY(qBound(30.0, newPos.y(), (double)height() - 30));
        nodePositions[draggedNode] = newPos;
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void GraphWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (draggedNode != -1) {
        draggedNode = -1;
        setCursor(Qt::ArrowCursor);
    }
    QWidget::mouseReleaseEvent(event);
}

void GraphWidget::calculateDefaultPositions()
{
    if (!currentGraph) return;

    int nodeCount = currentGraph->getNodeCount();
    if (nodeCount == 0) return;

    defaultPositions.clear();

    int margin = 60;
    int widthVal = width() - 2 * margin;
    int heightVal = height() - 2 * margin;

    if (widthVal <= 0 || heightVal <= 0) {
        widthVal = 700;
        heightVal = 600;
    }

    if (nodeCount == 1) {
        defaultPositions[1] = QPointF(widthVal / 2 + margin, heightVal / 2 + margin);
    } else {
        double angleStep = 2 * M_PI / nodeCount;
        double radius = qMin(widthVal, heightVal) / 2.5;
        QPointF center(widthVal / 2 + margin, heightVal / 2 + margin);

        for (int i = 1; i <= nodeCount; i++) {
            double angle = (i - 1) * angleStep - M_PI / 2;
            double x = center.x() + radius * cos(angle);
            double y = center.y() + radius * sin(angle);
            defaultPositions[i] = QPointF(x, y);
        }
    }
}

void GraphWidget::drawNode(QPainter* painter, int nodeId, const QPointF& pos)
{
    painter->save();

    bool isInPath = highlightedPath.contains(nodeId);

    QRadialGradient gradient(pos, 30);
    if (nodeId == 1 || nodeId == currentGraph->getNodeCount()) {
        gradient.setColorAt(0, QColor(255, 105, 180));
        gradient.setColorAt(1, QColor(255, 20, 147));
    } else if (isInPath) {
        gradient.setColorAt(0, QColor(255, 182, 193));
        gradient.setColorAt(1, QColor(255, 105, 180));
    } else {
        gradient.setColorAt(0, QColor(255, 218, 225));
        gradient.setColorAt(1, QColor(255, 182, 193));
    }

    painter->setBrush(gradient);
    painter->setPen(QPen(QColor(255, 20, 147), 2));
    painter->drawEllipse(pos, 30, 30);

    painter->setPen(QPen(QColor(139, 0, 69), 1));
    painter->setFont(QFont("Arial", 14, QFont::Bold));
    painter->drawText(QRectF(pos.x() - 25, pos.y() - 25, 50, 50),
                      Qt::AlignCenter, QString::number(nodeId));

    painter->restore();
}

void GraphWidget::drawEdge(QPainter* painter, int from, int to, int weight, bool isHighlighted)
{
    QPointF fromPos = getNodePosition(from);
    QPointF toPos = getNodePosition(to);

    painter->save();

    QPen pen;
    if (isHighlighted) {
        pen.setColor(QColor(255, 20, 147));
        pen.setWidth(5);
    } else {
        pen.setColor(QColor(219, 112, 147));
        pen.setWidth(3);
    }
    painter->setPen(pen);
    painter->drawLine(fromPos, toPos);

    // Рисуем стрелку
    drawPathArrow(painter, fromPos, toPos, isHighlighted);

    // Вес ребра
    QPointF midPoint = (fromPos + toPos) / 2;
    QLineF line(fromPos, toPos);
    QPointF normal = line.normalVector().unitVector().p2() - line.p1();
    normal *= 22;

    painter->setPen(QPen(isHighlighted ? QColor(255, 20, 147) : QColor(139, 0, 69), 2));
    painter->setFont(QFont("Arial", 12, QFont::Bold));
    painter->setBrush(QBrush(QColor(255, 240, 245)));

    QRectF textRect(midPoint.x() + normal.x() - 20,
                    midPoint.y() + normal.y() - 14,
                    40, 28);
    painter->drawRoundedRect(textRect, 6, 6);
    painter->drawText(textRect, Qt::AlignCenter, QString::number(weight));

    painter->restore();
}

void GraphWidget::drawPathArrow(QPainter* painter, const QPointF& from, const QPointF& to, bool isHighlighted)
{
    QLineF line(from, to);
    double length = line.length();
    if (length < 1e-5) return;

    double nodeRadius = 30.0;
    double arrowSize = 20.0;

    // Остриё стрелки на границе узла назначения
    QPointF arrowTip = to;
    if (length > nodeRadius) {
        arrowTip = line.pointAt((length - nodeRadius) / length);
    }

    // Угол направления ребра
    double angle = std::atan2(to.y() - from.y(), to.x() - from.x());

    // Левый и правый «ус» стрелки
    double arrowAngle = M_PI / 5.2; // ~34.6 градуса
    QPointF arrowLeft = arrowTip + QPointF(-arrowSize * cos(angle - arrowAngle),
                                           -arrowSize * sin(angle - arrowAngle));
    QPointF arrowRight = arrowTip + QPointF(-arrowSize * cos(angle + arrowAngle),
                                            -arrowSize * sin(angle + arrowAngle));

    QColor arrowColor = isHighlighted ? QColor(255, 20, 147) : QColor(199, 21, 133);
    painter->setPen(QPen(arrowColor, 2));
    painter->setBrush(arrowColor);

    QPolygonF arrowHead;
    arrowHead << arrowTip << arrowLeft << arrowRight;
    painter->drawPolygon(arrowHead);
}

QPointF GraphWidget::getNodePosition(int nodeId) const
{
    if (nodePositions.contains(nodeId)) {
        return nodePositions[nodeId];
    }
    if (defaultPositions.contains(nodeId)) {
        return defaultPositions[nodeId];
    }
    return QPointF(0, 0);
}

int GraphWidget::getNodeAtPosition(const QPointF& pos) const
{
    for (auto it = nodePositions.begin(); it != nodePositions.end(); ++it) {
        QPointF nodePos = it.value();
        double distance = QLineF(pos, nodePos).length();
        if (distance <= 35) {
            return it.key();
        }
    }
    for (auto it = defaultPositions.begin(); it != defaultPositions.end(); ++it) {
        if (!nodePositions.contains(it.key())) {
            QPointF nodePos = it.value();
            double distance = QLineF(pos, nodePos).length();
            if (distance <= 35) {
                return it.key();
            }
        }
    }
    return -1;
}

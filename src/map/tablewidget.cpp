#include "tablewidget.h"

#include <algorithm>

#include <QBrush>
#include <QFont>
#include <QHeaderView>
#include <QTableWidgetItem>

namespace {
constexpr int kSemanticRowCount = 6;
constexpr int kForwardLineRow = 0;
constexpr int kForwardCoordRow = 1;
constexpr int kForwardTimeRow = 2;
constexpr int kReverseLineRow = 3;
constexpr int kReverseCoordRow = 4;
constexpr int kReverseTimeRow = 5;
constexpr int kCompactRowHeight = 30;

const QColor kHighlightBackground(255, 235, 156);
const QColor kForwardLineBackground(255, 244, 229);
const QColor kReverseLineBackground(232, 244, 255);

QColor rowBaseBackground(int row)
{
    return row <= kForwardTimeRow ? kForwardLineBackground : kReverseLineBackground;
}

QString verticalHeaderText(int row)
{
    switch (row) {
    case kForwardLineRow:
        return QStringLiteral("正向触发线");
    case kForwardCoordRow:
        return QStringLiteral("触发坐标");
    case kForwardTimeRow:
        return QStringLiteral("触发时刻");
    case kReverseLineRow:
        return QStringLiteral("反向触发线");
    case kReverseCoordRow:
        return QStringLiteral("触发坐标");
    case kReverseTimeRow:
        return QStringLiteral("触发时刻");
    default:
        return QString();
    }
}

QString horizontalHeaderText(const BlockPlanResult& result, int cycleId)
{
    int forwardIndex = 0;
    int reverseIndex = 0;

    for (const auto& line : result.triggerLines) {
        if (line.blockId != cycleId) {
            continue;
        }
        if (line.direction > 0) {
            forwardIndex = line.lineIndex;
        } else if (line.direction < 0) {
            reverseIndex = line.lineIndex;
        }
    }

    if (forwardIndex > 0 && reverseIndex > 0) {
        return QString("正T%1\n反T%2")
            .arg(forwardIndex, 2, 10, QChar('0'))
            .arg(reverseIndex, 2, 10, QChar('0'));
    }
    if (forwardIndex > 0) {
        return QString("正T%1").arg(forwardIndex, 2, 10, QChar('0'));
    }
    if (reverseIndex > 0) {
        return QString("反T%1").arg(reverseIndex, 2, 10, QChar('0'));
    }
    return QString("T%1").arg(cycleId, 2, 10, QChar('0'));
}
}

TableWidget::TableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    setMinimumSize(400, 400);
    initTable();
}

void TableWidget::initTable()
{
    setColumnCount(0);
    setRowCount(0);
    setAlternatingRowColors(false);
    setWordWrap(true);
    setTextElideMode(Qt::ElideNone);
    setSortingEnabled(false);
    setSelectionMode(QAbstractItemView::NoSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setStyleSheet("QTableWidget { border: 1px solid #ddd; gridline-color: #d8d8d8; } "
                  "QTableWidget::item { padding: 3px; } "
                  "QHeaderView::section { background-color: #f0f0f0; border: 1px solid #ddd; padding: 3px; }");

    QFont tableFont = font();
    tableFont.setPointSize(std::max(7, tableFont.pointSize() - 2));
    setFont(tableFont);

    QFont headerFont = horizontalHeader()->font();
    headerFont.setPointSize(std::max(7, headerFont.pointSize() - 2));
    horizontalHeader()->setFont(headerFont);
    horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setMinimumSectionSize(72);

    QFont verticalFont = verticalHeader()->font();
    verticalFont.setPointSize(std::max(7, verticalFont.pointSize() - 1));
    verticalHeader()->setFont(verticalFont);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(kCompactRowHeight);
    verticalHeader()->setMinimumWidth(92);
}

void TableWidget::setBlockPlanResult(const BlockPlanResult& result)
{
    m_result = result;
    updateTableData();
}

void TableWidget::setTriggerHistory(const QVector<QStringList>& history)
{
    m_triggerHistory = history;
    updateTableData();
}

void TableWidget::setHighlightedCells(const QVector<QPoint>& cells)
{
    m_highlightedCells = cells;
    updateTableData();
}

void TableWidget::clearData()
{
    m_result = BlockPlanResult();
    m_triggerHistory.clear();
    m_highlightedCells.clear();
    updateTableData();
}

void TableWidget::updateTableData()
{
    clearContents();

    QStringList horizontalHeaders;
    int cycleCount = 0;
    for (const auto& line : m_result.triggerLines) {
        cycleCount = std::max(cycleCount, line.blockId);
    }
    horizontalHeaders.reserve(cycleCount);
    for (int cycleId = 1; cycleId <= cycleCount; ++cycleId) {
        horizontalHeaders << horizontalHeaderText(m_result, cycleId);
    }

    setColumnCount(horizontalHeaders.size());
    setHorizontalHeaderLabels(horizontalHeaders);

    QStringList verticalHeaders;
    for (int row = 0; row < kSemanticRowCount; ++row) {
        verticalHeaders << verticalHeaderText(row);
    }
    setRowCount(kSemanticRowCount);
    setVerticalHeaderLabels(verticalHeaders);

    for (int row = 0; row < kSemanticRowCount; ++row) {
        setRowHeight(row, kCompactRowHeight);
        for (int column = 0; column < columnCount(); ++column) {
            QString cellText;
            if (column < m_triggerHistory.size() && row < m_triggerHistory[column].size()) {
                cellText = m_triggerHistory[column][row];
            }

            auto* cell = new QTableWidgetItem(cellText);
            cell->setTextAlignment(Qt::AlignCenter);
            cell->setBackground(QBrush(rowBaseBackground(row)));

            if (m_highlightedCells.contains(QPoint(column, row))) {
                cell->setBackground(QBrush(kHighlightBackground));
            }

            setItem(row, column, cell);
        }
    }
}

#include "tablewidget.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QMouseEvent>
#include <QStringList>
#include "../core/common/logger.h"

namespace {
QString formatBoundary(const std::vector<BoundaryVertex>& vertices)
{
    QStringList parts;
    parts.reserve(static_cast<int>(vertices.size()));
    for (const auto& vertex : vertices) {
        parts << QString("(%1,%2)")
                     .arg(vertex.x, 0, 'f', 4)
                     .arg(vertex.y, 0, 'f', 4);
    }
    return parts.join(";");
}
}

/**
 * @brief 构造函数
 * @param parent 父组件
 */
TableWidget::TableWidget(QWidget *parent) : QTableWidget(parent)
{
    setMinimumSize(400, 400);
    initTable();
}

/**
 * @brief 初始化表格
 */
void TableWidget::initTable()
{
    setColumnCount(5);

    QStringList headers;
    headers << "小区序号" << "正向边界" << "反向边界" << "种子编号" << "播种状态";
    setHorizontalHeaderLabels(headers);

    setAlternatingRowColors(true);
    setStyleSheet("QTableWidget { border: 1px solid #ddd; } "
                  "QTableWidget::item { padding: 5px; } "
                  "QHeaderView::section { background-color: #f0f0f0; border: 1px solid #ddd; padding: 5px; } "
                  "QTableWidget::item:alternate { background-color: #f9f9f9; } ");

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setSortingEnabled(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

/**
 * @brief 设置小区规划结果
 * @param result 小区规划结果
 */
void TableWidget::setBlockPlanResult(const BlockPlanResult& result)
{
    m_result = result;
    updateTableData();
}

/**
 * @brief 更新指定小区的播种状态
 * @param blockId 小区编号
 * @param status 播种状态
 */
void TableWidget::updateBlockStatus(int blockId, const QString& status)
{
    for (int i = 0; i < m_result.blocks.size(); ++i) {
        if (m_result.blocks[i].blockId == blockId) {
            for (int row = 0; row < rowCount(); ++row) {
                QTableWidgetItem* seedItem = item(row, 3);
                if (seedItem && seedItem->text() == QString::number(blockId)) {
                    setItem(row, 4, new QTableWidgetItem(status));
                    return;
                }
            }
        }
    }
}

/**
 * @brief 清空表格数据
 */
void TableWidget::clearData()
{
    clearContents();
    setRowCount(0);
    m_result = BlockPlanResult();
}

/**
 * @brief 获取当前选中的小区编号
 */
int TableWidget::getSelectedBlockId() const
{
    const int row = currentRow();
    if (row < 0 || row >= static_cast<int>(m_result.blocks.size())) {
        return 0;
    }
    return m_result.blocks[row].blockId;
}

/**
 * @brief 更新表格
 */
void TableWidget::updateTableData()
{
    clearContents();
    setRowCount(static_cast<int>(m_result.blocks.size()));

    for (int i = 0; i < m_result.blocks.size(); ++i) {
        const BlockSpec& block = m_result.blocks[i];

        QString startRidgeStr = QString::number(block.startRow, 10).rightJustified(4, '0');
        QString endRidgeStr = QString::number(block.endRow, 10).rightJustified(4, '0');

        int headlandIndex = 0;
        for (const auto& headland : m_result.headlands) {
            if (headland.yStart >= block.yEnd) {
                headlandIndex = headland.headlandId;
                break;
            }
        }
        QString headlandStr = QString::number(headlandIndex, 10).rightJustified(3, '0');
        QString blockCode = QString("%1%2-%3").arg(startRidgeStr).arg(endRidgeStr).arg(headlandStr);

        QString forwardBoundary;
        QString reverseBoundary;
        for (const auto& boundary : m_result.boundaries) {
            if (boundary.blockId == block.blockId) {
                forwardBoundary = formatBoundary(boundary.forwardBoundary);
                reverseBoundary = formatBoundary(boundary.reverseBoundary);
                break;
            }
        }

        setItem(i, 0, new QTableWidgetItem(blockCode));
        setItem(i, 1, new QTableWidgetItem(forwardBoundary));
        setItem(i, 2, new QTableWidgetItem(reverseBoundary));
        setItem(i, 3, new QTableWidgetItem(QString::number(block.blockId)));
        setItem(i, 4, new QTableWidgetItem("未播种"));

        for (int j = 0; j < columnCount(); ++j) {
            if (item(i, j)) {
                item(i, j)->setTextAlignment(Qt::AlignCenter);
            }
        }
    }

    horizontalHeader()->resizeSection(1, 320);
    horizontalHeader()->resizeSection(2, 320);
}

/**
 * @brief 获取小区规划结果
 * @return 小区规划结果
 */
BlockPlanResult TableWidget::getBlockPlanResult() const
{
    return m_result;
}

/**
 * @brief 鼠标按下事件
 * @param event 鼠标事件
 */
void TableWidget::mousePressEvent(QMouseEvent *event)
{
    QTableWidget::mousePressEvent(event);

    const int selectedRow = currentRow();
    if (selectedRow >= 0 && selectedRow < m_result.blocks.size()) {
        emit blockSelected(m_result.blocks[selectedRow].blockId);
    }
}

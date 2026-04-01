#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QPoint>
#include <QTableWidget>
#include <QStringList>
#include <QVector>

#include "blockgenerator.h"

/**
 * @brief 表格组件，用于展示触发线定义和触发历史记录
 */
class TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit TableWidget(QWidget *parent = nullptr);

    void setBlockPlanResult(const BlockPlanResult& result);
    void setTriggerHistory(const QVector<QStringList>& history);
    void setHighlightedCells(const QVector<QPoint>& cells);
    void clearData();

private:
    void initTable();
    void updateTableData();

private:
    BlockPlanResult m_result;
    QVector<QStringList> m_triggerHistory;
    QVector<QPoint> m_highlightedCells;
};

#endif // TABLEWIDGET_H

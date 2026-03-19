#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>
#include <QMouseEvent>
#include "blockgenerator.h"

/**
 * @brief 表格组件，用于显示小区数据，效果相当于嵌入Excel界面
 * 
 * 只负责展示和选择，不负责几何计算
 */
class TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父组件
     */
    explicit TableWidget(QWidget *parent = nullptr);

    /**
     * @brief 设置小区规划结果
     * @param result 小区规划结果
     */
    void setBlockPlanResult(const BlockPlanResult& result);

    /**
     * @brief 更新指定小区的播种状态
     * @param blockId 小区编号
     * @param status 播种状态
     */
    void updateBlockStatus(int blockId, const QString& status);

    /**
     * @brief 清空表格数据
     */
    void clearData();

    /**
     * @brief 获取当前选中的小区编号
     * @return 小区编号，如果没有选中返回0
     */
    int getSelectedBlockId() const;

    /**
     * @brief 获取小区规划结果
     * @return 小区规划结果
     */
    BlockPlanResult getBlockPlanResult() const;

private:
    /**
     * @brief 初始化表格
     */
    void initTable();

    /**
     * @brief 更新表格数据
     */
    void updateTableData();

private:
    BlockPlanResult m_result; ///< 小区规划结果

signals:
    /**
     * @brief 当用户点击表格行时发出
     * @param blockId 小区编号
     */
    void blockSelected(int blockId);

protected:
    /**
     * @brief 鼠标按下事件
     * @param event 鼠标事件
     */
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // TABLEWIDGET_H

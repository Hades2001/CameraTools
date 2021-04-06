#ifndef NOFOCUSFRAMEDELEGATE_H
#define NOFOCUSFRAMEDELEGATE_H

#include <QWidget>
#include <QStyledItemDelegate>

class NoFocusFrameDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit NoFocusFrameDelegate(QWidget *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:

public slots:
};

#endif // NOFOCUSFRAMEDELEGATE_H

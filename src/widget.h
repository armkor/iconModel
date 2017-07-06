#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QListview>
#include <QFileSystemModel>
#include <QStyledItemDelegate>
#include <QLabel>
#include <QModelIndex>
#include <QMutex>
#include <QProgressDialog>


class IconModel;

namespace Ui {
class Widget;
}
class Widget;


class MyDelegate: public QStyledItemDelegate
{
public:
    MyDelegate(){}
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


class Logic: public QObject
{
    Q_OBJECT

public:
    Logic(Widget *_parent = 0);
    ~Logic(){}

    void setModelIndex(QModelIndex);
    void setIsStop(bool);
    bool isStopped();

signals:
    void isEmpty(bool);
    void workFinished();
    void setMax(int);
    void setProgress(int);

public slots:
    void selectDir();

private:
    QMutex	m_mutex;
    Widget* parent;
    QModelIndex ind;
    bool isStop;
};

class MyListWidget:public QListView
{
public:
    MyListWidget(QWidget *parent = 0);
    ~MyListWidget(){}
protected:
   void wheelEvent ( QWheelEvent * event );
private:
    qreal scale;
};


class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
public slots:
    void showIfEmpty(bool isEmpty);
    void prepareToShow(QModelIndex);
signals:
    void startTask();

public:
    QFileSystemModel *model;
    IconModel *iconModel;
    MyListWidget* listWidget;
    QProgressDialog *pd;

private:
    Logic* logic;
    Ui::Widget *ui;
};

#endif // WIDGET_H

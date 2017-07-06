#include "widget.h"
#include "ui_widget.h"
#include "iconmodel.h"

#include <QFileSystemModel>
#include <QWheelEvent>
#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include <QPainter>

void MyDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setPen(QPen(Qt::black, 1));
    painter->drawRect(option.rect.x()+1, option.rect.y(), option.rect.width()-1, option.rect.height());

    QStyledItemDelegate::paint(painter, option, index);
}

Logic::Logic(Widget *_parent)
{
    isStop = false;
    parent = _parent;
}

void Logic::setModelIndex(QModelIndex ind_)
{
    ind = ind_;
}

void Logic::selectDir()
{
    setIsStop(false);
    QModelIndex index = ind;
    QString filePath = parent->model->filePath(index);
    QDir dir(filePath);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.giff" << "*.tiff";
    QStringList fileInfoList = dir.entryList(filters, QDir::Files|QDir::NoDotAndDotDot);

    emit isEmpty(fileInfoList.isEmpty());
    if (!fileInfoList.isEmpty()) emit setMax(fileInfoList.size());

    parent->iconModel->clear();

    QList<QPixmap> imgList;

    int count = 0, totalCount = 0;
    Q_FOREACH(QString str, fileInfoList)
    {
        QPixmap newImage, scaledImage(QSize(200, 200));
        scaledImage.fill();
        if (newImage.load(filePath + "/" +  str))
        {
            newImage = newImage.scaled(QSize(200, 200), Qt::KeepAspectRatio);
            QPainter painter(&scaledImage);
            if (scaledImage.height() > newImage.height())
            {
                painter.drawPixmap(0,(scaledImage.height()-newImage.height())/2,newImage);
            }
            else
            {
                painter.drawPixmap((scaledImage.width()-newImage.width())/2, 0,newImage);
            }

            imgList.append(scaledImage);
        }


        count++;
        if ( count == 3)
        {
            if (isStopped()) return;
            emit setProgress(totalCount);
            parent->iconModel->addPieces(imgList);
            count = 0;
            imgList.clear();
        }
        totalCount++;
    }
    if (isStopped()) return;
    emit setProgress(totalCount);
    parent->iconModel->addPieces(imgList);
}

void Logic::setIsStop(bool value)
{
    m_mutex.lock();
    isStop = value;
    m_mutex.unlock();
}

bool Logic::isStopped()
{
    QMutexLocker locker (&m_mutex);
    return isStop;
}

MyListWidget::MyListWidget(QWidget *parent):QListView(parent)
{
    setViewMode(QListView::IconMode);
    setIconSize(QSize(200,200));
    setResizeMode(QListView::Adjust);
    setItemDelegate(new MyDelegate);
}


 void MyListWidget::wheelEvent ( QWheelEvent * event )
 {
     int newSize = iconSize().height();

     double scaleFactor = 1.10;
     if(event->delta() > 0) {
         // Zoom in
         newSize *= scaleFactor;

     } else {
         // Zooming out
           newSize *= 1.0 / scaleFactor;
     }

     if (newSize > 200) newSize = 200;
     if (newSize < 50) newSize = 50;
     setIconSize(QSize(newSize, newSize));
 }


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    logic(0),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    listWidget = new MyListWidget(this);
    pd = new QProgressDialog(this);
    pd->setCancelButton(0);
    pd->hide();

    QVBoxLayout *layoutW = new QVBoxLayout;
    layoutW->addWidget(listWidget);
    layoutW->addWidget(pd);
    showIfEmpty(false);
    ui->widget->setLayout(layoutW);



    model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());
    model->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    ui->fsTree->setModel(model);
    ui->fsTree->hideColumn(1);
    ui->fsTree->hideColumn(2);
    ui->fsTree->hideColumn(3);
    ui->fsTree->hideColumn(4);
    ui->fsTree->hideColumn(5);
    ui->fsTree->hideColumn(6);

    connect(ui->fsTree, SIGNAL(clicked(QModelIndex)), SLOT(prepareToShow(QModelIndex)));

    iconModel = new IconModel(200, this);

    listWidget->setModel(iconModel);


    logic = new Logic(this);
    connect(logic, SIGNAL(isEmpty(bool)), SLOT(showIfEmpty(bool)));
    connect(logic, SIGNAL(setMax(int)), pd, SLOT(setMaximum(int)));
    connect(logic, SIGNAL(setProgress(int)), pd, SLOT(setValue(int)));


    QThread *thread = new QThread();
    logic->moveToThread(thread);

    connect(this, SIGNAL(startTask()), logic, SLOT(selectDir()));
    connect( logic, SIGNAL(workFinished()), thread, SLOT(quit()) );
    connect( thread, SIGNAL(finished()), logic, SLOT(deleteLater()) );
    connect( thread, SIGNAL(finished()), thread, SLOT(deleteLater()) );

    thread->start();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::showIfEmpty(bool isEmpty)
{
    ui->widget->setVisible(!isEmpty);
    ui->noImageLbl->setVisible(isEmpty);
}

void Widget::prepareToShow(QModelIndex ind)
{
     logic->setModelIndex(ind);
     logic->setIsStop(true);
     emit startTask();
}

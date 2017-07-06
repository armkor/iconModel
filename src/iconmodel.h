#ifndef ICONMODEL_H
#define ICONMODEL_H
#include <QAbstractListModel>
#include <QList>
#include <QPixmap>
#include <QPoint>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE

class IconModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit IconModel(int pieceSize, QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool removeRows(int row, int count, const QModelIndex &parent) Q_DECL_OVERRIDE;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) Q_DECL_OVERRIDE;
    QMimeData *mimeData(const QModelIndexList &indexes) const Q_DECL_OVERRIDE;
    QStringList mimeTypes() const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;

    void addPiece(const QPixmap &pixmap);
    void addPieces(QList<QPixmap> pixmaps);

    void clear();

private:
    QList<QPixmap> pixmaps;

    int m_PieceSize;
};

#endif // ICONMODEL_H

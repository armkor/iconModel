#include "iconmodel.h"
#include <QIcon>
#include <QMimeData>

IconModel::IconModel(int pieceSize, QObject *parent)
    : QAbstractListModel(parent), m_PieceSize(pieceSize)
{
}

QVariant IconModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole)
        return QIcon(pixmaps.value(index.row()).scaled(m_PieceSize, m_PieceSize,
                         Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else if (role == Qt::UserRole)
        return pixmaps.value(index.row());

    return QVariant();
}

void IconModel::addPiece(const QPixmap &pixmap)
{
    int row;
    if (int(2.0 * qrand() / (RAND_MAX + 1.0)) == 1)
        row = 0;
    else
        row = pixmaps.size();

    beginInsertRows(QModelIndex(), row, row);
    pixmaps.insert(row, pixmap);
    endInsertRows();
}

Qt::ItemFlags IconModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return (QAbstractListModel::flags(index)|Qt::ItemIsDragEnabled);

    return Qt::ItemIsDropEnabled;
}

bool IconModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    if (row >= pixmaps.size() || row + count <= 0)
        return false;

    int beginRow = qMax(0, row);
    int endRow = qMin(row + count - 1, pixmaps.size() - 1);

    beginRemoveRows(parent, beginRow, endRow);

    while (beginRow <= endRow) {
        pixmaps.removeAt(beginRow);
        ++beginRow;
    }

    endRemoveRows();
    return true;
}

QStringList IconModel::mimeTypes() const
{
    QStringList types;
    types << "image/x-puzzle-piece";
    return types;
}

QMimeData *IconModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            QPixmap pixmap = qvariant_cast<QPixmap>(data(index, Qt::UserRole));
            QPoint location = data(index, Qt::UserRole+1).toPoint();
            stream << pixmap << location;
        }
    }

    mimeData->setData("image/x-puzzle-piece", encodedData);
    return mimeData;
}

bool IconModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                               int row, int column, const QModelIndex &parent)
{
    if (!data->hasFormat("image/x-puzzle-piece"))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    int endRow;

    if (!parent.isValid()) {
        if (row < 0)
            endRow = pixmaps.size();
        else
            endRow = qMin(row, pixmaps.size());
    } else {
        endRow = parent.row();
    }

    QByteArray encodedData = data->data("image/x-puzzle-piece");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        QPixmap pixmap;
        QPoint location;
        stream >> pixmap >> location;

        beginInsertRows(QModelIndex(), endRow, endRow);
        pixmaps.insert(endRow, pixmap);
        endInsertRows();

        ++endRow;
    }

    return true;
}

int IconModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return pixmaps.size();
}

Qt::DropActions IconModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

void IconModel::addPieces(QList<QPixmap> _pixmaps)
{
    int row = pixmaps.size();
    int mapSize = _pixmaps.size();
    if (mapSize == 0) return;

    beginInsertRows(QModelIndex(), row, row + mapSize - 1);
    pixmaps.append(_pixmaps);


    endInsertRows();
}

void IconModel::clear()
{
    if (pixmaps.isEmpty()) return;
    beginRemoveRows(QModelIndex(), 0, pixmaps.size() - 1);
    pixmaps.clear();
    endRemoveRows();
}

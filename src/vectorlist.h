#ifndef VECTORLIST_H
#define VECTORLIST_H

#include <QTableWidget>
#include "datacontainer.h"

class VectorList : public QTableWidget
{
    Q_OBJECT

    public:
        explicit VectorList(QWidget *parent = 0);
        virtual ~VectorList();

        void setData(DataContainer *filedataPass);

    signals:
        void fileClicked(int rowSelected);

    public slots:
        void populate();
        void update();
        void highlightRow(int rowSelected);
        void clearHighlight();

    protected:
        void setupConnections();

    protected slots:
        void rowSelect(int row, int column);
        void rowDoubleClicked(int row, int column);

    private:
        DataContainer *filedata;
        int rowHeight;
        int rows;
        int rowSelected;
        int previousRow;
};

#endif // VECTORLIST_H

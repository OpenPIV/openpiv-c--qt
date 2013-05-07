#ifndef COLOURBAR_H
#define COLOURBAR_H

#include <QWidget>
#include <QLabel>


class ColourBar : public QWidget
{
    Q_OBJECT
    public:
        explicit ColourBar(QWidget *parent = 0);

        QColor color(int percentile);
        void setRange(int minPass, int maxPass);

    signals:

    public slots:

    protected:
        void paintEvent(QPaintEvent *event);
        void drawLabels();
        void generateGradient();

    private:
        int min, max;
        double zero;
        QLabel *minLabel;
        QLabel *maxLabel;

        int labelWidth;
        int labelHeight;

        int nBins;
        QList<QColor> colourList;
        QLinearGradient linearGrad;
        bool onlyPositive;
        QRect colourBarRect;
};

#endif // COLOURBAR_H

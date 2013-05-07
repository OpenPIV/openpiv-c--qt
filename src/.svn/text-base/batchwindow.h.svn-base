#ifndef BATCHWINDOW_H
#define BATCHWINDOW_H

#include <QDialog>

#include "settings.h"
//#include "processing.h"
#include "ui_batchwindow.h"

class BatchWindow : public QDialog, private Ui::BatchWindow
{
    Q_OBJECT

    public:
        explicit BatchWindow(QWidget *parent = 0);
        virtual ~BatchWindow();

        void setSettings(Settings *settingsPass);
        void refresh();
        //void setProcess(Processing *processPass);

    signals:
        void startProcess();
        void stopProcess();

    public slots:
        void setProgressRange(int max);
        void addToProgress();

    protected:
        void setupConnections();

    protected slots:
        void refreshEnabled();
        void commitSettings();

        void startClicked();
        void stopClicked();

    private:
        Settings *_settings;
  //      Processing *_process;
        int progress;

};

#endif // BATCHWINDOW_H

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class QTcpSocket;
class QLabel;
class QStatusBar;

namespace Ui {
    class Dialog;
}

class Dialog : public QDialog {
    Q_OBJECT
public:
    enum ConState {
        awaitingClue,
        acceptingInvitation
    }state;

    Dialog(QWidget *parent = 0);
    ~Dialog();

public slots:
    void processReadyRead();
    void setClueImage();
    void setInviteImage();
    void connectToHost();
    void sendAnswer();
    void connected();
    void disconnected();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Dialog *ui;
    int blockSize;
    QPixmap clue;
    int inviteSize;
    QPixmap invite;
    QTcpSocket *socket;

    QLabel *statusLabel;
    QStatusBar *statusBar;
};

#endif // DIALOG_H

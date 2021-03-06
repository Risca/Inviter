/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

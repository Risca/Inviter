#include "dialog.h"
#include "ui_dialog.h"
#include <QTcpSocket>
#include <QLabel>
#include <QStatusBar>
#include <QBuffer>
#include <QImageReader>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    socket(new QTcpSocket),
    statusLabel(new QLabel("Disconnected", this)),
    statusBar(new QStatusBar(this))
{
    ui->setupUi(this);
    statusBar->addWidget(statusLabel);
    ui->verticalLayout->addWidget(statusBar);

    blockSize = 0;
    inviteSize = 0;
    clue.load("clue.jpg");

    state = awaitingClue;
    connect(socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectToHost()));
    connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(sendAnswer()));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::connectToHost()
{
    socket->abort();
    socket->connectToHost("127-0-0-1.no-ip.org", 1337);
}

void Dialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Dialog::processReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_0);

    if (blockSize == 0) {
        if (socket->bytesAvailable() < (int)sizeof(quint32))
            return;
        in >> blockSize;
        statusLabel->setText("Receiving image...");
    }

    if (socket->bytesAvailable() < blockSize)
             return;

    switch(state) {
    case awaitingClue:
        if(clue.loadFromData(socket->read(blockSize)))
            setClueImage();
        blockSize = 0;
        break;
    case acceptingInvitation:
        if(invite.loadFromData(socket->read(blockSize)))
            setInviteImage();
        blockSize = 0;
        break;
    default:
        break;
    }
    statusLabel->setText("Connected! Send your answers NOW!");
}

void Dialog::setClueImage()
{
    ui->clueLabel->setPixmap(clue);
    statusBar->showMessage("You got the clue!", 2000);
    state = acceptingInvitation;
}

void Dialog::setInviteImage()
{
    ui->inviteLabel->setPixmap(invite);
    adjustSize();
    statusBar->showMessage("You got an invitation... maybe...", 2000);
}

void Dialog::sendAnswer()
{
    QByteArray answer = ui->answerLineEdit->text().toUtf8();
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)answer.size();
    data.append(answer);

    if (socket->write(data) == data.size()) {
        statusBar->showMessage("Answer Sent!", 1000);
        statusLabel->setText("Awaiting image...");
    }
    else
        statusBar->showMessage("Couldn't send answer (some shit is wrong)!", 2000);
}

void Dialog::connected()
{
    ui->sendButton->setEnabled(true);
    statusLabel->setText("Connected! Send your answers NOW!");
}

void Dialog::disconnected()
{
    ui->clueLabel->clear();
    ui->sendButton->setDisabled(true);
    ui->inviteLabel->clear();
    statusLabel->setText("Disconnected");
    adjustSize();
    state = awaitingClue;
}

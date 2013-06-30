#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtNetwork>
#include <QMainWindow>
#include <QtNetwork/qudpsocket.h>
namespace Ui {
class MainWindow;
}

enum MessageType{Message,NewParticipant,ParticipantLeft};
//枚举变量标志信息的类型，分别为消息，新用户加入，和用户退出

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString getUserName();

private:
    Ui::MainWindow *ui;
    QUdpSocket *sender;
    QUdpSocket *receiver;

    QString localMessage; // 存放本地要发送的信息
    QString serverMessage;  //存放从服务器接收到的信息

    QString hostIP;
    QString UserName;
    QString getHostIP();

protected:
    void newParticipant(QString userName,QString localHostName,QString ipAddress);
    void participantLeft(QString userName,QString localHostName,QString time);

private slots:

    void sendMessage(MessageType type);
    void readMessage();
    void showMessage(bool isSend);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    bool eventFilter(QObject *target, QEvent *event);

};

#endif // MAINWINDOW_H



#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QPixmap>

int flag = 0;

void sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

QString MainWindow::getHostIP() //获取ip地址
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
       if(address.protocol() == QAbstractSocket::IPv4Protocol) //我们使用IPv4地址
            return address.toString();
    }
    return 0;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->plainTextEdit->installEventFilter(this);

    sender = new QUdpSocket(this);
    receiver = new QUdpSocket(this);   
    receiver->bind(45454,QUdpSocket::ShareAddress);
    connect(receiver,SIGNAL(readyRead()),this,SLOT(readMessage()));
    QString localHostName = QHostInfo::localHostName();
    //UserName = getUserName();
    hostIP = getHostIP();

    sendMessage(NewParticipant);

    if (hostIP == 0)
            ui->lineEdit->setText("127.0.0.1");
    else
    ui->lineEdit->setText(hostIP);
    ui->lineEdit_2->setText("45454");
    ui->lineEdit_3->setText(localHostName);


    ui->label_5->setAlignment(Qt::AlignCenter);
    ui->label_5->setScaledContents(true);
    QPixmap qp("images/show.jpg");
    ui->label_5->setPixmap(qp);
}
/*获取主机名*/
QString MainWindow::getUserName()
{
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();
    foreach (QString string, envVariables)
    {
        int index = environment.indexOf(QRegExp(string));
        if (index != -1)
        {

            QStringList stringList = environment.at(index).split('=');
            if (stringList.size() == 2)
            {
                return stringList.at(1);
                break;
            }
        }
    }
    return false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*发送消息*/
void MainWindow::sendMessage(MessageType type)
{
    localMessage = ui->plainTextEdit->toPlainText();
    /*
    QByteArray datagram = localMessage.toLatin1();
    sender->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, 45454);    //进行UDP数据报（datagrams）的发送
    */
    flag = 1;
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString address = getHostIP();
    QString localHostName = QHostInfo::localHostName();
    out << type <<getUserName() << localHostName;

    switch(type)
    {
        case ParticipantLeft:
            {
                break;
            }
        case NewParticipant:
            {
                out << address;
                sender->writeDatagram(data,data.length(),QHostAddress::Broadcast, 45454);
                break;
            }

        case Message :
            {
                if(ui->plainTextEdit->toPlainText() == "")
                {
                    QMessageBox::warning(0,tr("警告"),tr("发送内容不能为空"),QMessageBox::Ok);
                    return;
                }
                out << ui->plainTextEdit->toPlainText();
                sender->writeDatagram(data,data.length(),QHostAddress::Broadcast, 45454);
                showMessage(true);
                break;
            }
    }

         // 显示消息
}

/* 接收信息*/
void MainWindow::readMessage()
{
    while(receiver->hasPendingDatagrams())  //拥有等待的数据报
    {
        QByteArray datagram;        //拥于存放接收的数据报
        datagram.resize(receiver->pendingDatagramSize());       //让datagram的大小为等待处理的数据报的大小，这样才能接收到完整的数据
        receiver->readDatagram(datagram.data(),datagram.size());        //接收数据报，将其存放到datagram中
        QDataStream in(&datagram,QIODevice::ReadOnly);
        //in >> serverMessage;        //将数据报内容显示出来


        int messageType;
        in >> messageType;
        QString userName,localHostName,ipAddress,message;
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        switch(messageType)
        {
            case Message:
                {
                    in >>userName >>localHostName >>ipAddress >>message;
                    serverMessage = message;
                    showMessage(false);         // 显示消息
                    break;
                }
            case NewParticipant:
                {
                    in >>userName >>localHostName >>ipAddress;
                    newParticipant(userName,localHostName,ipAddress);

                    break;
                }
            case ParticipantLeft:
                {
                    in >>userName >>localHostName;
                    participantLeft(userName,localHostName,time);
                    break;
                }
        }
    }
    //showMessage(false);         // 显示消息
}

/* 显示消息*/
void MainWindow::showMessage(bool isSend)
{
    QDateTime time = QDateTime::currentDateTime();  //获取系统现在的时间
    QString str = time.toString("hh:mm:ss ddd");     //设置显示格式
    QFont font;
    font.setPixelSize(18);
    ui->textBrowser->setFont(font);
    if(flag == 0 || flag == 1)
    {
        if (isSend)
        {
            ui->textBrowser->setTextColor(QColor(0, 0, 0));     // 用不同的颜色显示信息所属和当前时间
            QString entraMess = "I say: " + str;
            ui->textBrowser->append(entraMess);

            ui->textBrowser->setTextColor(QColor(0, 0, 255));
            ui->textBrowser->append(localMessage);
            ui->plainTextEdit->clear();
            ui->label_3->setText("Status: send message successfully...");
         }
        else
        {           
            QPixmap qpq1("images/shou1.jpg");
            ui->label_5->setPixmap(qpq1);
            sleep(200);
            QPixmap qpq2("images/shou2.jpg");
            ui->label_5->setPixmap(qpq2);

            ui->textBrowser->setTextColor(QColor(0, 0, 0));     // 用不同的颜色显示信息所属和当前时间
            QString entraMess = "He/she: " + str;
            ui->textBrowser->append(entraMess);

            ui->textBrowser->setTextColor(QColor(255, 0, 0));
            ui->textBrowser->append(serverMessage);
            ui->label_3->setText("Status: new message coming...");
        }
    }
    if(flag != 0)
        flag++;
    if(flag >= 4)
        flag = 0;
}
/*点击发送按钮*/
void MainWindow::on_pushButton_clicked()
{
    QPixmap qpq("images/fa1.jpg");
    ui->label_5->setPixmap(qpq);
    sleep(200);
    QPixmap qpq0("images/fa2.jpg");
    ui->label_5->setPixmap(qpq0);
    sendMessage(Message);
}
/*点击关闭按钮*/
void MainWindow::on_pushButton_2_clicked()
{
    this->close();
}
/*删除聊天记录按钮*/
void MainWindow::on_pushButton_3_clicked()
{
    ui->plainTextEdit->clear();
    ui->textBrowser->clear();
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->plainTextEdit)
    {
        if(event->type() == QEvent::KeyPress)
        {
             QKeyEvent *k = static_cast<QKeyEvent *>(event);
             if(k->key() == Qt::Key_Return)
             {
                 on_pushButton_clicked();
                 return true;
             }
        }
    }
    return QWidget::eventFilter(target,event);
}

void MainWindow::newParticipant(QString userName,QString localHostName,QString ipAddress)
{
    bool bb = ui->tableWidget->findItems(localHostName,Qt::MatchExactly).isEmpty();
    if(bb)
    {
        QTableWidgetItem *user = new QTableWidgetItem(userName);
        QTableWidgetItem *host = new QTableWidgetItem(localHostName);
        QTableWidgetItem *ip = new QTableWidgetItem(ipAddress);
        ui->tableWidget->insertRow(0);
        ui->tableWidget->setItem(0,0,user);
        ui->tableWidget->setItem(0,1,host);
        ui->tableWidget->setItem(0,2,ip);
        ui->textBrowser->setTextColor(Qt::gray);
        ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
        ui->textBrowser->append(tr("%1 在线！").arg(userName));
        //ui->onlineUser->setText(tr("在线人数：%1").arg(ui->tableWidget->rowCount()));
        sendMessage(NewParticipant);
    }
}

//处理用户离开
void MainWindow::participantLeft(QString userName,QString localHostName,QString time)
{
    int rowNum = ui->tableWidget->findItems(localHostName,Qt::MatchExactly).first()->row();
    ui->tableWidget->removeRow(rowNum);
    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append(tr("%1 于 %2 离开！").arg(userName).arg(time));
    //ui->onlineUser->setText(tr("在线人数：%1").arg(ui->tableWidget->rowCount()));
}


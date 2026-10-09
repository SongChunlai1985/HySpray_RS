#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextCodec>
#include <QMessageBox>
#include <QTimer>
#include <QIntValidator>
#include <QButtonGroup>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QScrollBar>
//构造函数
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName("GBK");                                                       //字符转码
    //初始化变量
    preSndGrpIdx=0;
    timer1=new QTimer(this);                                                                       //初始化时间计时器
    serial=new QSerialPort(this);                                                                  //申请一个串口类对象
    intValidator=new QIntValidator(10,4000000,this);                                               //设定整数范围--自定义输入波特率
    QRegExp reg("[A-Fa-f0-9\\s]+");                                                                //正则表达式，只能输入16进制数字
    regExpValidator=new QRegExpValidator(reg,this);                                                //设定Hex发送时输入框设定
    labelStatus=new QLabel(this);                                                                  //状态栏标签
    ui->statusBar->addWidget(labelStatus,1);                                                       //占据整个状态栏

    fillCom();
    ui->comboBoxCom->setInsertPolicy(QComboBox::NoInsert);

    ui->comboBoxBaud->setInsertPolicy(QComboBox::InsertAlphabetically);                            //运行时输入波特率不会插入组合框
    ui->comboBoxBaud->addItem("1200",QSerialPort::Baud1200);
    ui->comboBoxBaud->addItem("2400",QSerialPort::Baud2400);
    ui->comboBoxBaud->addItem("4800",QSerialPort::Baud4800);
    ui->comboBoxBaud->addItem("9600",QSerialPort::Baud9600);
    ui->comboBoxBaud->addItem("19200",QSerialPort::Baud19200);
    ui->comboBoxBaud->addItem("38400",QSerialPort::Baud38400);
    ui->comboBoxBaud->addItem("57600",QSerialPort::Baud57600);
    ui->comboBoxBaud->addItem("115200",QSerialPort::Baud115200);
    ui->comboBoxBaud->addItem(tr("自定义"));
    ui->comboBoxBaud->setInsertPolicy(QComboBox::NoInsert);                                        //运行时输入波特率不会插入组合框

    ui->comboBoxDatabit->addItem(tr("5"),QSerialPort::Data5);
    ui->comboBoxDatabit->addItem(tr("6"),QSerialPort::Data6);
    ui->comboBoxDatabit->addItem(tr("7"),QSerialPort::Data7);
    ui->comboBoxDatabit->addItem(tr("8"),QSerialPort::Data8);
    ui->comboBoxDatabit->setCurrentIndex(3);                                                       //设置当前值为8

    ui->comboBoxParity->addItem(tr("无"),QSerialPort::NoParity);
    ui->comboBoxParity->addItem(tr("奇校验"),QSerialPort::OddParity);
    ui->comboBoxParity->addItem(tr("偶校验"),QSerialPort::EvenParity);
    ui->comboBoxParity->addItem(tr("置位校验"),QSerialPort::MarkParity);
    ui->comboBoxParity->addItem(tr("置零校验"),QSerialPort::SpaceParity);
    ui->comboBoxStop->addItem(tr("1"),QSerialPort::OneStop);
    ui->comboBoxStop->addItem(tr("1.5"),QSerialPort::OneAndHalfStop);
    ui->comboBoxStop->addItem(tr("2"),QSerialPort::TwoStop);

    ui->comboBoxFlow->addItem(tr("无"),QSerialPort::NoFlowControl);
    ui->comboBoxFlow->addItem(tr("RTS/CTS"),QSerialPort::HardwareControl);
    ui->comboBoxFlow->addItem(tr("XON/XOFF"),QSerialPort::SoftwareControl);

    ui->lineEditSendTime->setValidator(intValidator);
    ui->lineEditSendTime->setText(tr("100"));
    ui->plainTextEditRe->setReadOnly(true);
    ui->checkBoxAutoSend->setEnabled(false);
    ui->pushButtonSend1->setEnabled(false);

    rcvGroup=new QButtonGroup(this);
    sndGroup=new QButtonGroup(this);
    areaGroup=new QButtonGroup(this);
    //将两组单选框进行分组,并设置ID
    rcvGroup->addButton(ui->radioButtonReAscii,0);
    rcvGroup->addButton(ui->radioButtonReHex,1);
    sndGroup->addButton(ui->radioButtonSendAscii,0);
    sndGroup->addButton(ui->radioButtonSendHex,1);
    //设定单选框默认值
    ui->radioButtonReAscii->setChecked(true);
    ui->radioButtonSendAscii->setChecked(true);

    for (int i= 0; i< 256; i++) {
        ui->comboBox_2->addItem("喷头" + QString::number(i), i);
    }

    ui->comboBox->addItem("喷雾模式表", 1);
    ui->comboBox->addItem("单日定时表", 2);
    ui->comboBox->addItem("喷头分组表", 3);
    ui->comboBox->addItem("日任务表", 4);
    ui->comboBox->addItem("周任务表", 5);

    for (int i= 1; i< 11; i++) {
        ui->comboBox_3->addItem("ID:" + QString::number(i), i);
    }

    for (int i= 1; i< 49; i++) {
        ui->comboBox_6->addItem("CMD:" + QString::number(i), i);
    }

    for (int i= 1; i< 11; i++) {
        ui->comboBox_4->addItem("喷雾模式:" + QString::number(i), i);
    }

    for (int i= 1; i< 11; i++) {
        ui->comboBox_5->addItem("喷头组:" + QString::number(i), i);
    }

    ui->comboBox_8->setInsertPolicy(QComboBox::InsertAlphabetically);                            //运行时输入波特率不会插入组合框
    ui->comboBox_8->addItem("1200",QSerialPort::Baud1200);
    ui->comboBox_8->addItem("2400",QSerialPort::Baud2400);
    ui->comboBox_8->addItem("4800",QSerialPort::Baud4800);
    ui->comboBox_8->addItem("9600",QSerialPort::Baud9600);
    ui->comboBox_8->addItem("19200",QSerialPort::Baud19200);
    ui->comboBox_8->addItem("38400",QSerialPort::Baud38400);
    ui->comboBox_8->addItem("57600",QSerialPort::Baud57600);
    ui->comboBox_8->addItem("115200",QSerialPort::Baud115200);
    ui->comboBox_8->addItem(tr("自定义"));
    ui->comboBox_8->setInsertPolicy(QComboBox::NoInsert);                                          //运行时输入波特率不会插入组合框

    connect(sndGroup,SIGNAL(buttonClicked(int)),this,SLOT(sndCodeChange(int)));                    //当发送的数据类型发生变化时，清除所有发送框内容
    connect(ui->pushButtonSend1,SIGNAL(clicked()),this,SLOT(writeData()));                         //手动发送数据
    connect(timer1, SIGNAL(timeout()), this, SLOT(writeData()));                                   //自动向串口发送数据
    connect(serial,SIGNAL(readyRead()),this,SLOT(readData()));                                     //当串口数据准备好，读取数据
    connect(&n485.serialPort, &QSerialPort::readyRead, this, &MainWindow::read485);
}
//析构函数
MainWindow::~MainWindow()
{
    delete ui;
}
//填充端口
void MainWindow::fillCom()
{
    ui->comboBoxCom->clear();                                                                      //清除串口信息
    QString var;
    QList<QSerialPortInfo> ports=QSerialPortInfo::availablePorts();                                //扫描所有串口信息
    if(!ports.isEmpty())
    {
        foreach ( QSerialPortInfo info, ports )                                                     //遍历所有可以获得串口信息
        {
            var=info.portName()+":"+info.description();                                            //获取需要显示的信息
            ui->comboBoxCom->addItem(var,info.portName());                                         //添加扫描到串口的信息，用户数据位字符串--串口名称
            labelStatus->setText(tr("串口检测成功！"));
        }
    }else
    {
        labelStatus->setText((tr("<font color=red>错误：查询不到可用串口！</font>")));
    }
    ui->comboBoxCom->addItem(tr("自定义"));                                                           //添加自定义选项，用户数据为空

}
//自动发送复选框状态发生变化
void MainWindow::on_checkBoxAutoSend_toggled(bool ckd)
{
    int timeInterval=ui->lineEditSendTime->text().toInt();                                         //获取设定的时间间隔
    if(timeInterval<10)
    {
        labelStatus->setText(tr("<font color=red>错误：自动发送时间设定值小于10ms！</font>"));                    //提示错误信息
        ui->checkBoxAutoSend->setChecked(false);                                                   //复选框的状态不变
        return;
    }

    ui->lineEditSendTime->setEnabled(!ckd);
    ui->pushButtonSend1->setEnabled(!ckd);
    if(ckd)                                                                                        //自动发送字符串
    {
        timer1->start(timeInterval);                                                               //打开计时器
    }else
    {
        if(timer1->isActive())timer1->stop();                                                      //关闭计时器

    }
    labelStatus->setText(tr("发送时间设定成功"));
}
//清除接收区
void MainWindow::on_pushButtonClearRx_clicked()
{
    ui->plainTextEditRe->clear();
}

void MainWindow::on_pushButtonClearTx_clicked()                                                    //清除发送区
{
    ui->plainTextEdit_2->clear();
}
//检测串口信息
void MainWindow::on_pushButtonTest_clicked()
{
    fillCom();
}

void MainWindow::on_comboBoxBaud_currentIndexChanged(int idx)                                      //波特率组合框变化
{
    bool isCustomBaud=!ui->comboBoxBaud->itemData(idx).isValid();                                  //当前选项用户数据未添加
    ui->comboBoxBaud->setEditable(isCustomBaud);                                                   //是自定义选项，则可以编辑。若不是，则不可以编辑。
    if(isCustomBaud)
    {
        ui->comboBoxBaud->clearEditText();                                                         //清除文本内容。
        QLineEdit *lineEdit= ui->comboBoxBaud->lineEdit();                                         //返回编辑框
        lineEdit->setValidator(intValidator);

    }
}
//自定义串口名称
void MainWindow::on_comboBoxCom_currentIndexChanged(int idx)
{
    bool isCustomCom=!ui->comboBoxCom->itemData(idx).isValid();
    ui->comboBoxCom->setEditable(isCustomCom);
    if(isCustomCom)
        ui->comboBoxCom->clearEditText();
}
//读取串口数据
void MainWindow::readData()
{
    char c[4]={' ',' ',' ','\0'};
    QByteArray data=serial->readAll();
    if(ui->radioButtonReAscii->isChecked())                                                        //发送的是ASCII
    {
        ui->plainTextEditRe->insertPlainText(codec->toUnicode(data));
    }
    else                                                                                           //发送的是16进制数，需要将其转换成ASCII码
    {
        QByteArray dataHex=data.toHex().toUpper();                                                 //将16进制数变成字符串
        for(int i=0;i<dataHex.size();i+=2)
        {
            c[0]=dataHex.at(i);
            c[1]=dataHex.at(i+1);
            ui->plainTextEditRe->insertPlainText(c);
        }
    }

    QScrollBar *bar= ui->plainTextEditRe->verticalScrollBar();                                     //获取下拉框
    bar->setValue(bar->maximum());                                                                 //保持刷新
}

void MainWindow::writeData()                                                                       //写入串口数据
{
    QString strData;                                                                               //用来放置从lineEdit读取的数据
    QByteArray data;                                                                               //用来放置发送数据
    strData=ui->lineEditSend1->text();                                                             //获取要发送的内容

    data=strData.toLocal8Bit();                                                                    //将发送区字符串转换成ACSII
    if(!ui->radioButtonSendAscii->isChecked())
    {
        data=QByteArray::fromHex(data);                                                            //将字符转化为16进制
    }

    if(rs485)
    {
        write485(data);
    }
    else
    {
        serial->write(data);
    }
}

void MainWindow::sndCodeChange(int idx)                                                            //清除所有发送框内容
{
    if(idx!=preSndGrpIdx)                                                                          //数据变化
    {
        preSndGrpIdx=idx;

        if(idx==0)                                                                                 //发送ASCII
        {
            ui->lineEditSend1->setValidator(nullptr);
        }else                                                                                      //发送HEX
        {
            ui->lineEditSend1->setValidator(regExpValidator);
        }
    }
}

void MainWindow::on_radioButton_released()
{
    rs485 = 0;
}

void MainWindow::on_radioButton_2_released()
{
    rs485 = 1;
}

void MainWindow::on_pushButtonSend1_released()
{

}

void MainWindow::on_pushButtonCom_released()
{
    int baud = 0;

    if(ui->comboBoxBaud->currentIndex()==4)
    {
        baud = ui->comboBoxBaud->currentText().toInt();                                            //将字符转化成数字
    }else
    {
        baud = ui->comboBoxBaud->itemData(ui->comboBoxBaud->currentIndex()).toInt();
    }

    const QVariant &varCom = ui->comboBoxCom->itemData(ui->comboBoxCom->currentIndex());           //1.串口名称

    QString PortName;

    if(varCom.isValid())
    {
        PortName = varCom.toString();
    }
    else                                                                                           //若是自定义的情况
    {
        PortName = ui->comboBoxCom->currentText();
    }

    bool portopen = false;

    serial->setPortName(PortName);
    serial->setBaudRate(baud);                                                                     //2.设定波特率

    serial->setDataBits(static_cast<QSerialPort::DataBits>
                        (ui->comboBoxDatabit->itemData(ui->comboBoxDatabit->currentIndex()).toInt()));//3.设定数据位

    serial->setStopBits(static_cast<QSerialPort::StopBits>
                        (ui->comboBoxStop->itemData(ui->comboBoxStop->currentIndex()).toInt()));//4.设定停止位

    serial->setParity(static_cast<QSerialPort::Parity>
                      (ui->comboBoxParity->itemData(ui->comboBoxParity->currentIndex()).toInt()));//5.设置校验位

    serial->setFlowControl(static_cast<QSerialPort::FlowControl>
                           (ui->comboBoxFlow->itemData(ui->comboBoxFlow->currentIndex()).toInt()));//6.流控制

    portopen = serial->open(QIODevice::ReadWrite);                                                 //打开串口

    if(portopen)
    {
        ui->checkBoxAutoSend->setEnabled(true);
        ui->pushButtonSend1->setEnabled(true);
        labelStatus->setText(tr("串口打开成功！"));
    }
    else                                                                                           //串口打开错误
    {
        labelStatus->setText(tr("<font color=red>错误：串口打开错误！</font>"));
    }
}

void MainWindow::on_pushButton_open485_released()
{
    int baud = 0;

    if(ui->comboBoxBaud->currentIndex()==4)
    {
        baud = ui->comboBoxBaud->currentText().toInt();                                            //将字符转化成数字
    }else
    {
        baud = ui->comboBoxBaud->itemData(ui->comboBoxBaud->currentIndex()).toInt();
    }

    const QVariant &varCom = ui->comboBoxCom->itemData(ui->comboBoxCom->currentIndex());           //1.串口名称

    QString PortName;

    if(varCom.isValid())
    {
        PortName = varCom.toString();
    }
    else                                                                                           //若是自定义的情况
    {
        PortName = ui->comboBoxCom->currentText();
    }

    bool portopen = false;

    portopen = n485.open(AddrMaster, ui->comboBoxCom->itemData(ui->comboBoxCom->currentIndex()).toString(),
                         baud, bufferRX, BufferSize);
    if(portopen)
    {
        ui->checkBoxAutoSend->setEnabled(true);
        ui->pushButtonSend1->setEnabled(true);
        labelStatus->setText(tr("串口打开成功！"));
    }else                                                                                          //串口打开错误
    {
        labelStatus->setText(tr("<font color=red>错误：串口打开错误！</font>"));
    }
}

void MainWindow::on_pushButton_close232_released()
{
    if(serial->isOpen())serial->close();
}

void MainWindow::on_pushButton_close485_released()
{
    n485.close();
}

QByteArray MainWindow::read485()
{
    QByteArray msg = n485.readRequest();
#if 0
    if(msg == "")
    {
        labelStatus->setText(tr("<font color=yellow>设备无应答！</font>"));
    }
    else
    {
        labelStatus->setText(tr("<font color=yellow></font>"));
    }
#endif
    QByteArray msgHex = msg.toHex().toUpper();
    ui->plainTextEdit->insertPlainText(msgHex + "\n");
    //qDebug()<<__FUNCTION__<<msgHex<<ret;
    QScrollBar *bar = ui->plainTextEdit->verticalScrollBar();
    bar->setValue(bar->maximum());

    /*
    FAF50011F1F0150100
    070101010A050111
    */
    if(msgHex.left(8) == "FAF50017")
    {
        int len = msgHex.mid(6,2).toInt(nullptr,10) - 4;

        QByteArray msg = msgHex.mid(18, len * 2);
        qDebug()<<__FUNCTION__<<msgHex<<len;

        clearTable();

        ui->lineEdit11->setText(QString::number((msgHex.mid(16, 2) + msg.mid(0, 2)).toInt(nullptr, 16)));
        ui->lineEdit12->setText(QString::number(msg.mid(2, 2).toInt(nullptr, 16)));
        ui->lineEdit13->setText(QString::number(msg.mid(4, 2).toInt(nullptr, 16)));
        ui->lineEdit21->setText(QString::number(msg.mid(6, 2).toInt(nullptr, 16)));
        ui->lineEdit22->setText(QString::number(msg.mid(8, 2).toInt(nullptr, 16)));
        ui->lineEdit23->setText(QString::number(msg.mid(10, 2).toInt(nullptr, 16)));
        ui->lineEdit31->setText(QString::number(msg.mid(12, 2).toInt(nullptr, 16)));
        ui->lineEdit32->setText(QString::number(msg.mid(14, 2).toInt(nullptr, 16)));
        ui->lineEdit33->setText(QString::number(msg.mid(16, 2).toInt(nullptr, 16)));
        ui->lineEdit41->setText(QString::number(msg.mid(18, 2).toInt(nullptr, 16)));
        ui->lineEdit42->setText(QString::number(msg.mid(20, 2).toInt(nullptr, 16)));
        ui->lineEdit43->setText(QString::number(msg.mid(22, 2).toInt(nullptr, 16)));
        ui->lineEdit51->setText(QString::number(msg.mid(24, 2).toInt(nullptr, 16)));

        ui->lineEdit11->setToolTip("总时长 每次喷洒时间（秒）");
        ui->lineEdit12->setToolTip("雾化延时");
        ui->lineEdit13->setToolTip("风速切换 1，定风速 2，变风速");
        ui->lineEdit21->setToolTip("几档风速");
        ui->lineEdit22->setToolTip("速度1");
        ui->lineEdit23->setToolTip("保持时间");
        ui->lineEdit31->setToolTip("停止时间");
        ui->lineEdit32->setToolTip("速度2");
        ui->lineEdit33->setToolTip("保持时间");
        ui->lineEdit41->setToolTip("停止时间");
        ui->lineEdit42->setToolTip("速度3");
        ui->lineEdit43->setToolTip("保持时间");
        ui->lineEdit51->setToolTip("停止时间");
        ui->lineEdit52->setToolTip("速度4");
        ui->lineEdit53->setToolTip("保持时间");
        ui->lineEdit61->setToolTip("停止时间");
        ui->lineEdit62->setToolTip("速度5");
        ui->lineEdit63->setToolTip("保持时间");
        ui->lineEdit71->setToolTip("停止时间");
    }
    /*
    FAF5000EF1F0170101 140608041D
    */
    if(msgHex.left(8) == "FAF5000E")
    {
        QByteArray msg = msgHex.mid(14);
        qDebug()<<__FUNCTION__<<msgHex;

        clearTable();

        ui->lineEdit11->setText(QString::number(msg.mid(0, 2).toInt(nullptr, 16)));
        ui->lineEdit12->setText(QString::number(msg.mid(2, 2).toInt(nullptr, 16)));
        ui->lineEdit13->setText(QString::number(msg.mid(4, 2).toInt(nullptr, 16)));
        ui->lineEdit21->setText(QString::number(msg.mid(6, 2).toInt(nullptr, 16)));
        ui->lineEdit22->setText(QString::number(msg.mid(8, 2).toInt(nullptr, 16)));
        ui->lineEdit23->setText(QString::number(msg.mid(10, 2).toInt(nullptr, 16)));
        ui->lineEdit31->setText(QString::number(msg.mid(12, 2).toInt(nullptr, 16)));
        ui->lineEdit32->setText(QString::number(msg.mid(14, 2).toInt(nullptr, 16)));
        ui->lineEdit33->setText(QString::number(msg.mid(16, 2).toInt(nullptr, 16)));
        ui->lineEdit41->setText(QString::number(msg.mid(18, 2).toInt(nullptr, 16)));
        ui->lineEdit42->setText(QString::number(msg.mid(20, 2).toInt(nullptr, 16)));
        ui->lineEdit43->setText(QString::number(msg.mid(22, 2).toInt(nullptr, 16)));
        ui->lineEdit51->setText(QString::number(msg.mid(24, 2).toInt(nullptr, 16)));

        ui->lineEdit11->setToolTip("序号");
        ui->lineEdit12->setToolTip("循环");
        ui->lineEdit13->setToolTip("循环次数");
        ui->lineEdit21->setToolTip("时");
        ui->lineEdit22->setToolTip("分");
        ui->lineEdit23->setToolTip("时/间隔时间");
        ui->lineEdit31->setToolTip("分");
        ui->lineEdit32->setToolTip("时");
        ui->lineEdit33->setToolTip("分");
        ui->lineEdit41->setToolTip("时");
        ui->lineEdit42->setToolTip("分");
        ui->lineEdit43->setToolTip("时");
        ui->lineEdit51->setToolTip("分");
        ui->lineEdit52->setToolTip("时");
        ui->lineEdit53->setToolTip("分");
        ui->lineEdit61->setToolTip("时");
        ui->lineEdit62->setToolTip("分");
        ui->lineEdit63->setToolTip("时");
        ui->lineEdit71->setToolTip("分");
    }

    if(msgHex.left(8) == "FAF50030")
    {
        QByteArray msg = msgHex.mid(14);
        qDebug()<<__FUNCTION__<<msgHex;

        clearTable();

        /*
            FAF50030F1F019
            04080102050608090B0C09010203050607090A0B0601030507
            */

        int groupsSize = msg.left(2).toInt(nullptr, 16);
        QByteArray group = msg;
        group = group.mid(2);

        int grouplenth = 0;

        groups.clear();
        for (int i = 0; i < groupsSize; i++) {
            grouplenth = group.left(2).toInt(nullptr, 16);
            group = group.mid(2);
            groups.push_back(group.left(grouplenth * 2));
            group = group.mid(grouplenth * 2);
        }

        showGroup(ui->comboBox_3->currentData().toInt());

    }

    if(msgHex.left(8) == "FAF50010")
    {
        qDebug()<<__FUNCTION__<<msgHex;
        QByteArray week = msgHex.right(18).left(16);
        clearTable();

        ui->lineEdit11->setText(QString::number(week.mid(0, 2).toInt(nullptr, 16)));
        ui->lineEdit12->setText(QString::number(week.mid(2, 2).toInt(nullptr, 16)));
        ui->lineEdit13->setText(QString::number(week.mid(4, 2).toInt(nullptr, 16)));
        ui->lineEdit21->setText(QString::number(week.mid(6, 2).toInt(nullptr, 16)));
        ui->lineEdit22->setText(QString::number(week.mid(8, 2).toInt(nullptr, 16)));
        ui->lineEdit23->setText(QString::number(week.mid(10, 2).toInt(nullptr, 16)));
        ui->lineEdit31->setText(QString::number(week.mid(12, 2).toInt(nullptr, 16)));
        ui->lineEdit32->setText(QString::number(week.mid(14, 2).toInt(nullptr, 16)));

        ui->lineEdit11->setToolTip("当日");
        ui->lineEdit12->setToolTip("周一");
        ui->lineEdit13->setToolTip("周二");
        ui->lineEdit21->setToolTip("周三");
        ui->lineEdit22->setToolTip("周四");
        ui->lineEdit23->setToolTip("周五");
        ui->lineEdit31->setToolTip("周六");
        ui->lineEdit32->setToolTip("周日");
    }

    if(msgHex.left(8) == "FAF5000F")
    {
        QByteArray deviceTimeHex = msgHex.right(16).left(12);
        QByteArray deviceTimeByteArray = "20" + QString("%1%2%3%4%5%6")
                .arg(deviceTimeHex.mid( 0, 2).toInt(nullptr, 16), 2, 10, QLatin1Char('0'))
                .arg(deviceTimeHex.mid( 2, 2).toInt(nullptr, 16), 2, 10, QLatin1Char('0'))
                .arg(deviceTimeHex.mid( 4, 2).toInt(nullptr, 16), 2, 10, QLatin1Char('0'))
                .arg(deviceTimeHex.mid( 6, 2).toInt(nullptr, 16), 2, 10, QLatin1Char('0'))
                .arg(deviceTimeHex.mid( 8, 2).toInt(nullptr, 16), 2, 10, QLatin1Char('0'))
                .arg(deviceTimeHex.mid(10, 2).toInt(nullptr, 16), 2, 10, QLatin1Char('0')).toLatin1();

        QDateTime deviceTime = QDateTime::fromString(deviceTimeByteArray,"yyyyMMddhhmmss");
        ui->dateTimeEdit->setDateTime(deviceTime);
        qDebug()<<__FUNCTION__<<msgHex<<deviceTimeHex<<deviceTimeByteArray;
    }


    return msgHex;
}

QByteArray MainWindow::write485(QByteArray data)
{
    uchar address = 0;
    n485.writeOnly(address, 0x00, data, ushort(data.size()));                      //aa550008f0f12008
    ui->plainTextEdit_2->insertPlainText(data.toHex().toUpper() + "\n");

    QScrollBar *bar= ui->plainTextEdit_2->verticalScrollBar();
    bar->setValue(bar->maximum());

    return "";
}

void MainWindow::on_pushButton_released()
{
    write485(QByteArray::fromHex("aa550008f0f12008"));
}

void MainWindow::on_pushButton_33_released()
{
    ui->plainTextEdit->clear();
}

void MainWindow::on_pushButton_7_released()
{
    write485(QByteArray::fromHex("aa55000ff0f11f140c110e1715047d"));
}

void MainWindow::on_pushButton_25_released()
{
    QByteArray BaudRate = QString("%1").arg(ui->comboBox_8->currentText().toInt(), 8, 16,
                                            QLatin1Char('0')).toLatin1();
    write485(QByteArray::fromHex("aa55000cfef112"+ BaudRate +"b1"));
}

void MainWindow::on_pushButton_8_released()
{
    QByteArray SprayHeadId = QString("%1").arg(ui->comboBox_2->currentData().toInt(), 2, 16,
                                               QLatin1Char('0')).toLatin1();
    write485(QByteArray::fromHex("aa550008"+ SprayHeadId +"f10c08"));
    //qDebug()<<__FUNCTION__<<SprayHeadId;
}

void MainWindow::clearTable()
{
    ui->lineEdit11->setText("");
    ui->lineEdit12->setText("");
    ui->lineEdit13->setText("");

    ui->lineEdit21->setText("");
    ui->lineEdit22->setText("");
    ui->lineEdit23->setText("");

    ui->lineEdit31->setText("");
    ui->lineEdit32->setText("");
    ui->lineEdit33->setText("");

    ui->lineEdit41->setText("");
    ui->lineEdit42->setText("");
    ui->lineEdit43->setText("");

    ui->lineEdit51->setText("");
    ui->lineEdit52->setText("");
    ui->lineEdit53->setText("");

    ui->lineEdit61->setText("");
    ui->lineEdit62->setText("");
    ui->lineEdit63->setText("");

    ui->lineEdit71->setText("");
    ui->lineEdit72->setText("");
    ui->lineEdit73->setText("");


    ui->lineEdit11->setToolTip("");
    ui->lineEdit12->setToolTip("");
    ui->lineEdit13->setToolTip("");

    ui->lineEdit21->setToolTip("");
    ui->lineEdit22->setToolTip("");
    ui->lineEdit23->setToolTip("");

    ui->lineEdit31->setToolTip("");
    ui->lineEdit32->setToolTip("");
    ui->lineEdit33->setToolTip("");

    ui->lineEdit41->setToolTip("");
    ui->lineEdit42->setToolTip("");
    ui->lineEdit43->setToolTip("");

    ui->lineEdit51->setToolTip("");
    ui->lineEdit52->setToolTip("");
    ui->lineEdit53->setToolTip("");

    ui->lineEdit61->setToolTip("");
    ui->lineEdit62->setToolTip("");
    ui->lineEdit63->setToolTip("");

    ui->lineEdit71->setToolTip("");
    ui->lineEdit72->setToolTip("");
    ui->lineEdit73->setToolTip("");
}

void MainWindow::showGroup(int id)
{
    id++;
    ui->lineEdit11->setText(QString::number(groups.size()));

    ui->lineEdit12->setText(QString::number(id));

    if(!groups.size() || id > groups.size())return;

    QByteArray group = groups[id - 1];

    ui->lineEdit13->setText(QString::number(group.mid(0, 2).toInt(nullptr, 16)));
    ui->lineEdit21->setText(QString::number(group.mid(2, 2).toInt(nullptr, 16)));
    ui->lineEdit22->setText(QString::number(group.mid(4, 2).toInt(nullptr, 16)));
    ui->lineEdit23->setText(QString::number(group.mid(6, 2).toInt(nullptr, 16)));
    ui->lineEdit31->setText(QString::number(group.mid(8, 2).toInt(nullptr, 16)));
    ui->lineEdit32->setText(QString::number(group.mid(10, 2).toInt(nullptr, 16)));
    ui->lineEdit33->setText(QString::number(group.mid(12, 2).toInt(nullptr, 16)));
    ui->lineEdit41->setText(QString::number(group.mid(14, 2).toInt(nullptr, 16)));
    ui->lineEdit42->setText(QString::number(group.mid(16, 2).toInt(nullptr, 16)));
    ui->lineEdit43->setText(QString::number(group.mid(18, 2).toInt(nullptr, 16)));
    ui->lineEdit51->setText(QString::number(group.mid(20, 2).toInt(nullptr, 16)));
    ui->lineEdit52->setText(QString::number(group.mid(22, 2).toInt(nullptr, 16)));
    ui->lineEdit53->setText(QString::number(group.mid(24, 2).toInt(nullptr, 16)));
    ui->lineEdit61->setText(QString::number(group.mid(26, 2).toInt(nullptr, 16)));
    ui->lineEdit62->setText(QString::number(group.mid(28, 2).toInt(nullptr, 16)));
    ui->lineEdit63->setText(QString::number(group.mid(30, 2).toInt(nullptr, 16)));
    ui->lineEdit71->setText(QString::number(group.mid(32, 2).toInt(nullptr, 16)));
    ui->lineEdit72->setText(QString::number(group.mid(34, 2).toInt(nullptr, 16)));
    ui->lineEdit73->setText(QString::number(group.mid(36, 2).toInt(nullptr, 16)));

    ui->lineEdit11->setToolTip("组数");
    ui->lineEdit12->setToolTip("喷头数");
}

void MainWindow::on_pushButton_13_released()
{
    if(ui->comboBox->currentData().toInt() == 1){
        QByteArray ID = QString("%1").arg(ui->comboBox_3->currentData().toInt(), 2, 16,
                                          QLatin1Char('0')).toLatin1();
        write485(QByteArray::fromHex("aa550009f0f115" + ID )
                                     + char(ui->comboBox_3->currentData().toInt() - 2));
    }

    if(ui->comboBox->currentData().toInt() == 2){
        QByteArray ID = QString("%1").arg(ui->comboBox_3->currentData().toInt(), 2, 16,
                                          QLatin1Char('0')).toLatin1();
        write485(QByteArray::fromHex("aa550009f0f117" + ID )
                                     + char(ui->comboBox_3->currentData().toInt()));
    }

    if(ui->comboBox->currentData().toInt() == 3)
    {
        write485(QByteArray::fromHex("aa550008f0f11901"));
    }

    if(ui->comboBox->currentData().toInt() == 5)
    {
        write485(QByteArray::fromHex("aa550008f0f11d05"));
    }
}

void MainWindow::on_comboBox_3_currentIndexChanged(int index)
{
    if(ui->comboBox->currentData().toInt() == 3)showGroup(index);
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSerialPortInfo>
#include <QMainWindow>
#include <Net485.h>
#include <QDateTime>
#include <iosfwd>
                                                                                                   //头文件中尽量包含少一点头文件，尽量用类声明
namespace Ui {
class MainWindow;
}
class QIntValidator;
class QButtonGroup;
class QSerialPort;
class QRegExpValidator;
class QLabel;
                                                                                                   //主窗口类
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QByteArray write485(QByteArray data);
    void clearTable();
    void showGroup(int id);
    QByteArray read485();
private:
    Ui::MainWindow *ui;                                                                            //界面布局
    QIntValidator *intValidator;                                                                   //整数过滤器
    QRegExpValidator *regExpValidator;                                                             //字符串过滤器
    QButtonGroup *rcvGroup;                                                                        //接收数据单选框组
    QButtonGroup *sndGroup;                                                                        //发送数据单选框组
    QButtonGroup *areaGroup;                                                                       //选择发送区域单选框组
    QSerialPort *serial;                                                                           //串口操作
    QTimer *timer1;                                                                                //自动发送字符串的一个计时器
    QLabel *labelStatus;                                                                           //状态栏标签
    int preSndGrpIdx;                                                                              //发送单选框上一次值
    void fillCom();                                                                                //填充端口信息
    QTextCodec *codec;
    Net485 n485;
    int rs485 = 1;
    ushort BufferSize = 512;
    char bufferRX[512];
    QList<QByteArray> groups;

private slots:
    void on_pushButtonTest_clicked();                                                              //检测按钮
    void on_pushButtonClearTx_clicked();                                                           //清除发送区按钮
    void on_pushButtonClearRx_clicked();                                                           //清除接收区按钮
    void on_comboBoxCom_currentIndexChanged(int);                                                  //串口组合框
    void on_comboBoxBaud_currentIndexChanged(int);                                                 //波特率发生变化
    void on_checkBoxAutoSend_toggled(bool);                                                        //自动发送复选框状态改变

    void readData();                                                                               //读取串口数据，并将其显示在文本框上
    void writeData();                                                                              //向串口写数据
    void sndCodeChange(int);                                                                       //清除发送框所有内容

    void on_radioButton_released();
    void on_radioButton_2_released();
    void on_pushButtonSend1_released();
    void on_pushButtonCom_released();
    void on_pushButton_open485_released();
    void on_pushButton_close485_released();
    void on_pushButton_close232_released();
    void on_pushButton_released();
    void on_pushButton_33_released();
    void on_pushButton_7_released();
    void on_pushButton_25_released();
    void on_pushButton_8_released();
    void on_pushButton_13_released();
    void on_comboBox_3_currentIndexChanged(int index);
};

#endif // MAINWINDOW_H



#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QMediaPlayer>
#include <QAudioOutput>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //如何播放音乐，先加载multimedia模块
    audioOutput = new QAudioOutput(this);

    //媒体播放对象
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setAudioOutput(audioOutput);

//    //给播放器设置音乐
//    mediaPlayer->setSource(QUrl::fromLocalFile(""));
//    mediaPlayer->play();
    //获取当前媒体的时长 通过信号关联获取
    connect(mediaPlayer,&QMediaPlayer::durationChanged,this,[=](qint64 duration){
        ui->totLable->setText(QString("%1:%2").arg(duration/1000/60,2,10,QChar('0')).arg(duration/1000%60));
        ui->playCourseSlide->setRange(0,duration);
    });

    //获取当前播放时长
    connect(mediaPlayer,&QMediaPlayer::positionChanged,this,[=](qint64 position){
        ui->curLabel->setText(QString("%1%2").arg(position/1000/60,2,10,QChar('0')).arg(position/1000%60,2,10,QChar('0')));
        ui->playCourseSlide->setValue(position);
    });

    //拖动滑块，改变音乐进度
    connect(ui->playCourseSlide,&QSlider::sliderMoved,mediaPlayer,&QMediaPlayer::setPosition);

}

Widget::~Widget()
{
    delete ui;
}



void Widget::on_pushButton_clicked()
{
    qInfo()<<"hello student";
    //打开文件对话框，让用户选择要播放的音乐所在的目录
    auto path =  QFileDialog::getExistingDirectory(this,"请选择音乐目录","默认路径");

    //根据路径，获取里面所有的mp3、wav文件
    QDir dir(path);
    auto musicList = dir.entryList(QStringList()<<"*.mp3"<<"*.mav");
    //把音乐名字放到listwidge展示
    ui->listWidget->addItems(musicList);

    //默认选中第一个音乐
    ui->listWidget->setCurrentRow(0);
    //把音乐的完整路径保存起来
    for(auto file : musicList)
        playList.append(QUrl::fromLocalFile(path+"/"+file));

}


void Widget::on_pushButton_5_clicked()
{
    if(playList.empty())
        return;
    //获取播放状态
    switch (mediaPlayer->playbackState()) {
    case QMediaPlayer::PlaybackState::StoppedState: {//停止状态
        //播放当前选中的音乐
        curPlayIndex = ui->listWidget->currentRow();
        mediaPlayer->setSource(playList[curPlayIndex]);
        mediaPlayer->play();
        break;
        }
    case QMediaPlayer::PlaybackState::PlayingState: //停止状态
        mediaPlayer->pause();
        break;
    case QMediaPlayer::PlaybackState::PausedState: //暂停状态
        mediaPlayer->play();
        break;
    }
}

//上一曲
void Widget::on_pushButton_2_clicked()
{
    curPlayIndex--;
    curPlayIndex = curPlayIndex%playList.size();
    ui->listWidget->setCurrentRow(curPlayIndex);
    mediaPlayer->setSource(playList[curPlayIndex]);
    mediaPlayer->play();
}

//下一曲
void Widget::on_pushButton_3_clicked()
{
    curPlayIndex++;
    if(curPlayIndex >= playList.size())
        curPlayIndex = 0;
    ui->listWidget->setCurrentRow(curPlayIndex);
    mediaPlayer->setSource(playList[curPlayIndex]);
    mediaPlayer->play();
}

void Widget::on_listWidget_doubleClicked(const QModelIndex &index)
{
    curPlayIndex = index.row();
    mediaPlayer->setSource(playList[curPlayIndex]);
    mediaPlayer->play();
}


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QSlider>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);

    m_player->setVideoOutput(ui->videoWidget);

    //事件过滤器
    if (ui->videoWidget) {
        ui->videoWidget->installEventFilter(this);
        ui->videoWidget->setAttribute(Qt::WA_Hover, true);//支持悬浮事件
        ui->videoWidget->setMouseTracking(true);//鼠标按下后是否移动都行
    }

    // 通过对象名查找滑条，避免依赖 uic 生成的成员名
    m_positionSlider = findChild<QSlider*>(QStringLiteral("positionSlider"));
    m_volumeSlider = findChild<QSlider*>(QStringLiteral("volumeSlider"));

    // 初始化滑条（包含空指针判断）
    if (m_positionSlider) {
        m_positionSlider->setRange(0, 0);
    }
    if (m_volumeSlider) {
        m_volumeSlider->setRange(0, 100);
        m_volumeSlider->setValue(50);
    }
    m_audioOutput->setVolume(0.5);

    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::onOpenFile);
    connect(ui->playPauseButton, &QPushButton::clicked, this, &MainWindow::onPlayPause);

    connect(m_player, &QMediaPlayer::playbackStateChanged,
            this, &MainWindow::onPlayerStateChanged);
    connect(m_player, &QMediaPlayer::durationChanged,
            this, &MainWindow::onDurationChanged);
    connect(m_player, &QMediaPlayer::positionChanged,
            this, &MainWindow::onPositionChanged);

    //滑条相关交互
    if (m_positionSlider) {
        connect(m_positionSlider, &QSlider::sliderMoved,
                this, &MainWindow::onSeekBySlider);
        connect(m_positionSlider, &QSlider::sliderPressed, [this]{ m_isSeekingBySlider = true; });
        connect(m_positionSlider, &QSlider::sliderReleased, [this]{
            m_isSeekingBySlider = false;
            if (m_positionSlider) {
                m_player->setPosition(m_positionSlider->value());
            }
        });
    }
    if (m_volumeSlider) {
        connect(m_volumeSlider, &QSlider::valueChanged,
                this, &MainWindow::onVolumeChangedBySlider);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->videoWidget && event) {
        if (event->type() == QEvent::MouseButtonRelease) {
            auto *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                onPlayPause();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::onOpenFile()
{
    const QString file = QFileDialog::getOpenFileName(this, tr("选择媒体文件"), QString(),
                                                      tr("视频/音频 (*.mp4 *.mkv *.avi *.mov *.mp3 *.wav *.flac);;所有文件 (*.*)"));
    if (file.isEmpty()) return;

    m_player->setSource(QUrl::fromLocalFile(file));
    m_player->play();
}

void MainWindow::onPlayPause()
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_player->pause();
    } else {
        m_player->play();
    }
}

void MainWindow::onPlayerStateChanged(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::PlayingState) {
        ui->playPauseButton->setText(QStringLiteral("暂停"));
    } else {
        ui->playPauseButton->setText(QStringLiteral("播放"));
    }
}

void MainWindow::onDurationChanged(qint64 durationMs)
{
    if (m_positionSlider) {
        m_positionSlider->setRange(0, static_cast<int>(durationMs));
    }
}

void MainWindow::onPositionChanged(qint64 positionMs)
{
    if (m_positionSlider && !m_isSeekingBySlider) {
        m_positionSlider->setValue(static_cast<int>(positionMs));
    }
}

void MainWindow::onSeekBySlider(int sliderValue)
{
    if (m_isSeekingBySlider) {
        m_player->setPosition(static_cast<qint64>(sliderValue));
    }
}

void MainWindow::onVolumeChangedBySlider(int volume)
{
    const qreal vol = qBound<qreal>(0.0, static_cast<qreal>(volume) / 100.0, 1.0);
    m_audioOutput->setVolume(vol);
}

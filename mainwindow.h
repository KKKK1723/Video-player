#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSlider>
#include <QEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onOpenFile();
    void onPlayPause();
    void onPlayerStateChanged(QMediaPlayer::PlaybackState state);
    void onDurationChanged(qint64 durationMs);
    void onPositionChanged(qint64 positionMs);
    void onSeekBySlider(int sliderValue);
    void onVolumeChangedBySlider(int volume);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    bool m_isSeekingBySlider = false;

    QSlider *m_positionSlider = nullptr;
    QSlider *m_volumeSlider = nullptr;
};
#endif // MAINWINDOW_H

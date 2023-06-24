#ifndef CURPICFORLABELING_MAINWINDOW_H
#define CURPICFORLABELING_MAINWINDOW_H
/* Import the essential Qt Headers*/
#include <QMainWindow>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <QColor>
#include <QFileDialog>
#include <QTextStream>

/*Import the self labelQueryClass for detailed labels*/
#include <labelquerydialog.h>

/* Enumerate the usrChoice, here provides the basic*/
enum curPicChoosePolyPointsCount{
    CCSTDC_JLU_IMAGE_LABLE_STANDARD_RECT    =  2,
    CCSTDC_JLU_IMAGE_LABLE_ANYFOURPOLY      =  4,
    CCSTDC_JLU_IMAGE_LABLE_ANYFIVEPOLY      =  5
};

/* Enumerate the save Flags*/
enum isSaveFlag{
    CCSTDC_JLU_IMAGE_LABLE_SAVE             = 1,
    CCSTDC_JLU_IMAGE_LABLE_UNSAVE           = 0
};

#define USR_DEF_LABEL_METHOD 5

/*Save's Action Related macros*/
// 打印标签编号的前缀
#define LABLE_TEXT_INDEX_PREFIX             QString("[labels_index]: ")
// 打印标签名的前缀
#define LABLE_TEXT_LABLENAME_PREFIX         QString("[labels_name]: ")
// 打印点集的前缀
#define LABLE_TEXT_POINTSET_SHOW_PREFIX     QString("{")
// 打印单个点的坐标前缀
#define LABLE_TEXT_POINT_SHOW_PREFIX        QString("[")
// 换行
#define LABLE_TEXT_INSERT_SLASH             QString("\n")
// 点坐标分割符
#define LABLE_TEXT_POINT_SPLIT              QString(", ")
// 打印单个点的坐标后缀
#define LABLE_TEXT_POINT_SHOW_SUFFIX        QString("]")
// 打印X坐标的前缀
#define LABLE_TEXT_POS_X_PREFIX             QString("x: ")
// 打印Y坐标的前缀
#define LABLE_TEXT_POS_Y_PREFIX             QString("y: ")
// 打印点集的后缀
#define LABLE_TEXT_POINTSET_END_SHOW        QString("}")

typedef QList<QPoint> CurPolyPoints;
typedef unsigned int LabelIndex;
typedef QString      LabelName;
typedef QPair<LabelIndex,LabelName> LabelPair;

namespace Ui {
class curPicForLabeling_MainWindow;
}

class curPicForLabeling_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit curPicForLabeling_MainWindow(QWidget *parent = nullptr);
    void getPictures(QString picPath);
    ~curPicForLabeling_MainWindow();
    QPair<QList<CurPolyPoints>,QList<LabelPair>> returnLabelResToManuallyLabel(){return finalSigCurPicInfo;}
    QList<CurPolyPoints> curPicPoly;
    QList<LabelPair> labels;
    QPoint curPos;
    unsigned int curAllowMaxPointsCount;
    /*for get the one label pointList*/
    QList<QPoint> tempPointsList;
    QPair<QList<QList<QPoint>>,QList<LabelPair>> finalSigCurPicInfo;

    void initCurUsrPenFromManuallyLabel(QPen pen);

    /*更改标注点数量*/

    /*更改到标准矩形*/ /*实际上是只需要标注两个点即可*/
    void setcurAllowMaxPointsCountToStandardRect();
    /*更改到任意四边形*/
    void setcurAllowMaxPointsCountToAnyFourPolys();
    /*更改到任意五边形*/
    void setcurAllowMaxPointsCountToAnyFivePolys();

    /*保存标签与图片*/

    /*只保存图片*/
    void saveForResultCurPicButPicOnly();
    /*只保存标签*/
    void saveForResultCurPicButLabelOnly();
    /*他妈的我全都要*/
    void saveForResultCurPicAll();


protected:
    /*重写窗体重绘事件*/
    void paintEvent(QPaintEvent *);
    /*重写鼠标按下事件*/
    void mousePressEvent(QMouseEvent *);
    /*重写窗口关闭防止没保存导致发颠的窗口提示保存*/
    void closeEvent(QCloseEvent* events);
signals:
    void finishEditingPoints();

private:
    Ui::curPicForLabeling_MainWindow    *ui;
    QPixmap                             pixmap;
    QPen                                usrCurPen;
    labelQuerydialog                    *dialog;
    float width_ratio =                 1.0f;
    float height_ratio =                1.0f;
    unsigned int                        ticks;
    bool                                isSave;
private slots:
    void reLoadLabelPairList();
    void pushBackToFinalSigCurPicInfo();
};

#endif // CURPICFORLABELING_MAINWINDOW_H
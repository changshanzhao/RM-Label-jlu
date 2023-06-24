#include "curpicforlabeling_mainwindow.h"
#include "ui_curpicforlabeling_mainwindow.h"
/**************************************************************************************************
*
*   funtions type :     basic_init
*
*   work in where :     curPicForLabeling_MainWindow
*
*   Function Name:      icurPicForLabeling_MainWindow
*
*   Discriptions:       This function are used in
*
*                       1. basic class member params inits
*
*                       2. connecting and trigger others initialization
*
*                       3. ...
*
*   parameters :        QWidget *parent
*
*   return :            void
*
**************************************************************************************************/

curPicForLabeling_MainWindow::curPicForLabeling_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::curPicForLabeling_MainWindow)
{
    ui->setupUi(this);
    dialog = new labelQuerydialog;
    curAllowMaxPointsCount = USR_DEF_LABEL_METHOD;
    isSave = 0;
    this->centralWidget()->setMouseTracking(true);//开启鼠标实时追踪，监听鼠标移动事件，默认只有按下时才监听
    this->setMouseTracking(true);
    ticks = 0;
    /*重新加载可能新产生的标签链表*/
    connect(dialog,&labelQuerydialog::finishSelectingLabel,
            this,&curPicForLabeling_MainWindow::reLoadLabelPairList);

    /*每标记完一个轮廓刷新我们的最后的结果，等到用户表达结束或者返回的时候，直接返回拿到的数据*/
    connect(dialog,&labelQuerydialog::finishSelectingLabel,
            this,&curPicForLabeling_MainWindow::pushBackToFinalSigCurPicInfo);

    /*下面的三个信号则是链接标注点的个数问题的*/

    /*链接：切换到标准矩形*/
    connect(ui->actionchangeToRectMode,&QAction::triggered,this,&curPicForLabeling_MainWindow::setcurAllowMaxPointsCountToStandardRect);

    /*链接：切换到任意四边形*/
    connect(ui->actionchangeToAnyFourPoly,&QAction::triggered,this,&curPicForLabeling_MainWindow::setcurAllowMaxPointsCountToAnyFourPolys);

    /*链接：切换到任意五边形*/
    connect(ui->actionchangeToFivePoly,&QAction::triggered,this,&curPicForLabeling_MainWindow::setcurAllowMaxPointsCountToAnyFivePolys);

    /*下面的三个信号与槽完成保存工作*/

    /*链接：只保存图片*/
    connect(ui->actionsavePicOnly,&QAction::triggered,this,&curPicForLabeling_MainWindow::saveForResultCurPicButPicOnly);

    /*链接：只保存标签*/
    connect(ui->actionsaveLabelsOnly,&QAction::triggered,this,&curPicForLabeling_MainWindow::saveForResultCurPicButLabelOnly);

    /*链接：都保存*/
    connect(ui->actionsaveAll,&QAction::triggered,this,&curPicForLabeling_MainWindow::saveForResultCurPicAll);

    /*TODO 书写切换画笔颜色和粗度的selections*/

    /*TODO 书写撤销与清空的selctions和快键*/
}

// delete and release
curPicForLabeling_MainWindow::~curPicForLabeling_MainWindow()
{
    delete ui;
}
/**************************************************************************************************
*
*   funtions type :     [Group comments] basic_init
*
*   work in where :     curPicForLabeling_MainWindow
*
*   Function Name:      setcurAllowMaxPointsCount
*
*                       [1]setcurAllowMaxPointsCountToStandardRect : helps set to standard rects
*                       [2]setcurAllowMaxPointsCountToAnyFourPolys : helps set to any four Polygens
*                       [3]setcurAllowMaxPointsCountToAnyFivePolys : helps set to any five Polygens
*
*   Discriptions:       This function are used in set the shape of the polygens
*   parameters :        QWidget *parent
*   return :            void
*
**************************************************************************************************/

void curPicForLabeling_MainWindow::setcurAllowMaxPointsCountToStandardRect()
{
    curAllowMaxPointsCount = 2;
    QMessageBox::information(this,"注意!","成功切换到了标准矩形模式");
}


void curPicForLabeling_MainWindow::setcurAllowMaxPointsCountToAnyFourPolys()
{
    curAllowMaxPointsCount = 4;
    QMessageBox::information(this,"注意!","成功切换到了任意四边形模式");
}

void curPicForLabeling_MainWindow::setcurAllowMaxPointsCountToAnyFivePolys()
{
    curAllowMaxPointsCount = 5;
    QMessageBox::information(this,"注意!","成功切换到了任意五边形模式");
}


void curPicForLabeling_MainWindow::paintEvent(QPaintEvent*)
{

    QPainter painter(&pixmap);
    painter.setPen(usrCurPen);
    switch(curAllowMaxPointsCount)
    {
        // special for standard Rects
    case CCSTDC_JLU_IMAGE_LABLE_STANDARD_RECT:
    {
        for(int i = 0; i < curPicPoly.size();i++)
        {
            QRect thisRect;
            thisRect.setTopLeft(curPicPoly[i][0]);
            thisRect.setBottomRight(curPicPoly[i][1]);
            painter.drawRect(thisRect);
            update();
        }

        painter.end();
        emit finishEditingPoints();
    }break;
    case CCSTDC_JLU_IMAGE_LABLE_ANYFOURPOLY:
    case CCSTDC_JLU_IMAGE_LABLE_ANYFIVEPOLY:
    default:
    {
        // qDebug() << curPicPoly.size();

        for(int i = 0; i < curPicPoly.size();i++)
        {
            for(int j = 0; j < curPicPoly[i].size()-1;j++)
            {
               painter.drawLine(curPicPoly[i][j],curPicPoly[i][j + 1]);
               update();
            }
            painter.drawLine(curPicPoly[i][curPicPoly[i].size()-1],curPicPoly[i][0]);
            update();
        }
        painter.end();
        emit finishEditingPoints();
    }break;
    }
    painter.begin(this);//将当前窗体作为画布
    painter.drawPixmap(0, 0, pixmap);//将pixmap画到窗体
}
void curPicForLabeling_MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        //qDebug() << " 左键摁下";

        switch(curAllowMaxPointsCount)
        {
        case 2:
        case 4:
        case 5:
        {
            if(ticks < curAllowMaxPointsCount - 1)
            {
                qDebug() << "第"<< ticks <<"位置点已找好";
                qDebug() << e->pos();
                ticks ++;
                tempPointsList.push_back(e->pos());
            }
            else
            {
                qDebug() << "第"<< ticks <<"位置点已找好";
                qDebug() << e->pos();
                ticks = 0;
                tempPointsList.push_back(e->pos());
                curPicPoly.push_back(tempPointsList);
                tempPointsList.clear();
                dialog->setWindowFlags(Qt::WindowStaysOnTopHint);
                dialog->show();
                isSave = CCSTDC_JLU_IMAGE_LABLE_UNSAVE;
            }
        }break;
        }
    }
}

void curPicForLabeling_MainWindow::closeEvent(QCloseEvent* events)
{
    if(isSave == CCSTDC_JLU_IMAGE_LABLE_UNSAVE){
        if(QMessageBox::Yes == QMessageBox::question(this,"嗯？哪里跑？","小子跑路这快，不保存一下?不然会全部消失啊！点击确认以切到保存全部"))
        {
            saveForResultCurPicAll();
            events->accept();
        }
        else
        {
            finalSigCurPicInfo.first.clear();
            finalSigCurPicInfo.second.clear();
            events->ignore();
        }
    }
    events->accept();
}



void curPicForLabeling_MainWindow::getPictures(QString curPath)
{
    pixmap.load(curPath);
    width_ratio = (float)pixmap.width()/this->width();
    height_ratio = (float)pixmap.height()/this->height();
    pixmap = pixmap.scaled(this->width(),this->height(),Qt::KeepAspectRatio);
    QPainter painter(&pixmap);
    painter.drawPixmap(0, 0, pixmap);
    update();
}

void curPicForLabeling_MainWindow::reLoadLabelPairList()
{
    labels.push_back(dialog->returnFinalLabelToOutward());
}

void curPicForLabeling_MainWindow::pushBackToFinalSigCurPicInfo()
{
    finalSigCurPicInfo.first.push_back(curPicPoly.last());
    finalSigCurPicInfo.second.push_back(dialog->returnFinalLabelToOutward());

    qDebug() << "有 "<<curPicPoly.size() << "轮廓";
    for(int i = 0; i < curPicPoly.size();i++)
    {
        qDebug() << "这是第" << i << "个轮廓:";
        for(int j = 0; j < curPicPoly[i].size();j++)
        {
            qDebug() << curPicPoly[i][j];
        }
        qDebug() << "标签是:" << finalSigCurPicInfo.second[i].first<< ": "<< \
            finalSigCurPicInfo.second[i].second;
    }
}

void curPicForLabeling_MainWindow::initCurUsrPenFromManuallyLabel(QPen pen)
{
    usrCurPen = pen;
}



void curPicForLabeling_MainWindow::saveForResultCurPicButPicOnly()
{
SAVEAGAIN:
    QString usrDefinedSavedPath = QFileDialog::getSaveFileName(this,tr("保存图像"),"./",tr("*.bmp;; *.png;; *.jpg;; *.tif;; *.GIF"));
    if(usrDefinedSavedPath.isEmpty())
    {
        if(QMessageBox::Yes == QMessageBox::question(this,"Ops！","你真的真的不保存嘛？"))
        {
            QMessageBox::information(this,"确认","放弃保存！");
            return;
        }
        else{
            goto SAVEAGAIN;
        }
    }

    if(!pixmap.save(usrDefinedSavedPath)){
        QMessageBox::critical(NULL,"出错了","T_T保存出错了！！！");
        if(QMessageBox::Yes == QMessageBox::question(this,"Ops！","你要放弃嘛?"))
        {
            QMessageBox::information(NULL,"确认","放弃保存！");
            return;
        }
        else{
            goto SAVEAGAIN;
        }

    }
    QMessageBox::information(this,"成功",QString("成功保存到！\n") + usrDefinedSavedPath);
    isSave = CCSTDC_JLU_IMAGE_LABLE_SAVE;
}

void curPicForLabeling_MainWindow::saveForResultCurPicButLabelOnly()
{
    QString resultToText;
    for(int i = 0; i < finalSigCurPicInfo.second.size();i++)
    {
        resultToText = LABLE_TEXT_INDEX_PREFIX + QString::number(finalSigCurPicInfo.second[i].first) + \
                            LABLE_TEXT_INSERT_SLASH + LABLE_TEXT_LABLENAME_PREFIX + finalSigCurPicInfo.second[i].second + LABLE_TEXT_INSERT_SLASH;
        QString pointsSetShow = LABLE_TEXT_POINTSET_SHOW_PREFIX + LABLE_TEXT_INSERT_SLASH ;
        for(int j = 0 ; j < finalSigCurPicInfo.first[i].size(); j++)
        {
            pointsSetShow += LABLE_TEXT_POINT_SHOW_PREFIX + QString::number(finalSigCurPicInfo.first[i][j].x())\
                + LABLE_TEXT_POINT_SPLIT + QString::number(finalSigCurPicInfo.first[i][j].y()) +LABLE_TEXT_POINT_SHOW_SUFFIX+ LABLE_TEXT_INSERT_SLASH;
        }
        pointsSetShow += LABLE_TEXT_POINTSET_END_SHOW;
        resultToText += pointsSetShow;
    }
SAVEAGAIN_LABLES:
    QString savePath = QFileDialog::getSaveFileName(this,"保存标签文件","",".txt");
    if(savePath.isEmpty())
    {
        if(QMessageBox::Yes == QMessageBox::question(this,"Ops！","你真的真的不保存嘛？"))
        {
            QMessageBox::information(this,"确认","放弃保存！");
            return;
        }
        else{
            goto SAVEAGAIN_LABLES;
        }
    }

    QFile saveFile(savePath) ;
    if(saveFile.open(QIODevice::ReadWrite|QIODevice::Text)){
        QString temp = resultToText; // 写入内容

        // 将内容写入文件
        QTextStream out(&saveFile);
        out << temp;

        saveFile.close();
        QMessageBox::information(this,"成功",QString("成功保存到！\n") + savePath);
    }else{
        QMessageBox::critical(NULL,"出错了","T_T保存出错了！！！");
        if(QMessageBox::Yes == QMessageBox::question(this,"Ops！","你要放弃嘛?"))
        {
            QMessageBox::information(this,"确认","放弃保存！");
            return;
        }
        else{
            goto SAVEAGAIN_LABLES;
        }
    }

    return;
}

void curPicForLabeling_MainWindow::saveForResultCurPicAll()
{
    saveForResultCurPicButLabelOnly();
    saveForResultCurPicButPicOnly();
}
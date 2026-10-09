#include "mainwindow.h"
#include "ui_mainwindow.h"

#include    <QDate>
#include    <QTableWidgetItem>
#include    <QRandomGenerator>


//为一行的单元格创建 Items
void MainWindow::createItemsARow(int rowNo,QString name,QString sex,QDate birth,QString nation,bool isPM,int score)
{
    uint studID=202105000;  //学号基数
    //姓名
    QTableWidgetItem *item=new  QTableWidgetItem(name, MainWindow::ctName);   //数据项类型为MainWindow::ctName
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    studID  +=rowNo;        //学号 =基数 + 行号
    item->setData(Qt::UserRole,QVariant(studID));           //设置studID为用户数据
    ui->tableInfo->setItem(rowNo,MainWindow::colName,item);

    //性别
    QIcon   icon;
    if (sex=="男")
        icon.addFile(":/images/icons/boy.ico");
    else
        icon.addFile(":/images/icons/girl.ico");

    item=new  QTableWidgetItem(sex,MainWindow::ctSex);      //type为MainWindow::ctSex
    item->setIcon(icon);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    Qt::ItemFlags flags=Qt::ItemIsSelectable |Qt::ItemIsEnabled;    //不允许编辑
    item->setFlags(flags);
    ui->tableInfo->setItem(rowNo,MainWindow::colSex,item);  //为单元格设置Item

    //出生日期
    QString str=birth.toString("yyyy-MM-dd");   //日期转换为字符串
    item=new  QTableWidgetItem(str,MainWindow::ctBirth);        //type为MainWindow::ctBirth
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);   //文本对齐格式
    ui->tableInfo->setItem(rowNo,MainWindow::colBirth,item);

    //民族
    item=new  QTableWidgetItem(nation,MainWindow::ctNation);        //type为MainWindow::ctNation
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableInfo->setItem(rowNo,MainWindow::colNation,item);

    //是否党员
    item=new  QTableWidgetItem("党员",MainWindow::ctPartyM);      //type为 MainWindow::ctPartyM
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    flags= Qt::ItemIsSelectable | Qt::ItemIsUserCheckable |Qt::ItemIsEnabled;   //不允许编辑，但可以更改复选状态
    item->setFlags(flags);
    if (isPM)
        item->setCheckState(Qt::Checked);
    else
        item->setCheckState(Qt::Unchecked);
    item->setBackground(QBrush(Qt::yellow));   //设置背景颜色
    ui->tableInfo->setItem(rowNo,MainWindow::colPartyM,item);

    //分数
    str.setNum(score);
    item=new  QTableWidgetItem(str,MainWindow::ctScore);    //type为MainWindow::ctPartyM
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableInfo->setItem(rowNo,MainWindow::colScore,item);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //    setCentralWidget(ui->splitterMain);

    //状态栏初始化创建
    labCellIndex = new QLabel("当前单元格坐标：",this);
    labCellIndex->setMinimumWidth(550);

    labCellType=new QLabel("当前单元格类型：",this);
    labCellType->setMinimumWidth(200);

    labStudID=new QLabel("学生ID：",this);
    labStudID->setMinimumWidth(200);

    labBirthplace=new QLabel("籍贯：",this);

    ui->statusBar->addWidget(labCellIndex); //添加到状态栏
    ui->statusBar->addWidget(labCellType);
    ui->statusBar->addWidget(labStudID);
    ui->statusBar->addWidget(labBirthplace);
}

MainWindow::~MainWindow()
{
    delete ui;
}


//设置水平表头
void MainWindow::on_btnSetHeader_clicked()
{
    QStringList headerText;
    headerText<<"姓名"<<"性别"<<"出生日期"<<"民族"<<"分数"<<"是否党员";
    //    ui->tableInfo->setHorizontalHeaderLabels(headerText); //只设置标题
    ui->tableInfo->setColumnCount(headerText.size());      //设置表格列数
    for (int i=0;i<ui->tableInfo->columnCount();i++)
    {
        QTableWidgetItem *headerItem=new QTableWidgetItem(headerText.at(i));
        QFont font=headerItem->font();   //获取原有字体设置
        font.setBold(true);              //设置为粗体
        font.setPointSize(11);           //字体大小
        headerItem->setForeground(QBrush(Qt::red));  //设置文字颜色
        headerItem->setFont(font);       //设置字体
        ui->tableInfo->setHorizontalHeaderItem(i,headerItem);    //设置表头单元格的item
    }
}

//设置行数,设置的行数为数据区的行数，不含表头
void MainWindow::on_btnSetRows_clicked()
{
    ui->tableInfo->setRowCount(ui->spinRowCount->value());//设置数据区行数
    ui->tableInfo->setAlternatingRowColors(ui->chkBoxRowColor->isChecked()); //设置交替行背景颜色
}


//初始化表格数据
void MainWindow::on_btnIniData_clicked()
{
    QDate   birth(2001,4,6);        //初始化一个日期
    ui->tableInfo->clearContents(); //只清除工作区，不清除表头
    for (int i=0; i<ui->tableInfo->rowCount(); i++)
    {
        QString strName=QString("学生%1").arg(i);
        QString strSex= ((i % 2)==0)? "男":"女";
        bool isParty= ((i % 2)==0)? false:true;
        int score= QRandomGenerator::global()->bounded(60,100);   //随机数[60,100)
        createItemsARow(i, strName, strSex, birth,"汉族",isParty,score);  //为某一行创建items
        birth=birth.addDays(20);    //日期加20天
    }
}

void MainWindow::on_chkBoxTabEditable_clicked(bool checked)
{ //设置表格是否可编辑，以及进入编辑模式的方式
    if (checked)
        //双击或获取焦点后单击，进入编辑状态
        ui->tableInfo->setEditTriggers(QAbstractItemView::DoubleClicked
                                       | QAbstractItemView::SelectedClicked);
    else
        ui->tableInfo->setEditTriggers(QAbstractItemView::NoEditTriggers); //不允许编辑
}

void MainWindow::on_chkBoxHeaderH_clicked(bool checked)
{//是否显示水平表头
    ui->tableInfo->horizontalHeader()->setVisible(checked);
}

void MainWindow::on_chkBoxHeaderV_clicked(bool checked)
{//是否显示垂直表头
    ui->tableInfo->verticalHeader()->setVisible(checked);
}

void MainWindow::on_chkBoxRowColor_clicked(bool checked)
{ //行的底色交替采用不同颜色
    ui->tableInfo->setAlternatingRowColors(checked);
}

void MainWindow::on_rBtnSelectItem_clicked()
{//选择方式：单元格选择
    ui->tableInfo->setSelectionBehavior(QAbstractItemView::SelectItems);
}

void MainWindow::on_rBtnSelectRow_clicked()
{//选择方式：行选择
    ui->tableInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
}


//将 QTableWidget的所有行的内容提取字符串，显示在QPlainTextEdit里
void MainWindow::on_btnReadToEdit_clicked()
{
//    QString str;
    QTableWidgetItem    *item;

    ui->textEdit->clear();  //文本编辑器清空
    for (int i=0;i<ui->tableInfo->rowCount();i++)   //逐行处理
    {
        QString str=QString::asprintf("第 %d 行： ",i+1);
        for (int j=0;j<ui->tableInfo->columnCount()-1;j++) //逐列处理，但最后一列是check型，单独处理
        {
            item=ui->tableInfo->item(i,j);      //获取单元格的item
            str=str+item->text()+"   ";         //字符串连接
        }
        item=ui->tableInfo->item(i,colPartyM);  //最后一列，党员
        if (item->checkState()==Qt::Checked)    //根据check状态显示文字
            str=str+"党员";
        else
            str=str+"群众";
        ui->textEdit->appendPlainText(str);     //添加到编辑框作为一行
    }
}

//currentCellChanged()信号的槽函数，当前单元格发生变化时的响应
void MainWindow::on_tableInfo_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);

    QTableWidgetItem* item=ui->tableInfo->item(currentRow,currentColumn);
    if  (item==nullptr)
        return;

    labCellIndex->setText(QString::asprintf("当前单元格坐标：%d 行，%d 列",currentRow,currentColumn));

    int cellType=item->type();
    labCellType->setText(QString::asprintf("当前单元格类型：%d",cellType));

    if (ui->tableInfo->columnCount() == 7)
    {
        QTableWidgetItem *idItem=ui->tableInfo->item(currentRow,0);
        QTableWidgetItem *nameItem=ui->tableInfo->item(currentRow,1);
        labStudID->setText(idItem ? "学号："+idItem->text() : "学号：");

        QString birthplace=nameItem ? nameItem->data(Qt::UserRole+1).toString() : QString();
        labBirthplace->setText(birthplace.isEmpty() ? "籍贯：" : "籍贯："+birthplace);
    }
    else
    {
        item=ui->tableInfo->item(currentRow,MainWindow::colName);
        uint ID=item->data(Qt::UserRole).toUInt();
        labStudID->setText(QString::asprintf("学生ID：%d",ID));
        labBirthplace->setText("籍贯：");
    }
}

//插入一行
void MainWindow::on_btnInsertRow_clicked()
{
    int curRow=ui->tableInfo->currentRow();     //当前行号

    ui->tableInfo->insertRow(curRow);           //插入一行，但不会自动为单元格创建item
    createItemsARow(curRow, "新学生", "男",
                    QDate::fromString("2002-10-1","yyyy-M-d"),"苗族",true,80 ); //为某一行创建items
}

//添加一行
void MainWindow::on_btnAppendRow_clicked()
{
    int curRow=ui->tableInfo->rowCount();       //当前行号
    ui->tableInfo->insertRow(curRow);           //在表格尾部添加一行
    createItemsARow(curRow, "新生", "女",
                    QDate::fromString("2002-6-5","yyyy-M-d"),"满族",false,76 ); //为某一行创建items
}

//删除当前行及其items
void MainWindow::on_btnDelCurRow_clicked()
{
    int curRow=ui->tableInfo->currentRow();     //当前行号
    ui->tableInfo->removeRow(curRow);           //删除当前行及其items
}

void MainWindow::on_btnAutoHeght_clicked()
{
    ui->tableInfo->resizeRowsToContents();
}

void MainWindow::on_btnAutoWidth_clicked()
{
    ui->tableInfo->resizeColumnsToContents();
}

void MainWindow::on_actionQToolButton_triggered()
{
    const QStringList headers = {
        "学号", "姓名", "性别", "行政班级", "院系", "专业", "修读性质"
    };
    const QString students[5][8] = {
        {"2024414290115", "黎嘉耀", "男", "2024软件1班", "计算机学院", "软件工程", "本科", "江苏"},
        {"2024414290117", "李俊源", "男", "2024软件1班", "计算机学院", "软件工程", "本科", "浙江"},
        {"2024414290118", "李尚锦", "男", "2024软件1班", "计算机学院", "软件工程", "本科", "广东"},
        {"2024414290119", "廖绪乐", "男", "2024软件1班", "计算机学院", "软件工程", "本科", "河南"},
        {"2024414290120", "廖宇晖", "男", "2024软件1班", "计算机学院", "软件工程", "本科", "四川"}
    };
    const int ownStudentRow = 2;
    ui->tableInfo->clear();
    ui->tableInfo->setColumnCount(headers.size());
    ui->tableInfo->setHorizontalHeaderLabels(headers);
    ui->tableInfo->setRowCount(5);

    for (int row = 0; row < 5; ++row)
    {
        for (int column = 0; column < headers.size(); ++column)
        {
            QTableWidgetItem *item = new QTableWidgetItem(students[row][column]);
            item->setTextAlignment(Qt::AlignCenter);

            if (row == ownStudentRow && (column == 0 || column == 1))
            {
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
                item->setForeground(QBrush(Qt::red));
            }

            if (column == 1)
                item->setData(Qt::UserRole + 1, students[row][7]);

            ui->tableInfo->setItem(row, column, item);
        }
    }

    ui->tableInfo->resizeColumnsToContents();
}

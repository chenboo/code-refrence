//在Qt中进行布局的类没有像android或者win8 metro那么多
//但是应该是够用了，提供的主要的布局类有：
//QHBoxLayout、QVBoxLayout、QGridLayout、QStackedLayout
//当然你也可以不是用这写布局类，而使用绝对的布局，调用
//控件的setGeometry成员函数来为它指定在窗口中的位置。
//下面贴一小段布局的代码示范一下:

MainWindow::MainWindow()
{
    QLabel* pLabNamed = new QLabel(tr("Named:"));
    QLineEdit* pLineName = new QLineEdit();
    QLabel* pLabLookin = new QLabel(tr("Look in:"));

    QLineEdit* pLineLookin = new QLineEdit();
    QCheckBox* pCheckBox = new QCheckBox(tr("Include subfolders"));

    QTableWidget* pTableWidget = new QTableWidget();
    QStringList labels;
    labels << tr("Name") << tr("In Folder") << tr("Size")
           << tr("Modified");
    pTableWidget->setColumnCount(labels.size());
    pTableWidget->setHorizontalHeaderLabels(labels);

    QLabel* pLabMessage = new QLabel(tr("0 files found"));
    pLabMessage->setFrameShape(QLabel::Panel);
    pLabMessage->setFrameShadow(QLabel::Sunken);

    QGridLayout* pLeftLayout = new QGridLayout();
    pLeftLayout->addWidget(pLabNamed, 0, 0);//放在第一行第一列
    pLeftLayout->addWidget(pLineName, 0, 1);//放在第一行第二列
    pLeftLayout->addWidget(pLabLookin, 1, 0);
    pLeftLayout->addWidget(pLineLookin, 1, 1);
    pLeftLayout->addWidget(pCheckBox, 2, 0, 1, 2);//放在第三行第一列，并占用一行两列
    pLeftLayout->addWidget(pTableWidget, 3, 0, 1, 2);
    pLeftLayout->addWidget(pLabMessage, 4, 0, 1 , 2);

    QPushButton* pBtnFind = new QPushButton(tr("Find"));
    QPushButton* pBtnStop = new QPushButton(tr("Stop"));
    QPushButton* pBtnClose = new QPushButton(tr("Close"));
    QPushButton* pBtnHelp = new QPushButton(tr("Help"));
    connect(pBtnClose, SIGNAL(clicked()), this, SLOT(close()));

    QVBoxLayout* pRightLayout = new QVBoxLayout();
    pRightLayout->addWidget(pBtnFind);
    pRightLayout->addWidget(pBtnStop);
    pRightLayout->addWidget(pBtnClose);
    pRightLayout->addStretch();
    pRightLayout->addWidget(pBtnHelp);

    QHBoxLayout* pMainLayout = new QHBoxLayout();
    pMainLayout->addLayout(pLeftLayout);
    pMainLayout->addLayout(pRightLayout);
    setLayout(pMainLayout);

    setWindowTitle(tr("Find Files or Folders"));
}

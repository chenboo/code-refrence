在QT中为窗口添加工具条和菜单非常的方便和快捷。
QT是通过QAction来为菜单和工具条添加相应的操作和项的，想来QT在这方面也是
做得很不错，充分得为程序员着想，减轻编码负担，同一个Action可以添加到，菜单和工具条中，
而不需要另起炉灶，添加多余的代码，因为所需的信息是一样的。
下面来简单的介绍一下：
《一》创建菜单和工具条共同的基础QActions
//两个action的定义
    m_pOpenAction = new QAction(tr("打开(&O)"), this);
    m_pOpenAction->setIcon(QIcon(":/images/open.png"));//为Action添加图标
    m_pOpenAction->setShortcut(QKeySequence::Open);//为Action添加快捷方式
    m_pOpenAction->setStatusTip(tr("打开一个已经存在的文件"));//为Action添加状态栏提示
    connect(m_pOpenAction, SIGNAL(triggered()), this, SLOT(openFile()));

    m_pNewFileAction = new QAction(tr("新建(&N)"), this);
    m_pNewFileAction->setIcon(QIcon(":/images/new.png"));
    m_pNewFileAction->setShortcut(QKeySequence::New);
    m_pNewFileAction->setStatusTip(tr("创建一个新的文件"));
    connect(m_pNewFileAction, SIGNAL(triggered()), this, SLOT(newFile()));

《二》创建菜单并加入Actions
//通过函数menubar()可以获得这个窗口的菜单栏，通过addmenu可以添加一个菜单
    m_pFileMenu = menuBar()->addMenu(tr("文件(&F)"));
//向菜单中添加菜单项，其实就是Actions
    m_pFileMenu->addAction(m_pOpenAction);
    m_pFileMenu->addAction(m_pNewFileAction);

《三》创建工具条并加入Actions
//函数addToolBar()可以获得添加好的工具栏
    m_pToolBar = addToolBar(tr("&File"));
//向工具栏中添加工具按钮，其实就是Actions
    m_pToolBar->addAction(m_pOpenAction);
    m_pToolBar->addAction(m_pNewFileAction);


//至此OK了。


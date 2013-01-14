/*
经常想过如何在QT中进行布局时设定比例，比如说我布局了两行
部件widgest，无论窗口怎么样拉伸，我想让第一行的高度永远是第二行的两倍，
或者我创建了两列的部件widgts，我想让第一列的宽度永远是第二列的两倍，等等。
在QT中有个QGridLayout它是用来进行网格布局的，他提供了进行布局的拉伸因子。
下面以一个小的例子来说明一下：
*/

QLabel label1 = new QLabel("label1");
QLabel label2 = new QLabel("label2");
QLabel label3 = new QLabel("label3");

QGridLayout* pGridLayout = new QGridLayout();
pGridLayout->addWidget(label1, 0, 0);
pGridLayout->addWidget(label2, 1, 0);
pGridLayout->addWidget(label3, 0, 1, 2, 1);


//设定第一列的拉伸因子为1
pGridLayout->setColumnStretch(0, 1);
//设定第二列的拉伸因子为2
pGridLayout->setColumnStretch(1, 2);
//这样之后第一列和第二列的宽度之比为 1 : 2


//设定第一行的拉伸因子为1
pGridLayout -> setRowStretch(0, 1);
//设定第二行的拉伸因子为1
pGridLayout -> setRowStretch(1, 1);
//这样之后第一行和第二行的高度之比为 1 : 1








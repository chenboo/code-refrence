//在这里我们讲一个滑动条(slider)和微选框(spinBox)进行数据绑定的例子
//即改变滚动条的值，微选框的值也跟着改变，同理改变微选框的值，滑动条的值也跟着改变
//代码如下：

#include <QApplication>
#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QSpinBox>
#include <QtGui/QSlider>
#include <QtGui/QHBoxLayout>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget* window = new QWidget;
    window->setWindowTitle("Enter you age");

    QSpinBox* spinBox = new QSpinBox;
    QSlider* slider = new QSlider(Qt::Horizontal);
    spinBox->setRange(0, 130);
    slider->setRange(0, 130);

//将滑动条和微选框建立联系
//当滑动条的值改变时，将向微选框发送信号
//当微选框的值改变时，将向滑动条发送信号
    QObject::connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
    QObject::connect(spinBox,SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
    spinBox->setValue(35);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(spinBox);
    layout->addWidget(slider);
    window->setLayout(layout);
    window->show();
    
    return a.exec();
}


//或许你发现，那两个信号槽的链接操作会不会产生无限递归？因为steValue就会引发valueChanged信号！
//答案是不会。这两句语句实现了，当spinBox发出valueChanged信号的时候，会回调slider的setValue，
//以更新slider的值；而slider发出valueChanged信号的时候，又会回调slider的setValue。
//但是，如果新的value和旧的value是一样的话，是不会发出这个信号的，因此避免了无限递归。
 
//迷糊了吧？举个例子看。比如下面的spinBox->setValue(35)执行的时候，首先，spinBox会将自己的值设为35，
//这样，它的值与原来的不一样了(在没有setValue之前的时候，默认值是0)，于是它发出了valueChanged信号。
//slider接收到这个信号，于是回调自己的setValue函数，将它的值也设置成35，它也发出了valueChanged信号。
//当然，此时spinBox又收到了，不过它发现，这个35和它本身的值是一样的，于是它就不发出信号，所以信号传递就停止了。



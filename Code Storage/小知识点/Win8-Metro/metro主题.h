//有关metro的一些东西不得不说，metro刚出来很多的资料以及论坛
//还不是很完善，碰到问题上网搜，也不是那样的容易，昨天就碰到了一个问题
//我在xaml文件中放置了一个文本输入控件，textbox，在程序运行起来时，发现
//它的边缘没有边框，而我打开win8的示例程序，却可以明显的看到，textbox边缘
//有边框，而且会随着鼠标的靠近而改变颜色，我首先想到的是，可能这个例子中
//对控件使用了某种风格，然后我就在app.xaml文件中去找，可能是找得不够仔细吧
//找了半天也没有找到，搞得很是郁闷，很沮丧，今天我想，大不了我照例子从头做一下好了
//功夫不负有心人，一开始就找到了，在app.xaml中

<Application xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml" 
    x:Class="SDKSample.App"
    RequestedTheme="Light"
    xmlns:local="using:SDKSample.DataBinding"
    >

//RequestedTheme="Light" 这句话就是关键，它使程序使用了Light主题，不加这句，程序默认使用Dark主题
//哎，看来学习win8 metro还是路漫漫其修远兮啊。
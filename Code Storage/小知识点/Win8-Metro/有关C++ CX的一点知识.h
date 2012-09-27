//C++/CX是微软的又一次对C++的扩展，真搞不明白微软为什么总是喜欢拿C++改来改去，真的很烦人，
//而且不知道微软这次平板的战略能否成功，说不定只是一次失败的尝试，哎，搞得人很累。
//工作还是要工作，来讲讲C++/CX中的一些特点吧。

//一、首先是类的定义，举个例子
[Windows::UI::Xaml::Data::Bindable]
public ref class FeedData sealed
{
public:
	FeedData(void)
	{
		m_items = ref new Platform::Collections::Vector<FeedItem^>();
	}

	// The public members must be Windows Runtime types so that
	// the XAML controls can bind to them from a separate .winmd.
	property Platform::String^ Title;
	property Windows::Foundation::Collections::IVector<FeedItem^>^ Items
	{
		Windows::Foundation::Collections::IVector<FeedItem^>^ get() {return m_items; }
	}

	property Platform::String^ Description;
	property Windows::Foundation::DateTime PubDate;
	property Platform::String^ Uri;

private:
	~FeedData(void){}

	Platform::Collections::Vector<FeedItem^>^ m_items;
};


//二、新的关键字介绍

//1〉ref

//可以看到在上面的类的定义中多了个：ref 关键字
//ref代表引用类型，类似于Java里面的引用概念，ref 关键字指示该类是一个 Windows 运行时引用类型；使用 ref 使你无需编写大量下一代 COM 代码。
//从最基本的意义上说，ref 类是一个 COM 对象，它实现了 IInspectable 接口，其生命期是通过智能指针管理的。
//系统会负责他们的引用计数，当引用计数为0时，它们会被销毁。

//在新建一个这个类的对象时，要使用如下的方式：
FeedData^ page = ref new FeedData();
//or
auto page = ref new FeedData(); //编译器会自动推断对象的类型

//“ ^ ” 符号为“尖帽号”，它表示“对象句柄”即引用类型。在动态内存中创建 Windows 运行时类型时使用该符号，而不使用“ * ”
//访问类的具体成员时用“ -> ”

//2〉partial

//partial 关键字指示编译器在另一个代码文件中继续声明该类。
//虽然类定义包含在不同的地方，但在进行编码和编译时，它会被组合起来成为一个类

//3〉sealed
//表示该类不可以被继承。

//4〉property
//在变量的名字前添加 property 关键字后，系统会自动给该变量添加，get/set函数
//类似如下所示：
property int propertyX{  
    int get() { return mX; }  
    void set(int x) { mX = x; };  
} 

//当然你也可以自己写，这样你可以控制这个变量的读写属性。可读、可写、或可读写
property int propertyX{  
    int get() { return mX; }  
}  


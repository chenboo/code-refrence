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

//发现了一个很古怪的东西 “[Windows::UI::Xaml::Data::Bindable]”真服了微软，就喜欢做些修修改改，刚开始
//还真不懂这是个什么东西，又不是注释什么的，在标准C++里面从未见过，后来发现是这么一回事，这是为类添加
//“Bindable”特性，即绑定，这样的话在XAML文件中也能进行访问了。

//还有一个是在ref前面加了个public，ref稍后会讲到，只有在加了ref的类才可以定义为public，标准C++的类是
//不能加public的。

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



//inside C++/CX
Inside the C++/CX Design


Hello. This is Jim Springfield, an architect on the Visual C++ team.
Today, I want to give some insight into the new language extensions, officially called C++/CX, which was designed to support the new API model in Windows 8. If you attended //BUILD/, watched some of the sessions online, or have been playing with the prerelease of Visual Studio, you probably have seen some of the “new” syntax. For anyone who is familiar with C++/CLI (i.e. the language extensions we provide for targeting the CLR), the syntax shouldn’t seem much different.
Please note, however, that while the C++/CX syntax is very similar to C++/CLI, the underlying implementation is very different, it does not use the CLR or a garbage collector, and it generates completely native code (x64, x64, ARM depending on target).
Early on in the design of our support for Windows 8, we looked at many different ideas including a pure library approach as well as various ways to integrate support in the language. We have a long history of supporting COM in the Visual C++ team. From MFC to ATL to #import to attributed ATL. We also have a good bit of experience at targeting the CLR including the original managed extensions, C++/CLI, and the IJW support for compiling native code to MSIL. Our design team consisted of seven people and included people who worked on these and who have lots of experience in libraries, compiler implementation, and language design.
We actually did develop a new C++ template library for Windows 8 called WRL (Windows Runtime Library) that does support targeting Windows 8 without language extensions. WRL is quite good and it can be illuminating to take a look at it and see how all of the low-level details are implemented. It is used internally by many Windows teams, although it does suffer from many of same problems that ATL does in its support of classic COM.
Authoring of components is still very difficult. You have to know a lot of the low-level rules about interfaces.
You need a separate tool (MIDL) to author interfaces/types.
There is no way to automatically map interfaces from low-level to a higher level (modern) form that throws exceptions and has real return values.
There is no unification of authoring and consumption patterns.
With some of the new concepts in the Windows Runtime, these drawbacks become even more difficult than in classic COM/ATL. Interface inheritance isn’t vtable-based like it is in classic COM. Class inheritance is based on a mechanism similar to aggregation but with some differences including support for private and protected interfaces. We quickly realized that although there is a need for a low-level tool like WRL, for the vast majority of uses, it is just too hard to use and we could do a lot better while still preserving performance and providing a lot of control.
The #import feature that was available in VC6 provides a good mechanism for consuming COM objects that have a type library. We thought about providing something similar for the Windows Runtime (which uses a new .winmd file), but while that could provide a good consumption experience, it does nothing for authoring. Given that Windows is moving to a model where many things are asynchronous, authoring of callbacks is very important and there aren’t many consumption scenarios that wouldn’t include at least some authoring. Also, authoring is very important for writing UI applications as each page and user-defined control is a class derived from an existing Runtime class.
The design team spent a lot of time discussing what consumption of Windows Runtime components should look like. We decided early on that we should expose classes and interfaces at a higher level than what the ABI defines. Supporting modern C++ features such as exceptions was deemed to be important as well as mapping the Runtime definition of inheritance (both for interfaces and classes) to C++ in such a way that it was natural. It quickly became clear that we would need some new type categories to represent these as we couldn’t change what the existing C++ ABI meant. We went through a lot of different names and it wasn’t until we decided to use the ^ that we also decided to use ref class to indicate the authoring of a Windows Runtime class.
We also spent a lot of time exploring various approaches to how to hold onto a pointer to a WinRT class or interface. Part of this decision was also how to tell the difference between a low-level version of an interface and the high-level version of the interface. We had a lot of different proposals including just using a *, using * with a modifier, and using various other characters such as the ‘@’ symbol. In the original extensions we did for managed code, we in fact did use a * with a modifier (__gc). We realized we would have many of the same problems if we followed that route. Some of the breakthroughs came when we started thinking about what the type of a pointer dereference would be. This made us realize that what we were doing was similar to what we did when C++/CLI was designed. At one point, someone suggested “Why don’t we just use the ^ symbol?” After the laughter died down, it started making a lot of sense. On design point after design point, we often came to the same design decision we had made for C++/CLI.
Many of the concepts we were trying to express were already present in the C++/CLI syntax. Given that reference counting is a form of garbage collection, using ^ to represent a “refcounted” pointer in ZW fits quite well. Dereferencing of a ^ yields a %, also like C++/CLI. While many concepts are expressed the same way, there are a few areas where we decided to deviate from C++/CLI. For example, in C++/CX, the default interface on a class is specified through an attribute in the interface list while in C++/CLI it is an attribute on the class itself.
In C++/CX we have a much better story than C++/CLI when it comes to interoperating references types with regular types. In C++/CLI, managed objects can move around in memory as the garbage collector runs. This means you can’t get the real address of a member (without pinning) or even embed anything except primitive types (i.e. int) into your class. You also cannot put a ^ into a native class or struct. In C++/CX, objects do not move around in memory and thus all of these restrictions are gone. You can put any type into a ref class and you can put a ^ anywhere. This model is much friendlier to normal C++ types and gives the programmer more flexibility in C++/CX.
We will be providing more insight into our design over the coming months. If there are specific things you would like to know more about, please let us know.
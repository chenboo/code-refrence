//类的前向声明
   class CFoo;//declaration of the CFoo class
//可以声明一个类而不定义它
//这个声明,有时候被称为前向声明(forward declaration),在程序中引入了类类型的CFoo.
//在声明之后,定义之前,类CFoo是一个不完全类型(incompete type),即已知CFoo是一个类型,但不知道包含哪些成员.
//不完全类型只能以有限方式使用,不能定义该类型的对象,不完全类型只能用于定义指向该类型的指针及引用,
//或者用于声明(而不是定义)使用该类型作为形参类型或返回类型的函数.


//所举例子中,在头文件CTest.h中,前向声明了class CFoo;

//在测试文件Console.cpp中只包含了头文件CTest.h;程序运行正确。

//C++/CX��΢�����һ�ζ�C++����չ����㲻����΢��Ϊʲô����ϲ����C++������ȥ����ĺܷ��ˣ�
//���Ҳ�֪��΢�����ƽ���ս���ܷ�ɹ���˵����ֻ��һ��ʧ�ܵĳ��ԣ���������˺��ۡ�
//��������Ҫ������������C++/CX�е�һЩ�ص�ɡ�

//һ����������Ķ��壬�ٸ�����
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


//�����µĹؼ��ֽ���

//1��ref

//���Կ������������Ķ����ж��˸���ref �ؼ���
//ref�����������ͣ�������Java��������ø��ref �ؼ���ָʾ������һ�� Windows ����ʱ�������ͣ�ʹ�� ref ʹ�������д������һ�� COM ���롣
//���������������˵��ref ����һ�� COM ������ʵ���� IInspectable �ӿڣ�����������ͨ������ָ�����ġ�
//ϵͳ�Ḻ�����ǵ����ü����������ü���Ϊ0ʱ�����ǻᱻ���١�

//���½�һ�������Ķ���ʱ��Ҫʹ�����µķ�ʽ��
FeedData^ page = ref new FeedData();
//or
auto page = ref new FeedData(); //���������Զ��ƶ϶��������

//�� ^ �� ����Ϊ����ñ�š�������ʾ�������������������͡��ڶ�̬�ڴ��д��� Windows ����ʱ����ʱʹ�ø÷��ţ�����ʹ�á� * ��
//������ľ����Աʱ�á� -> ��

//2��partial

//partial �ؼ���ָʾ����������һ�������ļ��м����������ࡣ
//��Ȼ�ඨ������ڲ�ͬ�ĵط������ڽ��б���ͱ���ʱ�����ᱻ���������Ϊһ����

//3��sealed
//��ʾ���಻���Ա��̳С�

//4��property
//�ڱ���������ǰ��� property �ؼ��ֺ�ϵͳ���Զ����ñ�����ӣ�get/set����
//����������ʾ��
property int propertyX{  
    int get() { return mX; }  
    void set(int x) { mX = x; };  
} 

//��Ȼ��Ҳ�����Լ�д����������Կ�����������Ķ�д���ԡ��ɶ�����д����ɶ�д
property int propertyX{  
    int get() { return mX; }  
}  


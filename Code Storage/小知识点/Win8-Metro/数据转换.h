//在Win8种进行数据的绑定时，有时候要进行数据的转化
//比如说，对于学生成绩，我们需要根据不同的得分，来得出不同的等级
//1〉50分以下的为： “F”
//2〉50分以上60分以下的为： “D”
//3〉60分以上70分以下的为： “C”
//4〉70分以上80分以下的为： “B”
//5〉80分以上90分以下的为： “A”
//6〉90分以上100分以下的为： “A+”
//7〉100分为： “SUPER START！”

//这时候我们就需要进行数据的转化，将数字的值改为字符型的值，具体步骤如下
//1〉我们首先要进行数据的绑定，在XAML文件中

<Grid x:Name="Output" HorizontalAlignment="Left" VerticalAlignment="Top" Grid.Row="1">
    <StackPanel>
        <Border BorderBrush="LightBlue" BorderThickness="4" CornerRadius="20" Margin="5">
            <StackPanel Margin="5">
                <!-- Add converter as a resource to reference it from a Binding. -->
                <TextBlock Style="{StaticResource BasicTextStyle}" Text="Percent grade:" Margin="5" />
                <Slider x:Name="sliderValueConverter" Minimum="1" Maximum="100" Value="70" Margin="5"/>
                <TextBlock Style="{StaticResource BasicTextStyle}" Text="Letter grade:" Margin="5"/>
                <TextBox x:Name="tbValueConverterDataBound" 
                        Text="{Binding ElementName=sliderValueConverter, Path=Value, Mode=OneWay}" 
                            Margin="5" Width="150"/>
            </StackPanel>
        </Border>
    </StackPanel>
</Grid>

//这里将TextBox的数据源绑定为Slider控件的value，此时数据还未进行转化

//2〉开始转化的过程
//创建转化的类，要实现转化，该类必须实现 Windows::UI::Xaml::Data::IValueConverter接口
//具体如下：
//.h文件中：
#pragma once

#include "pch.h"

using namespace Platform;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Interop;

namespace ChangePage
{
	namespace DataBinding
	{
		//[Windows::Foundation::Metadata::WebHostHidden]
		public ref class S2Formatter sealed : Windows::UI::Xaml::Data::IValueConverter
		{
		public:
			S2Formatter();
			virtual Object^ Convert(Object^ value, TypeName targetType, Object^ parameter, String^ language);
			virtual Object^ ConvertBack(Object^ value, TypeName targetType, Object^ parameter, String^ language);
		private:
			~S2Formatter();
		};
	}
}

//实现Convert和ConvertBack函数，在.cpp文件中
#include "pch.h"
#include "S2Formatter.h"

using namespace ChangePage::DataBinding;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::NumberFormatting;

S2Formatter::S2Formatter()
{
}

S2Formatter::~S2Formatter()
{
}

Object^ S2Formatter::Convert(Object^ value, TypeName targetType, Object^ parameter, String^ language)
{
	String^ _grade = "";
	String^ _valueString = "";

	int _value = ((Windows::Foundation::IPropertyValue^)value)->GetInt32();

	if (_value < 50) {
		_grade = "F";
	}
	else if (_value < 60) {
		_grade = "D";
	}
	else if (_value < 70) {
		_grade = "C";
	}
	else if (_value < 80) {
		_grade = "B";
	}
	else if (_value < 90) {
		_grade = "A";
	}
	else if (_value < 100) {
		_grade = "A+";
	}
	else if(_value == 100) {
		_grade = "SUPER STAR!";
	}

	return _grade;
}

Object^ S2Formatter::ConvertBack(Object^ value, TypeName targetType, Object^ parameter, String^ language)
{
	int _value = 0;
	String^ _grade = (String^)value;

	if ("F" == _grade) {
		_value = 49;
	}
	else if ("D" == _grade) {
		_value = 59;
	}
	else if ("C" == _grade) {
		_value = 69;
	}
	else if ("B" == _grade) {
		_value = 79;
	}
	else if ("A" == _grade) {
		_value = 89;
	}
	else if ("A+" == _grade) {
		_value = 99;
	}
	else if ("SUPER START!" == _grade) {
		_value = 100;
	}

	return _value;
}

//这样这个类就具备了转化的功能，可以进行使用了

//3〉在XAML文件中添加转化
//最终的XAML如下：
        <Grid x:Name="Output" HorizontalAlignment="Left" VerticalAlignment="Top" Grid.Row="1">
            <StackPanel>
                <Border BorderBrush="LightBlue" BorderThickness="4" CornerRadius="20" Margin="5">
                    <StackPanel Margin="5">
                        <StackPanel.Resources>
                            <local:S2Formatter x:Key="GradeConverter"/> <!--添加转化的资源  -->
                        </StackPanel.Resources>
                        <TextBlock Style="{StaticResource BasicTextStyle}" Text="Percent grade:" Margin="5" />
                        <Slider x:Name="sliderValueConverter" Minimum="1" Maximum="100" Value="70" Margin="5"/>
                        <TextBlock Style="{StaticResource BasicTextStyle}" Text="Letter grade:" Margin="5"/>
                        <TextBox x:Name="tbValueConverterDataBound"
                                 Text="{Binding ElementName=sliderValueConverter, Path=Value, Mode=TwoWay, Converter={StaticResource GradeConverter}}"
                                 Margin="5" Width="150"/><!--在绑定中添加转化器  -->
                    </StackPanel>
                </Border>
            </StackPanel>
        </Grid>

//如此整个转化就完成了，从此可以看出，转化其实相当的容易，只要实现Windows::UI::Xaml::Data::IValueConverter接口就OK了。

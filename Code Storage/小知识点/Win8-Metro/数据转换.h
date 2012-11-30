//��Win8�ֽ������ݵİ�ʱ����ʱ��Ҫ�������ݵ�ת��
//����˵������ѧ���ɼ���������Ҫ���ݲ�ͬ�ĵ÷֣����ó���ͬ�ĵȼ�
//1��50�����µ�Ϊ�� ��F��
//2��50������60�����µ�Ϊ�� ��D��
//3��60������70�����µ�Ϊ�� ��C��
//4��70������80�����µ�Ϊ�� ��B��
//5��80������90�����µ�Ϊ�� ��A��
//6��90������100�����µ�Ϊ�� ��A+��
//7��100��Ϊ�� ��SUPER START����

//��ʱ�����Ǿ���Ҫ�������ݵ�ת���������ֵ�ֵ��Ϊ�ַ��͵�ֵ�����岽������
//1����������Ҫ�������ݵİ󶨣���XAML�ļ���

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

//���ｫTextBox������Դ��ΪSlider�ؼ���value����ʱ���ݻ�δ����ת��

//2����ʼת���Ĺ���
//����ת�����࣬Ҫʵ��ת�����������ʵ�� Windows::UI::Xaml::Data::IValueConverter�ӿ�
//�������£�
//.h�ļ��У�
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

//ʵ��Convert��ConvertBack��������.cpp�ļ���
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

//���������;߱���ת���Ĺ��ܣ����Խ���ʹ����

//3����XAML�ļ������ת��
//���յ�XAML���£�
        <Grid x:Name="Output" HorizontalAlignment="Left" VerticalAlignment="Top" Grid.Row="1">
            <StackPanel>
                <Border BorderBrush="LightBlue" BorderThickness="4" CornerRadius="20" Margin="5">
                    <StackPanel Margin="5">
                        <StackPanel.Resources>
                            <local:S2Formatter x:Key="GradeConverter"/> <!--���ת������Դ  -->
                        </StackPanel.Resources>
                        <TextBlock Style="{StaticResource BasicTextStyle}" Text="Percent grade:" Margin="5" />
                        <Slider x:Name="sliderValueConverter" Minimum="1" Maximum="100" Value="70" Margin="5"/>
                        <TextBlock Style="{StaticResource BasicTextStyle}" Text="Letter grade:" Margin="5"/>
                        <TextBox x:Name="tbValueConverterDataBound"
                                 Text="{Binding ElementName=sliderValueConverter, Path=Value, Mode=TwoWay, Converter={StaticResource GradeConverter}}"
                                 Margin="5" Width="150"/><!--�ڰ������ת����  -->
                    </StackPanel>
                </Border>
            </StackPanel>
        </Grid>

//�������ת��������ˣ��Ӵ˿��Կ�����ת����ʵ�൱�����ף�ֻҪʵ��Windows::UI::Xaml::Data::IValueConverter�ӿھ�OK�ˡ�

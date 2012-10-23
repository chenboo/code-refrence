//��win8�����ݰ���һ���Ƚ���Ҫ��֪ʶ�㣬���ｲһ��ListView���ݰ󶨵�����
//������֪ListView����Ԫ��ΪListItem������Ҫʹ��ListItem��ʾ��ͬ�����ݣ�������
//���Ǿ���Ҫ��������һЩ���ݽ��а󶨣����Ϊ����ListItem����һ���ĸ�ʽ��ʾ����
//���ǻ���Ҫ��ListItem���ģ�壬������һһ���н��ܡ�

//һ�����ȶ���Ҫ�󶨵����ݸ�ʽ
#pragma once
#include "pch.h"
#include <collection.h>

namespace Libgroupchat 
{
	[Windows::UI::Xaml::Data::Bindable]
	public ref class Item sealed
	{
	public:
		Item();
		property Platform::String^ Imagpath;

	private:
		~Item(){};

	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class ItemSource sealed
	{
	public:
		ItemSource ();

		property Windows::Foundation::Collections::IVector<Item^>^ Items
		{
			Windows::Foundation::Collections::IVector<Item^>^ get() {return m_items; }
		}

	private:

		Platform::Collections::Vector<Item^>^ m_items;

	private:
		~ItemSource () {};

	};
}

//���ﶨ���������࣬ItemSource�Լ�Item��Ϊ��������˰󶨵����ԣ���ʹ����xaml�ļ���Ҳ�ܽ��з��ʡ�

//����ʵ�������ݣ�����������ֵ

//MainPage.xaml.h��
#pragma once

#include "MainPage.g.h"
#include "ItemSource.h"

namespace Libgroupchat
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	private:
		void GetItemData();//���ڸ����ݽ������Ĳ���

	private:
		ItemSource^ m_item;//����һ�����ݵ�ʵ��
		void PageLoadedhandler(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);//��ҳ�濪ʼʱ���м���
	};
}

//MainPage.xaml.cpp��
void MainPage::GetItemData() 
{
	Item^ item;
	item = ref new Item();
	item->Imagpath = "SampleData/Images/1.png";
	m_item->Items->Append(item);

	item = ref new Item();
	item->Imagpath = "SampleData/Images/2.png";
	m_item->Items->Append(item);

	item = ref new Item();
	item->Imagpath = "SampleData/Images/3.png";
	m_item->Items->Append(item);

	item = ref new Item();
	item->Imagpath = "SampleData/Images/4.png";
	m_item->Items->Append(item);

	item = ref new Item();
	item->Imagpath = "SampleData/Images/5.png";
	m_item->Items->Append(item);

	item = ref new Item();
	item->Imagpath = "SampleData/Images/6.png";
	m_item->Items->Append(item);

	this->DataContext = m_item;//����Mainpage������������Ϊm_item��������maipage��xaml�ļ��оͿɼ���
}

void Libgroupchat::MainPage::PageLoadedhandler(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	GetItemData();
}

//������Ϊ���ݽ�������䣬���������ǾͿ�����.xaml�ļ��н�����صĲ�����

//������Ӱ�

<Page
    x:Class="Libgroupchat.MainPage"
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:local="using:Libgroupchat"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
    mc:Ignorable="d"
    Loaded="PageLoadedhandler">

    <Grid Background="White" Name="MainGrid">
        <Grid.RowDefinitions>
            <RowDefinition Height="53"/>
            <RowDefinition Height="*" />
        </Grid.RowDefinitions>

        <Image x:Name="Titlebar" AutomationProperties.Name="A placeholder image" Source="Assets/top_border_left.png"  
                   Stretch="Fill" VerticalAlignment="Top" HorizontalAlignment="Left"/>
        <TextBlock x:Name="TitleName" Text="LibGoupChat" HorizontalAlignment="Center" VerticalAlignment="Center" 
                   FontSize="26.667" Foreground="White" FontWeight="Bold"/>

        <Grid Name="LeftGrid" Grid.Row="1">
            <Grid.RowDefinitions>
                <RowDefinition Height="*"/>
                <RowDefinition Height="53"/>
            </Grid.RowDefinitions>
       
            <ListView x:Name="ItemListView" ItemsSource="{Binding Path=Items}" Margin="10,0,0,10">
                <ListView.ItemTemplate>
                    <DataTemplate>
                        <StackPanel>
                            <TextBlock Text="{Binding Path=Imagpath}" />
                        </StackPanel>
                    </DataTemplate>
                </ListView.ItemTemplate>
            </ListView>

            <Grid Name="BottomGrid" Grid.Row="1">
                <Grid.ColumnDefinitions>
                    <ColumnDefinition Width="*"/>
                    <ColumnDefinition Width="100"/>
                </Grid.ColumnDefinitions>
                <TextBox x:Name="InputText" Text="hello" VerticalAlignment="Center" 
                   FontSize="26.667" Foreground="Red" FontWeight="Bold" Margin="10,0"/>
                <Button x:Name="send" Content="Send" Grid.Column="1" VerticalAlignment="Center" Margin="10,10,10,0"/>
            </Grid>
        </Grid>
    </Grid>
</Page>


//�󶨺ܼ�
/*
<ListView x:Name="ItemListView" ItemsSource="{Binding Path=Items}" Margin="10,0,0,10">
    <ListView.ItemTemplate>
        <DataTemplate>
            <StackPanel>
                <TextBlock Text="{Binding Path=Imagpath}" />
            </StackPanel>
        </DataTemplate>
    </ListView.ItemTemplate>
</ListView>
*/

//���Ƚ�ListView��Items���а󶨣�Ȼ��ListItem�е�TextBlock��Item�е�Imagpath���а󶨡�
//����һ����OK�ˣ����н��Ҳ��ȷ������������̵�Ե�ɾͲ��ö�֪�ˡ�΢��Ķ���Ҳ����Դ������
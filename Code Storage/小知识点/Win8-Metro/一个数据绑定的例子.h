//在win8中数据绑定是一个比较重要的知识点，这里讲一个ListView数据绑定的例子
//众所周知ListView的子元素为ListItem，我们要使得ListItem显示不同的内容，在这里
//我们就需要将他们与一些数据进行绑定，其次为了让ListItem按照一定的格式显示数据
//我们还需要给ListItem设计模板，下面来一一进行介绍。

//一、首先定义要绑定的数据格式
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

//这里定义了两个类，ItemSource以及Item并为他们添加了绑定的属性，以使得在xaml文件中也能进行访问。

//二、实例化数据，即让数据有值

//MainPage.xaml.h中
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
		void GetItemData();//用于给数据进行填充的操作

	private:
		ItemSource^ m_item;//定义一个数据的实例
		void PageLoadedhandler(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);//在页面开始时进行加载
	};
}

//MainPage.xaml.cpp中
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

	this->DataContext = m_item;//设置Mainpage的数据上下文为m_item，这样在maipage的xaml文件中就可见了
}

void Libgroupchat::MainPage::PageLoadedhandler(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	GetItemData();
}

//这样就为数据进行了填充，接下来我们就可以在.xaml文件中进行相关的操作了

//三、添加绑定

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


//绑定很简单
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

//首先将ListView与Items进行绑定，然后将ListItem中的TextBlock与Item中的Imagpath进行绑定。
//这样一来就OK了，运行结果也正确。至于其中深刻的缘由就不得而知了。微软的东西也不开源，哎。
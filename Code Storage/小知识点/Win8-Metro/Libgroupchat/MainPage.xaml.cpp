//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace Libgroupchat;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
{
	InitializeComponent();
	m_item = ref new ItemSource();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	(void) e;	// Unused parameter
}

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

	this->DataContext = m_item;
}

void Libgroupchat::MainPage::PageLoadedhandler(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	GetItemData();
}

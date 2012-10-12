//
// ChatPage.xaml.cpp
// Implementation of the ChatPage class
//

#include "pch.h"
#include "ChatPage.xaml.h"

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

// The Basic Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234237

ChatPage::ChatPage()
{
	InitializeComponent();
	m_item = ref new ItemSource();
	GetItemData();
}

/// <summary>
/// Populates the page with content passed during navigation.  Any saved state is also
/// provided when recreating a page from a prior session.
/// </summary>
/// <param name="navigationParameter">The parameter value passed to
/// <see cref="Frame::Navigate(Type, Object)"/> when this page was initially requested.
/// </param>
/// <param name="pageState">A map of state preserved by this page during an earlier
/// session.  This will be null the first time a page is visited.</param>
void ChatPage::LoadState(Object^ navigationParameter, IMap<String^, Object^>^ pageState)
{
	(void) navigationParameter;	// Unused parameter
	(void) pageState;	// Unused parameter
}

/// <summary>
/// Preserves state associated with this page in case the application is suspended or the
/// page is discarded from the navigation cache.  Values must conform to the serialization
/// requirements of <see cref="SuspensionManager::SessionState"/>.
/// </summary>
/// <param name="pageState">An empty map to be populated with serializable state.</param>
void ChatPage::SaveState(IMap<String^, Object^>^ pageState)
{
	(void) pageState;	// Unused parameter
}

void ChatPage::GetItemData() 
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
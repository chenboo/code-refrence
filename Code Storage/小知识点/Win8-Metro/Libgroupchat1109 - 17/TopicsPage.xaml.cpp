//
// TopicsPage.xaml.cpp
// Implementation of the TopicsPage class
//

#include "pch.h"
#include "TopicsPage.xaml.h"
#include "ChatPage.xaml.h"

using namespace Libgroupchat;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Interop;

// The Items Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234233

TopicsPage::TopicsPage()
{
	InitializeComponent();
	m_topics = ref new TopicSource();

	Topic^ topic;
	topic = ref new Topic();
	topic->Name = "语文";
	topic->OnLineCount = 1;
	m_topics->Items->Append(topic);

	topic = ref new Topic();
	topic->Name = "数学";
	topic->OnLineCount = 2;
	m_topics->Items->Append(topic);

	topic = ref new Topic();
	topic->Name = "生物";
	topic->OnLineCount = 3;
	m_topics->Items->Append(topic);

	topic = ref new Topic();
	topic->Name = "化学";
	topic->OnLineCount = 4;
	m_topics->Items->Append(topic);

	topic = ref new Topic();
	topic->Name = "英语";
	topic->OnLineCount = 5;
	m_topics->Items->Append(topic);

	topic = ref new Topic();
	topic->Name = "物理";
	topic->OnLineCount = 6;
	m_topics->Items->Append(topic);
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
void TopicsPage::LoadState(Object^ navigationParameter, IMap<String^, Object^>^ pageState)
{
	(void) navigationParameter;	// Unused parameter
	(void) pageState;	// Unused parameter

	this->DefaultViewModel->Insert("Items", m_topics->Items);
}

//ItemsPage.xaml.cpp:
void TopicsPage::Item_Click(Object^ sender, ItemClickEventArgs^ e)
{
	Topic^ topic = safe_cast<Topic^>(e->ClickedItem);
    App::Current->Resources->Insert("CurrentTopic", topic);
	this->Frame->Navigate(TypeName(ChatPage::typeid), topic->Name);
}
//
// MainPage.xaml.cpp
// Implementation of the MainPage class
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
using namespace Windows::UI::Xaml::Interop;

// The Basic Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234237

MainPage::MainPage()
{
	InitializeComponent();

	// This frame is hidden, meaning it is never shown.  It is simply used to load
    // each scenario page and then pluck out the input and output sections and
    // place them into the UserControls on the main page.
    HiddenFrame = ref new Windows::UI::Xaml::Controls::Frame();
    HiddenFrame->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    ContentRoot->Children->Append(HiddenFrame);
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
void MainPage::LoadState(Object^ navigationParameter, IMap<String^, Object^>^ pageState)
{
	(void) navigationParameter;	// Unused parameter
	(void) pageState;	// Unused parameter

	LoadPage("Libgroupchat.ChatPage");
}

/// <summary>
/// Preserves state associated with this page in case the application is suspended or the
/// page is discarded from the navigation cache.  Values must conform to the serialization
/// requirements of <see cref="SuspensionManager::SessionState"/>.
/// </summary>
/// <param name="pageState">An empty map to be populated with serializable state.</param>
void MainPage::SaveState(IMap<String^, Object^>^ pageState)
{
	(void) pageState;	// Unused parameter
}

void MainPage::LoadPage(Platform::String^ scenarioName)
{
	// Load the ScenarioX.xaml file into the Frame.
    TypeName scenarioType = {scenarioName, TypeKind::Custom};
    HiddenFrame->Navigate(scenarioType, this);

    // Get the top element, the Page, so we can look up the elements
    // that represent the input and output sections of the ScenarioX file.
    Page^ hiddenPage = safe_cast<Page^>(HiddenFrame->Content);

    // Get each element.
    UIElement^ input = safe_cast<UIElement^>(hiddenPage->FindName("Input"));

    if (input == nullptr)
    {
        return;
    }

    // Find the LayoutRoot which parents the input and output sections in the main page.
    Panel^ panel = safe_cast<Panel^>(hiddenPage->FindName("LayoutRoot"));

    if (panel != nullptr)
    {
        unsigned int index = 0;
        UIElementCollection^ collection = panel->Children;

        // Get rid of the content that is currently in the intput and output sections.
        collection->IndexOf(input, &index);
        collection->RemoveAt(index);

        // Populate the input and output sections with the newly loaded content.
        PageContainer->Content = input;
    }
}

void MainPage::InvalidateSize()
{
    InvalidateViewState();
}

void MainPage::InvalidateViewState()
{
    //// Are we going to snapped mode?
    //if (ApplicationView::Value == ApplicationViewState::Snapped)
    //{
    //    Grid::SetRow(DescriptionText, 3);
    //    Grid::SetColumn(DescriptionText, 0);

    //    Grid::SetRow(InputSection, 4);
    //    Grid::SetColumn(InputSection, 0);
    //}
    //else
    //{
    //    Grid::SetRow(DescriptionText, 1);
    //    Grid::SetColumn(DescriptionText, 1);

    //    Grid::SetRow(InputSection, 2);
    //    Grid::SetColumn(InputSection, 1);
    //}

    ////  Since we don't load the scenario page in the traditional manner (we just pluck out the
    //// input and output sections from the page) we need to ensure that any VSM code used
    //// by the scenario's input and output sections is fired.
    //VisualStateManager::GoToState(InputSection, "Input" + LayoutAwarePage::DetermineVisualState(ApplicationView::Value), false);
    //VisualStateManager::GoToState(OutputSection, "Output" + LayoutAwarePage::DetermineVisualState(ApplicationView::Value), false);
}

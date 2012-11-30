//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace EditCtrl
{
	public ref class PictureInfo sealed
	{
	public:
		PictureInfo(){};
		property Platform::String^ ImagPath;
		property int Nindex;

	private:
		~PictureInfo(){};
	};

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
		void OnInsertPicture(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnInsertFace(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPaint(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSend(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSave(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void SendTheMsg2Self();

	private:
		Platform::String^ m_strText;
		Platform::Collections::Vector<PictureInfo^>^ m_PicTurePathes;
	};
}

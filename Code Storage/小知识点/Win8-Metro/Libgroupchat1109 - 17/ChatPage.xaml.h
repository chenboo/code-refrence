//
// ChatPage.xaml.h
// Declaration of the ChatPage class
//
#pragma once

#include "Common\LayoutAwarePage.h" // Required by generated header
#include "ChatPage.g.h"
#include "Picture.h"
#include "ChatDatas.h"

ref class Document;
ref class Element;

template <typename T>
inline void SafeRelease(T *&pI)
{
	if (NULL != pI)
	{
		pI->Release();
		pI = NULL;
	}
}

namespace Libgroupchat
{
	/// <summary>
	/// A basic page that provides characteristics common to most applications.
	/// </summary>
	public ref class ChatPage sealed
	{
	public:
		ChatPage();

	protected:
		void SendTheMsg2Self();
		void OnSend(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnInsertPicture(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		virtual void LoadState(Platform::Object^ navigationParameter,
			Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;
		virtual void SaveState(Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;

		void RebulidPictureCollections();
		void ParseStream2PictureInfo(Platform::String^ strFilePath, StructData::PictureInfo^ pictureInfo);
		[Windows::Foundation::Metadata::DefaultOverloadAttribute]
		void ParseStream2PictureInfo(Windows::Storage::Streams::IRandomAccessStream^ fileStream, StructData::PictureInfo^ pictureInfo);
		
		void OnDeviceLost();
		void OnWindowSizeChanged(
			_In_ Platform::Object^ sender,
			_In_ Windows::UI::Core::WindowSizeChangedEventArgs^ args
			);

	private:
		void InitializeWic();

		void LoadDocument();
		void DocumentLoaded(_In_ Document^ document);

	private:
		Platform::String^	m_strText;
		ChatDatas^			m_chatDatas;
		int					m_nPictureID;

		// Current document loaded
		Document^			m_document;
		PageModel^			m_PageModle;

		Microsoft::WRL::ComPtr<IWICImagingFactory2>						m_wicFactory;
		Microsoft::WRL::ComPtr<IWICBitmapDecoder>						m_wicDecoder;

		Platform::Collections::Vector<StructData::PictureInfo^>^		m_DynamicPictures;
		Platform::Collections::Vector<StructData::PictureInfo^>^		m_NormalPictures;

		Platform::Collections::Vector<StructData::PictureInfo^>^		m_DynamicListPics;
		Platform::Collections::Vector<StructData::PictureInfo^>^		m_NormalListPics;
	};
}

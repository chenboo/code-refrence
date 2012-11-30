//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <string.h>
#include <stdlib.h>

using namespace EditCtrl;

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
using namespace Windows::Storage::Pickers;
using namespace Windows::UI;
using namespace concurrency;
using namespace Windows::Storage;
using namespace Windows::UI::Text;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Media::Imaging;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
{
	InitializeComponent();
	m_strText = "";
	m_PicTurePathes = ref new Platform::Collections::Vector<PictureInfo^>();
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

void EditCtrl::MainPage::OnInsertPicture(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ITextDocument^ iDocument = Editor->Document;
	auto open = ref new FileOpenPicker();
	open->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
	open->ViewMode = PickerViewMode::Thumbnail;

	// Filter to include a sample subset of file types
	open->FileTypeFilter->Clear();
	open->FileTypeFilter->Append(".png");
	open->FileTypeFilter->Append(".jpg");
	open->FileTypeFilter->Append(".gif");

	// Open a stream for the selected file
	create_task(open->PickSingleFileAsync()).then([=](StorageFile^ file)
	{
		if (file)
		{
			// Ensure the stream is disposed once the image is loaded
			create_task(file->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([=](IRandomAccessStream^ fileStream)
			{
				// Set the image source to the selected bitmap
				auto bitmapImage = ref new BitmapImage();
				bitmapImage->SetSource(fileStream);
				int nWidth = bitmapImage->PixelWidth;
				int nHeight = bitmapImage->PixelHeight;
				String^ string = "[" + file->Path + "]";
				iDocument->Selection->InsertImage(nWidth, nHeight, 0, Text::VerticalCharacterAlignment::Baseline, file->Path, fileStream);
			});
		}
	});
}

void EditCtrl::MainPage::OnInsertFace(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}

void EditCtrl::MainPage::OnPaint(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}

void EditCtrl::MainPage::OnSend(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_strText = "";
	ITextDocument^ iDocument = Editor->Document;

	//get the text of the input richeditbox
	String^ strText = "";
	iDocument->GetText(TextGetOptions::None, &strText);
	m_strText = strText;

	int nLength = strText->Length();
	ITextRange^ iTextRange = nullptr;
	const wchar_t* pChar = strText->Data();

	//form the text and pictures array
	for (int i = 0; i < nLength; i++) {
		if (pChar[i] == 0xFFFC) {
			iTextRange = iDocument->GetRange(i, i + 1);

			String^ strImgPath = ref new String();
			iTextRange->GetText(TextGetOptions::UseObjectText, &strImgPath);

			PictureInfo^ picInfo = ref new PictureInfo();
			picInfo->Nindex = i;
			picInfo->ImagPath = strImgPath;
			m_PicTurePathes->Append(picInfo);
		}
	}

	String^ strOutPut = "";
	iDocument->SetText(TextSetOptions::None, strOutPut);
	SendTheMsg2Self();
}

void EditCtrl::MainPage::SendTheMsg2Self()
{
	ITextDocument^ iDocument = Listor->Document;
	iDocument->SetText(TextSetOptions::None, m_strText);
	int nPictureCount = m_PicTurePathes->Size;

	for (int i = 0; i < nPictureCount; i++) {
		PictureInfo^ picInfo = m_PicTurePathes->GetAt(i);

		auto feedOp = StorageFile::GetFileFromPathAsync(picInfo->ImagPath);
		create_task(feedOp).then([=](StorageFile^ file)
		{
			if (file)
			{
				// Ensure the stream is disposed once the image is loaded
				create_task(file->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([=](IRandomAccessStream^ fileStream)
				{
					// Set the image source to the selected bitmap
					auto bitmapImage = ref new BitmapImage();
					bitmapImage->SetSource(fileStream);
					int nWidth = bitmapImage->PixelWidth;
					int nHeight = bitmapImage->PixelHeight;

					ITextRange^ iTextRange = iDocument->GetRange(picInfo->Nindex, picInfo->Nindex + 1);
					iTextRange->InsertImage(nWidth, nHeight, 0, Text::VerticalCharacterAlignment::Baseline, picInfo->ImagPath, fileStream);
				});
			}
		});
	}

	m_PicTurePathes->Clear();
}

void EditCtrl::MainPage::OnSave(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{


}
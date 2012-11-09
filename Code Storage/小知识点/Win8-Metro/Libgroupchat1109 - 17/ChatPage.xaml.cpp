//
// ChatPage.xaml.cpp
// Implementation of the ChatPage class
//

#include "pch.h"
#include "ChatPage.xaml.h"
#include "TopicsPage.xaml.h"

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
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Text;
using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::Graphics::Display;
using namespace Windows::ApplicationModel;

using namespace StructData;

// The Basic Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234237

ChatPage::ChatPage() :
	m_PageModle(nullptr)
{
	InitializeComponent();
	InitializeWic();
	m_chatDatas = ref new ChatDatas();

	m_DynamicPictures = ref new Platform::Collections::Vector<PictureInfo^>();
	m_NormalPictures = ref new Platform::Collections::Vector<PictureInfo^>();
	m_DynamicListPics = ref new Platform::Collections::Vector<PictureInfo^>();
	m_NormalListPics = ref new Platform::Collections::Vector<PictureInfo^>();

	// Start loading the document content
	LoadDocument();
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
	//this->DefaultViewModel->Insert("Items", m_chatDatas->Items);

	if (nullptr != navigationParameter)
	{
		Platform::String^ Text = safe_cast<Platform::String^>(navigationParameter);
		Title->Text = Text;
	}
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

void ChatPage::OnSend(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//m_dispatcherTimer->Stop();

	RebulidPictureCollections();

	//assign the pictures to the listbox vectors
	//and clear the picture vectors in editbox
	m_DynamicListPics->Clear();
	m_NormalListPics->Clear();

	for (int i = 0; i < m_DynamicPictures->Size; i++) {
		m_DynamicListPics->Append(m_DynamicPictures->GetAt(i));
	}

	for (int i = 0; i < m_NormalPictures->Size; i++) {
		m_NormalListPics->Append(m_NormalPictures->GetAt(i));
	}

	m_DynamicPictures->Clear();
	m_NormalPictures->Clear();

	Editor->Document->SetText(TextSetOptions::None, "");
	SendTheMsg2Self();

	//if (m_DynamicListPics->Size > 0) {
	//	m_dispatcherTimer->Start();
	//}
}

void ChatPage::SendTheMsg2Self()
{
	String^ strSpeaker = "PSM";
	SYSTEMTIME sysTime = {0};
	GetLocalTime(&sysTime);

	String^ strTime = " (" + sysTime.wYear + "/" 
		+ sysTime.wMonth + "/"
		+ sysTime.wDay + " "
		+ sysTime.wHour + ":"
		+ sysTime.wMinute + ":"
		+ sysTime.wSecond + ")";

	String^ strText = "" + m_strText;
	//ChatMessage^ Message = ref new ChatMessage();
	//Message->Speaker = strSpeaker;
	//Message->Time = strTime;
	//Message->Content = strText;
	//m_chatDatas->Items->Append(Message);

	String^ strTitleText = strSpeaker + strTime + "\n";
	String^ strTalkText = "" + m_strText;

	auto contentRoot = m_document->GetContentRoot();
	Message^ message = ref new Message();
	contentRoot->AddChild(message);

	MTitle^ title = ref new MTitle();
	message->AddChild(title);
	Text^ titleText = ref new Text(m_document, strTitleText);
	title->AddChild(titleText);

	Talk^ talk = ref new Talk();
	message->AddChild(talk);
	Text^ talkText = ref new Text(m_document, strTalkText);
	talk->AddChild(talkText);

	m_PageModle->ContentChanged();

	D2D1_RECT_F rc = message->GetRcLayout();
	RECT rct;
	rct.left = (int)rc.left;
	rct.top = (int)rc.top;
	rct.right = (int)(rc.right + 0.5f);
	rct.bottom = (int)(rc.bottom + 0.5f);
	m_PageModle->InvalidateRect(rct);
}

void ChatPage::OnInsertPicture(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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
				PictureInfo^ picInfo = ref new PictureInfo();
				picInfo->ImagPath = file->Path + "[" + m_nPictureID++ + "]";
				ParseStream2PictureInfo(fileStream, picInfo);

				FrameInfo^ frameInfo = picInfo->GetNextFrame();
				ITextDocument^ iDocument = Editor->Document;
				iDocument->Selection->InsertImage(frameInfo->nWidth,
					frameInfo->nHeight,
					0,
					VerticalCharacterAlignment::Baseline,
					picInfo->ImagPath, frameInfo->FrameStream);

				if (picInfo->IsDynamicPicture()) {
					m_DynamicPictures->Append(picInfo);
				}
				else {
					m_NormalPictures->Append(picInfo);
				}
			});
		}
	});
}

void ChatPage::InitializeWic()
{
	DX::ThrowIfFailed(
		CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_wicFactory)
		)
		);
}

void ChatPage::ParseStream2PictureInfo(Platform::String^ strFilePath, StructData::PictureInfo^ pictureInfo)
{
	ITextDocument^ iDocument = Editor->Document;
	auto feedOp = StorageFile::GetFileFromPathAsync(strFilePath);
	create_task(feedOp).then([=](StorageFile^ file)
	{
		if (file)
		{
			// Ensure the stream is disposed once the image is loaded
			create_task(file->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([=](IRandomAccessStream^ fileStream)
			{
				ParseStream2PictureInfo(fileStream, pictureInfo);
			});
		}
	});
}

void ChatPage::ParseStream2PictureInfo(Windows::Storage::Streams::IRandomAccessStream^ fileStream, StructData::PictureInfo^ pictureInfo)
{
	// open the picture to the wic object IWICBitmapDecoder
	ComPtr<IStream> istream;
	DX::ThrowIfFailed(
		CreateStreamOverRandomAccessStream(
		reinterpret_cast<IUnknown*>(fileStream),
		IID_PPV_ARGS(&istream)
		)
		);

	HRESULT hr = m_wicFactory->CreateDecoderFromStream(
		istream.Get(),
		nullptr,
		WICDecodeMetadataCacheOnDemand,
		&m_wicDecoder
		);

	WICPixelFormatGUID pixelFormat = {0};
	UINT nFrameCount, nImageWidth, nImageHeight;
	m_wicDecoder->GetFrameCount(&nFrameCount);

	for (int i = 0; i < nFrameCount; i++) {

		//initliase the encode stream
		InMemoryRandomAccessStream^ memoryStream = ref new InMemoryRandomAccessStream();
		ComPtr<IStream> stream;
		DX::ThrowIfFailed(
			CreateStreamOverRandomAccessStream(memoryStream, IID_PPV_ARGS(&stream))
			);

		ComPtr<IWICBitmapEncoder> wicBitmapEncoder;
		DX::ThrowIfFailed(
			m_wicFactory->CreateEncoder(
			GUID_ContainerFormatBmp,
			nullptr,    // No preferred codec vendor.
			&wicBitmapEncoder
			)
			);

		DX::ThrowIfFailed(
			wicBitmapEncoder->Initialize(
			stream.Get(),
			WICBitmapEncoderNoCache
			)
			);

		//get the specific source frame and infomation
		IWICBitmapFrameDecode* pSourceFrame = NULL;
		m_wicDecoder->GetFrame(i, &pSourceFrame);
		pSourceFrame->GetSize(&nImageWidth, &nImageHeight);
		pSourceFrame->GetPixelFormat(&pixelFormat);

		// Create and initialize WIC Frame Encoder.
		IWICBitmapFrameEncode* pTargetFrame = NULL;
		DX::ThrowIfFailed(
			wicBitmapEncoder->CreateNewFrame(
			&pTargetFrame,
			nullptr     // No encoder options.
			)
			);

		DX::ThrowIfFailed(
			pTargetFrame->Initialize(nullptr)
			);

		pTargetFrame->SetSize(nImageWidth, nImageHeight);
		pTargetFrame->SetPixelFormat(&pixelFormat);
		pTargetFrame->WriteSource(pSourceFrame, 0);
		pTargetFrame->Commit();
		wicBitmapEncoder->Commit();
		SafeRelease(pSourceFrame);

		FrameInfo^ frameInfo = ref new FrameInfo();
		frameInfo->nWidth = nImageWidth;
		frameInfo->nHeight = nImageHeight;
		frameInfo->FrameStream = memoryStream;
		pictureInfo->Frames->Append(frameInfo);
	}
}

void ChatPage::RebulidPictureCollections()
{
	ITextDocument^ iDocument = Editor->Document;
	m_strText = ref new String();
	iDocument->GetText(TextGetOptions::None, &m_strText);

	int nLength = m_strText->Length();
	ITextRange^ iTextRange = nullptr;
	const wchar_t* pChar = m_strText->Data();

	Platform::Collections::Vector<StructData::PictureInfo^>^ dynamicPics = ref new Platform::Collections::Vector<PictureInfo^>();
	Platform::Collections::Vector<StructData::PictureInfo^>^ normalPics = ref new Platform::Collections::Vector<PictureInfo^>();

	//form the text and pictures array
	for (int i = 0; i < nLength; i++) {
		if (pChar[i] == 0xFFFC) {
			iTextRange = iDocument->GetRange(i, i + 1);

			String^ strImgPath = ref new String();
			iTextRange->GetText(TextGetOptions::UseObjectText, &strImgPath);

			for (int j = 0; j < m_NormalPictures->Size; j++) {
				PictureInfo^ pictureInfo = m_NormalPictures->GetAt(j);

				if (pictureInfo->ImagPath == strImgPath) {
					pictureInfo->nPos = i;
					normalPics->Append(pictureInfo);
					break;
				}
			}

			for (int k = 0; k < m_DynamicPictures->Size; k++) {
				PictureInfo^ pictureInfo = m_DynamicPictures->GetAt(k);

				if (pictureInfo->ImagPath == strImgPath) {
					pictureInfo->nPos = i;
					dynamicPics->Append(pictureInfo);
					break;
				}
			}
		}
	}

	m_NormalPictures->Clear();
	m_DynamicPictures->Clear();

	m_NormalPictures = normalPics;
	m_DynamicPictures = dynamicPics;
}

//add new lines
void ChatPage::LoadDocument()
{
    // Initialize DirectX renderer
    auto renderer = ref new Renderer(
        Window::Current->Bounds,
        DisplayProperties::LogicalDpi
        );

    // Start loading the document content
    auto document = ref new Document(Package::Current->InstalledLocation, "Message.xml", renderer);
    task<void>(document->LoadAsync()).then([=]()
    {
        DocumentLoaded(document);

    }, task_continuation_context::use_current());

    renderer->DeviceLost += ref new DeviceLostEventHandler(this, &ChatPage::OnDeviceLost);

    Window::Current->SizeChanged +=
        ref new WindowSizeChangedEventHandler(this, &ChatPage::OnWindowSizeChanged);
}

void ChatPage::DocumentLoaded(_In_ Document^ document)
{
    // Parse the document into an element tree.
    document->Parse();
	auto contentRoot = document->GetContentRoot();

	if (contentRoot != nullptr) {
		m_PageModle = ref new PageModel(contentRoot, document);
		this->DataContext = m_PageModle;
	}

	m_document = document;
}

void ChatPage::OnDeviceLost()
{
    DocumentLoaded(m_document);
}

void ChatPage::OnWindowSizeChanged(
    _In_ Platform::Object^ sender,
    _In_ Windows::UI::Core::WindowSizeChangedEventArgs^ args
    )
{
    if (m_document != nullptr)
    {
        // Update renderer with the new window size
        auto renderer = m_document->GetRenderer();
        renderer->UpdateWindowSize();

        // Window size change could mean changing display orientation. When this happens the display
        // content may change to suit the new display layout. We need to update the page model with
        // all the new content.
        auto contentRoot = m_document->GetContentRoot();

		if (nullptr != contentRoot) {
			m_PageModle->UpdateContent(contentRoot);
		}
    }
}
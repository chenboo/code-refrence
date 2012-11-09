#include "pch.h"

using namespace Microsoft::WRL;
using namespace Windows::Data::Xml::Dom;

Text::Text() :
	Element(),
	m_brush(nullptr),
	m_contentBitmap(),
	m_displaySize(),
	m_color(0)
{
}

Text::Text(
	_In_ Document^ document,
	Platform::String^ text
	) :
Element(),
	m_brush(nullptr),
	m_contentBitmap(),
	m_displaySize(),
	m_color(0)
{
	m_strText = text;
	InitializeTextLayout(document, text);
}

bool Text::Initialize(
	_In_ Document^ document,
	_In_ XmlElement^ xmlElement
	)
{
	if (!Element::Initialize(document, xmlElement))
	{
		return false;
	}

	// This call concatenates all text within this DOM element including
	// text inside each child element.
	Platform::String^ text = GetText(xmlElement);
	InitializeTextLayout(document, text);
	m_strText = text;
	return true;
}

void Text::Measure(
	_In_ Document^ document,
	_In_ D2D1_POINT_2F pos,
	_Out_ D2D1_RECT_F* bounds
	)
{

	Platform::String^ str = m_strText;

	D2D1_SIZE_F size = D2D1::SizeF(1200, FLT_MAX);

	// Set layout max width and height for text
	DX::ThrowIfFailed(
		m_textLayout->SetMaxWidth(size.width)
		);

	DX::ThrowIfFailed(
		m_textLayout->SetMaxHeight(size.height)
		);

	// Set proper horizontal alignment and line spacing
	DX::ThrowIfFailed(
		m_textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)
		);

	DX::ThrowIfFailed(
		m_textLayout->SetLineSpacing(
		DWRITE_LINE_SPACING_METHOD_DEFAULT,
		0,
		0
		)
		);

	// Calculate the text layout metrics
	DWRITE_TEXT_METRICS textMetrics = {0};

	DX::ThrowIfFailed(
		m_textLayout->GetMetrics(&textMetrics)
		);

	// The final height is the text layout height plus top and bottom margin.
	m_size = D2D1::SizeF(textMetrics.width, textMetrics.height);

	m_rcLayout.left = pos.x;
	m_rcLayout.top = pos.y;
	m_rcLayout.right = m_rcLayout.left + m_size.width;
	m_rcLayout.bottom = m_rcLayout.top + m_size.height;

	*bounds = D2D1::RectF(
		0,
		0,
		m_size.width,
		m_size.height
		);
}

bool Text::Draw(
	_In_ Document^ document,
	D2D1::Matrix3x2F const& transform
	)
{
	auto renderer = document->GetRenderer();

	ComPtr<ID2D1DeviceContext> d2dDeviceContext;
	renderer->GetD2DDeviceContext(&d2dDeviceContext);

	if (m_brush == nullptr)
	{
		ComPtr<ID2D1SolidColorBrush> solidColorBrush;

		DX::ThrowIfFailed(
			d2dDeviceContext->CreateSolidColorBrush(
			D2D1::ColorF(0x112233, 1),
			D2D1::BrushProperties(),
			&solidColorBrush
			)
			);

		m_brush = solidColorBrush;
	}

	d2dDeviceContext->SetTransform(transform);

	d2dDeviceContext->DrawTextLayout(
		D2D1::Point2F(0, 0),
		m_textLayout.Get(),
		m_brush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_NONE
		);

	return false;
}

Platform::String^ Text::GetText(_In_ Windows::Data::Xml::Dom::IXmlNode^ xmlNode)
{
	if (xmlNode->NodeType == NodeType::TextNode)
	{
		return dynamic_cast<Platform::String^>(xmlNode->NodeValue);
	}

	Platform::String^ text = nullptr;

	if (xmlNode->HasChildNodes())
	{
		IXmlNode^ childXmlNode = xmlNode->FirstChild;

		while (childXmlNode != nullptr)
		{
			text = Platform::String::Concat(text, GetText(childXmlNode));

			childXmlNode = childXmlNode->NextSibling;
		}
	}

	return text;
}

void Text::InitializeTextLayout(
	_In_ Document^ document,
	_In_ Platform::String^ text
	)
{
	float fontSize = 16.0f;
	DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL;
	DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL;
	DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL;

	ComPtr<IDWriteFactory> dwriteFactory;
	// Obtain DirectWrite factory from the current document
	document->GetDWriteFactory(&dwriteFactory);

	// Creating and setting up text format and layout. This steps are
	// merely property setting. The actual work of laying out text is
	// deferred until the application asks for result or until the layout
	// is being drawn.
	ComPtr<IDWriteTextFormat> textFormat;

	DX::ThrowIfFailed(
		dwriteFactory->CreateTextFormat(
		L"Verdana",
		nullptr,    // Proper font collection will be set upon drawing
		DWRITE_FONT_WEIGHT_NORMAL,
		fontStyle,
		fontStretch,
		fontSize,
		L"en-US",
		&textFormat
		)
		);

	m_textLength = text->Length();

	if (m_textLength != 0)
	{
		ComPtr<IDWriteTextLayout> textLayout;

		DX::ThrowIfFailed(
			dwriteFactory->CreateTextLayout(
			text->Data(),
			static_cast<uint32>(m_textLength),
			textFormat.Get(),
			FLT_MAX,
			FLT_MAX,
			&textLayout
			)
			);

		textLayout.As(&m_textLayout);
	}
}
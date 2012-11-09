#pragma once
#include "Element.h"

ref class Text : public Element
{
internal:
    Text();

	Text(
		_In_ Document^ document,
		Platform::String^ text
		);

    virtual bool Initialize(
        _In_ Document^ document,
        _In_ Windows::Data::Xml::Dom::XmlElement^ xmlElement
        ) override;

	virtual void Measure(
		_In_ Document^ document,
		_In_ D2D1_POINT_2F pos,
		_Out_ D2D1_RECT_F* bounds
		) override;

    virtual bool Draw(
        _In_ Document^ document,
        D2D1::Matrix3x2F const& transform
        ) override;


private:
	Platform::String^ GetText(_In_ Windows::Data::Xml::Dom::IXmlNode^ xmlNode);

	void InitializeTextLayout(
		_In_ Document^ document,
		_In_ Platform::String^ text
		);

private:
	// Brush to fill the text
	Microsoft::WRL::ComPtr<ID2D1Brush> m_brush;

	// Intermediate surface containing the drawing of text content.
	// This is only needed when effect is used for the element.
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_contentBitmap;

	// DirectWrite text layout
	Microsoft::WRL::ComPtr<IDWriteTextLayout1> m_textLayout;

	// The display size the content caching is for.
	D2D1_SIZE_F m_displaySize;

	// Text color
	uint32 m_color;

	// Total character length of the text including all spans
	size_t m_textLength;

	// Source text of the text frame
	::Text^ m_sourceText;


	Platform::String^ m_strText;
};

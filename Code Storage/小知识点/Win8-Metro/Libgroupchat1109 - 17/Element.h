#pragma once

ref class Document;
ref class Renderer;

ref class Element
{
internal:
    Element();

    virtual void ReleaseParent();

    template <typename T>
    static T^ Create(
        _In_ Document^ document,
        _In_ Windows::Data::Xml::Dom::IXmlNode^ xmlNode
        );

    virtual bool Initialize(
        _In_ Document^ document,
        _In_ Windows::Data::Xml::Dom::XmlElement^ xmlElement
        );

    virtual bool AcceptChildNode(
        _In_ Document^ document,
        _In_ Windows::Data::Xml::Dom::IXmlNode^ childXmlNode
        );

    virtual void Measure(
        _In_ Document^ document,
		_In_ D2D1_POINT_2F pos,
        _Out_ D2D1_RECT_F* bounds
        );

    virtual bool Draw(
        _In_ Document^ document,
        D2D1::Matrix3x2F const& transform
        );

    virtual float GetOpacity()
    {
        return 1.0f;
    }

    inline D2D1_SIZE_F GetSize()
    {
        return m_size;
    }

    inline Platform::String^ GetName()
    {
        return m_name;
    }

    void AddChild(_In_ Element^ element);
    void AddSibling(_In_ Element^ element);

    inline Element^ GetParent()
    {
        return m_parent;
    }

    inline Element^ GetFirstChild()
    {
        return m_firstChild;
    }

    inline Element^ GetNextSibling()
    {
        return m_nextSibling;
    }

	inline D2D1_RECT_F GetRcLayout() const
	{
		return m_rcLayout;
	}

protected private:
    bool DrawChildren(
        _In_ Document^ document,
        D2D1::Matrix3x2F const& transform
        );

    // Parse color string of the form "#RRGGBB" of the "color" attribute
    uint32 ParseColor(_In_ Platform::String^ value);

    // Parse resource's name identifier of the form "#name"
    Platform::String^ ParseNameIdentifier(_In_ Platform::String^ value);

    // Name identifier of the element
    Platform::String^ m_name;

    // Element size
    D2D1_SIZE_F m_size;

	//the layout rect of the element
	D2D1_RECT_F	m_rcLayout;

    // Reference to the parent element. This dependency needs to be explicitly
    // released to avoid cycle dependency.
    Element^ m_parent;

    // Reference to the next element of the same parent
    Element^ m_nextSibling;

    // Reference to the first child element
    Element^ m_firstChild;
};

//  Element creation wrapper, which instantiates and initializes a element as well as
//  all its children. The process stops when all element are created or when a element
//  is rejected due to parsing error.
//
template <typename T>
T^ Element::Create(
    _In_ Document^ document,
    _In_ Windows::Data::Xml::Dom::IXmlNode^ xmlNode
    )
{
    T^ element = nullptr;

    if (xmlNode->NodeType == NodeType::ElementNode)
    {
        Windows::Data::Xml::Dom::XmlElement^ xmlElement = dynamic_cast<Windows::Data::Xml::Dom::XmlElement^>(xmlNode);

        element = ref new T();

        if (element)
        {
            if (element->Initialize(document, xmlElement))
            {
                if (xmlNode->HasChildNodes())
                {
                    Windows::Data::Xml::Dom::IXmlNode^ childXmlNode = xmlNode->FirstChild;

                    while (childXmlNode != nullptr)
                    {
                        if (!element->AcceptChildNode(document, childXmlNode))
                        {
                            break;
                        }

                        childXmlNode = childXmlNode->NextSibling;
                    }
                }
            }
            else
            {
                element = nullptr;
            }
        }
    }

    return element;
}

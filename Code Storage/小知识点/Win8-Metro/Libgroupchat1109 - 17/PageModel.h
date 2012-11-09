#pragma once

class ContentImageSource;

ref class Document;
ref class Element;

namespace Libgroupchat
{
    [Windows::UI::Xaml::Data::Bindable]
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class PageModel sealed :
        public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

        property Windows::UI::Xaml::Media::ImageSource^ Content
        {
            Windows::UI::Xaml::Media::ImageSource^ get()
            {
                return m_content->GetImageSource();
            }
        }

        property int32 ContentWidth
        {
            int32 get()
            {
                return m_content->GetImageSize().cx;
            }
        }

        property int32 ContentHeight
        {
            int32 get()
            {
                return m_content->GetImageSize().cy;
            }
        }

    internal:
        PageModel(
            _In_ Element^ content,
            _In_ Document^ document
            );


        void UpdateContent(_In_ Element^ content);
		void ContentChanged();

		void InvalidateRect(const RECT& rc);

    private:
        // Image source of the content image
        Microsoft::WRL::ComPtr<ContentImageSource> m_content;
    };
}

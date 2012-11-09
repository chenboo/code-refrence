#pragma once

namespace Libgroupchat
{
	namespace StructData {
		public ref class FrameInfo sealed
		{
		public:
			FrameInfo();

			property int nWidth;
			property int nHeight;
			property Windows::Storage::Streams::IRandomAccessStream^ FrameStream;

		private:
			~FrameInfo();
		};
	}
}
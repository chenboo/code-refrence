//在Metro中对于文件的操作很是受限制

//以下说一个从文件路径创建文件的过程，首先要设置程序的packet.appxmanifest中capacity，让它具备访问
//公共文档的权限

//strpath的路径为公共文档的位置下的一张图片
String^ strPath = "D:\\Users\\Public\\Pictures\\ShareStart.png";
auto feedOp = StorageFile::GetFileFromPathAsync(strPath);
create_task(feedOp).then([=](StorageFile^ file) {
	if (file) {
		// Ensure the stream is disposed once the image is loaded
		create_task(file->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([=](IRandomAccessStream^ fileStream) {
		});
	}
});
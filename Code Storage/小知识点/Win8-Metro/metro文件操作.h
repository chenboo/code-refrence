//��Metro�ж����ļ��Ĳ�������������

//����˵һ�����ļ�·�������ļ��Ĺ��̣�����Ҫ���ó����packet.appxmanifest��capacity�������߱�����
//�����ĵ���Ȩ��

//strpath��·��Ϊ�����ĵ���λ���µ�һ��ͼƬ
String^ strPath = "D:\\Users\\Public\\Pictures\\ShareStart.png";
auto feedOp = StorageFile::GetFileFromPathAsync(strPath);
create_task(feedOp).then([=](StorageFile^ file) {
	if (file) {
		// Ensure the stream is disposed once the image is loaded
		create_task(file->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([=](IRandomAccessStream^ fileStream) {
		});
	}
});
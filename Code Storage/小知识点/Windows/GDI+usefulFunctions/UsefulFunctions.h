//Graphics
1>Clear:				使用某种颜色清除背景
2>ExcludeClip:				对绘制区域进行剪辑
3>SetClip:
4>DrawImage(Image *image,			
	const Point *destPoints,
	INT count);
//用于绘制扭曲的图像,count必须为3,
//由destPoints数组决定的三个点构成一个平行四边形,
//平行四边形的第四点由函数自己算出。


//Image
1>GetThumbnailImage:			获得图形的缩略图

//Color
包含alpha通道可以用于绘制具有透明度的图像
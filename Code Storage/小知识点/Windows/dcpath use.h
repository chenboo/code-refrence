//GDI 路径的使用

//常用法:

pDC->BeginPath();

//do something drawing opration

//dc.Polygon(pt, 4);
//dc.MoveTo(100, 100);
//dc.LineTo(200, 200);

pDC->EndPath();
pDC->StrokePath();


//当调用BeginPath将会自动丢弃之前的路径
//StrokePath()函数的功能是:
//使用当前DC中选择的画笔对路径进行描边。

//其他用法:
//将路径所在区域从剪辑区域中删除

pDC->BeginPath();
pDC->Ellipse(ptLT.x,ptLT.y,ptRB.x,ptRB.y);
pDC->EndPath();
pDC->SelectClipPath(RGN_DIFF);





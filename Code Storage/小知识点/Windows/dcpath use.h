//GDI ·����ʹ��

//���÷�:

pDC->BeginPath();

//do something drawing opration

//dc.Polygon(pt, 4);
//dc.MoveTo(100, 100);
//dc.LineTo(200, 200);

pDC->EndPath();
pDC->StrokePath();


//������BeginPath�����Զ�����֮ǰ��·��
//StrokePath()�����Ĺ�����:
//ʹ�õ�ǰDC��ѡ��Ļ��ʶ�·��������ߡ�

//�����÷�:
//��·����������Ӽ���������ɾ��

pDC->BeginPath();
pDC->Ellipse(ptLT.x,ptLT.y,ptRB.x,ptRB.y);
pDC->EndPath();
pDC->SelectClipPath(RGN_DIFF);





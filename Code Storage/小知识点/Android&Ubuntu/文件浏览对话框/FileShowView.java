package com.njtd.shareboard.controls;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.text.Layout.Alignment;
import android.text.StaticLayout;
import android.text.TextPaint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

import com.njtd.shareboard.common.FileType;

public class FileShowView extends View {
	private File f = null;
	private Bitmap m_bitmap = null;
	private String m_strText = null;
	private FileType m_fileType = null;
	private boolean m_bCanDrawContext = false;

	public static final int NULL = 4; // 不进行任何绘制
	private static final int TEXT_READ_LENGTH = 300;

	public FileShowView(Context context, AttributeSet paramAttributeSet) {
		super(context, paramAttributeSet);
	}

	public void setFilePath(String strPath) {
		m_fileType = FileType.getFileType(strPath);
		m_bCanDrawContext = false;

		if (FileType.DIR != m_fileType.getMainType()) {
			f = new File(strPath);
			boolean bRead = f.canRead();

			if (bRead) {
				if (FileType.TEXT == m_fileType.getMainType()) {
					if (FileType.TEXT_TXT == m_fileType.getSubType()
							|| FileType.TEXT_XML == m_fileType.getSubType()) {
						// 读取前30 个字符
						try {
							FileInputStream input = new FileInputStream(f);
							byte[] data = new byte[TEXT_READ_LENGTH];

							try {
								int iReadLength = input.read(data);
								byte[] res;

								if (iReadLength != TEXT_READ_LENGTH) {
									res = new byte[iReadLength];

									for (int i = 0; i < iReadLength; ++i) {
										res[i] = data[i];
									}
								} else {
									res = data;
								}

								m_strText = new String(res);
								m_bCanDrawContext = true;
							} catch (IOException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
						} catch (FileNotFoundException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
				} else if (FileType.IMAGE == m_fileType.getMainType()) {
					m_bitmap = BitmapFactory.decodeFile(strPath);

					if (null != m_bitmap) {
						m_bCanDrawContext = true;
					}
				}
			}
		}

		if (m_bCanDrawContext) {
			invalidate();
		}
	}

	public void clear() {
		m_fileType = null;
		invalidate();
	}

	@Override
	protected void onDraw(Canvas canvas) {

		super.onDraw(canvas);

		if (!m_bCanDrawContext || null == m_fileType) {
			return;
		}

		switch (m_fileType.getMainType()) {
		case FileType.TEXT:
			drawText(canvas);
			break;
		case FileType.IMAGE:
			drawImage(canvas);
			break;
		case FileType.DIR:
			drawDir(canvas);
			break;
		case FileType.OTHER:
			drawOther(canvas);
			break;
		case NULL:
			/*
			 * 不进行任何绘制
			 */
			break;
		}
	}

	private void drawText(Canvas canvas) {
		TextPaint textPaint = new TextPaint();
		textPaint.getColor();
		textPaint.setColor(Color.BLUE);

		StaticLayout layout = new StaticLayout(m_strText, textPaint,
				getWidth() - 40, Alignment.ALIGN_NORMAL, 1.0F, 0.0F, true);
		canvas.translate(20, 20);
		layout.draw(canvas);
	}

	private void drawImage(Canvas canvas) {
		Paint paint = new Paint();
		Rect rect = new Rect();

		rect.left = 0;
		rect.top = 0;
		rect.right = getWidth();
		rect.bottom = getHeight();
		int iBmpWidth = m_bitmap.getWidth();
		int iBmpHeight = m_bitmap.getHeight();

		if (iBmpWidth > rect.width() || iBmpHeight > rect.height()) {
			canvas.drawBitmap(m_bitmap, null, rect, paint);
		} else {
			int iXOffset = (rect.width() - iBmpWidth) / 2;
			int iYOffset = (rect.height() - iBmpHeight) / 2;

			canvas.drawBitmap(m_bitmap, (float) iXOffset, (float) iYOffset,
					paint);
		}
	}

	private void drawDir(Canvas canvas) {

	}

	private void drawOther(Canvas canvas) {

	}
}

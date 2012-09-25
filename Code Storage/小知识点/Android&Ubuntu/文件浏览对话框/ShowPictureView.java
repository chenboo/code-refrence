package com.njtd.shareboard.controls;

import com.njtd.shareboard.R;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.View;

public class ShowPictureView extends View {
	private int mnBmpWidth = 0;
	private int mnBmpHeight = 0;
	private Bitmap mbitmap = null;
	private Paint mpaintbmp = null;
	private Paint mpaintfrm = null;
	private Paint mpaintText = null;
	private int mntextSize = 20;
	private int mclrfrm = Color.rgb(242, 151, 2);
	private int mclrText = Color.rgb(128, 128, 128);
	private Rect mrcText = null;
	private String mstrNophoto = null;

	public ShowPictureView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		// TODO Auto-generated constructor stub
	}

	public ShowPictureView(Context context, AttributeSet attrs) {
		super(context, attrs);
		mpaintbmp = new Paint(Paint.ANTI_ALIAS_FLAG);
		mpaintfrm = new Paint(Paint.ANTI_ALIAS_FLAG);
		mpaintfrm.setStrokeWidth(2);
		mpaintfrm.setColor(mclrfrm);
		mpaintfrm.setStyle(Paint.Style.STROKE);

		mpaintText = new Paint(Paint.ANTI_ALIAS_FLAG);
		mpaintText.setColor(mclrText);
		mpaintText.setFakeBoldText(true);
		mpaintText.setTextSize(mntextSize);

		mrcText = new Rect();
		mstrNophoto = context.getResources().getString(R.string.nophoto2show);
		mpaintText.getTextBounds(mstrNophoto, 0, mstrNophoto.length(), mrcText);
	}

	public ShowPictureView(Context context) {
		super(context);
	}

	public void setBitmap(Bitmap bitmap) {
		mbitmap = bitmap;

		if (null != mbitmap) {
			mnBmpWidth = bitmap.getWidth();
			mnBmpHeight = bitmap.getHeight();
		}
		invalidate();
	}

	@Override
	protected void onDraw(Canvas canvas) {
		Rect rcDst = new Rect(1, 1, getWidth() - 1, getHeight() - 1);

		if (null != mbitmap) {
			Rect rcSrc = new Rect(0, 0, mnBmpWidth, mnBmpHeight);
			// to get the proper rect of the image to draw
			float fWidthRadio = (float) mnBmpWidth / rcDst.width();
			float fHeightRadio = (float) mnBmpHeight / rcDst.height();
			float fRadio = Math.max(fWidthRadio, fHeightRadio);
			int nWidth = (int) (mnBmpWidth / fRadio);
			int nHeight = (int) (mnBmpHeight / fRadio);

			if (fRadio <= 1) {
				nWidth = mnBmpWidth;
				nHeight = mnBmpHeight;
			}

			rcDst.left += (getWidth() - nWidth) / 2;
			rcDst.top += (getHeight() - nHeight) / 2;
			rcDst.right -= (getWidth() - nWidth) / 2;
			rcDst.bottom -= (getHeight() - nHeight) / 2;
			canvas.drawBitmap(mbitmap, rcSrc, rcDst, mpaintbmp);
		} else {
			canvas.drawText(mstrNophoto, mrcText.left, mrcText.bottom,
					mpaintText);
		}

		Rect rect = new Rect(0, 0, getWidth(), getHeight());
		canvas.drawRect(rect, mpaintfrm);
		super.onDraw(canvas);
	}

	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		super.onSizeChanged(w, h, oldw, oldh);

		if (w <= 0 || h <= 0) {
			return;
		} else {
			int ncenterx = w / 2;
			int ncentery = h / 2;
			mrcText.set(ncenterx - mrcText.width() / 2,
					ncentery - mrcText.height() / 2, ncenterx + mrcText.width()
							/ 2, ncentery + mrcText.height() / 2);
		}
	}
}

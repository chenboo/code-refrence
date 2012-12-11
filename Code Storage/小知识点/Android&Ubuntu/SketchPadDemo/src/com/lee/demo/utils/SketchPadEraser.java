package com.lee.demo.utils;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;

import com.lee.demo.interfaces.ISketchPadTool;

public class SketchPadEraser implements ISketchPadTool
{
    private static final float TOUCH_TOLERANCE = 0.0f;

    private float m_curX = 0.0f;
    private float m_curY = 0.0f;
    private boolean m_hasDrawn = false;
    private Path m_eraserPath = new Path();
    private Paint m_eraserPaint = new Paint();

    public SketchPadEraser(int eraserSize)
    {
        m_eraserPaint.setAntiAlias(true);
        m_eraserPaint.setDither(true);
        m_eraserPaint.setColor(0xFF000000);
        m_eraserPaint.setStrokeWidth(eraserSize);
        m_eraserPaint.setStyle(Paint.Style.STROKE);
        m_eraserPaint.setStrokeJoin(Paint.Join.ROUND);
        m_eraserPaint.setStrokeCap(Paint.Cap.SQUARE);
        m_eraserPaint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.DST_OUT));
    }

    @Override
    public void draw(Canvas canvas)
    {
        if (null != canvas)
        {
            canvas.drawPath(m_eraserPath, m_eraserPaint);
        }
    }

    @Override
    public boolean hasDraw()
    {
        return m_hasDrawn;
    }

    @Override
    public void cleanAll()
    {
        m_eraserPath.reset();
    }

    @Override
    public void touchDown(float x, float y)
    {
        m_eraserPath.reset();
        m_eraserPath.moveTo(x, y);
        m_curX = x;
        m_curY = y;
    }

    @Override
    public void touchMove(float x, float y)
    {
        float dx = Math.abs(x - m_curX);
        float dy = Math.abs(y - m_curY);
        
        if (dx >= TOUCH_TOLERANCE || dy >= TOUCH_TOLERANCE)
        {
            m_eraserPath.quadTo(m_curX, m_curY, (x + m_curX) / 2, (y + m_curY) / 2);
            
            m_hasDrawn = true;
            m_curX = x;
            m_curY = y;
        }
    }

    @Override
    public void touchUp(float x, float y)
    {
        m_eraserPath.lineTo(x, y);
    }
}

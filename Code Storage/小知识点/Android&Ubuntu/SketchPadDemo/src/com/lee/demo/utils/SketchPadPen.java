package com.lee.demo.utils;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;

import com.lee.demo.interfaces.ISketchPadTool;

public class SketchPadPen implements ISketchPadTool
{
    private static final float TOUCH_TOLERANCE = 4.0f;

    private float m_curX = 0.0f;
    private float m_curY = 0.0f;
    private boolean m_hasDrawn = false;
    private Path m_penPath = new Path();
    private Paint m_penPaint = new Paint();
    
    public SketchPadPen(int penSize, int penColor)
    {
        m_penPaint.setAntiAlias(true);
        m_penPaint.setDither(true);
        m_penPaint.setColor(penColor);
        m_penPaint.setStrokeWidth(penSize);
        m_penPaint.setStyle(Paint.Style.STROKE);
        m_penPaint.setStrokeJoin(Paint.Join.ROUND);
        m_penPaint.setStrokeCap(Paint.Cap.ROUND);
    }
    
    @Override
    public void draw(Canvas canvas)
    {
        if (null != canvas)
        {
            canvas.drawPath(m_penPath, m_penPaint);
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
        m_penPath.reset();
    }

    @Override
    public void touchDown(float x, float y)
    {
        m_penPath.reset();
        m_penPath.moveTo(x, y);
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
            m_penPath.quadTo(m_curX, m_curY, (x + m_curX) / 2, (y + m_curY) / 2);
            
            m_hasDrawn = true;
            m_curX = x;
            m_curY = y;
        }
    }

    @Override
    public void touchUp(float x, float y)
    {
        m_penPath.lineTo(x, y);
    }
}

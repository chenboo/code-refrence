package com.lee.demo.view;

import com.lee.demo.activity.R;

import android.content.Context;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.RelativeLayout;

public class ColorSelector extends RelativeLayout
{
    private static final int[] m_penColors =
    {
        Color.argb(255, 44, 152, 140),
        Color.argb(255, 48, 115, 170),
        Color.argb(255, 139, 26, 99),
        Color.argb(255, 112, 101, 89),
        Color.argb(255, 40, 36, 37),
        Color.argb(255, 226, 226, 226),
        Color.argb(255, 219, 88, 50),
        Color.argb(255, 129, 184, 69)
    };
 
    private static final int DONE_BUTTON_ID = 0x0010;
    private static final int CANCEL_BUTTON_ID = 0x0020;
    
    private int m_penColor = Color.argb(255, 40, 36, 37);
    private int m_penColorTemp = Color.argb(255, 255, 255, 255);
    private ColorSelectorCallback m_callback = null;
    
    private View.OnClickListener m_clickListener = new View.OnClickListener()
    {
        @Override
        public void onClick(View v)
        {
            switch(v.getId())
            {
            case DONE_BUTTON_ID:
                ColorSelector.this.setVisibility(View.GONE);
                m_penColorTemp = m_penColor;
                if (null != m_callback)
                {
                    m_callback.onColorSelectDone(ColorSelector.this);
                }
                break;
                
            case CANCEL_BUTTON_ID:
                ColorSelector.this.setVisibility(View.GONE);
                m_penColor = m_penColorTemp;
                if (null != m_callback)
                {
                    m_callback.onColorSelectCancel(ColorSelector.this);
                }
                break;
                
            default:
                Object tag = v.getTag();
                if (null != tag && tag instanceof Integer)
                {
                    m_penColor = ((Integer)tag).intValue();
                }
                
                if (null != m_callback)
                {
                    m_callback.onColorSelectChange(ColorSelector.this);
                }
                break;
            }
        }
    };
    
    public ColorSelector(Context context)
    {
        this(context, null);
    }
    
    public ColorSelector(Context context, AttributeSet attrs)
    {
        this(context, attrs, 0);
    }
    
    public ColorSelector(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);
        
        initialize(context);
    }
    
    public void initialize(Context context)
    {
        this.setBackgroundColor(Color.rgb(153, 168, 240));
        
        final int width = 55;
        final int height = 40;
        final int margin = 13;
        final int length = m_penColors.length;
        int left = 10;
        
        for (int i = 0; i < length; ++i)
        {
            int color = m_penColors[i];
            
            ImageButton imgBtn = new ImageButton(context);
            imgBtn.setBackgroundColor(color);
            imgBtn.setOnClickListener(m_clickListener);
            imgBtn.setTag(color);
            
            RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(width, height);
            params.setMargins(left, 10, 0, 0);
            left += (margin + width);
            
            this.addView(imgBtn, params);
        }
        
        // Done button.
        Button btnDone = new Button(context);
        btnDone.setText(this.getResources().getText(R.string.selectcolor_done));
        btnDone.setId(DONE_BUTTON_ID);
        btnDone.setOnClickListener(m_clickListener);
        RelativeLayout.LayoutParams btnDoneparams = new RelativeLayout.LayoutParams(100, 45);
        left += 30;
        btnDoneparams.setMargins(left, 9, 0, 0);
        this.addView(btnDone, btnDoneparams);
        
        // Cancel button.
        Button btnCancel = new Button(context);
        btnCancel.setText(this.getResources().getText(R.string.selectcolor_cancel));
        btnCancel.setId(CANCEL_BUTTON_ID);
        btnCancel.setOnClickListener(m_clickListener);
        RelativeLayout.LayoutParams btnCancelparams = new RelativeLayout.LayoutParams(100, 45);
        left += 110;
        btnCancelparams.setMargins(left, 9, 0, 0);
        this.addView(btnCancel, btnCancelparams);
    }
    
    public boolean onTouchEvent(MotionEvent event)
    {
        return true;
    }
    
    public void setCallback(ColorSelectorCallback callback)
    {
        m_callback = callback;
    }
    
    public int getPenColor()
    {
        return m_penColor;
    }
    
    public interface ColorSelectorCallback
    {
        public void onColorSelectDone(ColorSelector sender);
        public void onColorSelectCancel(ColorSelector sender);
        public void onColorSelectChange(ColorSelector sender);
    }
}

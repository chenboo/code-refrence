package com.lee.demo.interfaces;

import android.view.MotionEvent;

import com.lee.demo.view.SketchPadView;

public interface ISketchPadCallback
{
    public void onTouchDown(SketchPadView obj, MotionEvent event);
    public void onTouchUp(SketchPadView obj, MotionEvent event);
    public void onDestroy(SketchPadView obj);
}

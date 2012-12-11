package com.lee.demo.interfaces;


import android.graphics.Canvas;

public interface ISketchPadTool
{
    public void draw(Canvas canvas);
    public boolean hasDraw();
    public void cleanAll();
    public void touchDown(float x, float y);
    public void touchMove(float x, float y);
    public void touchUp(float x, float y);
}

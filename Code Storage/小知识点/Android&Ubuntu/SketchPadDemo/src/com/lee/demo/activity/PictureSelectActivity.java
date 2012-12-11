package com.lee.demo.activity;

import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.GridView;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

import com.lee.demo.utils.MediaUtil;
import com.lee.demo.utils.MediaUtil.IMediaCallback;
import com.lee.demo.utils.MediaUtil.ImageInfo;
import com.lee.demo.utils.MediaUtil.VideoInfo;

public class PictureSelectActivity extends Activity
{
    private LinearLayout m_progressBarView = null;
    private Button m_btnOk = null;
    private Button m_btnCancel = null;
    private GridView m_gridView = null;
    private MediaUtil m_mediaUtil = new MediaUtil();
    private List<ImageInfo> m_images = null;
    private PictureAdapter m_adapter = null;
    private ImageInfo m_curSelItem = null;

    private View.OnClickListener m_clickListener = new View.OnClickListener()
    {
        @Override
        public void onClick(View v)
        {
            switch(v.getId())
            {
            case R.id.ok_btn:
                onOKClick(v);
                break;
                
            case R.id.back_btn:
                onCancelClick(v);
                break;
            }
        }
    };
    
    private OnItemClickListener m_itemClickListener = new OnItemClickListener()
    {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id)
        {
            Object tag = view.getTag();
            if (null != tag && tag instanceof ImageInfo)
            {
                m_curSelItem = (ImageInfo)tag;
                if (null != m_btnOk)
                {
                    m_btnOk.setEnabled(true);
                }
            }
        }
    };
    
    private IMediaCallback m_mediaCallback = new IMediaCallback()
    {
        @Override
        public void onCheckSDCard(boolean isNoSDCard)
        {
            if (isNoSDCard)
            {
                Toast.makeText(PictureSelectActivity.this.getApplicationContext(),
                        PictureSelectActivity.this.getResources().getText(R.string.selectpicture_nosdcard),
                        Toast.LENGTH_SHORT).show();
            }
        }

        @Override
        public void onCompleteGetImages(List<ImageInfo> images)
        {
            m_images = images;
            
            if (0 == m_images.size())
            {
                TextView nopicsTxtView = (TextView)findViewById(R.id.no_picture);
                nopicsTxtView.setVisibility(View.VISIBLE);
                return;
            }
            
            if (null == m_adapter)
            {
                m_adapter = new PictureAdapter(PictureSelectActivity.this);
            }
            
            m_progressBarView.setVisibility(View.GONE);
            if (null != m_gridView)
            {
                m_gridView.setVisibility(View.VISIBLE);   
                m_gridView.setAdapter(m_adapter);
                m_gridView.setOnItemClickListener(m_itemClickListener);
            }
        }

        @Override
        public void onCompleteGetVideo(List<VideoInfo> videos)
        {
        }
        
    };

    public static Bundle getBundleFromImageInfo(ImageInfo imgInfo)
    {
        if (null == imgInfo)
        {
            return null;
        }
        
        Bundle bundle = new Bundle();
        
        bundle.putString("displayName", imgInfo.displayName);
        bundle.putString("uri", imgInfo.imageUri.toString());
        
        return bundle;
    }
    
    public static ImageInfo getImageInfoFromBundle(Bundle bundle)
    {
        if (null == bundle)
        {
            return null;
        }
        
        ImageInfo imgInfo = new MediaUtil().new ImageInfo();
        imgInfo.displayName = bundle.getString("displayName");
        String strUri = bundle.getString("uri");
        imgInfo.imageUri = Uri.parse(strUri);      
        
        return imgInfo;
    }
    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.pictureselect);
        
        m_progressBarView = (LinearLayout)findViewById(R.id.progress_view);
        m_btnOk = (Button)findViewById(R.id.ok_btn);
        m_btnCancel = (Button)findViewById(R.id.back_btn);
        m_gridView = (GridView)findViewById(R.id.gridView);
        m_gridView.setClickable(true);
        
        m_btnOk.setOnClickListener(m_clickListener);
        m_btnCancel.setOnClickListener(m_clickListener);
        
        m_mediaUtil.setCallback(m_mediaCallback);
        m_mediaUtil.startGetImagesAsync(this);
    }
    
    private void onOKClick(View v)
    {
        if (null != m_curSelItem)
        {
            Intent intent = new Intent();
            Bundle bundle = getBundleFromImageInfo(m_curSelItem);
            intent.putExtras(bundle);
            this.setResult(RESULT_OK, intent);
            this.finish();
        }
    }
    
    private void onCancelClick(View v)
    {
        if (null != this.m_mediaUtil)
        {
            this.m_mediaUtil.cancelGetImageOperation();
        }

        this.finish();
    }
    
    public class PictureAdapter extends BaseAdapter 
    {
        private LayoutInflater m_inflater = null;
        
        public PictureAdapter(Context context)
        {
            m_inflater = LayoutInflater.from(context);
        }
        
        @Override
        public int getCount()
        {
            return (null == m_images) ? 0 : m_images.size();
        }

        @Override
        public Object getItem(int position)
        {
            return position;
        }

        @Override
        public long getItemId(int position)
        {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent)
        {
            if (null  == convertView)
            {
                convertView = m_inflater.inflate(R.layout.pictureitem, null);
            }
            
            ImageInfo imgInfo = m_images.get(position);
            
            ImageButton imgBtn = (ImageButton)convertView.findViewById(R.id.imagebtn);
            imgBtn.setClickable(false);
            imgBtn.setFocusable(false);
            imgBtn.setImageBitmap(imgInfo.imageThumb);
            
            TextView txtView = (TextView)convertView.findViewById(R.id.label_name);
            txtView.setText(imgInfo.displayName);
            
            convertView.setTag(null);
            convertView.setTag(imgInfo);
            
            return convertView;
        }
    }
}

package com.lee.demo.utils;

import java.util.ArrayList;
import java.util.List;

import android.content.ContentResolver;
import android.content.Context;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.provider.MediaStore;

public class MediaUtil
{
    public static final int GET_VIDEOS_MESSAGE = 1001;
    public static final int GET_IMAGES_MESSAGE = 1002;
    
    private int m_thumbnailWidth = 155;
    private int m_thumbnailHeight = 105;
    private IMediaCallback m_callback = null;
    private MediaUtilHandler m_mediaUtilHandler = null;
    @SuppressWarnings("unused")
    private MediaUtilWorker m_videoWorker = null;
    @SuppressWarnings("unused")
    private MediaUtilWorker m_imageWorker = null;
    private boolean m_isGetImageCancelled = false;
    private boolean m_isGetVideoCancelled = false;
    private Context m_context = null;

    public static String converDurationToString(long duration)
    {
        String retVal = "00:00:00";
        
        if (duration <= 0)
        {
            return retVal;
        }
        
        final long totalSeconds = duration / 1000;
        final long hours = totalSeconds / 3600;
        final long minutes = (totalSeconds % 3600) / 60;
        final long seconds = (totalSeconds % 3600) % 60;
        
        retVal = String.format("%02d:%02d:%02d", hours, minutes, seconds);
        
        return retVal;
    }

    public void startGetVideosAsync(Context context)
    {
        cancelGetImageOperation();
        m_context = context;
        
        if (checkSDCard())
        {
            m_isGetVideoCancelled = false;
            m_mediaUtilHandler = new MediaUtilHandler(Looper.getMainLooper());
            m_videoWorker = new MediaUtilWorker(true, "videoworker");
        }
    }

    public void startGetImagesAsync(Context context)
    {
        cancelGetVideoOperation();
        m_context = context;
        
        if (checkSDCard())
        {
            m_isGetImageCancelled = false;
            m_mediaUtilHandler = new MediaUtilHandler(Looper.getMainLooper());
            m_imageWorker = new MediaUtilWorker(false, "imageworker");
        }
    }

    public void cancelGetVideoOperation()
    {
        m_isGetVideoCancelled = true;
        m_videoWorker = null;
    }

    public void cancelGetImageOperation()
    {
        m_isGetImageCancelled = true;
        m_imageWorker = null;
    }

    public void setThumbSize(int w, int h)
    {
        m_thumbnailWidth = w;
        m_thumbnailHeight = h;
    }

    public List<VideoInfo> getVideosFromSDCard(Context context)
    {
        List<VideoInfo> videoInfos = new ArrayList<VideoInfo>();
        
        if (checkSDCard() && null != context)
        {
            ContentResolver cr = context.getContentResolver();
            
            String[] proj = 
            {
                MediaStore.Video.Media._ID,
                MediaStore.Video.Media.DISPLAY_NAME,
                MediaStore.Video.Media.ALBUM,
                MediaStore.Video.Media.ARTIST,
                MediaStore.Video.Media.CATEGORY,
                MediaStore.Video.Media.DESCRIPTION,
                MediaStore.Video.Media.RESOLUTION,
                MediaStore.Video.Media.DURATION,
            };
            
            Uri baseUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;

            Cursor c = cr.query(baseUri, proj, null, null, null);
            
            if (null != c)
            {
                BitmapFactory.Options options = new BitmapFactory.Options();
                options.inSampleSize = 1;
                
                for (c.moveToFirst(); !c.isAfterLast(); c.moveToNext())
                {
                    VideoInfo videoInfo = new VideoInfo();
                    videoInfo.id = c.getLong(0);
                    videoInfo.displayName = c.getString(1);
                    videoInfo.album = c.getString(2);
                    videoInfo.artist = c.getString(3);
                    videoInfo.category = c.getString(4);
                    videoInfo.description = c.getString(5);
                    videoInfo.resolution = c.getString(6);
                    videoInfo.duration = converDurationToString(c.getLong(7));
                    videoInfo.videoUri = Uri.withAppendedPath(baseUri, String.valueOf(videoInfo.id));
                    videoInfo.videoThumb = MediaStore.Video.Thumbnails.getThumbnail(cr, videoInfo.id,
                                           MediaStore.Video.Thumbnails.MINI_KIND, options);
                    
                    videoInfo.formatValues();
                    
                    videoInfos.add(videoInfo);
                }
                
                c.close();
            }
        }
        
        return videoInfos;
    }

    public List<ImageInfo> getImagesFromSDCard(Context context)
    {
        List<ImageInfo> imageInfos = new ArrayList<ImageInfo>();
        
        if (checkSDCard() && null != context)
        {
            ContentResolver cr = context.getContentResolver();
            String[] proj = 
            {
                MediaStore.Images.Media._ID,
                MediaStore.Images.Media.DISPLAY_NAME,
                MediaStore.Images.Media.ORIENTATION,
            };
            
            Uri baseUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
            Cursor c = cr.query(baseUri, proj, null, null, null);
           
            if (null != c)
            {
                BitmapFactory.Options options = new BitmapFactory.Options();
                options.inSampleSize = 1;
                
                for (c.moveToFirst(); !c.isAfterLast(); c.moveToNext())
                {
                    ImageInfo imageInfo = new ImageInfo();
                    imageInfo.id = c.getLong(0);
                    imageInfo.displayName = c.getString(1);
                    imageInfo.orientation = c.getInt(2);
                    imageInfo.imageUri = Uri.withAppendedPath(baseUri, String.valueOf(imageInfo.id));
                    imageInfo.imageThumb = MediaStore.Images.Thumbnails.getThumbnail(cr, imageInfo.id,
                                           MediaStore.Images.Thumbnails.MINI_KIND, options);

                    imageInfo.formatValues();
                    imageInfos.add(imageInfo);
                }
                
                c.close();
            }
        }
        
        return imageInfos;
    }

    public void setCallback(IMediaCallback callback)
    {
        m_callback = callback;
    }
    
    public boolean checkSDCard()
    {
        boolean retVal = true;
        
        retVal = android.os.Environment.getExternalStorageState().equals(
                 android.os.Environment.MEDIA_MOUNTED);
        
        if (null != m_callback && !retVal)
        {
            m_callback.onCheckSDCard(true);
        }
        
        return retVal;
    }

    protected class MediaUtilHandler extends Handler
    {
        public MediaUtilHandler(Looper looper)
        {
        }

        @SuppressWarnings("unchecked")
        @Override
        public void handleMessage(Message msg)
        {
            final MediaUtil mediaUtil = MediaUtil.this;
            
            if (MediaUtil.GET_VIDEOS_MESSAGE == msg.what)
            {
                if (null != mediaUtil.m_callback && !mediaUtil.m_isGetVideoCancelled)
                {
                    List<VideoInfo> videos = (List<VideoInfo>)msg.obj;
                    mediaUtil.m_callback.onCompleteGetVideo(videos);
                }
                
                mediaUtil.cancelGetVideoOperation();
            }
            else if (MediaUtil.GET_IMAGES_MESSAGE == msg.what)
            {
                if (null != mediaUtil.m_callback && !mediaUtil.m_isGetImageCancelled)
                {
                    List<ImageInfo> images = (List<ImageInfo>)msg.obj;
                    mediaUtil.m_callback.onCompleteGetImages(images);
                }
                
                mediaUtil.cancelGetImageOperation();
            }
        }
    }

    protected class MediaUtilWorker implements Runnable
    {
        private boolean m_isGetVideo = false;

        public MediaUtilWorker(boolean isGetVideo, String threadName)
        {
            m_isGetVideo = isGetVideo;
            
            Thread thread = new Thread(null, this, threadName);
            thread.setPriority(Thread.MIN_PRIORITY);
            thread.start();
        }

        public void run()
        {
            final MediaUtilHandler handler = MediaUtil.this.m_mediaUtilHandler;

            if (null != handler)
            {
                if (m_isGetVideo)
                {
                    List<VideoInfo> videos = MediaUtil.this.getVideosFromSDCard(MediaUtil.this.m_context);
              
                    if (!MediaUtil.this.m_isGetVideoCancelled)
                    {
                        Message msg = new Message();
                        msg.what = MediaUtil.GET_VIDEOS_MESSAGE;
                        msg.obj = videos;
                        
                        // Notify the handler the message.
                        handler.sendMessage(msg);
                    }
                }
                else
                {
                    List<ImageInfo> images = MediaUtil.this.getImagesFromSDCard(MediaUtil.this.m_context);
                   
                    if (!MediaUtil.this.m_isGetImageCancelled)
                    {
                        Message msg = new Message();
                        msg.what = MediaUtil.GET_IMAGES_MESSAGE;
                        msg.obj = images;
                        
                        // Notify the handler the message.
                        handler.sendMessage(msg);
                    }
                }
            }
        }
    }

    public interface IMediaCallback
    {
        public void onCompleteGetVideo(List<VideoInfo> videos);
        public void onCompleteGetImages(List<ImageInfo> images);
        public void onCheckSDCard(boolean isNoSDCard);
    }

    public class VideoInfo
    {
        public long id = 0;
        public Bitmap videoThumb = null;
        public Uri videoUri = null;
        public String displayName = "";
        public String duration ="00:00:00";
        public String artist = "";
        public String album = "";
        public String resolution = "";
        public String description = "";
        public String category = "";
        
        public void formatValues()
        {
            displayName = (null == displayName) ? "" : displayName;
            artist = (null == artist) ? "" : artist;
            album = (null == album) ? "" : album;
            resolution = (null == resolution) ? "" : resolution;
            description = (null == description) ? "" : description;
            category = (null == category) ? "" : category;
            
            final Bitmap bmp = BitmapUtil.getSizedBitmap(videoThumb, m_thumbnailWidth, m_thumbnailHeight);
            videoThumb = (null != bmp) ? bmp : videoThumb;
        }
    }

    public class ImageInfo
    {
        public long id = 0;
        public Bitmap imageThumb = null;
        public Uri imageUri = null;
        public String displayName = "";
        public int orientation = 0;

        public void formatValues()
        {
            displayName = (null == displayName) ? "" : displayName;
            
            final Bitmap bmp = BitmapUtil.getSizedBitmap(imageThumb, m_thumbnailWidth, m_thumbnailHeight);
            imageThumb = (null != bmp) ? bmp : imageThumb;
        }
    }
}

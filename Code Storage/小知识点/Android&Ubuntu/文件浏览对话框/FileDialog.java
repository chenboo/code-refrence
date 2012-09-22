package com.njtd.shareboard.controls;

import java.io.File;
import java.sql.Date;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

import android.app.Dialog;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import com.njtd.shareboard.R;

public abstract class FileDialog extends Dialog {
	protected Context mcontext;
	protected Resources mResource = null;

	protected String mstrCurrentPath = null;
	protected Stack<String> mStackBackPath = new Stack<String>();
	protected Stack<String> mnextPathStack = new Stack<String>();

	protected HashMap<String, String> mFileTypeMap;
	protected List<String> mfilePaths = new ArrayList<String>();
	protected List<Map<String, Object>> mfileData = new ArrayList<Map<String, Object>>();

	protected boolean mbOpenFile = true;
	protected TextView mtextCurrentPath = null;
	protected int mclrSelcted = Color.rgb(242, 200, 54);

	protected ViewGroup mView;
	protected ListView mlistFile = null;
	protected LayoutInflater mFlater;

	protected final int FILE_UNKNOW = 0;
	protected final int FILE_FOLDER = 1;
	protected final int FILE_TEXT = 2;
	protected final int FILE_IMAGE = 3;
	protected final int FILE_AUDIO = 4;
	protected final int FILE_VIDEO = 5;
	protected final int FILE_COMPRESS = 6;
	protected final int FILE_PDF = 7;
	protected final int FILE_APP = 8;

	protected Bitmap mIconFolder = null;
	protected Bitmap mIconUnkownFile = null;
	protected Bitmap mIconTxt = null;
	protected Bitmap mIconPdf = null;
	protected Bitmap mIconDoc = null;
	protected Bitmap mIconXml = null;
	protected Bitmap mIconImg = null;
	protected Bitmap mIconAudio = null;
	protected Bitmap mIconVideo = null;
	protected Bitmap mIconCompress = null;
	protected Bitmap mIconApp = null;

	public FileDialog(Context context, boolean cancelable,
			OnCancelListener cancelListener) {
		super(context, cancelable, cancelListener);
		// TODO Auto-generated constructor stub
	}

	public FileDialog(Context context, int theme) {
		super(context, theme);
		// TODO Auto-generated constructor stub
	}

	public FileDialog(Context context) {
		super(context);

		mcontext = context;
		mResource = context.getResources();
		requestWindowFeature(Window.FEATURE_NO_TITLE);

		setMainview();
		setupUiHandles();
		setupUiEvents();
		setupFileTypes();
	}

	protected abstract void setMainview();

	protected abstract void setupUiHandles();

	protected abstract void setupUiEvents();

	protected void setupFileTypes() {
		mFileTypeMap = new HashMap<String, String>();
		mFileTypeMap.put(".txt", Integer.toString(FILE_TEXT));
		mFileTypeMap.put(".doc", Integer.toString(FILE_TEXT));
		mFileTypeMap.put(".xml", Integer.toString(FILE_TEXT));
		mFileTypeMap.put(".text", Integer.toString(FILE_TEXT));
		mFileTypeMap.put(".pdf", Integer.toString(FILE_PDF));

		mFileTypeMap.put(".bmp", Integer.toString(FILE_IMAGE));
		mFileTypeMap.put(".jpg", Integer.toString(FILE_IMAGE));
		mFileTypeMap.put(".jpeg", Integer.toString(FILE_IMAGE));
		mFileTypeMap.put(".png", Integer.toString(FILE_IMAGE));
		mFileTypeMap.put(".ico", Integer.toString(FILE_IMAGE));
		mFileTypeMap.put(".gif", Integer.toString(FILE_IMAGE));

		mFileTypeMap.put(".mp3", Integer.toString(FILE_AUDIO));
		mFileTypeMap.put(".wav", Integer.toString(FILE_AUDIO));
		mFileTypeMap.put(".ogg", Integer.toString(FILE_AUDIO));

		mFileTypeMap.put(".mp4", Integer.toString(FILE_VIDEO));
		mFileTypeMap.put(".avi", Integer.toString(FILE_VIDEO));
		mFileTypeMap.put(".rmvb", Integer.toString(FILE_VIDEO));

		mFileTypeMap.put(".zip", Integer.toString(FILE_COMPRESS));
		mFileTypeMap.put(".rar", Integer.toString(FILE_COMPRESS));
		mFileTypeMap.put(".exe", Integer.toString(FILE_APP));
		mFileTypeMap.put(".apk", Integer.toString(FILE_APP));

		mIconFolder = BitmapFactory
				.decodeResource(mResource, R.drawable.folder);
		mIconUnkownFile = BitmapFactory.decodeResource(mResource,
				R.drawable.unknown);
		mIconTxt = BitmapFactory.decodeResource(mResource, R.drawable.txt);
		mIconPdf = BitmapFactory.decodeResource(mResource, R.drawable.pdf);
		mIconDoc = BitmapFactory.decodeResource(mResource, R.drawable.doc);

		mIconXml = BitmapFactory.decodeResource(mResource, R.drawable.xml);
		mIconImg = BitmapFactory.decodeResource(mResource, R.drawable.bmp);
		mIconAudio = BitmapFactory.decodeResource(mResource, R.drawable.mp3);
		mIconVideo = BitmapFactory.decodeResource(mResource, R.drawable.mp4);
		mIconCompress = BitmapFactory.decodeResource(mResource, R.drawable.rar);
		mIconApp = BitmapFactory.decodeResource(mResource, R.drawable.app);
	}
	
	public String getCurrentPath(){
		return mstrCurrentPath;
	}

	protected void setDirectory(String strPath) {
		mfileData.clear();
		mfilePaths.clear();

		if (null == strPath) {
			strPath = android.os.Environment.getExternalStorageDirectory()
					.getPath();
		}

		mstrCurrentPath = strPath;
		File f = new File(strPath);
		File[] files = f.listFiles();

		for (int i = 0; i < files.length; ++i) {
			int nFileType = getFileType(files[i].getPath());

			Map<String, Object> map = new HashMap<String, Object>();
			map.put("icon", getFileIcon(nFileType));
			map.put("name", files[i].getName());
			Date date = new Date(files[i].lastModified());
			map.put("time", date.toLocaleString());

			if (files[i].isDirectory()) {
				map.put("size", mcontext.getString(R.string.file_dialog_folder));
			} else {
				map.put("size", convertSizeToString(files[i].length()));
			}

			mfileData.add(map);
			mfilePaths.add(files[i].getPath());
		}

		FileItemAdapter adapter = new FileItemAdapter(mcontext, mfileData);
		mlistFile.setAdapter(adapter);
	}

	protected String convertSizeToString(long lSize) {
		String strSize = new String();

		DecimalFormat decimalformat = new DecimalFormat("0.0");

		if (lSize < 1024) {
			strSize = "" + decimalformat.format((double) lSize) + "B";
		} else if (lSize < 1024 * 1024) {
			strSize = "" + decimalformat.format((double) lSize / 1024) + "KB";
		} else if (lSize < 1024 * 1024 * 1024) {
			strSize = "" + decimalformat.format((double) lSize / (1024 * 1024))
					+ "MB";
		} else {
			strSize = ""
					+ decimalformat.format((double) lSize
							/ (1024 * 1024 * 1024)) + "GB";
		}

		return strSize;
	}

	/**
	 * This is the file show list item define class.
	 * 
	 * @author mmmn143
	 * 
	 */
	class FileItemAdapter extends BaseAdapter {
		private int miSelectItemPos = -1;
		private LayoutInflater mInflater;
		private List<Map<String, Object>> mdata;

		public FileItemAdapter(Context context, List<Map<String, Object>> data) {
			this.mInflater = LayoutInflater.from(context);
			mdata = data;
		}

		public int getCount() {
			return mdata.size();
		}

		public Object getItem(int position) {
			return null;
		}

		public long getItemId(int position) {
			return 0;
		}

		public View getView(int position, View convertView, ViewGroup parent) {
			FileViewHolder holder = null;
			if (convertView == null) {

				holder = new FileViewHolder();
				convertView = mInflater.inflate(R.layout.file_list_item, null);
				holder.icon = (ImageView) convertView
						.findViewById(R.id.image_file_list_icon);
				holder.name = (TextView) convertView
						.findViewById(R.id.text_file_list_name);
				holder.size = (TextView) convertView
						.findViewById(R.id.text_file_list_size);
				convertView.setTag(holder);

			} else {
				holder = (FileViewHolder) convertView.getTag();
			}

			holder.icon
					.setImageBitmap((Bitmap) mdata.get(position).get("icon"));
			holder.name.setText((String) mdata.get(position).get("name"));
			holder.size.setText((String) mdata.get(position).get("size"));

			if (position == miSelectItemPos) {
				convertView.setBackgroundColor(mclrSelcted);
			} else {
				convertView.setBackgroundColor(Color.TRANSPARENT);
			}

			return convertView;
		}

		public int getSelectItemPos() {
			return miSelectItemPos;
		}

		public void setSelectItempos(int iItem) {
			miSelectItemPos = iItem;
		}
	}

	private Bitmap getFileIcon(int nFileType) {
		Bitmap bmp = null;

		switch (nFileType) {
		case FILE_FOLDER:
			bmp = mIconFolder;
			break;
		case FILE_TEXT:
			bmp = mIconTxt;
			break;
		case FILE_IMAGE:
			bmp = mIconImg;
			break;
		case FILE_AUDIO:
			bmp = mIconAudio;
			break;
		case FILE_VIDEO:
			bmp = mIconVideo;
			break;
		case FILE_COMPRESS:
			bmp = mIconCompress;
			break;
		case FILE_UNKNOW:
			bmp = mIconUnkownFile;
			break;
		case FILE_PDF:
			bmp = mIconPdf;
			break;
		case FILE_APP:
			bmp = mIconApp;
			break;
		default:
			bmp = mIconUnkownFile;
			break;
		}

		return bmp;
	}

	protected static String getFileExtentName(String strFileName) {
		String strRes = null;

		int iIndex = strFileName.lastIndexOf('.');

		if (-1 == iIndex) {
			strRes = null;
		} else {
			strRes = strFileName.substring(iIndex);
		}

		return strRes;
	}

	protected int getFileType(String strFileName) {
		int nFileType = FILE_UNKNOW;
		File file = new File(strFileName);

		if (file.isDirectory()) {
			nFileType = FILE_FOLDER;

		} else {
			String strExtent = getFileExtentName(strFileName);
			for (String m : mFileTypeMap.keySet()) {

				if (m.equalsIgnoreCase(strExtent)) {
					nFileType = Integer.parseInt(mFileTypeMap.get(m));
					break;
				}
			}
		}

		return nFileType;
	}

	/**
	 * used to contain the file view
	 * 
	 */
	class FileViewHolder {
		public ImageView icon; // File icon
		public TextView name; // File name
		public TextView size; // File size, the folder is't have this
	}
}

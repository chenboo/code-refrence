package com.njtd.shareboard.controls;

import java.io.File;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.Toast;

import com.njtd.shareboard.R;
import com.njtd.shareboard.uicontrols.ImageTextButton;

public class DlgFileBrowser extends FileDialog implements OnClickListener {

	protected ImageTextButton mbtnBack;
	protected ImageTextButton mbtnDone;
	protected ImageTextButton mbtnCancel;
	protected ShowPictureView mpictureView;
	protected DlgfileBrowserCallback mcallback;

	public DlgFileBrowser(Context context, boolean cancelable,
			OnCancelListener cancelListener) {
		super(context, cancelable, cancelListener);
	}

	public DlgFileBrowser(Context context, int theme) {
		super(context, theme);
	}

	public DlgFileBrowser(Context context) {
		super(context);
		setDirectory(null);
		mbtnBack.setEnabled(false);
		mbtnDone.setEnabled(false);
	}

	public void init(DlgfileBrowserCallback callback) {
		mcallback = callback;
	}

	@Override
	protected void setMainview() {
		setContentView(R.layout.file_dialog);
	}

	@Override
	public void setupUiHandles() {
		mlistFile = (ListView) findViewById(R.id.list_file_info);
		mpictureView = (ShowPictureView) findViewById(R.id.pic_preview);
		mbtnDone = (ImageTextButton) findViewById(R.id.filedlg_btnDone);
		mbtnBack = (ImageTextButton) findViewById(R.id.filedlg_btnBack);
		mbtnCancel = (ImageTextButton) findViewById(R.id.filedlg_btnCancel);
	}

	@Override
	public void setupUiEvents() {
		mlistFile.setOnItemClickListener(new OnFileItemClick());
		mbtnCancel.setOnClickListener(this);
		mbtnDone.setOnClickListener(this);
		mbtnBack.setOnClickListener(this);
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.filedlg_btnDone:
			int iSelectItemPos = ((FileItemAdapter) mlistFile.getAdapter())
					.getSelectItemPos();

			if (iSelectItemPos != -1) {

				String strFileName = mfilePaths.get(iSelectItemPos);
				File f = new File(strFileName);
				int nFileType = getFileType(strFileName);

				if (f.isDirectory()) {
					Toast.makeText(mcontext, "You have not select a file",
							Toast.LENGTH_SHORT).show();

				} else if (FILE_IMAGE != nFileType) {
					Toast.makeText(mcontext, "please select a picture",
							Toast.LENGTH_SHORT).show();
				} else {

					mcallback.onfileSelected(mfilePaths.get(iSelectItemPos));
					dismiss();
				}
			}
			break;
		case R.id.filedlg_btnCancel:
			dismiss();
			break;
		case R.id.filedlg_btnBack:
			if (!mStackBackPath.isEmpty()) {
				String strPath = mStackBackPath.pop();
				setDirectory(strPath);
				((FileItemAdapter) mlistFile.getAdapter()).setSelectItempos(-1);

				if (mStackBackPath.isEmpty()) {
					mbtnBack.setEnabled(false);
				}

				if (mpictureView != null) {
					mpictureView.setBitmap(null);
				}

				mbtnDone.setEnabled(false);
			}
			break;
		default:
			break;
		}
	}

	private class OnFileItemClick implements OnItemClickListener {

		public void onItemClick(AdapterView<?> parent, View view, int position,
				long id) {

			((FileItemAdapter) mlistFile.getAdapter())
					.setSelectItempos(position);

			String strPath = mfilePaths.get(position);
			File f = new File(strPath);

			if (f.isDirectory()) {
				if (f.canRead()) {
					mStackBackPath.push(mstrCurrentPath);
					setDirectory(strPath);
					mbtnBack.setEnabled(true);
					mbtnDone.setEnabled(false);
				}

				mpictureView.setBitmap(null);

			} else {
				((FileItemAdapter) mlistFile.getAdapter())
						.setSelectItempos(position);
				((FileItemAdapter) mlistFile.getAdapter())
						.notifyDataSetInvalidated();
				mbtnDone.setEnabled(true);

				if (mpictureView != null) {

					if (FILE_IMAGE == getFileType(strPath)) {
						// just get the bmp's bounds
						BitmapFactory.Options options = new BitmapFactory.Options();
						options.inJustDecodeBounds = true;
						BitmapFactory.decodeFile(strPath, options);

						// reset the inJustDecodeBounds to false
						options.inJustDecodeBounds = false;

						// calculate the parameters
						int nWidth = mpictureView.getWidth();
						int nHeight = mpictureView.getHeight();

						float fxscale = options.outWidth / nWidth;
						float fyscale = options.outHeight / nHeight;
						float fMaxScale = Math.max(fxscale, fyscale);

						if (fMaxScale <= 1) {
							fMaxScale = 1;
						}

						options.inSampleSize = (int) fMaxScale;
						mpictureView.setBitmap(BitmapFactory.decodeFile(
								strPath, options));
					} else {
						mpictureView.setBitmap(null);
					}
				}
			}
		}
	}

	// interface for callback
	public interface DlgfileBrowserCallback {
		public void onfileSelected(String strFilePath);
	}
}

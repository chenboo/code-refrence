package com.njtd.shareboard.controls;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.Toast;

import com.njtd.shareboard.R;
import com.njtd.shareboard.uicontrols.ImageTextButton;

public class DlgSaveFile extends FileDialog implements
		android.view.View.OnClickListener {
	protected EditText mEdittext;
	private Spinner mSpinnerFileType;
	protected ImageTextButton mbtnBack;
	protected ImageTextButton mbtnDone;
	protected ImageTextButton mbtnCancel;
	protected DlgSaveFileCallback mcallback;
	private String mstrSelectNoFile = null;
	private String mstrPlsSelectPic = null;
	private String mstrPlsInputName = null;

	private ArrayAdapter<String> mAdapterFileTypes;
	private List<String> mFiletypes;

	private static final String[] mstrFileTypes = { ".png", ".bmp", ".jpeg" };

	public DlgSaveFile(Context context, boolean cancelable,
			OnCancelListener cancelListener) {
		super(context, cancelable, cancelListener);
	}

	public DlgSaveFile(Context context, int theme) {
		super(context, theme);
	}

	public DlgSaveFile(Context context) {
		super(context);
		setDirectory(null);
		mbtnBack.setEnabled(false);

		// init the fontsize spinner data
		mFiletypes = new ArrayList<String>();
		for (int i = 0; i < mstrFileTypes.length; i++) {
			mFiletypes.add(mstrFileTypes[i]);
		}

		mAdapterFileTypes = new ArrayAdapter<String>(mcontext,
				android.R.layout.simple_spinner_item, mFiletypes);
		mAdapterFileTypes
				.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

		mSpinnerFileType.setAdapter(mAdapterFileTypes);
		mSpinnerFileType.setSelection(0);
		mSpinnerFileType.setOnItemSelectedListener(new FileTypeSelected());
	}

	public void setDefaultDirectory(String strDirectory) {
		setDirectory(strDirectory);
		initBackStackPath(strDirectory);
	}

	public void initBackStackPath(String strDirectory) {
		File sdcarddir = android.os.Environment.getExternalStorageDirectory();
		String strSDCard = sdcarddir.getPath();

		if (strDirectory.equalsIgnoreCase(strSDCard)) {
			return;
		}

		mStackBackPath.removeAll(mStackBackPath);
		char ch = ' ';
		String strParse = null;
		int nlength = strDirectory.length();

		for (int i = strSDCard.length(); i < nlength; i++) {
			ch = strDirectory.charAt(i);

			if (ch == '/') {
				if (i != 0) {
					strParse = strDirectory.substring(0, i);
					mStackBackPath.push(strParse);
					Log.d("psm", "strParse = " + strParse);
				}
			} else if (i == nlength - 1) {
				strParse = strDirectory.substring(0, i + 1);
				mStackBackPath.push(strParse);
				Log.d("psm", "strParse = " + strParse);
			}
		}

		mStackBackPath.pop();
		if (mStackBackPath.isEmpty()) {
			mbtnBack.setEnabled(false);
		} else {
			mbtnBack.setEnabled(true);
		}
	}

	public void init(DlgSaveFileCallback callback) {
		mcallback = callback;
		mstrSelectNoFile = mcontext.getResources().getString(
				R.string.selectnofile);
		mstrPlsSelectPic = mcontext.getResources().getString(
				R.string.plsselectpic);
		mstrPlsInputName = mcontext.getResources().getString(
				R.string.plsinputfilename);
	}

	@Override
	protected void setMainview() {
		setContentView(R.layout.dlg_save);
	}

	@Override
	public void setupUiHandles() {
		mlistFile = (ListView) findViewById(R.id.dlgsave_list);
		mEdittext = (EditText) findViewById(R.id.edit_file_name);
		mbtnDone = (ImageTextButton) findViewById(R.id.dlgsave_btnDone);
		mbtnBack = (ImageTextButton) findViewById(R.id.dlgsave_btnBack);
		mbtnCancel = (ImageTextButton) findViewById(R.id.dlgsave_btnCancel);
		mSpinnerFileType = (Spinner) findViewById(R.id.dlgsave_filetypes);
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
		case R.id.dlgsave_btnDone:
			String strText = mEdittext.getText().toString();
			if (strText.isEmpty()) {
				Toast.makeText(mcontext, mstrPlsInputName, Toast.LENGTH_SHORT)
						.show();
			} else {
				int iSelectItemPos = ((FileItemAdapter) mlistFile.getAdapter())
						.getSelectItemPos();

				if (iSelectItemPos != -1) {
					String strFileName = mfilePaths.get(iSelectItemPos);
					File f = new File(strFileName);
					int nFileType = getFileType(strFileName);

					if (f.isDirectory()) {
						Toast.makeText(mcontext, mstrSelectNoFile,
								Toast.LENGTH_SHORT).show();

					} else if (FILE_IMAGE != nFileType) {
						Toast.makeText(mcontext, mstrPlsSelectPic,
								Toast.LENGTH_SHORT).show();
					} else {
						mcallback.onfileSaved(mfilePaths.get(iSelectItemPos));
						dismiss();
					}
				} else {
					
					String strfile;
					if(strText.contains(".")){
						strfile = mstrCurrentPath + "/" + strText;
					}
					else{
						String strType = mAdapterFileTypes.getItem(mSpinnerFileType.getSelectedItemPosition());
						strfile = mstrCurrentPath + "/" + strText + strType;
					}

					mcallback.onfileSaved(strfile);
					dismiss();
				}
			}
			break;
		case R.id.dlgsave_btnCancel:
			dismiss();
			break;
		case R.id.dlgsave_btnBack:
			if (!mStackBackPath.isEmpty()) {
				String strPath = mStackBackPath.pop();
				setDirectory(strPath);
				((FileItemAdapter) mlistFile.getAdapter()).setSelectItempos(-1);

				if (mStackBackPath.isEmpty()) {
					mbtnBack.setEnabled(false);
				}
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
				}
			} else {
				((FileItemAdapter) mlistFile.getAdapter())
						.setSelectItempos(position);
				((FileItemAdapter) mlistFile.getAdapter())
						.notifyDataSetInvalidated();
			}
		}
	}

	public interface DlgSaveFileCallback {
		public void onfileSaved(String strFilePath);
	}

	private class FileTypeSelected implements OnItemSelectedListener {
		public void onItemSelected(AdapterView<?> parent, View view,
				int position, long id) {
		}

		public void onNothingSelected(AdapterView<?> parent) {
			// TODO Auto-generated method stub
		}
	}
}

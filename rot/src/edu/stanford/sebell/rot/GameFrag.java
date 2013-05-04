package edu.stanford.sebell.rot;

import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

public class GameFrag extends Fragment {
	
    GL2JNIView mView;
    
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState)
	{
		mView = new GL2JNIView(getActivity());
		return mView;
	}
	
}

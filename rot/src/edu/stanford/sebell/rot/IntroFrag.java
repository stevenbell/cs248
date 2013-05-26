package edu.stanford.sebell.rot;

import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

public class IntroFrag extends Fragment {
	
	Button startButton;
	GameSwitcher parent;
	
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState)
	{
		View v = inflater.inflate(R.layout.intro_screen, container);
		
		startButton = (Button) v.findViewById(R.id.button1);
		startButton.setText("Start!");
		startButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				startButton.setText("Again!");
				parent.startGame();
			}
		});
		
		parent = (GameSwitcher) getActivity();
		
		return v;
	}

}

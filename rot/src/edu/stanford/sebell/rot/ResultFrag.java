package edu.stanford.sebell.rot;

import android.app.Fragment;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

public class ResultFrag extends Fragment {
	
	Button againButton;
	TextView resultLabel;
	GameSwitcher parent;
	int result;
	
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState)
	{
		parent = (GameSwitcher) getActivity();
		
		View v = inflater.inflate(R.layout.result_display, container);
		
		resultLabel = (TextView) v.findViewById(R.id.resultLabel);
		
		againButton = (Button) v.findViewById(R.id.againButton);
		againButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				parent.newGame();
			}
		});
		
		return v;
	}
	
	public void setResult(int r)
	{
	    result = r;
	    getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(result == 1){
                    resultLabel.setText("Level complete!");
                    resultLabel.setTextColor(Color.GREEN);
                }
                else if(result == 2){
                    resultLabel.setText("You died.");
                    resultLabel.setTextColor(Color.RED);
                }               
            }
        });
	}

}

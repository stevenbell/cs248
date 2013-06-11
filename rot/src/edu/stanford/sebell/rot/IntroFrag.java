package edu.stanford.sebell.rot;

import java.util.ArrayList;

import android.app.Fragment;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.AdapterView.OnItemClickListener;

public class IntroFrag extends Fragment {
	
	Button startButton;
	ListView levelList;
	ArrayAdapter<String> listAdapter;
	GameSwitcher parent;
	
	int selectedLevel = 0;
	
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState)
	{
		parent = (GameSwitcher) getActivity();
		
		View v = inflater.inflate(R.layout.intro_screen, container);
		
		startButton = (Button) v.findViewById(R.id.playButton);
		startButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
			    // Level numbering starts at "01"
				parent.startGame(String.format("level%02d", selectedLevel + 1));
			}
		});
		
		// TODO: There's a lot more to be done to make this good.
		// See: http://javatechig.com/android/android-list-view-tutorial
		levelList = (ListView) v.findViewById(R.id.levelList);
		
		ArrayList<String> names = new ArrayList<String>();
		names.add("Level 1: Learn to walk");
		names.add("Level 2: Learn to twist");
		names.add("Level 3: A small conundrum");
		names.add("Level 4: Space");
		names.add("Level 5: Boxes");
		names.add("Level 6: Boxes hurt!");
		names.add("Level 7: Thinking with boxes");
		listAdapter = new ArrayAdapter<String>(getActivity(), R.layout.level_list_element, names);
		levelList.setAdapter(listAdapter);
		levelList.setOnItemClickListener(new OnItemClickListener() {
		    @Override
		    public void onItemClick(AdapterView<?> a, View v, int position, long id){
			    selectedLevel = position;
		    };
        });
		
		return v;
	}

}

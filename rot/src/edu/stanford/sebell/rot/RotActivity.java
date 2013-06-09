package edu.stanford.sebell.rot;

/* Entry-point activity for the Rot game
 * Steven Bell <sebell@stanford.edu>
 * 1 May 2013
 */

import android.app.Activity;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.os.Bundle;
import android.util.Log;

public class RotActivity extends Activity implements GameSwitcher {

    IntroFrag introFrag;
    GameFrag gameFrag;
    AssetLoader loader;

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        setContentView(R.layout.main);
        
        JniBridge.setAssetManager(getAssets());
        loader = new AssetLoader(getAssets());
        JniBridge.setPngLoader(loader);

        // Create/extract fragments for each of the game screens
        FragmentManager fm = getFragmentManager();
        introFrag = (IntroFrag) fm.findFragmentById(R.id.introFragment);
        gameFrag = (GameFrag) fm.findFragmentById(R.id.gameFragment);

        // Show the intro screen
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        ft.show(introFrag);
        ft.hide(gameFrag);
        ft.commit();
    }

    @Override
    public void startGame(String levelId) {
        Log.i("RotActivity", "Starting level " + levelId);
        
        // Switch to the "playing" fragment
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        ft.hide(introFrag);
        ft.show(gameFrag);
        ft.commit();
    }
    
    @Override
    public void endGame() {
        // Switch back to the level selection fragment
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        ft.show(introFrag);
        ft.hide(gameFrag);
        ft.commit();
    }

    @Override
    protected void onPause() {
        super.onPause();
        // TODO: pause/resume game
    }

    @Override
    protected void onResume() {
        super.onResume();
    }
}

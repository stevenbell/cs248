package edu.stanford.sebell.rot;

/* Entry-point activity for the Rot game
 * Steven Bell <sebell@stanford.edu>
 * 1 May 2013
 */

import android.app.Activity;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.app.backup.RestoreObserver;
import android.os.Bundle;
import android.util.Log;

public class RotActivity extends Activity implements GameSwitcher {

    IntroFrag introFrag;
    GameFrag gameFrag;
    ResultFrag resultFrag;
    AssetLoader loader;
    AudioPlayer player;

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        setContentView(R.layout.main);
        
        JniBridge.setAssetManager(getAssets());
        loader = new AssetLoader(getAssets());
        JniBridge.setPngLoader(loader);
        
        player = new AudioPlayer(this);
        //JniBridge.setAudioPlayer(player);

        // Create/extract fragments for each of the game screens
        FragmentManager fm = getFragmentManager();
        introFrag = (IntroFrag) fm.findFragmentById(R.id.introFragment);
        gameFrag = (GameFrag) fm.findFragmentById(R.id.gameFragment);
        resultFrag = (ResultFrag) fm.findFragmentById(R.id.resultFragment);

        // Show the intro screen
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        ft.show(introFrag);
        ft.hide(gameFrag);
        ft.hide(resultFrag);
        ft.commit();
    }

    @Override
    public void startGame(String levelId) {
        player.playSound(AudioPlayer.START);
        Log.i("RotActivity", "Starting level " + levelId);
		JniBridge.loadLevel("levels/" + levelId + "/level.dat");
        
        // Switch to the "playing" fragment
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        ft.hide(introFrag);
        ft.show(gameFrag);
        ft.commit();
    }
    
    @Override
    public void endGame(int result) {
        resultFrag.setResult(result);
        
        if(result == 1){
            player.playSound(AudioPlayer.WIN);
        }
        else{
            player.playSound(AudioPlayer.LOSE);
        }
        // Switch to the game result fragment
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        ft.show(resultFrag);
        ft.hide(gameFrag);
        ft.commit();
    }
    
    @Override
    public void newGame()
    {
        // Switch back to the level selection fragment
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        ft.show(introFrag);
        ft.hide(resultFrag);
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

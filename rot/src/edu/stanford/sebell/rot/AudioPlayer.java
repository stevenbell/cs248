package edu.stanford.sebell.rot;

import android.content.Context;
import android.media.AudioManager;
import android.media.SoundPool;

public class AudioPlayer {
    public static final int START = 0;
    public static final int WIN = 1;
    public static final int LOSE = 2;
    public static final int WALKING = 3;
   
    SoundPool pool;
    int playingId;
    int storedIds[] = new int[4];
    
    public AudioPlayer(Context c) {
        pool = new SoundPool(1, AudioManager.STREAM_MUSIC, 0);
        storedIds[START] = pool.load(c, R.raw.matchbegin, 1);
        storedIds[WIN] = pool.load(c, R.raw.three_bells, 1);
        storedIds[LOSE] = pool.load(c, R.raw.foghorn, 1);
        playingId = -1;
    }
    
    // Play without repeat
    public void playSound(int soundId) {
        pool.play(storedIds[soundId], 1.0f, 1.0f, 0, 0, 1.0f);
    }
    
    // Repeat until stopped
    public void loopSound(int soundId) {
        if(playingId == -1){
            playingId = pool.play(storedIds[soundId], 1.0f, 1.0f, -1, 0, 1.0f);
        }
    }
    
    public void stopSound() {
        pool.stop(playingId);
        playingId = -1;
    }
}

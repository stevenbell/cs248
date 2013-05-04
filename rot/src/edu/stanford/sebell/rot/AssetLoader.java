/* Java-side code to load assets stored with the APK.
 * PNG code based on http://lolengine.net/blog/2011/3/2/load-pngs-using-android-ndk
 *  
 */
package edu.stanford.sebell.rot;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class AssetLoader {
	private AssetManager manager;

    public Bitmap open(String path)
    {
        try {
            return BitmapFactory.decodeStream(manager.open(path));
        }
        catch (Exception e) { }
        return null;
    }

    public int getWidth(Bitmap bmp) { return bmp.getWidth(); }
    public int getHeight(Bitmap bmp) { return bmp.getHeight(); }

    public void getPixels(Bitmap bmp, int[] pixels)
    {
        int w = bmp.getWidth();
        int h = bmp.getHeight();
        bmp.getPixels(pixels, 0, w, 0, 0, w, h);
    }

    public void close(Bitmap bmp)
    {
        bmp.recycle();
    }
}
package de.alextape.openmaka.storage;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * Created by thinker on 11.05.15.
 */
@Deprecated
public class StorageHelper {

    private static final String TAG = "OpenMaka::StorageHelper";
    private final String STORAGE = "de.alextape.openmaka";
    private Context context;

    public StorageHelper(Context context) {
        this.context = context;
    }

    private SharedPreferences getSharedPreferences() {

        SharedPreferences sharedPreferences = this.context.getSharedPreferences(this.STORAGE, Context.MODE_PRIVATE);

        return sharedPreferences;
    }

    private SharedPreferences.Editor getSharedPrefEditor() {
        return getSharedPreferences().edit();
    }

    public void storeData(String key, String value) {
        SharedPreferences.Editor editor = getSharedPrefEditor();
        editor.putString(key, value);
        editor.commit();
    }

    public String getValue(String key) {
        SharedPreferences preferences = getSharedPreferences();
        return preferences.getString(key, null);
    }

}

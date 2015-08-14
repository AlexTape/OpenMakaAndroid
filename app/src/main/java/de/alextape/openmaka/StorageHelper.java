package de.alextape.openmaka;

import android.content.Context;
import android.content.SharedPreferences;

@Deprecated
public class StorageHelper {

    private static final String TAG = "OpenMaka::StorageHelper";

    private Context context;
    private final String STORAGE = "de.alextape.openmaka";

    public StorageHelper (Context context) {
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
        // should not be a string overall..
        return preferences.getString(key, null);
    }

}

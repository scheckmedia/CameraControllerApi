package com.example.camerarc;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.view.MenuItem;

public class Setup extends PreferenceActivity {
	SharedPreferences preferences;
	@SuppressWarnings("deprecation")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	    getActionBar().setDisplayHomeAsUpEnabled(true);
	    addPreferencesFromResource(R.xml.settings);
	}
	  @Override
	  public boolean onOptionsItemSelected(MenuItem item) {
	  	{
	  	       switch (item.getItemId()) 
	  	        {
	  	        case android.R.id.home: 
	  	            onBackPressed();
	            	startActivity (new Intent (this,CameraControl.class));
	            	finish();
	  	        default:
	  	}
	  	}    return super.onOptionsItemSelected(item);
	}
}

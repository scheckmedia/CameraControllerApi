package com.example.camerarc;

import android.os.Bundle;
import android.preference.PreferenceManager;
import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import org.json.JSONObject;
import org.json.simple.parser.JSONParser;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ConnectException;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.MalformedURLException;
import java.net.Socket;
import java.net.URL;

public class CameraControl extends Activity {
	SharedPreferences preferences;
	boolean liveViewRunning = false;
	boolean isLoading = false;
	Thread liveView;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_camera_control);
	    preferences = PreferenceManager.getDefaultSharedPreferences(this);
		String ip = preferences.getString("ip", "127.0.0.1");
		String port = preferences.getString("port", "8888");
		final String DEVICE_IP = ip + ":" + port;
	/**
	 * Launch the application.
	 */	
		liveView = new Thread(){
			@Override
			public void run() {
				// TODO Auto-generated method stub
				super.run();
				BufferedReader output = sendJsonRequest("http://"+DEVICE_IP+"/capture?action=live&value=start");
				
				try {
					JSONObject obj = (JSONObject)new JSONParser().parse(output);					
					JSONObject resp = (JSONObject)obj.get("cca_response");
					String status = (String)resp.get("state");
					
					if(status.equalsIgnoreCase("success")){
						JSONObject data = (JSONObject)resp.get("data");
						String ip =(String)data.get("ip_address");
						int port = Integer.parseInt((String)data.get("port"));
						
						
						Socket s = new Socket();
						s.connect(new InetSocketAddress(ip,port));		
						BufferedInputStream bis = new BufferedInputStream(s.getInputStream(),1024*8);					
										
						byte[] size = new byte[4];
						byte[] bin = null;		
						int available = 0;		
						liveViewRunning = true;

						while((available != -1 || bis.available() > 0) && liveViewRunning == true){
							available = bis.read(size);						
							int buffersize = ((size[3] & 0xFF) << 24)|((size[2] & 0xFF) << 16)|((size[1] & 0xFF) << 8)|(size[0] & 0xFF);						
							
							if(buffersize <= 0) continue;
							
							while((bis.available() <= (buffersize + available)) && liveViewRunning == true){
								System.out.println("not enough data... buffering");						
								continue;
							}
							
							System.out.println("Size: " + buffersize);
							
							bin = new byte[buffersize];
							available = bis.read(bin);							
						}
										
						bis.close();
						s.close();			
					} 					
				} catch (IOException e) {
					// TODO Auto-generated catch block	
					e.printStackTrace();
				} catch (Exception e) {
					// TODO Auto-generated catch block	
					e.printStackTrace();
				}
				Thread.currentThread().interrupt();
			}
		};
		};
		
		public void picture (View view) {
		    preferences = PreferenceManager.getDefaultSharedPreferences(this);
			String ip = preferences.getString("ip", "127.0.0.1");
			String port = preferences.getString("port", "8888");
			final String DEVICE_IP = ip + ":" + port;
				new Thread(new Runnable() {
					
					@Override
					public void run() {
						// TODO Auto-generated method stub
						BufferedReader output = sendJsonRequest("http://"+DEVICE_IP+"/capture?action=shot");				
						try {			
							JSONObject obj = (JSONObject) new JSONParser().parse(output);
							@SuppressWarnings("unused")
							JSONObject resp = (JSONObject)obj.get("cca_response");
						} catch (Exception e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
				}).start();		
		}
				
				public void focus(View view) {
				    preferences = PreferenceManager.getDefaultSharedPreferences(this);
					String ip = preferences.getString("ip", "127.0.0.1");
					String port = preferences.getString("port", "8888");
					final String DEVICE_IP = ip + ":" + port;
				new Thread(new Runnable() {
					
					@Override
					public void run() {
						sendJsonRequest("http://"+DEVICE_IP+"/capture?action=autofocus");
					}
				}).start();	
				}
				
	private BufferedReader sendJsonRequest(String urlString){
		try {
			toggleLoading();
			URL url = new URL(urlString);
			HttpURLConnection con = (HttpURLConnection)url.openConnection();
			con.setRequestMethod("GET");
			
			BufferedReader buffer = new BufferedReader(new InputStreamReader(con.getInputStream()));
			toggleLoading();
			return buffer;
			
		} catch (MalformedURLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			Log.e("Network", "Looks like another program");
			runOnUiThread(new Runnable(){
				   @Override
				   public void run() {
					   Toast.makeText(getApplicationContext(), getString(R.string.othertool), Toast.LENGTH_LONG).show();
				   }
			});
		} catch (ConnectException e) {
			// TODO Auto-generated catch block
			Log.e("Network", "Connection to RasPi failed");
			runOnUiThread(new Runnable(){
				   @Override
				   public void run() {
					   Toast.makeText(getApplicationContext(), getString(R.string.connectionfailed), Toast.LENGTH_LONG).show();
				   }
			});
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
		
	}
	
	private void toggleLoading(){
		if(!isLoading){
			new Thread(new Runnable() {
				
				@Override
				public void run() {
					// TODO Auto-generated method stub
					isLoading = true;	
					while(isLoading){
						try {
							Thread.sleep(500);
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
				}
			}).start();
		} else {
			isLoading = false;
		}	
	}	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.camera_control, menu);
		return true;
	}
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if(item.getItemId() == R.id.action_settings){
        	startActivity (new Intent (this, Setup.class));
            return true;
        }
        return super.onOptionsItemSelected(item);
}
}
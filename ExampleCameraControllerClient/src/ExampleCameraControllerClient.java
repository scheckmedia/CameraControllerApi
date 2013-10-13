import java.awt.EventQueue;

import javax.imageio.ImageIO;
import javax.net.ssl.SSLEngineResult.Status;
import javax.swing.BoxLayout;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JFrame;
import javax.swing.JButton;
import javax.swing.JTextPane;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import apple.laf.JRSUIUtils.ComboBox;
import sun.misc.BASE64Decoder;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.MalformedURLException;
import java.net.Socket;
import java.net.URL;
import java.util.List;

import javax.swing.JPanel;

import java.awt.FlowLayout;
import java.awt.Graphics;
import java.awt.Image;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.LayoutStyle.ComponentPlacement;
import java.awt.Color;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.JLabel;
import javax.swing.JComboBox;


public class ExampleCameraControllerClient {
	
	static final String DEVICE_IP = "192.168.178.211:8888";
	
	private JTextPane tbStatus;
	private JFrame frame;
	private SimpleImageFrame imageFrame;

	private boolean liveViewRunning = false;
	private boolean isLoading = false;
	private Thread liveView;
	
	private JComboBox cbIso, cbAperture, cbShutterSpeed;
	
	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					ExampleCameraControllerClient window = new ExampleCameraControllerClient();
					window.frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public ExampleCameraControllerClient() {
		initialize();
		loadConfig();
	}
		

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		frame = new JFrame();
		frame.setResizable(false);
		frame.setBounds(100, 100, 800, 600);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		tbStatus = new JTextPane();
		tbStatus.setForeground(Color.white);
		tbStatus.setBackground(Color.red);
		tbStatus.setText("");
		
		JPanel panel = new JPanel();
		panel.setBounds(12, 91, 782, 465);		
		panel.setBackground(new Color(154, 205, 50));
		
		imageFrame = new SimpleImageFrame();
		imageFrame.setBounds(panel.getX(), panel.getY(), panel.getWidth(), panel.getHeight());		
		imageFrame.setBackground(Color.BLACK);
		imageFrame.setLayout(new FlowLayout(FlowLayout.CENTER));
		panel = imageFrame;				
		
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
					tbStatus.setText(status);
					
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
							
							ByteArrayInputStream imagestream = new ByteArrayInputStream(bin);
							BufferedImage img = ImageIO.read(imagestream);								
							imagestream.close();
							if(img != null){
								imageFrame.setImg(img);							
							}							
						}
													
						bis.close();
						s.close();						
					} 										
				} catch (IOException e) {
					// TODO Auto-generated catch block					
					e.printStackTrace();
				} catch (ParseException e) {
					// TODO Auto-generated catch block					
					e.printStackTrace();
				}
				Thread.currentThread().interrupt();
			}
		};
		
		
		JButton btnSendRequest = new JButton("Take a picture");
		btnSendRequest.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent arg0) {
				new Thread(new Runnable() {
					
					@Override
					public void run() {
						// TODO Auto-generated method stub
						BufferedReader output = sendJsonRequest("http://"+DEVICE_IP+"/capture?action=shot");				
						try {												
							
							JSONObject obj = (JSONObject) new JSONParser().parse(output);
							JSONObject resp = (JSONObject)obj.get("cca_response");
							String status = (String)resp.get("state");
							tbStatus.setText(status);
							if(status.equalsIgnoreCase("success")){
								JSONObject data = (JSONObject)resp.get("data");
								String image = (String)data.get("image");
								InputStream stream = new ByteArrayInputStream(image.getBytes());
								image = null;
								
								BASE64Decoder decoder = new BASE64Decoder();
								byte[] imgdata = decoder.decodeBuffer(stream);					
													
								ByteArrayInputStream bis = new ByteArrayInputStream(imgdata);
								BufferedImage img = ImageIO.read(bis);
								bis.close();
								imageFrame.removeAll();
								imageFrame.setImg(img);
							}																		
						} catch (IOException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						} catch (ParseException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
				}).start();							
			}
		});		
		
		JButton btnStartLiveview = new JButton("Start live view");
		btnStartLiveview.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				try {					
					liveView.start();
				} catch(Exception ex){
					ex.printStackTrace();
				}
			}
		});
		
		JButton btnStopLiveview = new JButton("Stop live view");
		btnStopLiveview.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {								
				sendJsonRequest("http://"+DEVICE_IP+"/capture?action=live&value=stop");
				liveViewRunning = false;
			}
		});
		
		JButton btnAutofocus = new JButton("Focus");
		btnAutofocus.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				new Thread(new Runnable() {
					
					@Override
					public void run() {
						// TODO Auto-generated method stub
						sendJsonRequest("http://"+DEVICE_IP+"/capture?action=autofocus");	
					}
				}).start();	
			}
		});
		
		JLabel lblIso = new JLabel("ISO");
		
		cbIso = new JComboBox();
		cbIso.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				new Thread(new Runnable() {
					
					@Override
					public void run() {
						// TODO Auto-generated method stub
						sendJsonRequest("http://"+DEVICE_IP+"/settings?action=iso&value=" + cbIso.getSelectedItem());
					}
				}).start();
			}
		});
		
		cbAperture = new JComboBox();
		cbAperture.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				System.out.println(cbAperture.getSelectedItem());
				new Thread(new Runnable() {
					
					@Override
					public void run() {
						// TODO Auto-generated method stub
						sendJsonRequest("http://"+DEVICE_IP+"/settings?action=aperture&value=" + cbAperture.getSelectedItem());
					}
				}).start();
			}
		});
		
		JLabel lblAperture = new JLabel("Aperture");
		
		cbShutterSpeed = new JComboBox();
		cbShutterSpeed.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {				
				new Thread(new Runnable() {
					
					@Override
					public void run() {
						// TODO Auto-generated method stub
						sendJsonRequest("http://"+DEVICE_IP+"/settings?action=speed&value=" + cbShutterSpeed.getSelectedItem());
					}
				}).start();
			}
		});
		
		JLabel lblShutterSpeed = new JLabel("Shutter Speed");
		frame.getContentPane().add(panel);
		GroupLayout groupLayout = new GroupLayout(frame.getContentPane());
		groupLayout.setHorizontalGroup(
			groupLayout.createParallelGroup(Alignment.LEADING)
				.addGroup(groupLayout.createSequentialGroup()
					.addGap(6)
					.addComponent(btnSendRequest)
					.addGap(6)
					.addComponent(btnAutofocus, GroupLayout.PREFERRED_SIZE, 128, GroupLayout.PREFERRED_SIZE)
					.addGap(6)
					.addComponent(btnStartLiveview, GroupLayout.PREFERRED_SIZE, 128, GroupLayout.PREFERRED_SIZE)
					.addGap(6)
					.addComponent(btnStopLiveview, GroupLayout.PREFERRED_SIZE, 128, GroupLayout.PREFERRED_SIZE))
				.addGroup(groupLayout.createSequentialGroup()
					.addGap(12)
					.addGroup(groupLayout.createParallelGroup(Alignment.LEADING)
						.addGroup(Alignment.TRAILING, groupLayout.createSequentialGroup()
							.addComponent(tbStatus, GroupLayout.DEFAULT_SIZE, 782, Short.MAX_VALUE)
							.addContainerGap())
						.addGroup(groupLayout.createSequentialGroup()
							.addComponent(lblIso)
							.addGap(12)
							.addComponent(cbIso, GroupLayout.PREFERRED_SIZE, 100, GroupLayout.PREFERRED_SIZE)
							.addGap(27)
							.addComponent(lblAperture, GroupLayout.PREFERRED_SIZE, 64, GroupLayout.PREFERRED_SIZE)
							.addComponent(cbAperture, GroupLayout.PREFERRED_SIZE, 100, GroupLayout.PREFERRED_SIZE)
							.addGap(24)
							.addGroup(groupLayout.createParallelGroup(Alignment.LEADING)
								.addGroup(groupLayout.createSequentialGroup()
									.addGap(93)
									.addComponent(cbShutterSpeed, GroupLayout.PREFERRED_SIZE, 100, GroupLayout.PREFERRED_SIZE))
								.addComponent(lblShutterSpeed, GroupLayout.PREFERRED_SIZE, 94, GroupLayout.PREFERRED_SIZE)))))
		);
		groupLayout.setVerticalGroup(
			groupLayout.createParallelGroup(Alignment.LEADING)
				.addGroup(groupLayout.createSequentialGroup()
					.addGap(5)
					.addGroup(groupLayout.createParallelGroup(Alignment.LEADING)
						.addComponent(btnSendRequest)
						.addComponent(btnAutofocus)
						.addComponent(btnStartLiveview)
						.addComponent(btnStopLiveview))
					.addGap(6)
					.addGroup(groupLayout.createParallelGroup(Alignment.LEADING)
						.addGroup(groupLayout.createSequentialGroup()
							.addGap(5)
							.addComponent(lblIso))
						.addComponent(cbIso, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addGroup(groupLayout.createSequentialGroup()
							.addGap(5)
							.addComponent(lblAperture))
						.addGroup(groupLayout.createSequentialGroup()
							.addGap(1)
							.addComponent(cbAperture, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
						.addGroup(groupLayout.createSequentialGroup()
							.addGap(1)
							.addGroup(groupLayout.createParallelGroup(Alignment.LEADING)
								.addComponent(cbShutterSpeed, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
								.addGroup(groupLayout.createSequentialGroup()
									.addGap(4)
									.addComponent(lblShutterSpeed)))))
					.addGap(488)
					.addComponent(tbStatus, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
					.addContainerGap())
		);
		frame.getContentPane().setLayout(groupLayout);
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
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		tbStatus.setText("can't connect to " + DEVICE_IP);
		toggleLoading();
		return null;
		
	}
	
	private void loadConfig(){		
		BufferedReader output = sendJsonRequest("http://"+DEVICE_IP+"/settings?action=list");		
		try {
			JSONObject obj = (JSONObject) new JSONParser().parse(output);
			JSONObject resp = (JSONObject)obj.get("cca_response");
			String status = (String)resp.get("state");			
			tbStatus.setText(status);
			
			if(status.equalsIgnoreCase("success")){				
				JSONObject data = (JSONObject)resp.get("data");
				JSONObject main = (JSONObject)data.get("main");
				JSONObject imgsettings = (JSONObject)main.get("imgsettings");
				JSONObject capturesettings = (JSONObject)main.get("capturesettings");
				
				JSONObject iso = (JSONObject)imgsettings.get("iso");
				JSONObject aperture = (JSONObject)capturesettings.get("f-number");
				JSONObject shutterspeed = (JSONObject)capturesettings.get("shutterspeed2");
				
				List isoChoices = (List)iso.get("choices");
				cbIso.setModel(new DefaultComboBoxModel(isoChoices.toArray()));
				cbIso.setSelectedItem(iso.get("value"));
				
				List apertureChoices = (List)aperture.get("choices");
				cbAperture.setModel(new DefaultComboBoxModel(apertureChoices.toArray()));
				cbAperture.setSelectedItem(aperture.get("value"));
				
				List shutterspeedChoices = (List)shutterspeed.get("choices");
				cbShutterSpeed.setModel(new DefaultComboBoxModel(shutterspeedChoices.toArray()));
				cbShutterSpeed.setSelectedItem(shutterspeed.get("value"));
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		
		
	}
	
	private void toggleLoading(){
		if(!isLoading){
			new Thread(new Runnable() {
				
				@Override
				public void run() {
					// TODO Auto-generated method stub
					isLoading = true;	
					String loadingText = "Loading";
					while(isLoading){
						try {
							loadingText += ".";
							tbStatus.setText(loadingText);
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
		
	@SuppressWarnings("serial")
	class SimpleImageFrame extends JPanel{
		private BufferedImage img;
		
		void setImg(BufferedImage image){
			img = image;		
			repaint();
		}
		

		@Override
		public void paint(Graphics g) {
			// TODO Auto-generated method stub			
			super.paint(g);
			Image drawImage;
			if (img != null) {			
				drawImage = img.getScaledInstance(-1, this.getHeight() - 4, BufferedImage.SCALE_DEFAULT);
				int xPos = (getWidth()/2) - (drawImage.getWidth(null)/2);
	            g.drawImage(drawImage, xPos, 2, null);
	         }
		}
	}
}

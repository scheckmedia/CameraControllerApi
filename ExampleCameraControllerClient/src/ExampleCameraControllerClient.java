import java.awt.EventQueue;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JButton;
import javax.swing.JTextPane;

import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;
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
import javax.swing.JPanel;

import java.awt.Graphics;
import java.awt.Image;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.LayoutStyle.ComponentPlacement;
import java.awt.Color;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;


public class ExampleCameraControllerClient {
	
	static final String DEVICE_IP = "192.168.178.211:8888";

	private JFrame frame;
	private SimpleImageFrame imageFrame;

	private boolean liveViewRunning = false;
	private Thread liveView;
	
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
	}
		

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		frame = new JFrame();
		frame.setBounds(100, 100, 800, 600);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		final JTextPane tbStatus = new JTextPane();
		tbStatus.setText("test");
		
		final JPanel panel = new JPanel();
		panel.setBackground(new Color(154, 205, 50));
		
		imageFrame = new SimpleImageFrame();
		imageFrame.setBounds(panel.getX(), panel.getY(), panel.getWidth(), panel.getHeight());		
		imageFrame.setBackground(Color.BLACK);
		//panel.add(imageFrame);				
		
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
					Thread.currentThread().interrupt();
					e.printStackTrace();
				} catch (ParseException e) {
					// TODO Auto-generated catch block
					Thread.currentThread().interrupt();
					e.printStackTrace();
				}
			}
		};
		
		
		JButton btnSendRequest = new JButton("Send Request");
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
		
		JButton btnStartLiveview = new JButton("Start Liveview");
		btnStartLiveview.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				try {					
					liveView.start();
				} catch(Exception ex){
					ex.printStackTrace();
				}
			}
		});
		
		JButton btnStopLiveview = new JButton("Stop Liveview");
		btnStopLiveview.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {								
				sendJsonRequest("http://"+DEVICE_IP+"/capture?action=live&value=stop");
				liveViewRunning = false;
			}
		});
			
				
		GroupLayout groupLayout = new GroupLayout(frame.getContentPane());
		groupLayout.setHorizontalGroup(
			groupLayout.createParallelGroup(Alignment.TRAILING)
				.addGroup(groupLayout.createSequentialGroup()
					.addGroup(groupLayout.createParallelGroup(Alignment.LEADING)
						.addGroup(groupLayout.createSequentialGroup()
							.addContainerGap()
							.addGroup(groupLayout.createParallelGroup(Alignment.LEADING)
								.addComponent(tbStatus, GroupLayout.DEFAULT_SIZE, 788, Short.MAX_VALUE)
								.addGroup(groupLayout.createSequentialGroup()
									.addComponent(btnSendRequest)
									.addPreferredGap(ComponentPlacement.RELATED)
									.addComponent(btnStartLiveview, GroupLayout.PREFERRED_SIZE, 128, GroupLayout.PREFERRED_SIZE)
									.addPreferredGap(ComponentPlacement.RELATED)
									.addComponent(btnStopLiveview, GroupLayout.PREFERRED_SIZE, 128, GroupLayout.PREFERRED_SIZE))))
						.addGroup(groupLayout.createSequentialGroup()
							.addGap(12)
							.addComponent(imageFrame, GroupLayout.DEFAULT_SIZE, 782, Short.MAX_VALUE)))
					.addContainerGap())
		);
		groupLayout.setVerticalGroup(
			groupLayout.createParallelGroup(Alignment.LEADING)
				.addGroup(groupLayout.createSequentialGroup()
					.addGap(5)
					.addGroup(groupLayout.createParallelGroup(Alignment.BASELINE)
						.addComponent(btnSendRequest)
						.addComponent(btnStartLiveview)
						.addComponent(btnStopLiveview))
					.addPreferredGap(ComponentPlacement.RELATED)
					.addComponent(imageFrame, GroupLayout.DEFAULT_SIZE, 516, Short.MAX_VALUE)
					.addPreferredGap(ComponentPlacement.RELATED)
					.addComponent(tbStatus, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
					.addGap(0))
		);
		frame.getContentPane().setLayout(groupLayout);
	}
		
	
	private BufferedReader sendJsonRequest(String urlString){
		try {
			URL url = new URL(urlString);
			HttpURLConnection con = (HttpURLConnection)url.openConnection();
			con.setRequestMethod("GET");
			
			BufferedReader buffer = new BufferedReader(new InputStreamReader(con.getInputStream()));
			return buffer;
			
		} catch (MalformedURLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
		
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

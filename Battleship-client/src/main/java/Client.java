import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;


public class Client  
{ 
    public boolean keepConnected = true;
    public Reader reader = null;
    public PrintWriter writer = null;
    public Socket s = null;
    public CommunicationHandler ch = null;
    public Window window = null;
    public int client_id = 0;


    public Client(CommunicationHandler ch){
            this.ch = ch;
    }


    public void setGUI(Window window){
        this.window = window;
    }


    public void createConnection(String ipString, int port) throws IOException  
    { 
        Thread threadReader = null;
        try
        { 
              
            
            s = new Socket(ipString, port); 
      
            reader = new Reader(s, ch, window, this);
            threadReader = new Thread(reader);
            threadReader.start();
            writer = new PrintWriter(new OutputStreamWriter(s.getOutputStream()));
            System.out.println("Client socket created!");
        }catch(Exception e){ 
            window.connection_info_label.setText("Invalid connection parameters!");;
            System.out.println(e);
        } 
        
    } 

    public void endConnection(){
        try {
            writer.close();
            reader.stop();
            s.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void sendMessage(String message){
        try {
            writer.write("UPSC|" + message);
            writer.flush();
        } catch (Exception e) {

        }
    }

    public boolean hostAvailabilityCheck(String address, int port) { 
        try (Socket s = new Socket(address, port)) {
            return true;
        } catch (IOException ex) {
            /* ignore */
        }
        return false;
    }
} 
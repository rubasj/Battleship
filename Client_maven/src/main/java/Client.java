import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;


enum Status {
    IN_LOBBY, IN_GAME, DISCONNECTED
}
/**
 * Client class
 */
public class Client
{ 

    public Reader reader = null;
    public PrintWriter writer = null;
    public Socket skt = null;
    public CommunicationHandler ch = null;
    public Window window = null;

    public Status status;

    public Client(CommunicationHandler ch){
            this.ch = ch;
            this.status = Status.DISCONNECTED;
    }


    public void setGUI(Window window){
        this.window = window;
    }


    public void createConnection(String ip, int port) throws IOException
    { 
        Thread threadReader = null;
        try
        { 
              
            
            skt = new Socket(ip, port);
      
            reader = new Reader(skt, window, ch,this);
            threadReader = new Thread(reader);
            threadReader.start();
            writer = new PrintWriter(new OutputStreamWriter(skt.getOutputStream()));
            System.out.println("Player socket created!");
        }catch(Exception e){ 
            window.connectionInfoLB.setText("Invalid connection parameters!");;
            System.out.println(e);
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

    public void endConnection(){
        try {

            writer.close();
            reader.stop();
            skt.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void sendMessage(String message){
        try {
            System.out.println(message);
            writer.write("[" + message + "]\n");
            writer.flush();
        } catch (Exception e) {

        }
    }


} 
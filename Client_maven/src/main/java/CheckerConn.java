import javax.swing.*;
import java.io.IOException;

public class CheckerConn extends Thread{

    private Window window;
    private Client client;

    public boolean running;
//    public long last_check;

    public CheckerConn(Window window , Client client){
        this.window = window;
        this.client = client;
    }


    @Override
    public void run() {
        running = true;
        while(running){

            try {
                Thread.sleep(5000);
                client.sendMessage( "PING");
                System.out.println("Sending: PING");

            } catch (InterruptedException e) {
//                e.printStackTrace();
                Object[] options = {"OK"};
                JOptionPane.showOptionDialog(new JFrame(),
                        "Error server unreachable!","Error",
                        JOptionPane.PLAIN_MESSAGE,
                        JOptionPane.QUESTION_MESSAGE,
                        null,
                        options,
                        options[0]);
                this.client.endConnection();
                return;
            }

        }
    }


}

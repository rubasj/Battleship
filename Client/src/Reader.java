import java.awt.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

public class Reader implements Runnable
{

    private Window window = null;
    private Client client = null;
    private BufferedReader reader = null;
    private CommunicationHandler handler = null;

    private Socket socket;
    public Reader(Socket socket, Window window, CommunicationHandler handler, Client client){
        this.handler = handler;
        this.client = client;
        this.window = window;
        this.socket = socket;
        try {
            reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        } catch (Exception e) {
            //TODO: handle exception
        }
    }

    @Override
    public void run(){
        String message = null;
        String []splited = null;
        while(true){
            boolean is_valid = false;
            try {
                message = reader.readLine();
                if(message != null){
                    System.out.println("Message accepted!");
                    System.out.println(message);
                    window.infoLB.setText(message);
                    splited = handler.splitMessage(message);
                    

                    /*              MESSAGE DECODER                */
                    if(splited.length > 1){
                        if(splited[1].equalsIgnoreCase("ATTACK"))
                        {
                            if(!splited[2].equalsIgnoreCase("NOT_IN_GAME"))
                            {
                                if(splited[2].equalsIgnoreCase("YOU"))
                                {
                                    if(splited[3].equalsIgnoreCase("ALREADY_ATTACKED" ) || splited[3].equalsIgnoreCase("NOT_YOUR_TURN"))
                                    {
                                        is_valid = true;
                                    }
                                    else{
                                        int position = Integer.parseInt(splited[4]);
                                        if(splited[3].equalsIgnoreCase("NO_HIT")){
                                        window.opponentBTs[position].setBackground(Color.GREEN);
                                        }
                                        else{
                                        window.opponentBTs[position].setBackground(Color.RED);
                                        }
                                    }
                                
    
                                }
                                else{
                                    int position = Integer.parseInt(splited[4]);
                                    if(splited[3].equalsIgnoreCase("NO_HIT")){
                                        window.yourBTs[position].setBackground(Color.GREEN);
                                    }
                                    else{
                                        window.yourBTs[position].setBackground(Color.RED);
                                    }
    
                                }
                            }
                            is_valid = true;
                        }
                        if(splited[1].equalsIgnoreCase("NICKNAME"))
                        {
                            if(splited[2].equalsIgnoreCase("OK"))
                            {
                                window.yourBTsPanel.removeAll();
                                window.opponentBTsPanel.removeAll();
                                window.firstWindow.setContentPane(window.gamePanel);
                                window.firstWindow.revalidate();
                                is_valid = true;
                            }
                            if(splited[2].equalsIgnoreCase("NICKNAME_ALREADY_USED"))
                            {
                                window.firstWindow.setContentPane(window.loginPanel);
                                window.connectionInfoLB.setText("Nickname already in use!");
                                window.firstWindow.revalidate();
                                is_valid = true;
                            }
                        }

                        if(splited[1].equalsIgnoreCase("FIND_GAME")){
                            /* CREATE BOARD */
                            try {
                                System.out.println("NEW GAME FOUND" + splited[2]);
                                /* CREATE BUTTONS */
                                window.yourBTsPanel.removeAll();
                                window.opponentBTsPanel.removeAll();
                                window.yourBTs = window.initBTsInBoard(splited[2], true);
                                window.addButtonsToBoard(window.yourBTsPanel, window.yourBTs);
                                window.opponentBTs = window.initBTsInBoard("0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", false);
                                window.addButtonsToBoard(window.opponentBTsPanel, window.opponentBTs);
                                System.out.println("All buttons initiated");
                                window.yourBTsPanel.revalidate();
                                window.opponentBTsPanel.revalidate();
                            } catch (Exception e)
                            {
                                //TODO: handle exception
                            }
                            is_valid = true;
                        }
    
    
                        if(splited[1].equalsIgnoreCase("GAME_STATE")){
                            System.out.println("Game state received");
                            if(splited[2].equalsIgnoreCase("RECONNECT"))
                            {
                                try {
                                    System.out.println("RECONNECTING TO EXISTING GAME");
                                    /* CREATE BUTTONS */
                                    window.firstWindow.setContentPane(window.gamePanel);
                                    window.firstWindow.revalidate();
                                    window.yourBTsPanel.removeAll();
                                    window.opponentBTsPanel.removeAll();
                                    window.yourBTs = window.initBTsInBoard(splited[3], true);
                                    window.addButtonsToBoard(window.yourBTsPanel, window.yourBTs);
                                    window.opponentBTs = window.initBTsInBoard(splited[4], false);
                                    window.addButtonsToBoard(window.opponentBTsPanel, window.opponentBTs);
                                    System.out.println("All buttons initiated");
                                    window.yourBTsPanel.revalidate();
                                    window.opponentBTsPanel.revalidate();
                                }
                                catch (Exception e)
                                {
                                    //TODO: handle exception
                                }
                            }
                            if(splited[2].equalsIgnoreCase("GAME_ENDED"))
                            {
                                System.out.println("Game ended received");
                                if(splited[3].equalsIgnoreCase("OPPONENT_DISCONNECTED"))
                                {
                                    System.out.println("Opponent disconnected");
                                    window.yourBTsPanel.removeAll();
                                    window.opponentBTsPanel.removeAll();
                                    window.yourBTsPanel.revalidate();
                                    window.opponentBTsPanel.revalidate();
                                }
                            }
                            is_valid = true;
                        }
                        if(splited[1].equalsIgnoreCase("DISCONNECT")){
                            client.endConnection();
                            window.firstWindow.setContentPane(window.loginPanel);
                            window.firstWindow.revalidate();
                        }
                        is_valid = true;
                    }
                    
                }
            }
            catch (IOException e)
            {
                window.connectionInfoLB.setText("Invalid server!");
            }
            if(!is_valid){
                try {
                    socket.close();
                } catch (Exception e) {
                    //TODO: handle exception
                }
            }
            is_valid = false;
        }
    }

    public void stop() {
		try {
			reader.close();
		} catch (IOException e) {

		}
	}
}

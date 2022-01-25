import java.awt.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

public class Reader implements Runnable
{

    private Window window = null;
    private Client Player = null;
    private BufferedReader reader = null;
    private CommunicationHandler handler = null;

    private Socket socket;
    public Reader(Socket socket, Window window, CommunicationHandler handler, Client Player){
        this.handler = handler;
        this.Player = Player;
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


                    if (splited.length == 1 && splited[0].equalsIgnoreCase("[PING]")) {
                        Player.sendMessage("PING");
                    }

                    /*              MESSAGE DECODER                */
                    if(splited.length > 1){
                        if(splited[0].equalsIgnoreCase("ATTACK"))
                        {
                                if(splited[1].equalsIgnoreCase("HIT"))
                                {

                                    if (splited[2].equalsIgnoreCase("YOU")) {
                                        int position = Integer.parseInt(splited[4]);
                                        window.yourBTs[position].setBackground(Color.RED);
                                    } else {
                                        int position = Integer.parseInt(splited[4]);
                                        window.opponentBTs[position].setBackground(Color.RED);
                                    }
    
                                }
                                else if(splited[1].equalsIgnoreCase("MISS")){

                                    if (splited[2].equalsIgnoreCase("YOU")) {
                                        int position = Integer.parseInt(splited[4]);
                                        window.yourBTs[position].setBackground(Color.LIGHT_GRAY);
                                    } else {
                                        int position = Integer.parseInt(splited[4]);
                                        window.opponentBTs[position].setBackground(Color.LIGHT_GRAY);
                                    }

                                }
                                else{
                                        is_valid = true;
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
                            if(splited[2].equalsIgnoreCase("ALREADY_USED"))
                            {
                                window.firstWindow.setContentPane(window.loginPanel);
                                window.connectionInfoLB.setText("Nickname already in use!");
                                window.firstWindow.revalidate();
                                is_valid = true;
                            }
                            if(splited[2].equalsIgnoreCase("PLAYERS"))
                            {
                                window.firstWindow.setContentPane(window.loginPanel);
                                window.connectionInfoLB.setText("Players queue is full.");
                                window.firstWindow.revalidate();
                                is_valid = true;
                            }
                        }

                        if(splited[1].equalsIgnoreCase("WAIT")){
                            window.infoLB.setText("Waiting for game!");
                        }

                        if(splited[1].equalsIgnoreCase("START") ){
                            /* CREATE BOARD */
                            try {
                                System.out.println("NEW GAME FOUND with opponent: " + splited[1]);
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

                        if(splited[0].equalsIgnoreCase("RECONNECT"))
                        {
                            try {
                                System.out.println("RECONNECTING TO EXISTING GAME");
                                /* CREATE BUTTONS */
                                window.firstWindow.setContentPane(window.gamePanel);
                                window.firstWindow.revalidate();
                                window.yourBTsPanel.removeAll();
                                window.opponentBTsPanel.removeAll();
                                window.yourBTs = window.initBTsInBoard(splited[4], true);
                                window.addButtonsToBoard(window.yourBTsPanel, window.yourBTs);
                                window.opponentBTs = window.initBTsInBoard(splited[6], false);
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
                        if(splited[2].equalsIgnoreCase("GAMEEEND")) {
                            System.out.println("Game ended received");

                            if (Integer.parseInt(splited[2]) == 0) {
                                window.infoLB.setText("You loose.");
                            } else {
                                window.infoLB.setText("You win.");
                            }
                            window.yourBTsPanel.removeAll();
                            window.opponentBTsPanel.removeAll();
                            window.yourBTsPanel.revalidate();
                            window.opponentBTsPanel.revalidate();
                        }
                            is_valid = true;

                        if (splited[0].equalsIgnoreCase("OPP") && splited[1].equalsIgnoreCase("RECONNECTED")){
                            window.infoLB.setText("Opponent reconnected");
                        }


                        if(splited[0].equalsIgnoreCase("EXIT")){
                            window.yourBTsPanel.removeAll();
                            window.opponentBTsPanel.removeAll();
                            window.yourBTsPanel.revalidate();
                            window.opponentBTsPanel.revalidate();
                        }
                        if (splited[0].equalsIgnoreCase("LEAVE")){
                            window.yourBTsPanel.removeAll();
                            window.opponentBTsPanel.removeAll();
                            window.yourBTsPanel.revalidate();
                            window.opponentBTsPanel.revalidate();
                            window.infoLB.setText("Opponent left - end game.");
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

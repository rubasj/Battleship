import java.awt.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

public class Reader implements Runnable
{
    final long max_sec = 20;
    private Window window = null;
    private Client player = null;
    private BufferedReader reader = null;
    private CommunicationHandler handler = null;
    boolean running;


    private Socket socket;
    public Reader(Socket socket, Window window, CommunicationHandler handler, Client Player){
        this.handler = handler;
        this.player = Player;
        this.window = window;
        this.socket = socket;
        this.running = true;
        try {
            reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        } catch (Exception e) {
            //TODO: handle exception
        }
    }

    @Override
    public void run(){
//        long time_ping  = System.currentTimeMillis();
//        long time_ping2 = System.currentTimeMillis();
//        long curr_time;
        String message = null;
        String []splited = null;
        while(running)
        {
            boolean is_valid = false;
            try {
                message = reader.readLine();
                if(message != null){
                    System.out.println("Message accepted!");
                    System.out.println(message);
                    splited = handler.splitMessage(message);

                    if (!splited[0].equalsIgnoreCase("PING")) {
                        window.infoLB.setText(message);
                    }




                    /*******************************
                     *
                     *
                     *              PING
                     *
                     *
                      *****************************/
//                    curr_time = System.currentTimeMillis();
//                    if (curr_time - time_ping2 > 5_000) {         // 5s
//                        player.sendMessage("PING");
//
//                    }
//
//                    if (curr_time - time_ping > 20_000) {         // 20s - max interval pingu ze serveru
//                        player.sendMessage("DISCONNECT");
//                        window.firstWindow.setContentPane(window.loginPanel);
//                        window.firstWindow.revalidate();
//                        player.status = Status.DISCONNECTED;
//                        is_valid = true;
//                        player.endConnection();
//
//                    }
//
//                    if (splited.length == 1 && splited[0].equalsIgnoreCase("PING")) {
//                        time_ping = System.currentTimeMillis();
//                        player.sendMessage("PING");
//                        is_valid = true;
//                    }


                    /*              MESSAGE DECODER                */

                    /*********************
                     *
                     *
                     *      CONNECT
                     *
                     *
                     **********************/
                    if (player.status == Status.DISCONNECTED  && splited[0].equalsIgnoreCase("CONNECT")) {


                        if (splited.length == 2 && splited[1].equalsIgnoreCase("OK")) {
                            window.yourBTsPanel.removeAll();
                            window.opponentBTsPanel.removeAll();
                            window.firstWindow.setContentPane(window.gamePanel);
                            window.firstWindow.revalidate();
                            player.status = Status.IN_LOBBY;
                            is_valid = true;
                        }

                        if (splited.length == 3 && splited[1].equalsIgnoreCase("ERR")) {
                            if (splited[2].equalsIgnoreCase("MAX_PLAYERS")) {
                                window.firstWindow.setContentPane(window.loginPanel);
                                window.connectionInfoLB.setText("Players queue is full.");
                                window.firstWindow.revalidate();
                                is_valid = true;
                            } else if (splited[2].equalsIgnoreCase("ALREADY_USED")) {
                                window.firstWindow.setContentPane(window.loginPanel);
                                window.connectionInfoLB.setText("Nickname already used!");
                                window.firstWindow.revalidate();
                                is_valid = true;
                            } else {
                                System.out.println("Unknown problem.");
                            }
                        }

                    }
                    /*******************************************
                     *
                     *
                     *                     LOBBY
                     *
                     *
                     *******************************************/
                    else if (player.status == Status.IN_LOBBY && splited[0].equalsIgnoreCase("PLAY")) {

                        /* WAITING FOR GAME */
                        if (splited.length == 2 && splited[1].equalsIgnoreCase("WAIT")) {
                            window.yourBTsPanel.removeAll();
                            window.opponentBTsPanel.removeAll();
                            window.infoLB.setText("Searching game...");
                            window.yourBTs = window.
                                    initBTsInBoard("0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
                                            false);
                            window.addButtonsToBoard(window.yourBTsPanel, window.yourBTs);
                            window.opponentBTs = window.
                                    initBTsInBoard("0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
                                            false);
                            window.addButtonsToBoard(window.opponentBTsPanel, window.opponentBTs);
                            System.out.println("All buttons initiated");
                            window.yourBTsPanel.revalidate();
                            window.opponentBTsPanel.revalidate();
                            is_valid = true;

                        }
                        if (splited.length == 5 && splited[1].equalsIgnoreCase("START")) {
                            System.out.println("Game started. Opponent: " + splited[2]);
                            window.infoLB.setText("Game started. Opponent: " + splited[2]);
                            /* CREATE BUTTONS */
                            window.yourBTsPanel.removeAll();
                            window.opponentBTsPanel.removeAll();
                            window.yourBTs = window.initBTsInBoard(splited[3], true);
                            window.oppLB.setText("Opponent: " + splited[2]);
                            window.addButtonsToBoard(window.yourBTsPanel, window.yourBTs);
                            window.opponentBTs = window.initBTsInBoard("0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", false);
                            window.addButtonsToBoard(window.opponentBTsPanel, window.opponentBTs);
                            System.out.println("All buttons initiated");
                            window.yourBTsPanel.revalidate();
                            window.opponentBTsPanel.revalidate();
                            is_valid = true;
                            player.status = Status.IN_GAME;

                        }

                        if (splited[1].equalsIgnoreCase("ERR")) {
                            window.infoLB.setText("Invalid request!");
                            is_valid = true;
                        }
                        is_valid = true;

                    }


                    /************************************
                     *
                     *
                     *              GAME
                     *
                     *
                     * *********************************/

                    if (player.status == Status.IN_GAME) {
                        if(splited[0].equalsIgnoreCase("ATTACK")) {

                            if (splited.length == 5) {
                                if (splited[1].equalsIgnoreCase("HIT")) {
                                    set_board(splited, Color.RED);
                                } else if (splited[1].equalsIgnoreCase("MISS")) {
                                    set_board(splited, Color.LIGHT_GRAY);

                                }
                                }
                            else {
                                if (splited[1].equalsIgnoreCase("INV")){
                                    System.out.println("Invalid ATTACK.");
                                    window.infoLB.setText( "Invalid ATTACK, try again.");
                                } else if (splited[1].equalsIgnoreCase("NOT_YOUR_TURN")){
                                    System.out.println("Invalid ATTACK - Not your turn.");
                                    window.infoLB.setText( "Invalid ATTACK, Not your turn.");
                                }
                            }


                            is_valid = true;
                        }

                        if(splited[0].equalsIgnoreCase("GAME_OVER")) {
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

                            player.status = Status.IN_LOBBY;
                            is_valid = true;
                        }


                    }

                    if(splited[0].equalsIgnoreCase("RECONNECT")) { //TODO reconnect
                        try {
                            System.out.println("RECONNECTING TO EXISTING GAME");
                            /* CREATE BUTTONS */
                            window.firstWindow.setContentPane(window.gamePanel);
                            window.firstWindow.revalidate();
                            window.yourBTsPanel.removeAll();
                            window.opponentBTsPanel.removeAll();
                            window.oppLB.setText("Opponent: " + splited[2]);
                            window.yourBTs = window.initBTsInBoard(splited[4], true);
                            window.addButtonsToBoard(window.yourBTsPanel, window.yourBTs);
                            window.opponentBTs = window.initBTsInBoard(splited[6], false);
                            window.addButtonsToBoard(window.opponentBTsPanel, window.opponentBTs);
                            System.out.println("All buttons initiated");
                            window.yourBTsPanel.revalidate();
                            window.opponentBTsPanel.revalidate();
                            is_valid = true;
                            player.sendMessage("RECONNECT|OK");
                        } catch (Exception e) {
                            // ccc
                        }
                    }

                    if (splited[0].equalsIgnoreCase("OPP") && splited[1].equalsIgnoreCase("RECONNECTED")){
                            window.infoLB.setText("Opponent reconnected");
                        is_valid = true;
                    }


                        if(splited[0].equalsIgnoreCase("DISCONNECT")){
                            window.yourBTsPanel.removeAll();
                            window.opponentBTsPanel.removeAll();
                            window.yourBTsPanel.revalidate();
                            window.opponentBTsPanel.revalidate();
                            is_valid = true;
                        }
                        if (splited[0].equalsIgnoreCase("LEAVE")){
                            window.yourBTsPanel.removeAll();
                            window.opponentBTsPanel.removeAll();
                            window.yourBTsPanel.revalidate();
                            window.opponentBTsPanel.revalidate();
                            window.infoLB.setText("Opponent left - end game.");
                            is_valid = true;
                        }

                        if (splited[0].equalsIgnoreCase("DISCONNECT")) {
                            window.firstWindow.setContentPane(window.loginPanel);
                            window.firstWindow.revalidate();
                            player.status = Status.DISCONNECTED;
                            player.endConnection();
                            is_valid = true;
                        }
                        is_valid = true;
                    }
                    

            }
            catch (IOException e)
            {
                //window.connectionInfoLB.setText("Invalid server!");
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

    private void set_board(String[] splited, Color lightGray) {
        if (splited[2].equalsIgnoreCase("YOU")) {
            int position = Integer.parseInt(splited[3]);
            window.yourBTs[position].setBackground(lightGray);
            window.yourBTs[position].setEnabled(false);         // TODO vyresit jestli muze nemuze hrac zautocit
        } else {
            int position = Integer.parseInt(splited[3]);
            window.opponentBTs[position].setBackground(lightGray);
            window.opponentBTs[position].setEnabled(false);
        }
    }

    public void stop() {
		try {
		    running = false;
			reader.close();
		} catch (IOException e) {

		}
	}
}

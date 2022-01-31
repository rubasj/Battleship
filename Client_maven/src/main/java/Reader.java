import javax.swing.*;
import java.awt.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.util.Timer;

public class Reader implements Runnable
{
    final long max_sec = 20;
    private Window window = null;
    private Client user = null;
    private BufferedReader reader = null;
    private CommunicationHandler handler = null;
    boolean running;
    boolean is_valid = false;

    private CheckerConn cc;

    private final String EMPTY_BOARD = "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";


    private Socket socket;
    public Reader(Socket socket, Window window, CommunicationHandler handler, Client user){
        this.handler = handler;
        this.user = user;
        this.window = window;
        this.socket = socket;
        this.running = true;
        this.cc = new CheckerConn(this.window, this.user);
        try {
            reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        } catch (Exception e) {
            //TODO: handle exception
        }
    }


    @Override
    public void run(){

        cc.start();
        String message = null;
        String []splited = null;
        String opp_name = null;

        long ping_time = 0;
        while(running)
        {

            try {
                message = reader.readLine();
                if(message != null && message.length() < 400){
                    System.out.println("Message accepted!");
                    System.out.println(message);
                    splited = handler.splitMessage(message);


                    /*              MESSAGE DECODER                */

//                    if (user.status != Status.DISCONNECTED) {
//                        if (ping_time - curr_time > 20000) {
//                            JOptionPane.showMessageDialog(null, "Connection lost..");
//                            user.endConnection();
//                        }
//                    }
                    /*********************
                     *
                     *
                     *      CONNECT
                     *
                     *
                     **********************/
                    if (user.status == Status.DISCONNECTED  && splited[0].equalsIgnoreCase("CONNECT")) {
                        ping_time = System.currentTimeMillis();
                        connect(splited);

                    }
                    /*******************************************
                     *
                     *
                     *                     LOBBY
                     *
                     *
                     *******************************************/
                    else if (user.status == Status.IN_LOBBY && splited[0].equalsIgnoreCase("PLAY")) {
                        ping_time = System.currentTimeMillis();
                        msgPlay(splited);

                    }


                    /************************************
                     *
                     *
                     *              GAME
                     *
                     *
                     * *********************************/

                    if (user.status == Status.IN_GAME) {

                        if(splited[0].equalsIgnoreCase("ATTACK")) {
                            ping_time = System.currentTimeMillis();
                            attack(splited);

                        }

                        if(splited[0].equalsIgnoreCase("GAME_OVER")) {
                            System.out.println("Game ended received");
                            ping_time = System.currentTimeMillis();
                            gameOver(splited);

                        }


                    }

                    if (splited[0].equalsIgnoreCase("[SERVEROFF]")) {
                        JOptionPane.showMessageDialog(null, "Server offline...");
                        cc.running = false;
                        user.endConnection();
                    }
                    if (splited[0].equalsIgnoreCase("[PING]")) {
                        ping_time = System.currentTimeMillis();
                        System.out.println("PING from server OK.");
                    }

                    long ping = System.currentTimeMillis() - ping_time;
                    if (ping > 10000) {
                        JOptionPane.showMessageDialog(null, "ERR> Wrong connection from server.. You are disconnected.");
                        cc.running = false;
                        user.endConnection();
                    }

                    if(splited[0].equalsIgnoreCase("RECONNECT")) {
                        ping_time = System.currentTimeMillis();
                       reconnect(splited);
                    }

                    if (splited[0].equalsIgnoreCase("OPP") && splited[1].equalsIgnoreCase("RECONNECTED")){
                        window.infoLB.setText("Opponent reconnected");
                        is_valid = true;
                    }

                    if (splited[0].equalsIgnoreCase("OPP") && splited[1].equalsIgnoreCase("DISCONNECTED")){
                        window.infoLB.setText("Opponent disconnected .. waiting for reconnect.");
                        is_valid = true;
                    }


                        if (splited[0].equalsIgnoreCase("LEAVE")){
                            ping_time = System.currentTimeMillis();
                            opp_left();

                        }

                        if (splited[0].equalsIgnoreCase("GAME_EXIT")){
                            user_left();

                        }


                        if (splited[0].equalsIgnoreCase("DISCONNECT")) {
                            window.firstWindow.setContentPane(window.loginPanel);
                            window.firstWindow.revalidate();
                            user.status = Status.DISCONNECTED;
                            cc.running = false;
                            user.endConnection();
                            socket.close();
                            is_valid = true;
                        }
                        is_valid = true;
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

    private void user_left() {
        window.yourBTsPanel.removeAll();
        window.opponentBTsPanel.removeAll();
        window.yourBTsPanel.revalidate();
        window.opponentBTsPanel.revalidate();
        window.infoLB.setText("You left from game.");
        JOptionPane.showMessageDialog(null, "You left from game.");
        window.firstWindow.setContentPane(window.gamePanel);
        is_valid = true;
    }

    private void opp_left() {
        window.yourBTsPanel.removeAll();
        window.opponentBTsPanel.removeAll();
        window.yourBTsPanel.revalidate();
        window.opponentBTsPanel.revalidate();
        window.infoLB.setText("Opponent left - end game.");
        JOptionPane.showMessageDialog(null, "Opponent left from game.");
        window.firstWindow.setContentPane(window.gamePanel);
        is_valid = true;
    }

    private void gameOver(String[] splited) {
        if (Integer.parseInt(splited[1]) == 0) {
            window.infoLB.setText("You loose.");
            JOptionPane.showMessageDialog(null, "GAME OVER, You lose.");
        } else {
            window.infoLB.setText("You win.");
            JOptionPane.showMessageDialog(null, "GAME OVER, You win.");
        }
        window.yourBTsPanel.removeAll();
        window.opponentBTsPanel.removeAll();
        window.yourBTsPanel.revalidate();
        window.opponentBTsPanel.revalidate();
        window.firstWindow.setContentPane(window.gamePanel);
        user.status = Status.IN_LOBBY;
        is_valid = true;
    }

    private void reconnect(String[] splited) {
        if (splited[1].equalsIgnoreCase("1")) {
            connectToLobby("You are reconnected now.");
        }
        try {
            System.out.println("RECONNECTING TO EXISTING GAME");
            /* CREATE BUTTONS */
            window.firstWindow.setContentPane(window.gamePanel);
            window.firstWindow.revalidate();
            window.yourBTsPanel.removeAll();
            window.opponentBTsPanel.removeAll();
            window.oppLB.setText("Opponent: " + splited[2]);
            window.clientButtons = window.initGameButtons(splited[4], false);// ok
            window.addButtonsToBoard(window.yourBTsPanel, window.clientButtons);
            window.oppButtons = window.initGameButtons(splited[6], true); //
            on_turn(splited[1]);
            window.addButtonsToBoard(window.opponentBTsPanel, window.oppButtons);
            System.out.println("All buttons initiated");
            window.yourBTsPanel.revalidate();
            window.opponentBTsPanel.revalidate();
            is_valid = true;
            user.sendMessage("RECONNECT|OK");
            user.status = Status.IN_GAME;
            window.yourBTsPanel.revalidate();
            window.opponentBTsPanel.revalidate();

        } catch (Exception e) {
            // ccc
        }
    }


    private void attack(String[] splited) {

        if (splited.length == 6 && splited[1].equalsIgnoreCase("2")) {
            setBoardItem(splited, Color.LIGHT_GRAY);

            on_turn(splited[4]);
        }
        else if (splited[1].equalsIgnoreCase("3") && splited.length == 7) {
            setBoardItem(splited, Color.RED);
            on_turn(splited[5]);
            if (splited[2].equalsIgnoreCase("OPP"))
                window.oppLB.setText("Opponent: " + window.oppNameString + " ships count: " + splited[6]);
            else {
                window.userLB.setText("Your ship count: " + splited[4]);
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


    private void msgPlay(String[] splited) {
        /* WAITING FOR GAME */
        if (splited.length == 2 && splited[1].equalsIgnoreCase("WAIT")) {
            window.yourBTsPanel.removeAll();
            window.opponentBTsPanel.removeAll();
            window.infoLB.setText("Searching game...");

            window.clientButtons = window.initGameButtons(EMPTY_BOARD, false);
            window.addButtonsToBoard(window.yourBTsPanel, window.clientButtons);
            window.oppButtons = window.initGameButtons(EMPTY_BOARD, true);
            window.addButtonsToBoard(window.opponentBTsPanel, window.oppButtons);
            System.out.println("All buttons initiated");
            window.yourBTsPanel.revalidate();
            window.opponentBTsPanel.revalidate();
            is_valid = true;

        }
        if (splited.length == 7 && splited[1].equalsIgnoreCase("START")) {
            System.out.println("Game started. Opponent: " + splited[2]);
            window.infoLB.setText("Game started. Opponent: " + splited[2]);
            window.oppNameString = splited[2];
            /* CREATE BUTTONS */
            window.yourBTsPanel.removeAll();
            window.opponentBTsPanel.removeAll();

            window.clientButtons = window.initGameButtons(splited[3], false);
            window.oppLB.setText("Opponent: " + window.oppNameString + " ships count: " + splited[6] );
            window.userLB.setText("Your ship count: " + splited[5]);
            window.addButtonsToBoard(window.yourBTsPanel, window.clientButtons);
            window.oppButtons = window.initGameButtons(EMPTY_BOARD, true);
            window.addButtonsToBoard(window.opponentBTsPanel, window.oppButtons);
            on_turn(splited[4]);
            System.out.println("All buttons initialized");
            window.yourBTsPanel.revalidate();
            window.opponentBTsPanel.revalidate();
            is_valid = true;
            user.status = Status.IN_GAME;

        }

        if (splited[1].equalsIgnoreCase("ERR")) {
            window.infoLB.setText("Invalid request!");
            is_valid = true;
        }
        is_valid = true;


    }

    /**
     * For success connection client to server
     * @param splited recv message from server
     */
    private void connect(String[] splited) {

        if (splited.length == 2 && splited[1].equalsIgnoreCase("OK")) {
            connectToLobby("You are connected now.");
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

    private void connectToLobby(String s) {

        window.infoLB.setText(s);
        window.yourBTsPanel.removeAll();
        window.opponentBTsPanel.removeAll();
        window.firstWindow.setContentPane(window.gamePanel);
        window.firstWindow.revalidate();
        user.status = Status.IN_LOBBY;
    }

    private void setBoardItem(String[] splited, Color color) {
        int position = Integer.parseInt(splited[3]);

        if (splited[2].equalsIgnoreCase("YOU")) {
            window.clientButtons[position].setBackground(color);

            window.clientButtons[position].setEnabled(false);
        } else {
            window.oppButtons[position].setText("X");
            window.oppButtons[position].setBackground(color);
            window.oppButtons[position].setEnabled(false);
        }
    }


    private void on_turn(String splited) {
        if (Integer.parseInt(splited) == 0) {
            for (int i = 0; i < window.oppButtons.length; i++ ) {
                window.oppButtons[i].setEnabled(false);
            }
        } else {
            for (int i = 0; i < window.oppButtons.length; i++ ) {
                if (window.oppButtons[i].getText().equalsIgnoreCase("X"))

                    continue;

                window.oppButtons[i].setEnabled(true);
            }
        }
    }

    public void stop() {
		try {
		    running = false;
			reader.close();
		} catch (IOException e) {
            //
		}
	}
}

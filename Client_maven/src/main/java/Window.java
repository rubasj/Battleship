import javax.swing.*;
import java.awt.*;

public class Window {

    /**
     * Frames
     */
    public JFrame firstWindow;

    /**
     * PANELS
     */
    public JPanel yourBTsPanel;
    public JPanel opponentBTsPanel;
    public JPanel loginPanel;
    public JPanel gamePanel;
    public JPanel infoPanel;
    public JPanel controlPanel;


    /**
     * Game Boards
     */
    public JButton [] opponentBTs;
    public JButton [] yourBTs;

    /** Butons */
    public JButton findGameBT;
    public JButton disconnBT;

    /**
     * Labels
     */
    public JLabel infoLB;
    public JLabel connectionInfoLB;
    public JLabel oppLB;
    public JLabel yourLB;

    public final Client player;
    public final CommunicationHandler handler;


    /**
     * Set gui
     */
    public Window(Client Player, CommunicationHandler handler){
        this.player = Player;
        this.handler = handler;

        firstWindow = new JFrame("Battleships - Player");
        loginPanel = new JPanel();
        JLabel ipLB = new JLabel("IPv4 adress: ");
        JLabel portLB = new JLabel("Port: ");
        JLabel nicknameLabel = new JLabel("Nick: ");

        final JTextField nicknameText = new JTextField("jenda");
        final JTextField ipText = new JTextField("147.228.67.10");
        final JTextField portText = new JTextField("4757");



        JButton connBT = new JButton("CONNECT");



        ipText.setPreferredSize(new Dimension(100,30));
        nicknameText.setPreferredSize(new Dimension(100,30));
        portText.setPreferredSize(new Dimension(100,30));
        connectionInfoLB = new JLabel();



        loginPanel.add(ipLB);
        loginPanel.add(ipText);
        loginPanel.add(portLB);
        loginPanel.add(portText);
        loginPanel.add(nicknameLabel);
        loginPanel.add(nicknameText);
        loginPanel.add(connBT);
        loginPanel.add(connectionInfoLB);


        this.player.setGUI(this);
        this.createPanelForGame();
        this.getControlButtons();
        connBT.addActionListener(e-> {

            String port = portText.getText();

            String ip;
            if (ipText.getText().equalsIgnoreCase("localhost"))
                ip = "127.0.0.1";
            else {
                ip = ipText.getText();
            }

                int portInt = Integer.parseInt(port);
                try {
                    this.player.createConnection(ip, portInt);
                } catch (Exception eee) {
                    //TODO: handle exception
                }
            String nicknameString = nicknameText.getText();
            try {
                if(!this.handler.isNicknameOK(nicknameString)){
                    connectionInfoLB.setText("Invalid nickname! Enter only letters or numbers!");
                }
                else{
                    this.player.sendMessage("CONNECT|"+nicknameString);

                }

            } catch (Exception ee) {
                //TODO: handle exception
            }

        });


        firstWindow.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        firstWindow.setPreferredSize(new Dimension(500,500));

        firstWindow.getContentPane().add(loginPanel);
        firstWindow.pack();
        firstWindow.setVisible(true);
    }

    public void getControlButtons(){
        findGameBT = new JButton("Find game!");
        findGameBT.addActionListener(e-> {
                /*      FIND NEW GAME       */
                player.sendMessage("PLAY");
        });


        controlPanel.add(findGameBT);

        disconnBT = new JButton("Exit");
        controlPanel.add(disconnBT);
        disconnBT.addActionListener(e->{

                /* Disconnect */
                try {
                    player.sendMessage("EXIT");
                    Thread.sleep(100);
                } catch (Exception eee) {
                    //TODO: handle exception
                }
        });

    }

    /** Game Panel */
    public void createPanelForGame(){
        this.gamePanel = new JPanel();
        gamePanel.setLayout(new GridLayout(3,2));

        controlPanel = new JPanel();
        infoPanel = new JPanel();

        yourBTsPanel = new JPanel(new GridLayout(10,10));
        opponentBTsPanel = new JPanel(new GridLayout(10,10));
        infoLB = new JLabel("Generic message!");
        oppLB = new JLabel("Opponent");
        yourLB = new JLabel("Your ship count: ");

        gamePanel.add(infoPanel);
        gamePanel.add(controlPanel);
        gamePanel.add(yourLB);
        gamePanel.add(oppLB);
        gamePanel.add(yourBTsPanel);
        gamePanel.add(opponentBTsPanel);
        infoPanel.add(infoLB);
    }

    /**
     * Set buttons

     * @return buttons
     */
    public JButton[] initBTsInBoard(String board, boolean isAlly){
        JButton [] buttons = new JButton[100];
        JButton button;
        for(int i = 0; i < 100; i++){
            if(isAlly){

                if(board.charAt(i) == '0')
                    buttons[i] = new JButton("");
                if(board.charAt(i) == '1') {
                    buttons[i] = new JButton("X");
                    buttons[i].setBackground(Color.GREEN);
                }
                if(board.charAt(i) == '2'){
                    buttons[i] = new JButton("");

                }
                if(board.charAt(i) == '3'){
                    buttons[i] = new JButton("X");
                    buttons[i].setBackground(Color.RED);
                }
            }
            else{
                if(board.charAt(i) == '0'){
                    button = new JButton("");
                    setTextBT(button, i);
                    buttons[i] = button;
                }
                if(board.charAt(i) == '1'){
                    button = new JButton("X");
                    setTextBT(button, i);
                    buttons[i] = button;
                }
                if(board.charAt(i) == '2'){
                    button = new JButton("");
                    button.setBackground(Color.YELLOW);
                    setTextBT(button, i);
                    buttons[i] = button;
                }
                if(board.charAt(i) == '3'){
                    button = new JButton("X");
                    button.setBackground(Color.RED);
                    setTextBT(button, i);
                    buttons[i] = button;
                }
                
            }
            
        }
        return buttons;
    }



    public void addButtonsToBoard(JPanel panel, JButton [] buttons){
        for(int i = 0; i < 100; i++){
            panel.add(buttons[i]);
        }
    }

    private void setTextBT(JButton button, int i){
        final int i1 = i;
        final JButton bt = button;

        bt.addActionListener(e-> {

            player.sendMessage("ATTACK|POSITION|" + i1);
            bt.setText("");

        });
        button = bt;
    }

}

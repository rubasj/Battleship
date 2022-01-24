/**
 * Main class.
 */
public class Main

{
    public static void main(String[] args) throws Exception {

        Client Player = null;
        CommunicationHandler ch = new CommunicationHandler();
        Player = new Client(ch);
        Window window = new Window(Player, ch);
    }
}

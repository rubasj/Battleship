public class Main {
    public static void main(String[] args) throws Exception {

        Player Player = null;
        CommunicationHandler ch = new CommunicationHandler();
        Player = new Player(ch);
        Window window = new Window(Player, ch);
    }
}

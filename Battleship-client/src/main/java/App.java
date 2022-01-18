public class App {
    public static void main(String[] args) throws Exception {

        Client client = null;
        CommunicationHandler ch = new CommunicationHandler();
        client = new Client(ch);
        Window window = new Window(client, ch);
    }
}

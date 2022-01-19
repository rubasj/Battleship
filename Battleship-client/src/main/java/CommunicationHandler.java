import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * For communication
 */
public class CommunicationHandler {


    public boolean isNicknameOK(String nickname){
        Pattern pattern = Pattern.compile("\\w+");
        Matcher matcher = pattern.matcher(nickname);
        return matcher.matches();
    }

    public String[] splitMessage(String message){

        String []tok = message.split("\\|");;
        return tok;
    }
}

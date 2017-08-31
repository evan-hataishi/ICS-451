import java.net.ServerSocket;
import java.net.Socket;
import java.io.PrintWriter;
import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

public class DaytimeServer {

  public static void main(String[] args) {
    if (!isValidArgument(args)) {
      System.out.println("Invalid argument for port");
    } else {
      int portNumber = Integer.parseInt(args[0]);
      try {
        runServer(portNumber);
      } catch (Exception e) {
        System.out.println(e.getMessage());
      }
    }
  }

  private static boolean isValidArgument(String[] args) {
    if (args == null || args.length != 1) {
      return false;
    } else if (!args[0].matches("^-?\\d+$")) {
      return false;
    } else {
      return true;
    }
  }

  private static void runServer(int portNumber) throws Exception {
    ServerSocket serverSocket = new ServerSocket(portNumber);
    Socket socket = serverSocket.accept();
    sendServerResponse(socket);
  }

  public static void sendServerResponse(Socket socket) throws Exception {
    PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
    out.println(getDateTime());
    out.close();
  }

  public static String getDateTime() {
    DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
    Date date = new Date();
    return dateFormat.format(new Date());
  }

}

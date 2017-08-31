import java.net.ServerSocket;
import java.net.Socket;

public class DaytimeServer {

  public static void main(String[] args) {
    if (!isValidPortArgument(args)) {
      System.out.println("Invalid argument for port");
    } else {
      int portNumber = Integer.parseInt(args[0]);
      try {
        runServer(portNumber);
      } catch (Exception e) {
        System.out.println("Unable to start server on port" + portNumber);
      }
    }
  }

  public static boolean isValidPortArgument(String[] args) {
    if (args == null || args.length != 1) {
      return false;
    } else if (!args[0].matches("^-?\\d+$")) {
      return false;
    } else {
      return true;
    }
  }

  public static void runServer(int portNumber) throws Exception {
    ServerSocket serverSocket = new ServerSocket(portNumber);
    Socket socket = serverSocket.accept();
  }

}

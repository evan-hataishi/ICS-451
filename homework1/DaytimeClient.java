import java.net.Socket;
import java.io.BufferedReader;
import java.io.InputStreamReader;

public class DaytimeClient {

  public static void main(String[] args) {
    if (!isValidArgument(args)) {
      System.out.println("Invalid argument for port");
    } else {
      int portNumber = Integer.parseInt(args[0]);
      try {
        runClient(portNumber);
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

  private static void runClient(int portNumber) throws Exception {
    Socket clientSocket = new Socket("localhost", portNumber);
    printServerResponse(clientSocket);
  }

  private static void printServerResponse(Socket clientSocket) throws Exception {
    BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
    String response = in.readLine();
    in.close();
    if (response != null) {
      System.out.println(response);
    }
  }

}

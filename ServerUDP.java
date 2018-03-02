import java.io.*;
import java.net.*;
import java.nio.ByteBuffer;

public class ServerUDP
{
   public static void main(String args[]) throws Exception
   {
      DatagramSocket serverSocket = new DatagramSocket(Integer.parseInt(args[0]));
      byte[] receiveData = new byte[1024];
      DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
      while(true){
      serverSocket.receive(receivePacket);
      String received = new String(receivePacket.getData(), 0, receivePacket.getLength());
      String sentence = new String(receivePacket.getData());
      receiveData = sentence.getBytes();
            
      System.out.print("RECEIVED: ");
      for(int i = 0; i < receiveData[0]; i++) {
	  System.out.print(receiveData[i]  +  " ");
      }
      byte[] sendData = computeSendData(receiveData);
      System.out.print("\nSENDING: ");
      for(int i = 0; i < sendData[0]; i++) {
	  System.out.print(Integer.toHexString(sendData[i]) + " ");
      }
      System.out.println("");
      InetAddress IPAddress = receivePacket.getAddress();
      int port = receivePacket.getPort();
      String capitalizedSentence = sentence.toUpperCase();
      DatagramPacket sendPacket = new DatagramPacket(sendData, sendData[0], IPAddress, port);
      serverSocket.send(sendPacket);
      }
   }
      
   public static byte[] computeSendData(byte[] receiveData) {
      byte[] sendData = new byte[1024];
      byte totalLength = receiveData[0];
      byte requestID = receiveData[1];
      byte opCode = receiveData[2];
      int numOperands = receiveData[3];
      int operand1 = ((receiveData[4]) << 8) | receiveData[5];
      operand1 = operand1 & 0x0000ffff;
      System.out.println("\nOperand 1:" +  operand1);
      int operand2 = (receiveData[6] << 8) | receiveData[7];
      operand2 = operand2 & 0x0000ffff;
      System.out.println("\nOperand 2:" +  operand2);
      int result = 0;
      int errorCode = 0;
      switch(opCode) {
         case 0:
	    result = operand1 + operand2;
            break;
         case 1:
            result = operand1 - operand2;
            break;
      case 2:
            result = operand1 | operand2;
            break;
         case 3:
            result = operand1 & operand2;
            break;
         case 4:
            result = operand1 >> operand2;
            break;
         case 5:
            result = operand1 << operand2;
            break;
         default: errorCode = 127;
            break;
      }
      sendData[0] = (byte) (totalLength - 1);
      sendData[1] = requestID;
      System.out.print("\nRESULT: " + result);
      sendData[2] = (byte) errorCode;
      byte[] resultArray = ByteBuffer.allocate(4).putInt(result).array();
      sendData[3] = resultArray[0];
      sendData[4] = resultArray[1];
      sendData[5] = resultArray[2];
      sendData[6] = resultArray[3];
      return sendData;
   }
}


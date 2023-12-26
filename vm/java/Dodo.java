import java.io.*;

public class Dodo {
	public static void main(String[] args) {
		Context x = new Context(64, 64, 64);

		System.out.println("Dodo v0.1");

    BufferedReader obj = 
      new BufferedReader(
        new InputStreamReader(
          System.in)); 
   
    while (true) {
      System.out.print("-> ");
			try {
	      x.assembler(obj.readLine());
			} catch (IOException e) {
			}
    }     
	}
}

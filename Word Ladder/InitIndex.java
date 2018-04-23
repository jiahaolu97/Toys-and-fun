package WordLadder;
import java.util.*;
import java.io.*;
public class InitIndex {
	public static void main(String[] args) throws Exception {
		String path = "src/WordLadder/index.txt";
		BufferedWriter bw = new BufferedWriter(new FileWriter(new File(path)));
		for(int i=0;i<267751;i++){
			bw.write("null\n");
		}
	}
}

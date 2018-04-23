package WordLadder;
import java.util.*;
import java.util.concurrent.LinkedBlockingDeque;
import java.io.*;
public class Main {
	public static int maxUsingTimes = 100;
	public static String[] dic;
	public static LinkedBlockingDeque<Stack<Integer>> q;
	public static ArrayList<Integer>[] neighbors;
	public static boolean[] used;
	public static void main(String[] args) throws Exception {
		dic = new String[267751];
		used = new boolean[267751];
		neighbors = new ArrayList[267751];
		for(int i=0;i<267751;i++){
			neighbors[i] = new ArrayList<Integer>();
		}
		q = new LinkedBlockingDeque<Stack<Integer>>();
		String path = "src/WordLadder/WordLadder_dictionary.txt";
		BufferedReader reader = new BufferedReader(
				new InputStreamReader(new FileInputStream(path)));
		String line = "";
		int index = 0;
		while((line = reader.readLine())!=null){
			dic[index++] = line;
		}
		path = "src/WordLadder/index.txt";
		reader = new BufferedReader(
				new InputStreamReader(new FileInputStream(path)));
		int id = 0;
		while((line = reader.readLine())!=null){
			id++;
//			if(id%100==0) System.out.println(id);
			if(line.equals("null")) continue;
			String temparr[] = line.split(",");
			for(int i=0;i<temparr.length;i++){
//				if(i%100==0) System.out.println(i);
				neighbors[i].add(Integer.parseInt(temparr[i]));
			}
		}
		System.out.println("欢迎使用word ladder! 输入88退出该程序。");
		Scanner scan = new Scanner(System.in);
		for(int i=0;i<maxUsingTimes;i++){
			System.out.println();
			System.out.print("请输入word ladder的起点：");
			String start = scan.next().toLowerCase();
			if(start.equals("88")){
				System.out.println("再见。");
				updateIndex();
				return;
			}
			if(!valid(start)){
				System.out.println("您输入的英文单词不合法。");
				continue;
			}
			if(find(start)==-1){
				System.out.println("字典中没有单词"+start+";");
				continue;
			}
			
			System.out.print("请输入word ladder的终点：");
			String end = scan.next().toLowerCase();
			if(start.equals("end")){
				System.out.println("再见。");
				updateIndex();
				return;
			}
			if(!valid(end)){
				System.out.println("您输入的英文单词不合法。");
				continue;
			}
			if(find(end)==-1){
				System.out.println("字典中没有单词"+end+";");
				continue;
			}
			System.out.println("正在构建从"+start+"到"+end+"的 word ladder...");
			boolean res = buildLadder(start,end);
			if(res == false) System.out.println("不存在符合条件的word ladder");
			else{
				Stack<Integer> result = q.getLast();
				System.out.println("找到长度为"+result.size()+"的ladder:");
				printPath(result);
			}
		}
		updateIndex();
	}
	
//	public static void main(String[] args) throws Exception {
//		dic = new String[267751];
//		used = new boolean[267751];
//		neighbors = new ArrayList[267751];
//		for(int i=0;i<267751;i++){
//			neighbors[i] = new ArrayList<Integer>();
//		}
//		q = new LinkedBlockingDeque<Stack<Integer>>();
//		String path = "src/WordLadder/WordLadder_dictionary.txt";
//		BufferedReader reader = new BufferedReader(
//				new InputStreamReader(new FileInputStream(path)));
//		String line = "";
//		int index = 0;
//		while((line = reader.readLine())!=null){
//			dic[index++] = line;
//		}
//		path = "src/WordLadder/index.txt";
//		reader = new BufferedReader(
//				new InputStreamReader(new FileInputStream(path)));
//		while((line = reader.readLine())!=null){
//			if(line.equals("null")) continue;
//			String temparr[] = line.split(",");
//			for(int i=0;i<temparr.length;i++){
//				neighbors[i].add(Integer.parseInt(temparr[i]));
//			}
//		}
//		ArrayList<Integer> arr = findNeighbor(find("boy"));
//		Iterator<Integer> it = arr.iterator();
//		while(it.hasNext()){
//			System.out.println(dic[it.next()]);
//		}
//	}
//	
	
	
	public static void printPath(Stack<Integer> stack){
		Stack<Integer> inverse = new Stack<Integer>();
		while(stack.size()>0){
			inverse.push(stack.pop());
		}
		int cnt = 0;
		StringBuffer sb = new StringBuffer();
		while(inverse.size()>1){
			sb.append(dic[inverse.pop()]);
			cnt++;
			sb.append("\n");
		}
		sb.append(dic[inverse.pop()]);
		System.out.println(sb.toString());
	}
	
	public static boolean valid(String w){
		for(int i=0;i<w.length();i++){
			if(!Character.isLetter(w.charAt(i))) return false;
		}
		return true;
	}
	
	public static boolean buildLadder(String st,String ed){
		q.clear();
		Arrays.fill(used, false);
		int start = find(st);
		Stack<Integer> path = new Stack<Integer>();
		path.push(start);
		q.add(path);
		while(!q.isEmpty()){
			Stack<Integer> s = q.poll();
			int head = s.peek();
			ArrayList<Integer> temparr = findNeighbor(head);
			Iterator<Integer> it = temparr.iterator();
			while(it.hasNext()){
				int k = it.next();
				if(used[k]) continue;
				Stack<Integer> go = (Stack<Integer>) s.clone();
				go.push(k);
				used[k] = true;
				q.add(go);
				if(k == find(ed))	return true;
			}
		}
		return false;
	}
	
	public static ArrayList<Integer> findNeighbor(int self){
		if(neighbors[self].size()!=0) return neighbors[self];
		ArrayList<Integer> result = new ArrayList<Integer>();
		String root = dic[self];
		StringBuffer sb = new StringBuffer(root);
		int has = 0;
		//replace a character
		for(int i=0;i<root.length();i++){
			sb = new StringBuffer(root);
			for(char r='a';r<='z';r++){
				sb.setCharAt(i, r);
				has = find(sb.toString());
				if(has!=-1) result.add(has);
			}
		}
		//delete a character
		for(int i=0;i<root.length();i++){
			sb = new StringBuffer(root);
			sb.delete(i, i+1);
			has = find(sb.toString());
			if(has != -1) result.add(has);
		}
		//add a character
		sb = new StringBuffer(root);
		for(int i=0;i<root.length()+1;i++){
			for(char r='a';r<='z';r++){
				sb.insert(i, r);
				has = find(sb.toString());
				if(has!=-1) result.add(has);
				sb.delete(i, i+1);
			}
		}
		neighbors[self] = result;
		return result;
	}
	
	
	public static int find(String target){
		return binSearch(0,dic.length-1,target);
	}
	public static int binSearch(int s,int e,String target){
		int mid = (e-s)/2+s;
		if(dic[mid].equals(target))	return mid;
		if(s>=e) return -1;
		if(target.compareTo(dic[mid])<0) return binSearch(s,mid-1,target);
		if(target.compareTo(dic[mid])>0) return binSearch(mid+1,e,target);
		return -1;
	}
	public static void updateIndex() throws Exception{
		String path = "src/WordLadder/index.txt";
		BufferedWriter bw = new BufferedWriter(new FileWriter(new File(path)));
		for(int i=0;i<267751;i++){
			if(neighbors[i].size() == 0)
				bw.write("null\n");
			else{
				StringBuffer sb = new StringBuffer();
				Iterator<Integer> it = neighbors[i].iterator();
				sb.append(it.next());
				while(it.hasNext()){
					sb.append(",");
					sb.append(it.next());
				}
				bw.write(sb.toString());
				bw.write("\n");
			}
		}
		bw.flush();
	}
}

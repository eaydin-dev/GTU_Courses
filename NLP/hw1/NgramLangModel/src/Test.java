import java.util.ArrayList;
import java.util.List;

public class Test {

    public static void main(String[] args) {

        List<String> res = getNgrams("abcdef", 3);
        System.out.println(res);
    }

    public static List<String> getNgrams(String str, int n)
    {
        List<String> ngrams = new ArrayList<>();

        int c;
        for (int i = 0; i < str.length() - n + 1; i++) {
            String sub = str.substring(i, i+n);
            ngrams.add(sub);
        }

        return ngrams;
    }
}

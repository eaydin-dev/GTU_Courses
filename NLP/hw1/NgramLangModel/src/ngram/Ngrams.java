package ngram;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Ngrams implements Serializable {

    private Map<Ngram, Integer> ngrams = null;
    private List<Integer> counts = null;

    public Ngrams(String str, int n) {
        ngrams = new HashMap<>();
        counts = new ArrayList<>();

        char[] chars = str.replaceAll(" ", "").toCharArray();
        for (int i = 1; i <= n ; i++) {
            Map<Ngram, Integer> t = getNgrams(chars, i);
            int count = t.values().stream().mapToInt(Integer::intValue).sum();
            counts.add(count);
            for (Map.Entry entry : t.entrySet()) {
                ngrams.merge((Ngram)entry.getKey(), (Integer)entry.getValue(), (a, b) -> (a + b));
            }
        }
    }

    public int totalNgrams() {
        return counts.stream().mapToInt(Integer::intValue).sum();
    }

    public int getTotalNgramsFor(int n) {
        return counts.get(n-1);
    }

    // dummy smoothing
    public int getCountForNgram(Ngram ngram) {
//        if (ngrams.containsKey(ngram)){
//            if(ngrams.get(ngram) == 0) {
//                System.err.println("WHAAT");
//            }
//            return ngrams.get(ngram);
//        }
        if (ngrams.containsKey(ngram))
            return ngrams.get(ngram) + 1;
        return 1;
    }

    private Map<Ngram, Integer> getNgrams(char[] chars, int n) {
        Map<Ngram, Integer> ngrams = new HashMap<>();

        int c;
        for (int i = 0; i < chars.length - n + 1; i++) {
            char[] arr = new char[n];
            c = 0;
            for (int j = i; j < i + n; j++)
                arr[c++] = chars[j];

            ngrams.merge(new Ngram(arr), 1, (a, b) -> a + b);
        }

        return ngrams;
    }
}

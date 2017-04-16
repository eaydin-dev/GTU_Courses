import java.io.Serializable;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Ngrams implements Serializable {
    private final double[] lambdas = {0.1, 0.2, 0.2, 0.2, 0.3};
    private transient Map<String, Integer> ngrams = null;
    private Map<String, Double> probabilities = null;
    private List<Integer> counts = null;

    public Ngrams(String str, int n) {
        ngrams = new HashMap<>();
        counts = new ArrayList<>();
        probabilities = new HashMap<>();

        for (int i = 1; i <= n ; i++) {
            Map<String, Integer> t = getNgrams(str, i);
            int count = t.values().stream().mapToInt(Integer::intValue).sum();
            counts.add(count);
            ngrams.putAll(t);
        }

        for (String ngram : ngrams.keySet()) {
            for (int i = 1; i <= n; i++)
                probabilities.put(ngram, getProbOfNgramTrain(ngram));
        }
    }

    public double getProbOfNgram(String ngram) {
        if (probabilities.containsKey(ngram))
            return probabilities.get(ngram);
        else  {
            //System.err.println("> '" + ngram + "'");
            return 1.0 / getTotalNgramsFor(ngram.length());
        }
    }

    public double getProbOfNgramInterpolation(String ngram) {
        double res = 0.0;
        int index = 0, len = ngram.length()-1;

        List<String> subGrams = new ArrayList<>();
        while (ngram.length() > 0) {
            subGrams.add(ngram);
            ngram = ngram.substring(0, ngram.length()-1);
        }

        for (int i = len; i >= 0; i--, index++) {
            //System.out.println(">> " + getProbOfNgram(subGrams.get(index)) + ", " + lambdas[i]);
            res += (getProbOfNgram(subGrams.get(index)) * lambdas[i]);
        }

        return res;
    }

    private double getProbOfNgramTrain(String ngram) {
        double count = getCountForNgram(ngram);
        if (ngram.length() == 1) {
            return count / getTotalNgramsFor(1);
        }
        else {
            return count / getCountForNgram(ngram.substring(0, ngram.length()-1));
        }
    }

    public int totalNgrams() {
        return counts.stream().mapToInt(Integer::intValue).sum();
    }

    public int getTotalNgramsFor(int n) {
        return counts.get(n-1);
    }

    // dummy smoothing
    public int getCountForNgram(String ngram) {
        if (ngrams.containsKey(ngram))
            return ngrams.get(ngram);
        return 1;
    }

    private Map<String, Integer> getNgrams(String str, int n) {
        Map<String, Integer> ngrams = new HashMap<>();

        for (int i = 0; i < str.length() - n + 1; i++) {
            String sub = str.substring(i, i + n);
            ngrams.merge(sub, 1, (a, b) -> a + b);
        }

        return ngrams;
    }
}

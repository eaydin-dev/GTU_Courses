package ngram;

import java.util.*;

public class NgramGenerator {

    public static List<Ngram> getNgrams(String str, int n)
    {
        return getNgrams(str.replaceAll(" ", "").toCharArray(), n);
    }


    private static List<Ngram> getNgrams(char[] chars, int n) {
        List<Ngram> ngrams = new ArrayList<>();

        int c;
        for (int i = 0; i < chars.length - n + 1; i++) {
            char[] arr = new char[n];
            c = 0;
            for (int j = i; j < i + n; j++)
                arr[c++] = chars[j];

            ngrams.add(new Ngram(arr));
        }

        return ngrams;
    }

    private static void addOrInc(List<Map.Entry<Ngram, Integer>> ngrams, Ngram newGram) {
        for (Map.Entry entry : ngrams) {
            if (entry.getKey().equals(newGram)) {
                entry.setValue((Integer)entry.getValue() + 1);
                return;
            }
        }

        ngrams.add(new AbstractMap.SimpleEntry<Ngram, Integer>(newGram, 1));
    }
}

import ngram.Ngram;
import ngram.NgramGenerator;
import ngram.Ngrams;

import java.util.List;
import java.util.Map;

public class PerplexityCalculator {

    // P(A|B) = P(A,B) / P(B)
    // P(A|B C) = P(A,B,C) / P(B,C)
    public double getPerplexity(String test, int n, Ngrams ngrams) {
        String[] sentences = test.split("\\."); // dummy way to get sentences
        double res = 0.0;
        for (String sentence : sentences) {
            sentence = sentence.replaceAll(" ", "");
            if (sentence.length() > 5) { // just in case
                Double prob = getProbOfSentence(sentence, n, ngrams);
                if (Double.isNaN(prob)) {
                    System.err.println("NAN");
                }
                else if (Double.isNaN(log2(prob))) {
                    System.err.println("nan with: " + prob);
                }

//                if (n < 4)
//                    System.out.println(">> prob: " + prob);
                if (prob == 0) {
                    System.err.println("fucked up n = " + n + " > " + sentence);
                }
                res += (prob * log2(prob));
            }
        }

        System.out.println("> n: " + n + ", res: " + res);
        return Math.pow(2, -res);
    }


    public double getProbOfSentence(String sentence, int n, Ngrams ngrams) {
        List<Ngram> sentenceGrams = NgramGenerator.getNgrams(sentence, n);
        List<Ngram> sentenceSubGrams = null;
        if (n != 1)
            sentenceSubGrams = NgramGenerator.getNgrams(sentence, n-1);
        double res = 0.0;
        int subIndex = 0;
        //double totalGrams = ngrams.getTotalNgramsFor(n);
        int totalGrams = ngrams.getTotalNgramsFor(n);

        for (Ngram sGram : sentenceGrams) {
            int c = ngrams.getCountForNgram(sGram);
            int c2 = (n == 1 ? totalGrams : ngrams.getCountForNgram(sentenceSubGrams.get(subIndex++)));
            double subRes = ((double)c / c2);
            if (subRes == 0) {
                System.err.println("n=" + n + ", " + sGram);
            }
            System.out.println("res: " + res + ", subRes: " + subRes);
            res += Math.log(subRes);

            //System.out.println("> " + res);
        }

//        if (res == 0)
//            System.err.println("> zero");
        return Math.exp(res);
    }

//    private double getProbOfNgram(Ngram ngram, Ngrams ngrams) {
//        int n = ngram.n();
//        if (n == 1) {
//            int c = ngrams.getCountForNgram(ngram);
//            double res = (c / ngrams.getTotalNgramsFor(ngram.n()));
//            return res;
//        }
//
//        else {
//
//        }
//
//    }

    private double perp1(String sentence, int n, Ngrams ngrams) {
        List<Ngram> sentenceGrams = NgramGenerator.getNgrams(sentence, n);
        double res = 0.0;
        double totalGrams = ngrams.getTotalNgramsFor(n);
        double subRes;

        for (Ngram sGram : sentenceGrams) {
            subRes = getProbOfSentence(sentence, n, ngrams);
            res += (log2(subRes)); // res += (subRes * log2(subRes));
        }

        return Math.pow(2, ((-res)/sentenceGrams.size()));
    }


    private double perpOther(String sentence, int n, Ngrams ngrams) {
        double res = 0.0;
        double totalGrams1 = ngrams.getTotalNgramsFor(n - 1);
        double totalGrams2 = ngrams.getTotalNgramsFor(n);

        List<Ngram> sentenceGrams1 = NgramGenerator.getNgrams(sentence, n-1);
        List<Ngram> sentenceGrams2 = NgramGenerator.getNgrams(sentence, n);

        int i2 = 1;

        for (int i = 0; i < sentenceGrams2.size() - 1; i++) {
            int c1 = ngrams.getCountForNgram(sentenceGrams1.get(i2++));
            int c2 = ngrams.getCountForNgram(sentenceGrams2.get(i));
            //System.out.println("multiplying with: " + ((c1 / totalGrams1) * (c2 / totalGrams2)));
            //if (c1 != -1 && c2 != -1) {
            double subRes = (c1 / totalGrams1) * (c2 / totalGrams2);
            res += (subRes * log2(subRes));
            //}
        }

        return Math.pow(2, ((-res)/sentenceGrams2.size()));
    }

    private Double log2(double num) {
        return Math.log(num) / Math.log(2);
    }

    private int getCount(List<Map.Entry<Ngram, Integer>> ngrams, Ngram ngram) {

        for (Map.Entry entry : ngrams) {
            if (entry.getKey().equals(ngram))
                return (int) entry.getValue();
        }

        return -1;
    }

//
//    private double perp1(String sentence, int n, Ngrams ngrams) {
//        List<Ngram> sentenceGrams = NgramGenerator.getNgrams(sentence, n);
//        double res = 0.0;
//        double totalGrams = ngrams.getTotalNgramsFor(n);
//
//        for (Ngram sGram : sentenceGrams) {
//            int c = ngrams.getCountForNgram(sGram);
//            double subRes = (c / totalGrams);
//            res += (subRes * log2(subRes));
//        }
//
//        return Math.pow(2, ((-res)/sentenceGrams.size()));
//    }
}

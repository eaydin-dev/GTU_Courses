import ngram.Ngram;
import ngram.NgramGenerator;
import ngram.Ngrams;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class LanguageModel {
    private final int MAX_N = 5;
    private final Ngrams ngrams;
    private final double[] lambdas = {0.1, 0.2, 0.2, 0.2, 0.3};

    public LanguageModel(Ngrams ngrams) {
        this.ngrams = ngrams;
    }

    public String getPerplexityChainReport(String test, int n) {
        double entropy = getEntropy(test, n);
        return "Perplexity for Chain Rule n=" + n + "> " + Math.exp(entropy);
    }

    public String getPerplexityInterpolationReport(String test) {
        double prob = getProbOfSentenceInterpolation(test);
        String s = "Perplexity for Interpolation with l1..l5 = " + Arrays.toString(lambdas) + " > ";
        System.out.println("prob: " + prob);
        s += Math.exp(prob * Math.log(prob));
        return s;
    }

    public double getProbOfSentenceChain(String sentence, int n) {
        if (sentence.contains(" "))
            sentence = sentence.replaceAll(" ", "");

        double res = 0.0, subRes, c2;
        int subIndex = 0;
        int c;
        double totalGrams = ngrams.getTotalNgramsFor(n);
        List<Ngram> sentenceGrams = NgramGenerator.getNgrams(sentence, n);
        List<Ngram> sentenceSubGrams = null;
        if (n != 1)
            sentenceSubGrams = NgramGenerator.getNgrams(sentence, n - 1);

        for (Ngram sGram : sentenceGrams) {
            c = ngrams.getCountForNgram(sGram);
            c2 = (n == 1 ? totalGrams : ngrams.getCountForNgram(sentenceSubGrams.get(subIndex++)));
            subRes = ((double) c / c2);
            res += Math.log(subRes);
        }
        return Math.exp(res);
    }

    private double getEntropy(String sentence, int n) {
        if (sentence.contains(" "))
            sentence = sentence.replaceAll(" ", "");

        double res = 0.0, subRes, c2;
        int subIndex = 0;
        int c;
        double totalGrams = ngrams.getTotalNgramsFor(n);
        List<Ngram> sentenceGrams = NgramGenerator.getNgrams(sentence, n);
        List<Ngram> sentenceSubGrams = null;
        if (n != 1)
            sentenceSubGrams = NgramGenerator.getNgrams(sentence, n - 1);

        for (Ngram sGram : sentenceGrams) {
            c = ngrams.getCountForNgram(sGram);
            c2 = (n == 1 ? totalGrams : ngrams.getCountForNgram(sentenceSubGrams.get(subIndex++)));
            subRes = ((double) c / c2);
            res += subRes * Math.log(subRes);
        }
        return -res / sentenceGrams.size();
    }

    public double getProbOfSentenceInterpolation(String sentence) {
        if (sentence.contains(" "))
            sentence = sentence.replaceAll(" ", "");

        List<Double> results = new ArrayList<>();
        for (int i = 1; i <= MAX_N; i++) {
            results.add(getProbOfSentenceChain(sentence, i) * lambdas[i - 1]);
            //System.out.println("1- " + getProbOfSentenceChain(sentence, i) + ", 2- " + lambdas[i - 1]);
        }

        //System.out.println("results: " + results.toString());
        return results.stream().mapToDouble(Double::doubleValue).sum();
    }

}

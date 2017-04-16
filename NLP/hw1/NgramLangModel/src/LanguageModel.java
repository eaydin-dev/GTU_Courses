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
        double perp = getProbChain(test, n, true);
        return String.format("Perplexity for Chain Rule n=%d> %s", n, perp);
    }

    public String getPerplexityInterpolationReport(String test) {
        double perp = getProbInterpolation(test, true);
        return String.format("Perplexity for Interpolation with l1..l5 = %s > %s", Arrays.toString(lambdas), perp);
    }

    public double getProbChain(String sentence, int n) {
        return getProbChain(sentence, n, false);
    }

    private double getProbChain(String sentence, int n, boolean perplexity) {
        double res = 0.0, subRes;
        List<String> sentenceGrams = getNgrams(sentence, n);

        for (String sGram : sentenceGrams) {
            subRes = ngrams.getProbOfNgram(sGram);
            if (perplexity)
                res += (subRes * Math.log(subRes));
            else
                res += Math.log(subRes);
        }
        if (perplexity)
            return Math.exp(-res / sentenceGrams.size());
        else
            return Math.exp(res);
    }

    public double getProbInterpolation(String str) {
        return getProbInterpolation(str, false);
    }

    private double getProbInterpolation(String str, boolean perplexity) {
        List<String> strGrams = getNgrams(str, 5);
        double res = 0.0;
        for (String strGram : strGrams) {
            double subRes = ngrams.getProbOfNgramInterpolation(strGram);
            if (perplexity)
                res += subRes * Math.log(subRes);
            else
                res += Math.log(subRes);
        }

        if (perplexity)
            return Math.exp(-res / strGrams.size());
        else
            return Math.exp(res);
    }

    private List<String> getNgrams(String str, int n)
    {
        List<String> ngrams = new ArrayList<>();

        for (int i = 0; i < str.length() - n + 1; i++) {
            String sub = str.substring(i, i+n);
            ngrams.add(sub);
        }

        return ngrams;
    }


//    private double getEntropy(String sentence, int n) {
//        List<String> sentenceGrams = NgramGenerator.getNgrams(sentence, n);
//        double res = 0.0, subRes;
//
//        for (String sGram : sentenceGrams) {
//            subRes = ngrams.getProbOfNgram(sGram);
//            res += subRes * Math.log(subRes);
//        }
//        return -res / sentenceGrams.size();
//    }


//    public double getProbChain(String sentence, int n) {
//        if (sentence.contains(" "))
//            sentence = sentence.replaceAll(" ", "");
//
//        double res = 0.0, subRes, c2;
//        int subIndex = 0;
//        int c;
//        double totalGrams = ngrams.getTotalNgramsFor(n);
//        List<String> sentenceGrams = NgramGenerator.getNgrams(sentence, n);
//        List<String> sentenceSubGrams = null;
//        if (n != 1)
//            sentenceSubGrams = NgramGenerator.getNgrams(sentence, n - 1);
//
//        for (String sGram : sentenceGrams) {
//            c = ngrams.getCountForNgram(sGram);
//            c2 = (n == 1 ? totalGrams : ngrams.getCountForNgram(sentenceSubGrams.get(subIndex++)));
//            subRes = ((double) c / c2);
//            res += Math.log(subRes);
//        }
//        return Math.exp(res);
//    }

//
//    private double getEntropy(String sentence, int n) {
//        if (sentence.contains(" "))
//            sentence = sentence.replaceAll(" ", "");
//
//        double res = 0.0, subRes, c2;
//        int subIndex = 0;
//        int c;
//        double totalGrams = ngrams.getTotalNgramsFor(n);
//        List<String> sentenceGrams = NgramGenerator.getNgrams(sentence, n);
//        List<String> sentenceSubGrams = null;
//        if (n != 1)
//            sentenceSubGrams = NgramGenerator.getNgrams(sentence, n - 1);
//
//        for (String sGram : sentenceGrams) {
//            c = ngrams.getCountForNgram(sGram);
//            c2 = (n == 1 ? totalGrams : ngrams.getCountForNgram(sentenceSubGrams.get(subIndex++)));
//            subRes = ((double) c / c2);
//            res += subRes * Math.log(subRes);
//        }
//        return -res / sentenceGrams.size();
//    }

}

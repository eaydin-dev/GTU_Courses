import javafx.util.Pair;
import ngram.Ngram;
import ngram.Ngrams;
import utils.ReadFile;
import utils.Serialization;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class Main {

    final static String TRAINING_DATA_PATH = "C:\\Users\\EA\\Desktop\\1150haber\\raw_texts\\";
    final static String MODEL_PATH = "ngram.model";
    final static String TEST_PATH = "test";
    final static int MAX_NGRAMS = 5;

    final static boolean DEBUG = true;

    public static void main(String[] args) {
        Ngrams ngrams;
        String test;
        if (!(new File(MODEL_PATH).exists())) {
            System.out.println("Model file couldn't found. Training again.");
            Pair<Ngrams, String> pair = trainNgramModel();
            ngrams = pair.getKey();
            test = pair.getValue();
        }
        else {
            System.out.println("Loading model file.");
            ngrams = (Ngrams) Serialization.readObject(MODEL_PATH);
            test = (String) Serialization.readObject(TEST_PATH);
            System.out.println("> Model reading done.\n");
        }

        if (!DEBUG) {
            PerplexityCalculator pc = new PerplexityCalculator();
            String sent = "Hava çok güzel";
            int n = 2;
            double res = pc.getPerplexity(sent, n, ngrams);
            System.out.println("Sentence: " + sent);
            System.out.println("Perplexity with ngram with n=" + n + ": " + res);
        }

        else {  // DEBUG
            PerplexityCalculator pc = new PerplexityCalculator();
            String sent = "Hava çok güzel.";
            System.out.println("> " + pc.getProbOfSentence(sent, 4, ngrams));
//            double r1 = pc.getProbOfSentence(sent, 1, ngrams);
//            double r2 = pc.getProbOfSentence(sent, 2, ngrams);
//            double r3 = pc.getProbOfSentence(sent, 3, ngrams);
//            double r4 = pc.getProbOfSentence(sent, 4, ngrams);
//            double r5 = pc.getProbOfSentence(sent, 5, ngrams);
//
//            System.out.println("r1: " + r1);
//            System.out.println("r2: " + r2);
//            System.out.println("r3: " + r3);
//            System.out.println("r4: " + r4);
//            System.out.println("r5: " + r5);

//            String report = step2And3(ngrams, test);
//            System.out.println(report);
        }
    }

    private static Double log2(double num) {
        return Math.log(num) / Math.log(2);
    }


    public static String step2And3(Ngrams ngrams, String test) {
        PerplexityCalculator calc = new PerplexityCalculator();
        System.out.println("> Calculating perplexities.");
        List<Double> perplexities = new ArrayList<>();

        for (int j = 1; j < 5; j++) {
            Double perp = calc.getPerplexity(test, j+1, ngrams);
            //System.err.println("> " + String.format("%.9f", newSent) + " (n=" + j + ")");
            perplexities.add(perp);
        }

        System.out.println("> Job done. Creating report.");
        StringBuilder report = new StringBuilder("\nPerplexity Results\n");

        int n = 1;
        for (Double sum : perplexities)
            report.append(String.format("n=%d -> ", (n++))).append(String.format("%.8f", sum)).append("\n");
        report.append("\n\n");

        return report.toString();
    }

    public static Pair<Ngrams, String> trainNgramModel() {
        System.out.println("> Splitting data.");
        List<File> files = new ArrayList<>();
        getAllPaths(new File(TRAINING_DATA_PATH), files);
        StringBuilder sbTrain = new StringBuilder();
        StringBuilder sbTest = new StringBuilder();

        Collections.shuffle(files);
        int testSize = (files.size() * 5 / 100) + 1;
        int i;
        for (i = 0; i < testSize; i++)
            sbTest.append(ReadFile.readFileAsSingleString(files.get(i)).replaceAll(" ", ""));

        for (; i < files.size(); i++)
            sbTrain.append(ReadFile.readFileAsSingleString(files.get(i)).replaceAll(" ", ""));

        System.out.println("> Splitting complete. Training length: " + sbTrain.length() + ", Test length: " + sbTest.length());
        System.out.println("> Computing ngrams with training set.");
        Ngrams ngrams = new Ngrams(sbTrain.toString(), MAX_NGRAMS);
        String test = sbTest.toString();

        System.out.println("> Ngram computing done. Number of all ngrams: " + ngrams.totalNgrams());
        Serialization.writeObject(ngrams, MODEL_PATH);
        Serialization.writeObject(test, TEST_PATH);

        return new Pair<>(ngrams, test);
    }

    public static void getAllPaths(File root, List<File> list) {

        if (!root.exists())
            throw new IllegalArgumentException("No file: " + root.getAbsolutePath());

        if (root.isDirectory()) {
            File[] children = root.listFiles();
            assert children != null;

            for (File child : children) {
                if (child.isDirectory())
                    getAllPaths(child, list);
                else
                    list.add(child);
            }
        }
    }
}

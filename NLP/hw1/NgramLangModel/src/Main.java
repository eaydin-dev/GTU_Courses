import javafx.util.Pair;
import ngram.Ngrams;
import utils.ReadFile;
import utils.Serialization;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Scanner;

public class Main {

    final static String TRAINING_DATA_PATH = "C:\\Users\\EA\\Desktop\\1150haber\\raw_texts\\";
    final static String MODEL_PATH = "ngram.model";
    final static String TEST_PATH = "test";
    final static int MAX_NGRAMS = 5;

    final static boolean DEBUG = false;

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
            System.out.println("> Loading model file.");
            ngrams = (Ngrams) Serialization.readObject(MODEL_PATH);
            test = (String) Serialization.readObject(TEST_PATH);
            System.out.println("> Model reading done.\n");
        }

        if (!DEBUG) {
            LanguageModel model = new LanguageModel(ngrams);
            String sent = "Hava çok güzel";
            Scanner scanner = new Scanner(System.in);
            boolean useInterpolation = false;
            int n = -1;
            double res = -1;
            String t;

            boolean end = false;
            while (!end) {
                System.out.print("Use interpolation: ");
                t = scanner.next();
                useInterpolation = t.equals("Y")||t.equals("y");

                if (!useInterpolation) {
                    System.out.print("N-gram: ");
                    n = scanner.nextInt();
                }

                System.out.print("Enter sentence: ");
                sent = scanner.next();

                if (useInterpolation)
                    res = model.getProbOfSentenceInterpolation(sent);
                else
                    res = model.getProbOfSentenceChain(sent, n);

                System.out.println("The probability of the sentence is: " + res + "\n\n");
            }
        }

        else {  // DEBUG
            LanguageModel model = new LanguageModel(ngrams);
            String sent = "Şam'ın batısında rejim kuşatmasındaki Madaya beldesinden 2 bin 200 ve Zebadani ilçesinden 150 askeri muhalif ve sivilin bindirildiği otobüsler, dün sabah erken saatlerde hareket etmişti.";
            sent = "ali ata bak";
            System.out.println(model.getPerplexityInterpolationReport(test));
//            System.out.println("> " + model.getProbOfSentenceChain(sent, 1));
//            System.out.println("> " + model.getProbOfSentenceChain(sent, 2));
//            System.out.println("> " + model.getProbOfSentenceChain(sent, 3));
//            System.out.println("> " + model.getProbOfSentenceChain(sent, 4));
//            System.out.println("> " + model.getProbOfSentenceChain(sent, 5));
//
//            System.out.println("> " + model.getProbOfSentenceInterpolation(sent));
//
//            System.out.println();
//            System.out.println();
//            double r1 = pc.getProbOfSentenceChain(sent, 1, ngrams);
//            double r2 = pc.getProbOfSentenceChain(sent, 2, ngrams);
//            double r3 = pc.getProbOfSentenceChain(sent, 3, ngrams);
//            double r4 = pc.getProbOfSentenceChain(sent, 4, ngrams);
//            double r5 = pc.getProbOfSentenceChain(sent, 5, ngrams);
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
        LanguageModel model = new LanguageModel(ngrams);
        System.out.println("> Calculating perplexities.");
        List<String> perplexities = new ArrayList<>();

        for (int j = 0; j < 5; j++) {
            //System.err.println("> " + String.format("%.9f", newSent) + " (n=" + j + ")");
            perplexities.add(model.getPerplexityChainReport(test, j+1));
        }

        System.out.println("> Job done. Creating report.");
        StringBuilder report = new StringBuilder("\nPerplexity Results\n");

        int n = 1;
        for (String perp : perplexities)
            report.append(perp).append("\n");
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

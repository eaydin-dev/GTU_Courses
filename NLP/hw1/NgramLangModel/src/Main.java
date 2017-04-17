import javafx.util.Pair;
import utils.ReadFile;
import utils.Serialization;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class Main {

    final static String TRAINING_DATA_PATH = "C:\\Users\\EA\\Desktop\\1150haber\\raw_texts\\";
    final static String MODEL_PATH = "ngram.model";
    final static String TEST_PATH = "test";
    final static int MAX_NGRAMS = 5;

    static boolean DEBUG = true;

    public static void main(String[] args) {
        if (args.length != 0) {
            DEBUG = args[0].equals("-d");
        }

        Ngrams ngrams;
        String test;
        if (!(new File(MODEL_PATH).exists())) {
            System.out.println("> Model file couldn't found. Training again.");
            Pair<Ngrams, String> pair = trainNgramModel();
            ngrams = pair.getKey();
            test = pair.getValue();
        }
        else {
            System.out.println("> Loading model file.");
            ngrams = (Ngrams) Serialization.readObject(MODEL_PATH);
            test = (String) Serialization.readObject(TEST_PATH);
            System.out.println("> Model reading done.");
        }

        if (!DEBUG) {
            LanguageModel model = new LanguageModel(ngrams);
            String sent = "Hava çok güzel";
            BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
            boolean useInterpolation;
            int n = -1;
            double res = -1;
            String t;

            boolean end = false;
            try {
                while (!end) {
                    System.out.print("\nUse interpolation: ");
                    t = br.readLine().trim();
                    useInterpolation = t.equals("Y")||t.equals("y");

                    if (!useInterpolation) {
                        System.out.print("N-gram: ");
                        n = Integer.parseInt(br.readLine().trim());
                    }

                    System.out.print("Enter sentence: ");
                    sent = br.readLine();

                    if (useInterpolation)
                        res = model.getProbInterpolation(sent);
                    else
                        res = model.getProbChain(sent, n);

                    System.out.println("The probability of the sentence is: " + res + "\n");

                    System.out.print("Continue? > ");
                    t = br.readLine().trim();
                    if (!(t.equals("Y")||t.equals("y")))
                        end = true;
                }
            } catch (IOException ex) {
                System.err.println("IO Error");
                ex.printStackTrace();
            } finally {
                try {
                    br.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        else {  // DEBUG
            /*LanguageModel model = new LanguageModel(ngrams);
            String sent = "Şam'ın batısında rejim kuşatmasındaki Madaya beldesinden 2 bin 200 ve Zebadani ilçesinden 150 askeri muhalif ve sivilin bindirildiği otobüsler, dün sabah erken saatlerde hareket etmişti.";
            //sent = "ali ata bak";

            System.out.println("Sample sentence: " + sent);
            System.out.println("> " + model.getProbChain(sent, 1));
            System.out.println("> " + model.getProbChain(sent, 2));
            System.out.println("> " + model.getProbChain(sent, 3));
            System.out.println("> " + model.getProbChain(sent, 4));
            System.out.println("> " + model.getProbChain(sent, 5));

            System.out.println("\n> " + model.getProbInterpolation(sent));*/


            String report = testPerplexities(ngrams, test);
            System.out.println("\n" + report);
        }
    }

    public static String testPerplexities(Ngrams ngrams, String test) {
        LanguageModel model = new LanguageModel(ngrams);
        System.out.println("> Calculating perplexities.");
        List<String> perplexities = new ArrayList<>();

        for (int j = 0; j < 5; j++)
            perplexities.add(model.getPerplexityChainReport(test, j+1));

        System.out.println("> Job done. Creating report.");
        StringBuilder report = new StringBuilder("\nPerplexity Results\n");

        int n = 1;
        for (String perp : perplexities)
            report.append(perp).append("\n");

        report.append("\n").append(model.getPerplexityInterpolationReport(test)).append("\n");
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
        // sbTest.append(ReadFile.readFileAsSingleString(files.get(i)).replaceAll("[\\'\\\"\\^\\+\\-\\*\\/\\?\\:\\;\\(\\)\\[\\]\\,]", "").toLowerCase().replaceAll("\\s{2,}", " ").trim());

        for (i = 0; i < testSize; i++)
            sbTest.append(ReadFile.readFileAsSingleString(files.get(i)).replaceAll("[^a-zA-Z ]", "").toLowerCase().replaceAll("\\s{2,}", " ").trim());

        for (; i < files.size(); i++)
            sbTrain.append(ReadFile.readFileAsSingleString(files.get(i)).replaceAll("[^a-zA-Z ]", "").toLowerCase().replaceAll("\\s{2,}", " ").trim());

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

import javafx.util.Pair;
import utils.Serialization;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;

public class Main {
    private static boolean DEBUG = false;

    private static final String DATA_PATH = "C:\\Users\\EA\\Desktop\\raw_texts\\";
    private static final String MODEL_PATH = "tc.model";
    private static final String TEST_PATH = "tc.test";

    private static TextCategorizator model;
    private static List<Map.Entry<File, String>> tests;


    public static void main(String[] args) throws IOException {

        if (!(new File(MODEL_PATH).exists())) {
            System.out.println("> Model file couldn't found. Training.");
            Pair<TextCategorizator, List<Map.Entry<File, String>>>
                    pair = trainModel();
            model = pair.getKey();
            tests = pair.getValue();
        }
        else {
            System.out.println("> Loading model file.");
            model = (TextCategorizator) Serialization.readObject(MODEL_PATH);
            Map<File, String> testMap =
                    (Map<File, String>) Serialization.readObject(TEST_PATH);
            tests = new ArrayList<> (testMap.entrySet());
            System.out.println("> Model reading done.");
        }

        if (DEBUG)
            debugMode();
        else
            interactiveMode();
    }

    private static void interactiveMode() throws IOException {
        String filePath, method, temp;
        String s1 = "Similarity score is: ";
        String s2 = "Number of matches in neighborhood is: ";

        int k;
        File file;
        Map.Entry result;
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        boolean end = false;
        while (!end) {
            System.out.print("\nEnter file path: ");
            filePath = br.readLine().trim();
            file = new File(filePath);
            if (!(file.exists())) {
                System.out.println("File doesn't exist.");
                continue;
            }
            System.out.print("Enter method (r or k): ");
            method = br.readLine().toLowerCase().trim();
            if (method.equals("k")) {
                System.out.print("Enter k: ");
                k = Integer.parseInt(br.readLine().trim());
                result = model.classifyKnn(file, k);
                temp = s2;
            }
            else{
                result = model.classifyRocchio(file);
                temp = s1;
            }

            System.out.println("Assigned class: " + result.getKey());
            System.out.println(temp + result.getValue());

            System.out.print("Continue? > ");
            temp = br.readLine().trim();
            if (!(temp.equals("Y") || temp.equals("y")))
                end = true;
        }

        br.close();
    }

    private static void debugMode(){
        long start = System.currentTimeMillis();

        AtomicInteger knn3 = new AtomicInteger(0);
        AtomicInteger knn5 = new AtomicInteger(0);
        AtomicInteger rocchio = new AtomicInteger(0);

        tests.parallelStream().forEach(entry -> {
            File file = entry.getKey();
            String real = entry.getValue();
            String resultKnn3 = model.classifyKnn(file, 3).getKey();
            String resultKnn5 = model.classifyKnn(file, 5).getKey();
            String resultRocchio = model.classifyRocchio(file).getKey();

            if (real.equals(resultKnn3))
                knn3.incrementAndGet();
            if (real.equals(resultKnn5))
                knn5.incrementAndGet();
            if (real.equals(resultRocchio))
                rocchio.incrementAndGet();
        });

        System.out.println("\nNumber of test files: " + tests.size());
        System.out.println("Correctly classified: " + knn3 + ", " + knn5 + ", " + rocchio);

        long end = System.currentTimeMillis();
        NumberFormat formatter = new DecimalFormat("#0.00000");
        System.out.println("Debug done in " +
                formatter.format((end - start) / 1000d) + " seconds.");
    }

    private static
    Pair<TextCategorizator, List<Map.Entry<File, String>>> trainModel() {
        long start = System.currentTimeMillis();

        Map<File, String> trainingFiles = new HashMap<>();
        Map<File, String> testFiles = new HashMap<>();

        Map<File, String> files = new HashMap<>();
        getAllPaths(new File(DATA_PATH), files);
        List<Map.Entry<File, String>> data = new ArrayList<>(files.entrySet());
        Collections.shuffle(data);

        int testSize = (files.size() * 5 / 100) + 1;
        int i;
        for (i = 0; i < testSize; ++i){
            Map.Entry<File, String> sample = data.get(i);
            testFiles.put(sample.getKey(), sample.getValue());
        }
        for (; i < files.size(); ++i){
            Map.Entry<File, String> sample = data.get(i);
            trainingFiles.put(sample.getKey(), sample.getValue());
        }

        TextCategorizator tc = new TextCategorizator();
        tc.trainModel(trainingFiles, MODEL_PATH);

        Serialization.writeObject(testFiles, TEST_PATH);

        long end = System.currentTimeMillis();
        NumberFormat formatter = new DecimalFormat("#0.00000");
        System.out.println("Training time is " +
                formatter.format((end - start) / 1000d) + " seconds.");

        return new Pair<>(tc, new ArrayList<>(testFiles.entrySet()));
    }

    /**
     * Recursively get all paths in a folder.
     * @param root starting path
     * @param fileCategoryMap result. map: <path, category>
     */
    private static void getAllPaths(File root, Map<File, String> fileCategoryMap) {
        if (!root.exists())
            throw new IllegalArgumentException("No file: " + root.getAbsolutePath());

        if (root.isDirectory()) {
            File[] children = root.listFiles();
            assert children != null;

            for (File child : children) {
                if (child.isDirectory())
                    getAllPaths(child, fileCategoryMap);
                else
                    fileCategoryMap.put(child, root.getName());
            }
        }
    }

}

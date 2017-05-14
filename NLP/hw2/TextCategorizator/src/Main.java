import javafx.util.Pair;
import utils.ReadFile;
import utils.Serialization;

import java.io.File;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.*;

public class Main {

    private static final String DATA_PATH = "C:\\Users\\EA\\Desktop\\sample\\";
    private static final String MODEL_PATH = "tc.model";
    private static final String TEST_PATH = "tc.test";

    public static void main(String[] args) {

        TextCategorizator model;
        List<Map.Entry<File, String>> tests;
        if (!(new File(MODEL_PATH).exists())) {
            System.out.println("> Model file couldn't found. Training again.");
            Pair<TextCategorizator, List<Map.Entry<File, String>>> pair = trainModel();
            model = pair.getKey();
            tests = pair.getValue();
        }
        else {
            System.out.println("> Loading model file.");
            model = (TextCategorizator) Serialization.readObject(MODEL_PATH);
            Map<File, String> testMap = (Map<File, String>) Serialization.readObject(TEST_PATH);
            tests = new ArrayList<> (testMap.entrySet());
            System.out.println("> Model reading done.");
        }


        System.out.println("Actual: " + tests.get(0).getValue());
        System.out.println("Result: " + model.classifyKnn(tests.get(0).getKey(), 3));


    }

    private static Pair<TextCategorizator, List<Map.Entry<File, String>>> trainModel() {
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
        System.out.println("Training time is " + formatter.format((end - start) / 1000d) + " seconds.");

        return new Pair<>(tc, new ArrayList<>(testFiles.entrySet()));
    }

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

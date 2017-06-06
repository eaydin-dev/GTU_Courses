import utils.Serialization;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;

@SuppressWarnings("unchecked")
public class Main {

    private static boolean DEBUG = false;
//    private static String VEC_PATH = "C:\\Users\\EA\\Desktop\\vec.txt";
//    private static String DATA_PATH = "C:\\Users\\EA\\Desktop\\raw_texts\\";
    private static String VEC_PATH = "";
    private static String DATA_PATH = "";

    private static String MODEL_PATH = "tc.model";
    private static String TEST_PATH = "tc.test";

    private static TextCategorizator model = null;
    private static Map<File, String> tests = null;

    public static void main(String[] args) throws IOException {
        for (int i = 0; i < args.length; i++) {
            switch (args[i]) {
                case "-d":
                    DEBUG = true;
                    break;
                case "-vec":
                    VEC_PATH = args[++i];
                    break;
                case "-data":
                    DATA_PATH = args[++i];
                    break;
                default:
                    System.err.println("what is " + args[i] + "?");
                    return;
            }
        }

        File f = new File(MODEL_PATH);
        if (!f.exists()) {
            if (VEC_PATH.equals("") || DATA_PATH.equals("")) {
                System.err.println("Give needed files: vectors and data.");
                System.err.println("Usage: main -d -vec [ ] -data [ ]");
                return;
            }

            System.out.println("> Model file couldn't found. Training.");
            Map.Entry<TextCategorizator, Map<File, String>> e = trainModel();

            model = e.getKey();
            tests = e.getValue();
        }
        else {
            System.out.println("> Loading model file.");
            model = (TextCategorizator) Serialization.readObject(MODEL_PATH);
            tests = (Map<File, String>) Serialization.readObject(TEST_PATH);
            System.out.println("> Model reading done.");
        }

        if (DEBUG)
            debugMode();
        else
            interactiveMode();
    }

    private static void debugMode(){
        long start = System.currentTimeMillis();

        AtomicInteger knn3Avg = new AtomicInteger(0);
        AtomicInteger knn3Max = new AtomicInteger(0);
        AtomicInteger knn3Min = new AtomicInteger(0);

        AtomicInteger knn5Avg = new AtomicInteger(0);
        AtomicInteger knn5Max = new AtomicInteger(0);
        AtomicInteger knn5Min = new AtomicInteger(0);

        AtomicInteger rocchioAvg = new AtomicInteger(0);
        AtomicInteger rocchioMax = new AtomicInteger(0);
        AtomicInteger rocchioMin = new AtomicInteger(0);

        tests.entrySet().parallelStream().forEach(entry -> {
            File file = entry.getKey();
            String real = entry.getValue();
            String resultKnn3Avg = model.classifyKnn(file, 3, 1).getKey();
            String resultKnn3Max = model.classifyKnn(file, 3, 2).getKey();
            String resultKnn3Min = model.classifyKnn(file, 3, 3).getKey();

            String resultKnn5Avg = model.classifyKnn(file, 5, 1).getKey();
            String resultKnn5Max = model.classifyKnn(file, 5, 2).getKey();
            String resultKnn5Min = model.classifyKnn(file, 5, 3).getKey();

            String resultRocchioAvg = model.classifyRocchio(file, 1).getKey();
            String resultRocchioMax = model.classifyRocchio(file, 2).getKey();
            String resultRocchioMin = model.classifyRocchio(file, 3).getKey();

            if (real.equals(resultKnn3Avg))
                knn3Avg.incrementAndGet();
            if (real.equals(resultKnn3Max))
                knn3Max.incrementAndGet();
            if (real.equals(resultKnn3Min))
                knn3Min.incrementAndGet();
            if (real.equals(resultKnn5Avg))
                knn5Avg.incrementAndGet();
            if (real.equals(resultKnn5Max))
                knn5Max.incrementAndGet();
            if (real.equals(resultKnn5Min))
                knn5Min.incrementAndGet();
            if (real.equals(resultRocchioAvg))
                rocchioAvg.incrementAndGet();
            if (real.equals(resultRocchioMax))
                rocchioMax.incrementAndGet();
            if (real.equals(resultRocchioMin))
                rocchioMin.incrementAndGet();
        });

        System.out.println("\nNumber of test files: " + tests.size());
        System.out.println("Correctly classified for knn3: " + knn3Avg + ", " + knn3Max + ", " + knn3Min);
        System.out.println("Correctly classified for knn5: " + knn5Avg + ", " + knn5Max + ", " + knn5Min);
        System.out.println("Correctly classified for rocchio: " + rocchioAvg + ", " + rocchioMax + ", " + rocchioMin);

        long end = System.currentTimeMillis();
        NumberFormat formatter = new DecimalFormat("#0.00000");
        System.out.println("Debug done in " +
                formatter.format((end - start) / 1000d) + " seconds.");
    }

    private static void interactiveMode() throws IOException {
        String filePath, classificationMethod, method, temp;
        String s1 = "Similarity score is: ";
        String s2 = "Number of matches in neighborhood is: ";

        int k, option;
        File file;
        Map.Entry result;
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        boolean end = false;
        while (!end) {
            System.out.print("\nEnter file path: ");
            filePath = br.readLine().trim();
            file = new File(filePath);
            if (!(file.exists())) {
                if (file.getName().equals("leave"))
                    break;
                System.out.println("File doesn't exist.");
                continue;
            }
            System.out.print("Enter classification method (r or k): ");
            classificationMethod = br.readLine().toLowerCase().trim();

            System.out.print("Enter method (avg, min or max): ");
            method = br.readLine().toLowerCase().trim();
            if (method.equals("max"))
                option = 2;
            else if (method.equals("min"))
                option = 3;
            else
                option = 1;

            if (classificationMethod.equals("k")) {
                System.out.print("Enter k: ");
                k = Integer.parseInt(br.readLine().trim());
                result = model.classifyKnn(file, k, option);
                temp = s2;
            }
            else{
                result = model.classifyRocchio(file, option);
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

    private static Map.Entry<TextCategorizator, Map<File, String>> trainModel() {
        long start = System.currentTimeMillis();

        Map<File, String> trainingFiles = Collections.synchronizedMap(new HashMap<>());
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

        TextCategorizator tc = new TextCategorizator(trainingFiles, VEC_PATH);

        Serialization.writeObject(tc, MODEL_PATH);
        Serialization.writeObject(testFiles, TEST_PATH);

        long end = System.currentTimeMillis();
        NumberFormat formatter = new DecimalFormat("#0.00000");
        System.out.println("Training done in " +
                formatter.format((end - start) / 1000d) + " seconds.");

        return new AbstractMap.SimpleEntry<>(tc, testFiles);
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

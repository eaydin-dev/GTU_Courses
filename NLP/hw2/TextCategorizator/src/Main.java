import utils.ReadFile;
import utils.Serialization;

import java.io.File;
import java.util.*;

public class Main {

    public static final String DATA_PATH = "C:\\Users\\EA\\Desktop\\raw_texts\\";
    private static final String MODEL_PATH = "tc.model";
    private static final String TEST_PATH = "tc.test";
    private static Map<File, String> trainingFiles = new HashMap<>();
    private static Map<File, String> testFiles = new HashMap<>();

    public static void main(String[] args) {
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
        tc.trainModel(trainingFiles);

        System.out.println(tc);
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

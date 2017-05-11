import utils.ReadFile;
import utils.Serialization;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class Main {

    public static final String DATA_PATH = "C:\\Users\\EA\\Desktop\\raw_texts\\";
    private static final String MODEL_PATH = "tc.model";
    private static final String TEST_PATH = "tc.test";
    private static List<File> trainingFiles = new ArrayList<>();
    private static List<File> testFiles = new ArrayList<>();


    public static void main(String[] args) {

        List<File> files = new ArrayList<>();
        getAllPaths(new File(DATA_PATH), files);
        Collections.shuffle(files);
        int testSize = (files.size() * 5 / 100) + 1;
        int i;
        for (i = 0; i < testSize; ++i)
            testFiles.add(files.get(i));
        for (; i < files.size(); ++i)
            trainingFiles.add(files.get(i));


        TextCategorizator tc = new TextCategorizator();
        tc.trainModel(trainingFiles);

        System.out.println(tc);
    }

    private static void getAllPaths(File root, List<File> list) {

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

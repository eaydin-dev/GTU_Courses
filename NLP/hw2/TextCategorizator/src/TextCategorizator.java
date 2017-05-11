import utils.ReadFile;
import utils.Serialization;

import java.io.File;
import java.util.*;

public class TextCategorizator {

    private List<String> words = null;

    public void trainModel(List<File> trainingFiles) {
        getWords(trainingFiles);
    }

    /**
     * Gets the unique words of all training documents.
     * @param trainingFiles
     */
    private void getWords(List<File> trainingFiles) {
        Set<String> set = new HashSet<>();

        for (File file : trainingFiles) {
            String content = ReadFile.readFileAsSingleString(file);

            // replace all non-letter chars and reduce multiple spaces to one.
            content = content.replaceAll("[^a-zA-Z\\s]", "")
                    .replaceAll("\\s+", " ");

            String[] tokens = content.split("\\s+");

            // get only the first 5 chars of word
            for (int i = 0; i < tokens.length; i++) {
                if (tokens[i].length() > 5)
                    tokens[i] = tokens[i].substring(0, 5);
            }

            set.addAll(Arrays.asList(tokens));
        }

        words = new ArrayList<>(set);
    }


    @Override
    public String toString() {
        String s = "Total words: " + (words == null ? 0 : words.size());

        return s;
    }
}

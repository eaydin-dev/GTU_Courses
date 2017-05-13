import utils.ReadFile;

import java.io.File;
import java.util.*;

public class TextCategorizator {

    private List<String> words = null;

    public void trainModel(Map<File, String> trainingFiles) {
        getWords(trainingFiles);
    }

    /**
     * Gets the unique words of all training documents.
     * @param trainingFiles
     */
    private void getWords(Map<File, String> trainingFiles) {
        Set<String> set = new HashSet<>();

        for (File file : trainingFiles.keySet()) {
            String[] tokens = ReadFile.readWords(file);

            // get only the first 5 chars of word
            for (int i = 0; i < tokens.length; i++) {
                if (tokens[i].length() > 5)
                    tokens[i] = tokens[i].substring(0, 5);
            }

            set.addAll(Arrays.asList(tokens));
        }

        words = new ArrayList<>(set);
    }

    /**
     * Calculates the term-frequency of the term in file.
     * @param term
     * @param file
     * @return
     */
    private int tf(String term, File file) {
        term = term.trim();

        String[] words = ReadFile.readWords(file);
        int counter = 0;
        for (String word : words)
            if (word.equals(term))
                ++counter;

        return counter;
    }

    @Override
    public String toString() {
        String s = "Total words: " + (words == null ? 0 : words.size());

        return s;
    }
}

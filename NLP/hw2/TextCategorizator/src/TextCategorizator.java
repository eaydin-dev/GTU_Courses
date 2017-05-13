import utils.ReadFile;

import java.io.File;
import java.util.*;

public class TextCategorizator {

    private List<String> words = null;
    private Map<File, String> trainingFiles;
    private Map<String, Double> idfs;

    public void trainModel(Map<File, String> trainingFiles) {
        this.trainingFiles = trainingFiles;
        getWords();
        calculateIdfs();
    }

    /**
     * Gets the unique words of all training documents.
     * @param trainingFiles
     */
    private void getWords() {
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
     * Calculates the tfidf...
     * @param term
     * @param file
     * @return
     */
    private double tfidf(String term, File file) {
        int tf = tf(term, file);
        Double idf = idfs.get(term);
        if (idf == null)
            idf = 1.0;

        return tf * idf;
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

    /**
     * Calculate the idf of the given word.
     * @param term
     * @return
     */
    private double idf(String term) {
        double containingFiles = 1; // smoothing
        for (File file : trainingFiles.keySet()) {
            if (containsTerm(term, file))
                ++containingFiles;
        }

        return Math.log(trainingFiles.size() / containingFiles);
    }

    /**
     * Checks if the given file contains the term.
     * @param term
     * @param file
     * @return
     */
    private boolean containsTerm(String term, File file) {
        String[] words = ReadFile.readWords(file);
        for (String word : words)
            if (word.equals(term))
                return true;

        return false;
    }

    /**
     * Calculates the idf of all words in corpora.
     */
    private void calculateIdfs() {
        idfs = new HashMap<>();
        for (String term : words) {
            idfs.put(term, idf(term));
        }
    }

    @Override
    public String toString() {
        String s = "Total words: " + (words == null ? 0 : words.size());

        return s;
    }
}

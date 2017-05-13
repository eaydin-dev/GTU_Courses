import utils.ReadFile;
import utils.Serialization;

import java.io.File;
import java.io.Serializable;
import java.util.*;
import java.util.stream.Collectors;


/**
 * TODO: for idf, read each file while keeping the histogram of words for each file.
 * TODO: this way, for each term, the overhead of opening and reading all files will be eliminated
 */

public class TextCategorizator implements Serializable{

    private transient List<String> terms = null;
    private Map<File, String> trainingFiles;
    private transient Map<String, Double> idfs;
    private Map<File, double[]> tfidfMap;
    private Set<String> classes;

    public void trainModel(Map<File, String> trainingFiles, String outputModel) {

        this.trainingFiles = trainingFiles;
        System.out.println("> Getting words.");
        getWords();
        System.out.println("> Calculating idfs.");
        calculateIdfs();
        tfidfMap = new HashMap<>();
        classes = new HashSet<>();

        int c = 1, size = trainingFiles.size();
        for (Map.Entry<File, String> entry : trainingFiles.entrySet()) {
            System.out.println(String.format("%3d/%d -> %s", (c++), size, entry.getKey().getName()));
            tfidfMap.put(entry.getKey(), getTfidfVector(entry.getKey()));
            classes.add(entry.getValue());
        }

        if (outputModel != null)
            Serialization.writeObject(this, outputModel);
    }

    public String classifyKnn(File file, int k) {
        double[] givenVec = getTfidfVector(file);
        Map<File, Double> distances = new HashMap<>();
        for (Map.Entry<File, double[]> entry : tfidfMap.entrySet()) {
            distances.put(entry.getKey(), cosSimilarity(givenVec, entry.getValue()));
        }

        distances = sortByValue(distances, true);

        List<File> sortedFiles = new ArrayList<>(distances.keySet());
        sortedFiles = sortedFiles.subList(0, k);
        Map<String, Integer> counts = new HashMap<>();
        for (String category : classes) {
            counts.put(category, countClass(sortedFiles, category));
        }

        return (new ArrayList<>(sortByValue(counts, false).keySet())).get(0);
    }

    /**
     * Taken from: http://stackoverflow.com/questions/109383/sort-a-mapkey-value-by-values-java
     * @param map
     * @param <K>
     * @param <V>
     * @return
     */
    private <K, V extends Comparable<? super V>> Map<K, V> sortByValue(Map<K, V> map, boolean reverse) {

        return map.entrySet()
                .stream()
                .sorted((reverse ? Map.Entry.comparingByValue(Collections.reverseOrder()) : Map.Entry.comparingByValue()))
                .collect(Collectors.toMap(
                        Map.Entry::getKey,
                        Map.Entry::getValue,
                        (e1, e2) -> e1,
                        LinkedHashMap::new
                ));
    }

    private int countClass(List<File> files, String c) {
        int counter = 0;
        for (File file : files) {
            if (trainingFiles.get(file).equals(c))
                ++counter;
        }

        return counter;
    }

    public Map<File, double[]> getTfidfMap() {
        return tfidfMap;
    }

    private double cosSimilarity(double[] vec1, double[] vec2) {
        return dotProduct(vec1, vec2) / (Math.sqrt(norm(vec1)) * Math.sqrt(norm(vec2)));
    }

    private double norm(double[] vec) {
        double norm = 0.0;
        for (double aVec : vec) norm += Math.pow(aVec, 2);
        return norm;
    }

    private double dotProduct(double[] vec1, double[] vec2) {
        double dotProduct = 0.0;
        for (int i = 0; i < vec1.length; i++)
            dotProduct += vec1[i] * vec2[i];
        return dotProduct;
    }

    /**
     * Get tfidf vector of the given file.
     * @param file
     * @return
     */
    private double[] getTfidfVector(File file) {
        double[] tfidfs = new double[terms.size()];
        int i = 0;
        for (String term : terms)
            tfidfs[i++] = tfidf(term, file);

        return tfidfs;
    }

    /**
     * Gets the unique words of all training documents.
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

        terms = new ArrayList<>(set);
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
        for (String term : terms) {
            idfs.put(term, idf(term));
        }
    }

    @Override
    public String toString() {
        String s = "Total terms: " + (terms == null ? 0 : terms.size());

        return s;
    }
}

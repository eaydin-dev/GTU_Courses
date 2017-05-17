import utils.FileUtils;
import utils.Serialization;

import java.io.File;
import java.io.Serializable;
import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.Collectors;

public class TextCategorizator implements Serializable{

    private List<String> terms = null;
    private Map<File, String> trainingFiles = null;

    // file to tfidf vector
    private Map<File, double[]> tfidfMap = null;

    // class names
    private Set<String> classes = null;

    // file to histogram map
    private transient Map<File, int[]> histogramMap = null;

    // term to idf map
    private Map<String, Double> idfs = null;

    /**
     * Train the model with given files.
     * @param trainingFiles
     * @param outputModel
     */
    public void trainModel(Map<File, String> trainingFiles, String outputModel) {
        this.trainingFiles = trainingFiles;
        System.out.println("> Getting words.");
        getWords();
        System.out.println("# words: " + terms.size());
        //FileUtils.printCollectionToTxt(terms, "terms.txt");

        System.out.println("> Calculating histograms");
        getTermHistograms();
        System.out.println("> Calculating idfs.");
        calculateIdfs();
        tfidfMap = new HashMap<>();
        classes = new HashSet<>();

        System.out.println("> Calculating tfidfs.");
        trainingFiles.entrySet().parallelStream().forEach(entry -> {
            tfidfMap.put(entry.getKey(), getTfidfVector(entry.getKey()));
            classes.add(entry.getValue());
        });

        if (outputModel != null)
            Serialization.writeObject(this, outputModel);
    }

    private void getCosineSimilarities(double[] givenVec, Map<File, Double> distances) {
        tfidfMap.entrySet().parallelStream().forEach(entry -> {
            distances.put(entry.getKey(),
                    cosSimilarity(givenVec, entry.getValue()));
        });
    }

    /**
     * Classify a given file with knn.
     * @param file
     * @param k
     * @return
     */
    public Map.Entry<String, Integer> classifyKnn(File file, int k) {
        double[] givenVec = getTfidfVector(file);
        Map<File, Double> distances = new HashMap<>();

        getCosineSimilarities(givenVec, distances);
        distances = sortByValue(distances, true);

        List<File> sortedFiles = new ArrayList<>(distances.keySet());
        sortedFiles = sortedFiles.subList(0, k);
        Map<String, Integer> counts = new HashMap<>();
        for (String category : classes) {
            counts.put(category, countClass(sortedFiles, category));
        }

        ArrayList<Map.Entry<String, Integer>> resultList =
                new ArrayList(sortByValue(counts, true).entrySet());

        return resultList.get(0);
    }

    /**
     * Classify a given file with Rocchio.
     * @param file
     * @return
     */
    public Map.Entry<String, Double> classifyRocchio(File file) {
        double[] givenVec = getTfidfVector(file);
        Map<File, Double> distances = new HashMap<>();
        for (Map.Entry<File, double[]> entry : tfidfMap.entrySet()) {
            distances.put(entry.getKey(),
                    cosSimilarity(givenVec, entry.getValue()));
        }

        distances = sortByValue(distances, true);
        List<Map.Entry<File, Double>> sortedFiles =
                new ArrayList<>(distances.entrySet());

        return new AbstractMap.SimpleEntry<>
                (trainingFiles.get(sortedFiles.get(0).getKey()),
                sortedFiles.get(0).getValue());
    }

    /**
     * Traverses all files and creates their term histogram.
     */
    private void getTermHistograms(){
        histogramMap = new HashMap<>();

        for (File file : trainingFiles.keySet()) {
            String[] words = FileUtils.readWords(file);
            int[] hist = getFreshArr(terms.size());
            int index;
            for (String term : words) {
                index = terms.indexOf(term);
                if (index != -1)
                    hist[index]++;
            }

            histogramMap.put(file, hist);
        }
    }

    /**
     * Creates and initializes(with 0) an int array with length n.
     * @param n
     * @return
     */
    private int[] getFreshArr(int n) {
        int[] arr = new int[n];
        for (int i = 0; i < n; i++)
            arr[i] = 0;

        return arr;
    }

    /**
     * Taken from: http://stackoverflow.com/questions/109383/sort-a-mapkey-value-by-values-java
     * @param map
     * @param <K>
     * @param <V>
     * @return
     */
    private <K, V extends Comparable<? super V>> Map<K, V>
        sortByValue(Map<K, V> map, boolean reverse) {
        return map.entrySet()
                .stream()
                .sorted((reverse ?
                        Map.Entry.comparingByValue(Collections.reverseOrder()) :
                        Map.Entry.comparingByValue()))
                .collect(Collectors.toMap(
                        Map.Entry::getKey,
                        Map.Entry::getValue,
                        (e1, e2) -> e1,
                        LinkedHashMap::new
                ));
    }

    /**
     * Counts the number of class c in given files. (Using file to class map)
     * @param files
     * @param c
     * @return
     */
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

    /**
     * Calculate cosine similarity of two vectors
     * @param vec1
     * @param vec2
     * @return
     */
    private double cosSimilarity(double[] vec1, double[] vec2) {
        double norm1 = 0.0;
        double norm2 = 0.0;
        double dotProduct = 0.0;
        for (int i = 0; i < vec1.length; i++) {
            norm1 += Math.pow(vec1[i], 2);
            norm2 += Math.pow(vec2[i], 2);
            dotProduct += vec1[i] * vec2[i];
        }
        return dotProduct / (Math.sqrt(norm1) * Math.sqrt(norm2));
    }

    /**
     * Get tfidf vector of the given file.
     * @param file
     * @return
     */
    private double[] getTfidfVector(File file) {
        double[] tfidfs = new double[terms.size()];
        AtomicInteger i = new AtomicInteger(0);
        terms.parallelStream().forEach(term -> {
            tfidfs[i.getAndIncrement()] = tfidf(term, file);
        });

        return tfidfs;
    }

    /**
     * Gets the unique words of all training documents.
     */
    private void getWords() {
        Set<String> set = new HashSet<>();

        for (File file : trainingFiles.keySet()) {
            String[] tokens = FileUtils.readWords(file);
            getWords(Arrays.asList(tokens), set);
        }

        terms = new ArrayList<>(set);
    }

    private void getWords(List<String> tokens, Set<String> set) {
        tokens.parallelStream().forEach(token -> {
            // get only the first 5 chars of word
            if (token.length() > 5)
                token = token.substring(0, 5);
            set.add(token);
        });
    }

    /**
     * Calculates the tfidf...
     * @param term
     * @param file
     * @return
     */
    private double tfidf(String term, File file) {
        int tf = tf(term, file);
        Double idf;
        if (idfs != null) {
            idf = idfs.get(term);
            if (idf == null)
                idf = 1.0;
        }
        else
            idf = idf(term);

        return tf * idf;
    }

    /**
     * Calculates the term-frequency of the term in file.
     * @param term
     * @param file
     * @return
     */
    private int tf(final String term, File file) {
        int[] hist;

        if (histogramMap != null && (hist = histogramMap.get(file)) != null) {
            int index = terms.indexOf(term);
            if (index == -1)
                return 0;
            return hist[index];
        }

        else {
            String[] words = FileUtils.readWords(file);
            AtomicInteger counter = new AtomicInteger(0);
            if (words.length > 100) {
                List<String> list = Arrays.asList(words);
                list.parallelStream().forEach(word -> {
                    if (word.equals(term))
                        counter.getAndIncrement();
                });
            }

            return counter.get();
        }
    }

    /**
     * Calculate the idf of the given word.
     * @param term
     * @return
     */
    private double idf(String term) {
        AtomicInteger containingFiles = new AtomicInteger(1); // smoothing
        int index = terms.indexOf(term);

        histogramMap.values().parallelStream().forEach((int[] hist) -> {
            if (hist[index] != 0)
                containingFiles.getAndIncrement();
        });

        return Math.log((double) trainingFiles.size() / containingFiles.get());
    }

    /**
     * Calculates the idf of all words in corpora.
     */
    private void calculateIdfs() {
        idfs = new HashMap<>();
        terms.parallelStream().forEach(term -> {
            idfs.put(term, idf(term));
        });
    }

    @Override
    public String toString() {
        String s = "Total terms: " + (terms == null ? 0 : terms.size());

        return s;
    }
}

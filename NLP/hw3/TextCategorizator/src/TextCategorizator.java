import utils.FileUtils;

import java.io.File;
import java.io.Serializable;
import java.util.*;
import java.util.stream.Collectors;

@SuppressWarnings("unchecked")
public class TextCategorizator implements Serializable {
    private Map<String, String> wordVectors = null;
    private Map<File, List<List<Double>>> docVectors = null;
    private Map<File, String> trainingFiles = null;
    private Set<String> classes = null;

    public TextCategorizator(Map<File, String> trainingFiles, String trainedVectors) {
        wordVectors = Collections.synchronizedMap(new HashMap<>());
        docVectors = Collections.synchronizedMap(new HashMap<>());
        classes = Collections.synchronizedSet(new HashSet<>());

        this.trainingFiles = trainingFiles;
        List<String> lines = FileUtils.readFileAsList(new File(trainedVectors));

        System.out.println("> Reading word vector file.");
        lines.parallelStream().forEach(line -> {
            String name = line.substring(0, line.indexOf(' '));
            wordVectors.put(name, line);
        });

        train(trainingFiles);
    }

    private void train(Map<File, String> trainingFiles) {
        System.out.println("> Starting training parallel.");
        trainingFiles.entrySet().parallelStream().forEach(entry -> {
            docVectors.put(entry.getKey(), getVectorsOfDoc(entry.getKey()));
            classes.add(entry.getValue());
        });
    }

    private List<List<Double>> getVectorsOfDoc(File doc) {
        List<List<Double>> lists = new ArrayList<>();

        List<Double> resultVecAvg = new ArrayList<>();
        List<Double> resultVecMax = new ArrayList<>();
        List<Double> resultVecMin = new ArrayList<>();
        int vecSize = 100;

        for (int i = 0; i < vecSize; i++) {
            resultVecAvg.add(0.0);
            resultVecMax.add(0.0);
            resultVecMin.add(0.0);
        }

        String[] words = FileUtils.readWords(doc);
        for (String word : words) {
            String line = wordVectors.get(word);
            if (line != null) {
                List<Double> vec = new ArrayList<>();
                String[] tokens = line.split(" ");
                for (int i = 1; i < tokens.length; i++) {
                    vec.add(Double.parseDouble(tokens[i]));
                }

                for (int i = 0; i < vec.size(); i++) {
                    resultVecAvg.set(i, resultVecAvg.get(i) + (vec.get(i) / vecSize));
                    resultVecMax.set(i, Math.max(resultVecMax.get(i), vec.get(i)));
                    resultVecMin.set(i, Math.min(resultVecMin.get(i), vec.get(i)));
                }
            }
        }

        lists.add(resultVecAvg); lists.add(resultVecMax); lists.add(resultVecMin);

        return lists;
    }

    private void getCosineSimilarities(List<Double> givenVec, int option, Map<File, Double> distances) {
        for (Map.Entry<File, List<List<Double>>> entry : docVectors.entrySet()) {
            List<Double> vec = null;
            if (option == 1) // AVG
                vec = entry.getValue().get(0);
            else if (option == 2) // MAX
                vec = entry.getValue().get(1);
            else if (option == 3) // MIN
                vec = entry.getValue().get(2);

            distances.put(entry.getKey(), cosSimilarity(givenVec, vec));
        }
    }

    private double cosSimilarity(List<Double> vec1, List<Double> vec2) {
        double norm1 = 0.0;
        double norm2 = 0.0;
        double dotProduct = 0.0;
        for (int i = 0; i < vec1.size(); i++) {
            norm1 += Math.pow(vec1.get(i), 2);
            norm2 += Math.pow(vec2.get(i), 2);
            dotProduct += vec1.get(i) * vec2.get(i);
        }
        return dotProduct / (Math.sqrt(norm1) * Math.sqrt(norm2));
    }

    // from http://stackoverflow.com/questions/109383/sort-a-mapkey-value-by-values-java
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

    private int countClass(List<File> files, String c) {
        int counter = 0;
        for (File file : files) {
            if (trainingFiles.get(file).equals(c))
                ++counter;
        }
        return counter;
    }

    public Map.Entry<String, Integer> classifyKnn(File file, int k, int option) {
        List<List<Double>> vecs = getVectorsOfDoc(file);
        List<Double> vec = getProperVector(vecs, option);

        Map<File, Double> distances = new HashMap<>();

        getCosineSimilarities(vec, option, distances);
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

    private List<Double> getProperVector(List<List<Double>> lists, int option) {
        List<Double> vec = null;
        if (option == 1) // AVG
            vec = lists.get(0);
        else if (option == 2) // MAX
            vec = lists.get(1);
        else if (option == 3) // MIN
            vec = lists.get(2);

        return vec;
    }

    public Map.Entry<String, Double> classifyRocchio(File file, int option) {
        List<List<Double>> vecs = getVectorsOfDoc(file);
        List<Double> vec = getProperVector(vecs, option);

        Map<File, Double> distances = new HashMap<>();
        getCosineSimilarities(vec, option, distances);

        distances = sortByValue(distances, true);
        List<Map.Entry<File, Double>> sortedFiles =
                new ArrayList<>(distances.entrySet());

        return new AbstractMap.SimpleEntry<>
                (trainingFiles.get(sortedFiles.get(0).getKey()),
                        sortedFiles.get(0).getValue());
    }
}

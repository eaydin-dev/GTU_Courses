
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class DatasetBuilder {
    private final String OTHER = "O";
    private String collectingWord1 = "B-protein";
    private String collectingWord2 = "I-protein";
    private final String SPLITTER = "\\s+";
    private final String EMPTY = "";
    private List<Sentence> sentences;

    public DatasetBuilder(){

    }
    public DatasetBuilder(String collectingWord1, String collectingWord2){
        this.collectingWord1 = collectingWord1;
        this.collectingWord2 = collectingWord2;
    }

    public void setCollectingWords(String collectingWord1, String collectingWord2){
        this.collectingWord1 = collectingWord1;
        this.collectingWord2 = collectingWord2;
    }

    public List<Sentence> build(String filePath){
        List<String> lines = ReadFile.readFileAsList(filePath);
        List<Sentence> sentenceLists = new ArrayList<>();

        Iterator<String> iterator = lines.iterator();
        String[] tokens;

        List<String> list = new ArrayList<>();
        while (iterator.hasNext()){
            String line = iterator.next();
            if (!line.equals(EMPTY)){
                list.add(line);
            }
            else{
                int start = 0, end = 0;
                Sentence sentence = new Sentence(list);
                for (int i = 0; i < list.size(); ++i){
                    String word = list.get(i);
                    tokens = word.split(SPLITTER);
                    if (tokens[1].equals(collectingWord1)){
                        start = i;
                        if (i >= list.size()-1 || !list.get(i+1).split(SPLITTER)[1].equals(collectingWord2))
                            end = i;
                        else{
                            int j = i+1;
                            while (j < list.size() && list.get(j).split(SPLITTER)[1].equals(collectingWord2))
                                ++j;
                            end = j - 1;
                        }

                        sentence.setProtein(start, end);
                    }
                }

                if (sentence.numOfProteins() != 0)
                    sentenceLists.add(sentence);
                list.clear();
            }
        }

        sentences = sentenceLists;
        return sentenceLists;
    }

    public String asString(){
        StringBuilder sb = new StringBuilder();
        for (Sentence sentence : sentences){
            sb.append(sentence.getAsText()).append("\n");
            //System.out.println(sentence.getAsText());
        }

        return sb.toString();
    }
}

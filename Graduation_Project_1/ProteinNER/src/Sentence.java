import java.util.ArrayList;
import java.util.List;

/**
 * Created by Emre on 15.12.2016.
 */
public class Sentence {

    private String[] tokens;
    private List<Integer> proteinBegins;
    private List<Integer> proteinEnds;
    private List<String> proteinNames = null;

    public Sentence(String[] tokens){
        this.tokens = tokens;
        proteinBegins = new ArrayList<>();
        proteinEnds = new ArrayList<>();
        proteinNames = new ArrayList<>();
    }

    public Sentence(List<String> tokens){
        this.tokens = new String[tokens.size()];
        int c = 0;
        for (String s : tokens){
            String[] splitted = s.split("\\s+");
            if (splitted != null){
                this.tokens[c++] = splitted[0];
            }
            else
                this.tokens[c++] = s;
        }

        proteinBegins = new ArrayList<>();
        proteinEnds = new ArrayList<>();
        proteinNames = new ArrayList<>();
    }


    public String getAsText(){
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < tokens.length; ++i){
            sb.append(tokens[i]);
            if (searchInList(i, proteinBegins)){
                sb.append("|B-PROTEIN");
            }
            else if (searchInList(i, proteinEnds))
                sb.append("|I-PROTEIN");
            else
                sb.append("|O");

            sb.append(" ");
        }

        return sb.toString();
    }

    private boolean searchInList(int index, List<Integer> list){
        for (int num : list){
            if (num == index)
                return true;
        }

        return false;
    }

    public String[] getTokens(){
        return tokens;
    }

    public int getStartOf(int index){
        return proteinBegins.get(index);
    }

    public int getEndOf(int index){
        return proteinEnds.get(index);
    }

    public int numOfProteins(){
        return proteinEnds.size();
    }

    public void setProtein(int start, int end){
        proteinBegins.add(start);
        proteinEnds.add(end);
        String s = "";
        for (int j = start; j <= end; ++j){
            s += tokens[j] + " ";
        }
        proteinNames.add(s);
    }

    public List<String> getProteins(){
//        if (proteinNames == null){
//
//
//            int size = proteinBegins.size();
//            for (int i = 0; i < size; ++i){
//                String s = "";
//                int b = proteinBegins.get(i);
//                int e = proteinEnds.get(i);
//                for (int j = b; j <= e; ++j){
//                    s += tokens[j] + " ";
//                }
//
//                result.add(s.trim());
//            }
//        }
//
//
//        return result;

        return proteinNames;
    }
}

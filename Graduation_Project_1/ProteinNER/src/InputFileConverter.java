import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class InputFileConverter {

    public static void main(String[] args) {
        String inpPath = "data/corpus/1_enhanced.iob2";
        String outputPath = "data/corpus/output.txt";

        List<String> lines = ReadFile.readFileAsList(inpPath);

        List<String> result = new ArrayList<>();

        Iterator<String> it = lines.iterator();
        StringBuilder sentence = new StringBuilder();
        while (it.hasNext()){
            String line = it.next();
            if (!line.equals("")){
                String[] tokens = line.split("\t");
                sentence.append(tokens[0]).append("|").append(tokens[1]).append(" ");
            }
            else{
                result.add(sentence.toString().trim());
                sentence.delete(0, sentence.length());
            }
        }

        FileUtil.printCollectionToTxt(result, outputPath);
    }
}

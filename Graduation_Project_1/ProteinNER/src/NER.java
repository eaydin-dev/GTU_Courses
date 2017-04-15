
import edu.umass.cs.mallet.base.fst.CRF4;
import edu.umass.cs.mallet.base.pipe.Pipe;
import edu.umass.cs.mallet.base.pipe.iterator.LineGroupIterator;
import edu.umass.cs.mallet.base.types.Instance;
import edu.umass.cs.mallet.base.types.InstanceList;
import edu.umass.cs.mallet.base.types.Sequence;

import java.io.StringReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.regex.Pattern;

public class NER {

    private CRF4 crf;
    private Pipe pipe;

    public NER(String inputFile){
        crf = (CRF4) Serialization.readObject(inputFile);
        pipe = crf.getInputPipe();
    }

    public  String tagIOB(String text) {
        StringBuffer tmp = new StringBuffer();
        // first, do the annotations
        List<String[][]> tagged = tag(text);
        String tag = "";
        for (int i=0; i<tagged.size(); i++) {
            String sent[][] = (String[][])tagged.get(i);
            for (int j=0; j<sent[0].length; j++) {
                tmp.append(sent[0][j]+"\t");
                tmp.append(sent[1][j]+"\n");
            }
            if (sent[0].length > 0)
                tmp.append("\n");
        }
        return tmp.toString();
    }

    public List<String[][]> tag(String text){
        List<String[][]> result = new ArrayList<>();

        InstanceList data = new InstanceList(pipe);
        data.add(new LineGroupIterator(new StringReader(tokenize(text)), Pattern.compile("^.*$"), false));

        for (int i=0; i<data.size(); i++) {
            // nab the sentence, set up the input, and
            Instance instance = data.getInstance(i);
            Sequence input = (Sequence) instance.getData();

            // get the predicted labeling...

            //Sequence predOutput = crf.viterbiPath(input).output();
            Sequence predOutput = crf.transduce(input);
            assert (input.size() == predOutput.size());

            String[][] tokens = new String[2][];
            tokens[0] = ((String) instance.getSource().toString()).split("[ \t]+");
            tokens[1] = new String[tokens[0].length];
            if (tokens[0].length > 0) {
                for (int j=0; j<predOutput.size(); j++)
                    tokens[1][j] = predOutput.get(j).toString();
            }
            result.add(tokens);
        }

        return result;
    }

    public  String tokenize(String s) {
        StringBuffer sb = new StringBuffer();
        try {
            Scanner scanner = new Scanner(new StringReader(s));;
            String t;
            while ((t = scanner.next()) != null) {
                sb.append(t+" ");
                if (t.toString().matches("[?!\\.]"))
                    sb.append("\n");
            }
            return sb.toString();
        } catch (Exception e) {
            System.err.println(e);
        }
        return sb.toString();
    }

    public  String[] getEntities(String text, String tag) {
        String[] result;
        List<String> tmpSegs = new ArrayList<>();
        List<String> tmpTags = new ArrayList<>();
        List<String[][]> tagged = tag(text);
        // cycle through all the sentences
        for (int i=0; i<tagged.size(); i++) {
            String sent[][] = (String[][])tagged.get(i);
            // we need to be sure that this isn't a blank line.
            if (sent[0].length > 0) {
                // cycle through words and build the segments
                StringBuffer tmpSeg = new StringBuffer(sent[0][0]);
                String tmpTag = sent[1][0].replaceAll("[BI]-","");
                for (int j=1; j<sent[0].length; j++) {
                    // if we're starting a new segment, store the
                    // seg-in-progress and start the new one...
                    if (!sent[1][j].replaceAll("[BI]-","").equals(tmpTag)) {
                        if (tmpTag.equals(tag)) {
                            tmpSegs.add(tmpSeg.toString());
                            tmpTags.add(tmpTag);
                        }
                        tmpSeg = new StringBuffer(sent[0][j]);
                        tmpTag = sent[1][j].replaceAll("[BI]-","");
                    }
                    // if same segment, just tack on this word...
                    else tmpSeg.append(" "+sent[0][j]);
                }
                if (tmpTag.equals(tag)) {
                    tmpSegs.add(tmpSeg.toString());
                    tmpTags.add(tmpTag);
                }
            }
        }
        // done. load it up!
        result = new String[tmpSegs.size()];
        for (int j=0; j<result.length; j++) {
            result[j] = (String)tmpSegs.get(j);
        }
        return result;
    }

}

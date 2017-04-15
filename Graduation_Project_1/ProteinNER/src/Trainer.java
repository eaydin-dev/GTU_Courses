

import abner.Input2TokenSequence;
import edu.umass.cs.mallet.base.fst.CRF;
import edu.umass.cs.mallet.base.fst.CRF4;
import edu.umass.cs.mallet.base.fst.MultiSegmentationEvaluator;
import edu.umass.cs.mallet.base.pipe.*;
import edu.umass.cs.mallet.base.pipe.iterator.LineGroupIterator;
import edu.umass.cs.mallet.base.pipe.tsf.OffsetConjunctions;
import edu.umass.cs.mallet.base.pipe.tsf.RegexMatches;
import edu.umass.cs.mallet.base.pipe.tsf.TokenTextCharPrefix;
import edu.umass.cs.mallet.base.pipe.tsf.TokenTextCharSuffix;
import edu.umass.cs.mallet.base.types.Instance;
import edu.umass.cs.mallet.base.types.InstanceList;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Pattern;

public class Trainer {
    private int numEvaluations = 5;
    private static String GREEK = "(alpha|beta|gamma|delta|epsilon|zeta|eta|theta|iota|kappa|lambda|mu|nu|xi|omicron|pi|rho|sigma|tau|upsilon|phi|chi|psi|omega)";

    public Trainer(){

    }

    /**
     * Trains the
     * @param trainFile
     * @param output
     * @param tags
     */
    public void train(String trainFile, String output, String[] tags) throws IOException {
        System.out.println("Training started");

        SerialPipes e = new SerialPipes(new Pipe[]{new Input2TokenSequence(), new RegexMatches("INITCAPS", Pattern.compile("[A-Z].*")), new RegexMatches("INITCAPSALPHA", Pattern.compile("[A-Z][a-z].*")), new RegexMatches("ALLCAPS", Pattern.compile("[A-Z]+")), new RegexMatches("CAPSMIX", Pattern.compile("[A-Za-z]+")), new RegexMatches("HASDIGIT", Pattern.compile(".*[0-9].*")), new RegexMatches("SINGLEDIGIT", Pattern.compile("[0-9]")), new RegexMatches("DOUBLEDIGIT", Pattern.compile("[0-9][0-9]")), new RegexMatches("NATURALNUMBER", Pattern.compile("[0-9]+")), new RegexMatches("REALNUMBER", Pattern.compile("[-0-9]+[.,]+[0-9.,]+")), new RegexMatches("HASDASH", Pattern.compile(".*-.*")), new RegexMatches("INITDASH", Pattern.compile("-.*")), new RegexMatches("ENDDASH", Pattern.compile(".*-")), new TokenTextCharPrefix("PREFIX=", 3), new TokenTextCharPrefix("PREFIX=", 4), new TokenTextCharSuffix("SUFFIX=", 3), new TokenTextCharSuffix("SUFFIX=", 4), new OffsetConjunctions(new int[][]{{-1}, {1}}), new RegexMatches("ALPHANUMERIC", Pattern.compile(".*[A-Za-z].*[0-9].*")), new RegexMatches("ALPHANUMERIC", Pattern.compile(".*[0-9].*[A-Za-z].*")), new RegexMatches("ROMAN", Pattern.compile("[IVXDLCM]+")), new RegexMatches("HASROMAN", Pattern.compile(".*\\b[IVXDLCM]+\\b.*")), new RegexMatches("GREEK", Pattern.compile(GREEK)), new RegexMatches("HASGREEK", Pattern.compile(".*\\b" + GREEK + "\\b.*")), new RegexMatches("PUNCTUATION", Pattern.compile("[,.;:?!-+]")), new TokenSequence2FeatureVectorSequence(true, true)});
        CRF4 crf = new CRF4(e, (Pipe)null);
        System.out.println("Reading \'" + trainFile + "\' file...");
        InstanceList trainingData = new InstanceList(e);
        trainingData.add(new LineGroupIterator(new FileReader(new File(trainFile)), Pattern.compile("^.*$"), false));
        System.out.println("Doing the deed...");
        System.out.println("Number of features = " + e.getDataAlphabet().size());
        System.out.println("Training on " + trainingData.size() + " training instances...");
        crf.addStatesForLabelsConnectedAsIn(trainingData);
        if(tags == null) {
            crf.train(trainingData, (InstanceList)null, (InstanceList)null, (MultiSegmentationEvaluator)null, 99999, 10, new double[]{0.2D, 0.5D, 0.8D});
        } else {
            String[] bTags = new String[tags.length];
            String[] iTags = new String[tags.length];
            int i = 0;

            while(true) {
                if(i >= tags.length) {
                    MultiSegmentationEvaluator eval = new MultiSegmentationEvaluator(bTags, iTags, false);
                    crf.train(trainingData, (InstanceList)null, (InstanceList)null, eval, 99999, 10, new double[]{0.2D, 0.5D, 0.8D});
                    break;
                }

                bTags[i] = "B-" + tags[i];
                iTags[i] = "I-" + tags[i];
                ++i;
            }
        }

        crf.write(new File(output));
    }

    private List<Instance> getInstances(String path) throws IOException {
        List<Instance> res = new ArrayList<>();

        String line;
        int groupIndex = 0;
        Pattern p = Pattern.compile("^.*$");
        StringReader reader = new StringReader(ReadFile.readFileAsSingleStringNL(path));
        LineNumberReader r = new LineNumberReader(reader);
        while (((line = r.readLine()) != null)){
            if (p.matcher(line).matches()){
                Instance instance = new Instance(line, null,"linegroup" + (groupIndex++), null);
                res.add(instance);
            }
        }

        System.out.println(res.size());
        return res;
    }
}


//    Pipe pipe;
//    CRF4 crf;
//
//    List<Pipe> pipes = new ArrayList<>();
//        pipes.add(new Input2CharSequence());
//                pipes.add(new RegexMatches ("INITCAPSALPHA", Pattern.compile ("[A-Z][a-z].*")));
//                pipes.add(new RegexMatches("ALLCAPS", Pattern.compile ("[A-Z]+")));
//                pipes.add(new RegexMatches ("CAPSMIX", Pattern.compile ("[A-Za-z]+")));
//                pipes.add(new RegexMatches ("HASDIGIT", Pattern.compile (".*[0-9].*")));
//                pipes.add(new RegexMatches ("SINGLEDIGIT", Pattern.compile ("[0-9]")));
//                pipes.add(new RegexMatches ("DOUBLEDIGIT", Pattern.compile ("[0-9][0-9]")));
//                pipes.add(new RegexMatches ("NATURALNUMBER", Pattern.compile ("[0-9]+")));
//                pipes.add(new RegexMatches ("REALNUMBER", Pattern.compile ("[-0-9]+[.,]+[0-9.,]+")));
//                pipes.add(new RegexMatches ("HASDASH", Pattern.compile (".*-.*")));
//                pipes.add(new RegexMatches ("INITDASH", Pattern.compile ("-.*")));
//                pipes.add(new RegexMatches ("ENDDASH", Pattern.compile (".*-")));
//                pipes.add(new TokenTextCharPrefix("PREFIX=", 3));
//                pipes.add(new TokenTextCharPrefix ("PREFIX=", 4));
//                pipes.add(new TokenTextCharSuffix("SUFFIX=", 3));
//                pipes.add(new TokenTextCharSuffix("SUFFIX=", 4));
//                pipes.add(new OffsetConjunctions(new int[][] {{-1}, {1}}));
//                pipes.add(new RegexMatches ("ALPHANUMERIC", Pattern.compile (".*[A-Za-z].*[0-9].*")));
//                pipes.add(new RegexMatches ("ALPHANUMERIC", Pattern.compile (".*[0-9].*[A-Za-z].*")));
//                pipes.add(new RegexMatches ("ROMAN", Pattern.compile ("[IVXDLCM]+")));
//                pipes.add(new RegexMatches ("HASROMAN", Pattern.compile (".*\\b[IVXDLCM]+\\b.*")));
//                pipes.add(new RegexMatches ("GREEK", Pattern.compile (GREEK)));
//                pipes.add(new RegexMatches ("HASGREEK", Pattern.compile (".*\\b"+GREEK+"\\b.*")));
//                pipes.add(new RegexMatches ("PUNCTUATION", Pattern.compile ("[,.;:?!-+]")));
//                pipes.add(new TokenSequence2FeatureVectorSequence(true, true));
//
//                pipe = new SerialPipes(pipes);
//
//                crf = new CRF4(pipe, null);
//                System.out.println("pipes and CRF created");
//
//                InstanceList instances = new InstanceList(pipe);
//                instances.add(new LineGroupIterator(
//                new StringReader(ReadFile.readFileAsSingleStringNL(trainFile)),
//                Pattern.compile("^.*$"), false));
//
//                System.out.println ("Number of features = " + pipe.getDataAlphabet().size());
//                System.out.println ("Training on " + instances.size() + " training instances...");
//
//                crf.addStatesForLabelsConnectedAsIn(instances);
//
//                String[] bTags = new String[tags.length];
//                String[] iTags = new String[tags.length];
//                for (int i=0; i<tags.length; i++) {
//        bTags[i] = "B-"+tags[i];
//        iTags[i] = "I-"+tags[i];
//        }
//
//        MultiSegmentationEvaluator evaluator = new MultiSegmentationEvaluator(bTags, iTags, false);
//        crf.train(instances, null, null, evaluator);

//pipe = new SerialPipes(new Pipe[] {
//        new Input2CharSequence(),
//        new RegexMatches("INITCAPS", Pattern.compile ("[A-Z].*")),
//        new RegexMatches ("INITCAPSALPHA", Pattern.compile ("[A-Z][a-z].*")),
//        new RegexMatches ("ALLCAPS", Pattern.compile ("[A-Z]+")),
//        new RegexMatches ("CAPSMIX", Pattern.compile ("[A-Za-z]+")),
//        new RegexMatches ("HASDIGIT", Pattern.compile (".*[0-9].*")),
//        new RegexMatches ("SINGLEDIGIT", Pattern.compile ("[0-9]")),
//        new RegexMatches ("DOUBLEDIGIT", Pattern.compile ("[0-9][0-9]")),
//        new RegexMatches ("NATURALNUMBER", Pattern.compile ("[0-9]+")),
//        new RegexMatches ("REALNUMBER", Pattern.compile ("[-0-9]+[.,]+[0-9.,]+")),
//        new RegexMatches ("HASDASH", Pattern.compile (".*-.*")),
//        new RegexMatches ("INITDASH", Pattern.compile ("-.*")),
//        new RegexMatches ("ENDDASH", Pattern.compile (".*-")),
//        new TokenTextCharPrefix("PREFIX=", 3),
//        new TokenTextCharPrefix ("PREFIX=", 4),
//        new TokenTextCharSuffix ("SUFFIX=", 3),
//        new TokenTextCharSuffix("SUFFIX=", 4),
//        new OffsetConjunctions(new int[][] {{-1}, {1}}),
//        new RegexMatches ("ALPHANUMERIC", Pattern.compile (".*[A-Za-z].*[0-9].*")),
//        new RegexMatches ("ALPHANUMERIC", Pattern.compile (".*[0-9].*[A-Za-z].*")),
//
//        new RegexMatches ("ROMAN", Pattern.compile ("[IVXDLCM]+")),
//        new RegexMatches ("HASROMAN", Pattern.compile (".*\\b[IVXDLCM]+\\b.*")),
//        new RegexMatches ("GREEK", Pattern.compile (GREEK)),
//        new RegexMatches ("HASGREEK", Pattern.compile (".*\\b"+GREEK+"\\b.*")),
//
//        new RegexMatches ("PUNCTUATION", Pattern.compile ("[,.;:?!-+]")),
//
//        new TokenSequence2FeatureVectorSequence(true, true)
//        });

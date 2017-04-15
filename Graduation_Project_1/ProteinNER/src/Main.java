import edu.umass.cs.mallet.base.pipe.iterator.LineGroupIterator;
import edu.umass.cs.mallet.base.types.Instance;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.LineNumberReader;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Pattern;

public class Main {

    public static void main(String[] args) throws IOException {
        String output = "data/crf.crf";
        String testFile = "data/test.txt";
        String crfFile = "crf.crf";

        //String trainFile = "data/corpus/1_enhanced.iob2";
        String trainFile = "data/corpus/output.txt";

        Trainer trainer = new Trainer();
        //trainer.train(trainFile, output, new String[]{"protein", "cell_type", "Junk", "DNA"});
        //trainer.train(trainFile, "data/crf2.crf", new String[]{"protein"});

        //test2(trainFile);
        //testt("data/corpus/d.txt");
        //testt("data/corpus/training.file");
        //NER ner = new NER(output);
        NER ner = new NER("data/crf2.crf");
        String res = ner.tagIOB(ReadFile.readFileAsSingleString(testFile));
        String[] entities = ner.getEntities(ReadFile.readFileAsSingleString(testFile), "protein");
        for (String s : entities){
            System.out.println("> " + s);
        }
        //System.out.println(res);
    }

//    public static void abnerTester(){
//        String output = "crf.crf";
//        String trainFile = "data/corpus/1_enhanced.iob2";
//
//        abner.Trainer trainer = new Trainer();
//        trainer.train(trainFile, output, new String[]{"protein", "cell_type", "Junk", "DNA"});
//    }


    public static void testt(String path) throws FileNotFoundException {
        //LineGroupIterator it = new LineGroupIterator()
        LineGroupIterator lineGroupIterator = new LineGroupIterator(
                new LineNumberReader(new StringReader(ReadFile.readFileAsSingleStringNL(path))),
                Pattern.compile("^.*$"), false);

        int c = 1;
        while (lineGroupIterator.hasNext()){
            System.out.println((c++));
            lineGroupIterator.next();
            //System.out.println(lineGroupIterator.next().getData());
        }
    }

    public static void test2(String path) throws IOException {
        String s = "O O ---- what W=the@1 W=hell@2\n\n"
                + "O O ---- the W=what@-1 W=hell@1\n\n"
                + "O O ---- hell W=what@-2 W=the@-1";
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
        // ^\s*$
//        LineGroupIterator it = new LineGroupIterator(new StringReader(s), Pattern.compile("\n"), true);
//        int c = 1;
//        while (it.hasNext()){
//            System.out.println((c++));
//            System.out.println(it.next().getData());
//        }

//        Pattern p = Pattern.compile("^.*$");
//        Reader reader = new FileReader(new File(path));
//        reader = new StringReader("\nasfgasdfasfg\nasfgsdf");
//        LineNumberReader r = new LineNumberReader(reader);
//
//        String line;
//        while (((line = r.readLine()) != null)){
//            if (p.matcher(line).matches()){
//                System.out.println("111s");
//            }
//        }
    }
}

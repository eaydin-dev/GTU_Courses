import java.io.FileWriter;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

/**
 * Created by EA on 22.04.2016.
 */
public class FileUtil {

    public static void printCollectionToTxt(Collection<String> list, String outPath){
        try {
            FileWriter fw = new FileWriter(outPath);
            for (String s : list)
                fw.write(s + "\n");

            fw.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static String readFile(String path, Charset encoding) {

        byte[] encoded = new byte[0];
        try {
            encoded = Files.readAllBytes(Paths.get(path));
        } catch (IOException e) {
            e.printStackTrace();
        }
        return new String(encoded, encoding);
    }

    /**
     * diffs of f1 over f2. they both should be sorted.
     * @param f1
     * @param f2
     * @param sense true if you want to check case insensitive
     * @return
     */
    public static List<String> diff(String f1, String f2, boolean sense){
        List<String> pats = ReadFile.readFileAsList(f1);
        List<String> hosts = ReadFile.readFileAsList(f2);
        List<String> result = new ArrayList<>();

        if (sense){
            for (int i = 0; i < pats.size(); ++i)
                pats.set(i, pats.get(i).toLowerCase());

            for (int i = 0; i < hosts.size(); ++i)
                hosts.set(i, hosts.get(i).toLowerCase());
        }

        for (int i = 0; i < pats.size(); ++i){
            System.out.println("> " + i);
            if (Collections.binarySearch(hosts, pats.get(i)) < 0)
                result.add(pats.get(i));
        }

        return result;
    }
}

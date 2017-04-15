import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Stream;

/**
 * static class to handle reading file.
 * Created by EA on 23.11.2016.
 */
public final class ReadFile {

    private ReadFile() { }

    /**
     * uses Java 8 streams to read file lines.
     * @param path
     * @return
     */
    public static List<String> readFileAsList(String path) {
        assert path != null : "Path cannot be null.";

        List<String> lines = new ArrayList<>();
        try (Stream<String> stream = Files.lines(Paths.get(path), Charset.defaultCharset())) {
            stream.forEach(lines::add);
        } catch (IOException ex) {
            System.err.println("File not found.");
            ex.printStackTrace();
            return null;
        }

        return lines;
    }

    /**
     * calls the method above and converts to array.
     * @param path
     * @return
     */
    public static String[] readFileAsArray(String path) {
        assert path != null : "Path cannot be null.";

        List<String> lines = readFileAsList(path);

        assert lines != null;

        String[] lineArr = new String[lines.size()];
        for (int i = 0; i < lines.size(); ++i) {
            lineArr[i] = lines.get(i);
        }

        return lineArr;
    }


    public static String readFileAsSingleString(String path){
        List<String> lines = readFileAsList(path);
        StringBuffer sb = new StringBuffer();
        assert lines != null;
        for (String line : lines)
            sb.append(line).append(" ");
        return sb.toString();
    }

    public static String readFileAsSingleStringNL(String path){
        List<String> lines = readFileAsList(path);
        StringBuffer sb = new StringBuffer();
        assert lines != null;
        for (String line : lines)
            sb.append(line).append("\n");
        return sb.toString();
    }
}

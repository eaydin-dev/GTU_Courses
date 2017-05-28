package utils;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.regex.Pattern;
import java.util.stream.Stream;

/**
 * static class to handle reading file.
 * Created by EA on 23.11.2016.
 */
public final class FileUtils {

    private static final Pattern UNDESIRABLES = Pattern.compile("[]\\[(){},.;:!?<>%'‘’\"“”]");

    private FileUtils() { }

    /**
     * uses Java 8 streams to read file lines.
     * @param file
     * @return
     */
    public static List<String> readFileAsList(File file) {
        assert file != null : "Path cannot be null.";

        List<String> lines = new ArrayList<>();
        try (Stream<String> stream = Files.lines(file.toPath(),
                Charset.forName("WINDOWS-1254"))) {
            stream.forEach(lines::add);
        } catch (IOException ex) {
            System.err.println("File not found.");
            ex.printStackTrace();
            return null;
        }

        return lines;
    }

    /**
     * Reads file lines, combines them while replacing all non-letter chars.
     * @param path
     * @return
     */
    public static String readFileAsSingleString(File path){
        List<String> lines = readFileAsList(path);
        StringBuilder sb = new StringBuilder();
        assert lines != null;
        for (String line : lines)
            sb.append(line).append(" ");

        return UNDESIRABLES.matcher(sb.toString()).replaceAll("");
        // replace all non-letter chars and reduce multiple spaces to one.
//        return sb.toString().replaceAll("[^a-zA-Z\\s]", "")
//                .replaceAll("\\s+", " ");
    }

    /**
     * Reads file as a single string, and than splits words.
     * @param path
     * @return
     */
    public static String[] readWords(File path) {
        String content = readFileAsSingleString(path).toLowerCase();
        String[] tokens = content.split("\\s+");
        List<String> temp = new ArrayList<>();
        for (String token : tokens)
            if (token.length() > 2)
                temp.add(token);
        String[] result = new String[temp.size()];
        int i = 0;
        for (String token : temp)
            result[i++] = token;
        return result;
    }


    public static void printCollectionToTxt(Collection<String> list,
                                            String outPath){
        try {
            FileWriter fw = new FileWriter(outPath);
            for (String s : list)
                fw.write(s + "\n");

            fw.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

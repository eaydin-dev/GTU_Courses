package utils;

import java.io.File;
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

        // replace all non-letter chars and reduce multiple spaces to one.
        return sb.toString().replaceAll("[^a-zA-Z\\s]", "")
                .replaceAll("\\s+", " ");
    }

    /**
     * Reads file as a single string, and than splits words.
     * @param path
     * @return
     */
    public static String[] readWords(File path) {
        String content = readFileAsSingleString(path);
        return content.split("\\s+");
    }

}

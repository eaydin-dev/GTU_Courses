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
        try (Stream<String> stream = Files.lines(file.toPath(), Charset.forName("ISO-8859-9"))) {
            stream.forEach(lines::add);
        } catch (IOException ex) {
            System.err.println("File not found.");
            ex.printStackTrace();
            return null;
        }

        return lines;
    }


    public static String readFileAsSingleString(File path){
        List<String> lines = readFileAsList(path);
        StringBuilder sb = new StringBuilder();
        assert lines != null;
        for (String line : lines)
            sb.append(line).append(" ");
        return sb.toString();
    }

}

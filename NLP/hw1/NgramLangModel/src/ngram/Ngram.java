package ngram;

import java.io.Serializable;
import java.util.Arrays;

public class Ngram implements Serializable {
    private final int MAX = 5;
    private StringBuilder sb = new StringBuilder();
    private int n = 0;

    private void printArr(char[] arr) {
        System.out.printf("-> ");
        for (char c : arr)
            System.out.printf("%c", c);
        System.out.println();
    }

    public Ngram(char... grams) {
        //printArr(grams);
        if (grams.length > 5)
            throw new UnsupportedOperationException("Max n = 5");

        n = grams.length;
        for (char gram : grams)
            sb.append(gram).append(' ');
    }

    public boolean add(char c) {
        if (n == MAX)
            return false;
        sb.append(c).append(' ');
        n++;
        return true;
    }

    public int n() {
        return n;
    }

    public String toString() {
        return sb.toString().trim();
    }

    @Override
    public boolean equals(Object obj) {
        return obj instanceof Ngram && obj.toString().equals(this.toString());
    }

    @Override
    public int hashCode() {
        return 37 * this.toString().hashCode();
    }
}

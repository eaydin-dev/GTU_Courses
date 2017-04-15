public class Test {

    public static void main(String[] args) {

        int[][] arr = {
                {1, 2, 3, 4},
                {5, 6, 7, 8},
                {9, 5, 4, 7}};

        int n = 3;
        int m = 4;

        System.out.println("[ ");
        String s = "";
        for (int i = 0; i < n; i++) {
            s = "  ";
            for (int j = 0; j < m; j++) {
                s += (String.format("%3d", arr[i][j] ) + " ");
            }
            System.out.println(s);
        }
        for (int j = 0; j < s.length(); j++) {
            System.out.print(" ");
        }
        System.out.print("  ]");
    }
}

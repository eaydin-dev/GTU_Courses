import java.io.*;

public final class Serialization {

    public static void writeObject(Object obj, String path){
        try (ObjectOutputStream oos =
                     new ObjectOutputStream(new FileOutputStream(path))) {

            oos.writeObject(obj);
            //System.out.println("Done");
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }


    public static Object readObject(String path){
        Object obj = null;

        FileInputStream fin = null;
        ObjectInputStream ois = null;

        try {
            fin = new FileInputStream(path);
            ois = new ObjectInputStream(fin);
            obj = ois.readObject();

        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            if (fin != null) {
                try {
                    fin.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            if (ois != null) {
                try {
                    ois.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        return obj;
    }
}

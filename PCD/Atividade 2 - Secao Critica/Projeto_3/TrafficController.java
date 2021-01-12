public class TrafficController {

    boolean traffic_free = true;

    public synchronized int enterLeft() {
        if (traffic_free == false) return 0;

        traffic_free = false;
        return 1;
    }
    public synchronized int enterRight() {
        if (traffic_free == false) return 0;

        traffic_free = false;
        return 1;
    }
    public synchronized void leaveLeft() {
        traffic_free = true;
    }

    public synchronized void leaveRight() {
        traffic_free = true;
    }

}